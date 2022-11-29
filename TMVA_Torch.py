from ROOT import TMVA, TFile, TTree, TCut
from subprocess import call
from os.path import isfile
 
import torch
from torch import nn

import importlib
disco = importlib.import_module('Disco')

global LEARNING_RATE, DISCO_LAMBDA
LEARNING_RATE = 0.001
DISCO_LAMBDA = 0.1

##########################################
############# Loss Function ##############
##########################################

# Decorrelation
def disco_loss(y_pred, y_true, mass_tensor=[], weight_tensor=[], penalty_rate=0.1):
    loss_term = nn.functional.binary_cross_entropy_with_logits(y_pred, y_true)
    y_signal = y_true[:,0]
    bkg_indices = (y_signal < 0.001).nonzero(as_tuple=True)[0] # CHECK HOW LABELS ARE DEFINED!!
    if len(bkg_indices) <= 2:
        return loss_term
    else:
        bkg_y_pred = y_pred[:,0][bkg_indices]
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
        hidden_nodes_dim = input_shape*4
        self.linear_stack = nn.Sequential(
            nn.Linear(input_shape, hidden_nodes_dim),
            nn.ReLU(),
            nn.Linear(hidden_nodes_dim, hidden_nodes_dim),
            nn.ReLU(),
            nn.Linear(hidden_nodes_dim, 2),
            #nn.Sigmoid(),
            #nn.Linear(hidden_nodes_dim, hidden_nodes_dim),
            #nn.Sigmoid(),
            #nn.Linear(hidden_nodes_dim, hidden_nodes_dim),
            #nn.Sigmoid(),
            #nn.Linear(hidden_nodes_dim, 1),
            #nn.Tanh()
            nn.Softmax(dim=1),
        )
    def forward(self, x):
        logits = self.linear_stack(x)
        return logits
 
# Define train function
def train(model, train_loader, val_loader, num_epochs, batch_size, optimizer, criterion, save_best, scheduler):
    trainer = optimizer(model.parameters(), lr=LEARNING_RATE)
    schedule, schedulerSteps = scheduler
    best_val = None
    disco_lambda = DISCO_LAMBDA
    
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
            output = model(X[:,2:])
            #train_loss = criterion(output, y)
            # disco_loss(y_pred, y_true, mass_tensor=[], weight_tensor=[], penalty_rate=0.1)
            train_loss = criterion(output, y, HCandMass, weights, disco_lambda)
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
                output = model(X[:,2:])
                #val_loss = criterion(output, y)                
                val_loss = criterion(output, y, HCandMass, weights, disco_lambda)
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
            for j in range(len(data)):
                X = data[j]
                outputs = model(X[:,2:])
                predictions.append(outputs)
        preds = torch.cat(predictions)
 
    return preds.numpy()

##########################################
############# TMVA Loading ###############
##########################################

def load_data(cat='GF',
              meson='Rho',
              year=[2018],
              sgn_mc=[1027],
              bkg_mc=[10,11,12,13,14]): # TODO: move to utils module
    """
    Helper function to load data into TMVA.
    
    Arguments
    cat: (string) name of the category, e.g. 'GF', 'VBF',  'VBFlow', 'Zinv', etc.
    meson: (string) name of the meson produced, e.g. 'Phi', 'Rho'
    year: (list) year of the data/MC taken/produced
    sgn_mc: (list) MC number(s) of the signal process; - for data
    bkg_mc: (list) MC number(s) of the background process; - for data
    Returns
    (list of signal TFiles, list of background TFiles)
    """
    top_level_dir = '/work/submit/mariadlf/Hrare/SEPT24'
    file_format = top_level_dir + '/{yr}/outname_mc{mc}_{cat}cat_{meson}Cat_{yr}.root'

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

def add_files_to_dataloader(signal_files,
                            background_files,
                            dataloader,
                            signal_weight=1,
                            background_weight=1):
    """
    Adds files to dataloader.
    """

    for sgn_file in signal_files:
        sgn_tree = sgn_file.Get('events')
        dataloader.AddSignalTree(sgn_tree, signal_weight)
    for bkg_file in background_files:
        bkg_tree = bkg_file.Get('events')
        dataloader.AddBackgroundTree(bkg_tree, background_weight)

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
dataloader = TMVA.DataLoader('dataset')
features = ['HCandMass', 'w', # It is crucial to keep these two variables in indices 0 and 1
            'HCandPT',
            'goodPhotons_pt',
            'goodMeson_pt',
            'goodPhotons_eta',
            'sigmaHCandMass_Rel2',
            'goodPhotons_mvaID',
            'goodMeson_mass',
            'goodMeson_iso',
            'goodMeson_sipPV',
            'dPhiGammaMesonCand',
            'dEtaGammaMesonCand',
            'nGoodJets',
            'SoftActivityJetNjets5',
            'DeepMETResolutionTune_pt']

for feature in features:
    dataloader.AddVariable(feature)
#dataloader.SetWeightExpression('w')

# Define splits and cuts
#split_train = '(Entry$ % 3) < 2'
#split_test = '(Entry$ % 3) == 2'

higgs_mass_window = 'HCandMass > 100 && HCandMass < 170'
nan_remove = ' && '.join(['!TMath::IsNaN({})'.format(var) for var in features])

cut_signal = ' && '.join((higgs_mass_window, nan_remove))
cut_background = ' && '.join((higgs_mass_window, nan_remove))

# Load data
s, b = load_data()
add_files_to_dataloader(s, b, dataloader)
dataloader.PrepareTrainingAndTestTree(cut_signal,
                                      cut_background,
                                      'nTrain_Signal=20000:nTrain_Background=20000:'
                                      'nTest_Signal=5000:nTest_Background=5000:'
                                      'SplitMode=Alternate:MixMode=Random:NormMode=NumEvents:!V')

# Construct loss function and optimizer
loss = torch.nn.BCEWithLogitsLoss()
optimizer = torch.optim.Adam

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

vars_for_transform = ','.join([f'_V{i}_' for i in range(2, len(features))])
torch_method_VarTransform_string = 'D+G({0})'.format(vars_for_transform)
factory.BookMethod(dataloader, TMVA.Types.kPyTorch, 'PyTorch',
                   'H:!V:VarTransform=' + torch_method_VarTransform_string +
                   ':FilenameModel=modelClassification.pt:'
                   'FilenameTrainedModel=trainedModelClassification.pt:NumEpochs=40:BatchSize=64') 

# Train, test, and evaluate
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods() 

# Convert model to onnx file
model = torch.jit.load("trainedModelClassification.pt")
xinput = torch.zeros((1,len(features)-2))   # define input shape (assume batch size = 1)
torch.onnx.export(model,xinput,"trainedModelClassification.onnx",export_params=True)

# Plot ROC Curves
roc = factory.GetROCCurve(dataloader)
roc.SaveAs('ROC_ClassificationPyTorch.png')
