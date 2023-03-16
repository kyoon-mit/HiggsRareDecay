from ROOT import *
import sys

if __name__=='__main__':

    Meson = sys.argv[1]

    # Files
    Torino_presel_filename = '/work/submit/mariadlf/cards_march9/230309_{0}GammaLimit/workspace_STAT_{0}_GFpreselection_2018.root'.format(Meson)
    MIT_presel_filename = '/work/submit/mariadlf/cards_march9/DATACARDS_MARCH9/workspace_STAT_{0}_GFcat_2018.root'.format(Meson)

    Torino_presel = TFile.Open(Torino_presel_filename)
    MIT_presel = TFile.Open(MIT_presel_filename)

    # Workspaces
    Torino_workspace_name = 'workspace_STAT_{}_GFpreselection_2018'.format(Meson)
    MIT_workspace_name = 'w'

    Torino_workspace = Torino_presel.Get(Torino_workspace_name)
    MIT_workspace = MIT_presel.Get(MIT_workspace_name)

    # Histograms
    # Torino_ggH_signal = Torino_workspace.data('dataset_ggH').createHistogram('mesonGammaMass')
    # MIT_ggH_signal = MIT_workspace

    # Torino_VBF_signal = Torino_workspace.data('dataset_VBF').createHistogram('mesonGammaMass')
    # MIT_VBF_signal = MIT_workspace

    Torino_data_bkg = Torino_workspace.data('observed_data').Torino_data_bkg.createHistogram('mesonGammaMass')
    MIT_data_bkg = MIT_workspace.data('observed_data').createHistogram('mh')

    # Draw & Save
    c_data = TCanvas()
    Torino_data_bkg.SetMarkerStyle(kFullCircle)
    Torino_data_bkg.SetMarkerColor(kBlue)
    MIT_data_bkg.SetMarkerStyle(kFullSquare)
    MIT_data_bkg.SetMarkerColor(kRed)
    Torino_data_bkg.Draw('E P0')
    MIT_data_bkg.Draw('E P0 SAME')
    c_data.SaveAs('observed_data_compare.png')