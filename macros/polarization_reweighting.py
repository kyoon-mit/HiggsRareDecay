from ROOT import *
from os import path, listdir
import pandas as pd

ROOT.EnableImplicitMT()

functions_dir = '/work/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis'
if path.join(functions_dir, 'functions.so') not in gSystem.GetLibraries():
    gSystem.CompileMacro(path.join(functions_dir, 'functions.cc'), 'k')

def inspect_file (fileNames, meson='rho'):
    """Function for inspecting contents of a file.

    Args:
        fileNames (list(str)): Names of the ROOT file.
    """
    chain = TChain('Events')
    for fileName in fileNames:
        chain.Add(fileName)

    df = RDataFrame(chain)

    dfNew = (df.Define('GenPhiPolarizationAngle', 'getPhiPolarizationAngle(GenPart_pdgId,\
                                                   GenPart_genPartIdxMother,\
                                                   GenPart_pt, GenPart_eta, GenPart_phi, GenPart_mass)')
               .Define('GenPhiCosinePolarizationAngle', 'TMath::Cos(GenPhiPolarizationAngle)')
            )
    hist1 = dfNew.Histo1D('GenPhiPolarizationAngle')
    hist2 = dfNew.Histo1D('GenPhiCosinePolarizationAngle')
    c1 = TCanvas()
    hist1.Draw()
    c1.SaveAs('test_polarization.png')
    c1.Close()
    c2 = TCanvas()
    hist2.Draw()
    c2.SaveAs('test_cosine_polarization.png')
    c2.Close()

if __name__=='__main__':
    dirName = '/data/submit/cms/store/user/mariadlf/nano/D01/'
    folderName = 'GluGlu_HToPhiGamma_M125_TuneCP5_PSWeights_13TeV_powheg_pythia8+RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3+MINIAODSIM'
    fileNames = []
    for fileName in listdir(path.join(dirName, folderName)):
        if fileName.endswith('.root'):
            fileNames.append(path.join(dirName, folderName, fileName))
    inspect_file(fileNames)