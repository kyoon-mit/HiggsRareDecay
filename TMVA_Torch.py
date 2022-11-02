from ROOT import TMVA, TFile, TTree, TCut
from subprocess import call
from os.path import isfile
 
import torch
from torch import nn

import importlib
disco = importlib.import_module('Disco')

##########################################
############# Loss Function ##############
##########################################

# Decorrelation
def disco_loss(y_pred, y_true, mass_tensor=[], weight_tensor=[], penalty_rate=0.1):
    loss_term = nn.functional.binary_cross_entropy_with_logits(y_pred, y_true)
    bkg_indices = (y_true < 0.001).nonzero(as_tuple=True)[0] # CHECK HOW LABELS ARE DEFINED!!
    if len(bkg_indices) <= 2:
        return loss_term
    else:
        bkg_y_pred = y_pred[bkg_indices]
        bkg_masses = mass_tensor[bkg_indices]
        bkg_weights = weight_tensor
        if torch.is_tensor(weight_tensor):
            bkg_weights = weight_tensor[bkg_indices]
        regularization_term = disco.distance_corr(bkg_y_pred, bkg_masses, bkg_weights)
        return loss_term + penalty_rate * regularization_term

##########################################
########### PyTorch Functions ############
##########################################

# Define model
class Net(nn.Module):
    def __init__(self, input_shape):
        super(Net, self).__init__()
        hidden_nodes_dim = input_shape*2
        self.linear_relu_stack = nn.Sequential(
            nn.Linear(input_shape, hidden_nodes_dim),
            nn.ReLU(),
            nn.Linear(hidden_nodes_dim, hidden_nodes_dim),
            nn.ReLU(),
            nn.Linear(hidden_nodes_dim, 1),
            nn.Sigmoid()
        )
    def forward(self, x):
        logits = self.linear_relu_stack(x)
        return logits
 
# Define train function
def train(model, train_loader, val_loader, num_epochs, batch_size, optimizer, criterion, save_best, scheduler):
    trainer = optimizer(model.parameters(), lr=0.01)
    schedule, schedulerSteps = scheduler
    best_val = None
    disco_lambda = 0.1
    
    for epoch in range(num_epochs):
        # Training Loop
        # Set to train model
        model.train()
        running_train_loss = 0.0
        running_val_loss = 0.0
        for i, (X, y) in enumerate(train_loader):
            trainer.zero_grad()
            HCandMass = X[:,0].unsqueeze(1)
            weights = X[:,1].unsqueeze(1)
            y_signal = y[:,0].unsqueeze(1)
            # bkg = y[:,1]
            output = model(X[:,2:])
            # train_loss = criterion(output, y)
            # disco_loss(y_pred, y_true, mass_tensor=[], weight_tensor=[], penalty_rate=0.1)
            train_loss = criterion(output, y_signal, HCandMass, weights, disco_lambda)
            train_loss.backward()
            trainer.step()
 
            # print train statistics
            # running_train_loss += train_loss.item()
            # if i % 128 == 127:    # print every 32 mini-batches
                # print("[{}, {}] train loss: {:.3f}".format(epoch+1, i+1, running_train_loss / 128))
                # running_train_loss = 0.0
 
        if schedule:
            schedule(optimizer, epoch, schedulerSteps)
 
        # Validation Loop
        # Set to eval mode
        model.eval()
        with torch.no_grad():
            for i, (X, y) in enumerate(val_loader):
                HCandMass = X[:,0].unsqueeze(1)
                weights = X[:,1].unsqueeze(1)
                y_signal = y[:,0].unsqueeze(1)
                # bkg = y[:,1]
                output = model(X[:,2:])
                # val_loss = criterion(output, y)
                val_loss = criterion(output, y_signal, HCandMass, weights, disco_lambda)
                running_val_loss += val_loss.item() 
            curr_val = running_val_loss / len(val_loader)
            if save_best:
               if best_val==None:
                   best_val = curr_val
               best_val = save_best(model, curr_val, best_val)
 
            # print val statistics per epoch
            print("[{}] val loss: {:.3f}".format(epoch+1, curr_val))
            running_val_loss = 0.0
 
    print("Finished Training on {} Epochs!".format(epoch+1))
 
    return model

def predict(model, test_X, batch_size=32):
    # Set to eval mode
    model.eval()
 
    test_dataset = torch.utils.data.TensorDataset(torch.Tensor(test_X))
    test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=batch_size, shuffle=False)
 
    predictions = []
    with torch.no_grad():
        for i, data in enumerate(test_loader):
            X = data[0]
            outputs = model(X[:,2:])
            predictions.append(outputs)
        preds = torch.cat(predictions)
 
    return preds.numpy()

