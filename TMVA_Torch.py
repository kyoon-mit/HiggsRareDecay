from ROOT import TMVA, TFile, TTree, TCut
from subprocess import call
from os.path import isfile

import torch
from torch import nn

# Setup TMVA
TMVA.Tools.Instance()
TMVA.PyMethodBase.PyInitialize()

outfile = 'torch_test.root'
output = TFile.Open(outfile, 'RECREATE')

factory = TMVA.Factory('TMVAClassification',
                       output,
                      '!V:!Silent:Color:DrawProgressBar:Transformations=G:AnalysisType=Classification')

# Load data
def load_data(cat='ggH', meson='rho', year=[2018], sgn_mc=[1027], bkg_mc=[6,7,8,9,10]): # TODO: move to utils module
    """
    Helper function to load data into TMVA.
    
    [Arguments]
    cat: (string) name of the category, e.g. 'ggH', 'VBF',  'VBFlow', 'Zinv', etc.
    meson: (string) name of the meson produced, e.g. 'phi', 'rho'
    year: (list) year of the data/MC taken/produced
    sgn_mc: (list) MC number(s) of the signal process; - for data
    bkg_mc: (list) MC number(s) of the background process; - for data

    [Returns]
    (list of signal TFiles, list of background TFiles)
    """

    top_level_dir = '/work/submit/kyoon/RareHiggs/data/'
    file_format = top_level_dir + 'cat_{meson}/cat_{meson}_{cat}/test/test_mc{mc}_{cat}cat_{meson}cat_{yr}.root'

    signal_files = []
    background_files = []
    
    for yr_ in year:
        for mc_ in sgn_mc:
            f = TFile.Open(file_format.format(**{'yr':yr_, 'mc':mc_, 'cat':cat, 'meson':meson}), 'READ')
            #f.SetDirectory(0)
            signal_files.append(f)
        for mc_ in bkg_mc:
            f = TFile.Open(file_format.format(**{'yr':yr_, 'mc':mc_, 'cat':cat, 'meson':meson}), 'READ')
            #f.SetDirectory(0)
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
        
s, b = load_data()
features = []

dataloader = TMVA.DataLoader('Dataset')
for feature in features:
    dataloader.AddVariable(feature)
    
add_files_to_dataloader(s, b, dataloader)