##########################################
############# TMVA Loading ###############
##########################################

def load_data(cat='ggH', meson='rho', year=[2018], sgn_mc=[1027], bkg_mc=[6,7,8,9,10]): # TODO: move to utils module
    """
    Helper function to load data into TMVA.
    
    Arguments
    cat: (string) name of the category, e.g. 'ggH', 'VBF',  'VBFlow', 'Zinv', etc.
    meson: (string) name of the meson produced, e.g. 'phi', 'rho'
    year: (list) year of the data/MC taken/produced
    sgn_mc: (list) MC number(s) of the signal process; - for data
    bkg_mc: (list) MC number(s) of the background process; - for data

    Returns
    (list of signal TFiles, list of background TFiles)
    """
    top_level_dir = '/work/submit/kyoon/RareHiggs/data/'
    file_format = top_level_dir + 'cat_{meson}/cat_{meson}_{cat}/test/test_mc{mc}_{cat}cat_{meson}cat_{yr}.root'

    signal_files = []
    background_files = []
    
    for yr_ in year:
        for mc_ in sgn_mc:
            f = TFile.Open(file_format.format(**{'yr':yr_, 'mc':mc_, 'cat':cat, 'meson':meson}), 'READ')
            signal_files.append(f)
        for mc_ in bkg_mc:
            f = TFile.Open(file_format.format(**{'yr':yr_, 'mc':mc_, 'cat':cat, 'meson':meson}), 'READ')
            background_files.append(f)
    return signal_files, background_files

def add_files_to_dataloader(signal_files, background_files, dataloader,
                            cut_train='',
                            cut_test='',
                            signal_weight=1,
                            background_weight=1):
    """
    Adds files to dataloader.
    """

    for sgn_file in signal_files:
        dataloader.AddTree(sgn_file.events, 'Signal', signal_weight, cut_train, 'train')
        dataloader.AddTree(sgn_file.events, 'Signal', signal_weight, cut_test, 'test')
    for bkg_file in background_files:
        dataloader.AddTree(bkg_file.events, 'Background', background_weight, cut_train, 'train')
        dataloader.AddTree(bkg_file.events, 'Background', background_weight, cut_test, 'test')

##########################################
############## Script part ###############
##########################################
        
# Setup TMVA
TMVA.Tools.Instance()
TMVA.PyMethodBase.PyInitialize()
 
output = TFile.Open('TMVA.root', 'RECREATE')
factory = TMVA.Factory('TMVAClassification', output,
                       '!V:!Silent:Color:DrawProgressBar:Transformations=D,G:AnalysisType=Classification')

# Setup dataloader
features = ['HCandMass', 'w', # It is crucial to keep these two variables in indices 0 and 1
            'HCandPT',
            'goodPhotons_pt',
            'goodMeson_iso']

dataloader = TMVA.DataLoader('dataset')
for feature in features:
    dataloader.AddVariable(feature)

# Define splits and cuts
split_train = '(Entry$ % 3) > 0'
split_test = '(Entry$ % 3) == 0'

higgs_mass_window = 'HCandMass > 115 && HCandMass < 135'
# nan_remove = ''

cut_train = ' && '.join((split_train, higgs_mass_window))
cut_test = ' && '.join((split_test, higgs_mass_window))

print(cut_train)

# Load data
s, b = load_data()
add_files_to_dataloader(s, b, dataloader, cut_train, cut_test)

# Construct loss function and optimizer
loss = torch.nn.MSELoss()
optimizer = torch.optim.SGD

load_model_custom_objects = {"optimizer": optimizer, "criterion": disco_loss,
                             "train_func": train, "predict_func": predict}

# Create the model
# Convert the model to torchscript before saving
model = Net(len(features)-2)
m = torch.jit.script(model)
torch.jit.save(m, "modelClassification.pt")
print(m)
 
# Book methods
# TODO: Compare with BDTG

torch_method_VarTransform_string =\
    'N(' + ','.join([f'_V{i}_' for i in range(2, len(features))]) + ')'
factory.BookMethod(dataloader, TMVA.Types.kPyTorch, 'PyTorch',
                   'H:!V:VarTransform=' + torch_method_VarTransform_string + 
                   ':FilenameModel=modelClassification.pt:'
                   'FilenameTrainedModel=trainedModelClassification.pt:NumEpochs=40:BatchSize=128') 

# Train, test, and evaluate
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods() 
 
# Plot ROC Curves
roc = factory.GetROCCurve(dataloader)
roc.SaveAs('ROC_ClassificationPyTorch.png')
