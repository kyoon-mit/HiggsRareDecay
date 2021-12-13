"""Module to fit signal data.

Loads histogram from a ROOT file. Fits Gauss conv. w. Gauss (or three Gauss')
to the histogram data with RooFit. Saves result to a ROOT file and saves plot
to a JPEG file.

    Typical usage example:
    
    In Bash command line,
        >> python3 signal_fitting.py
"""

import os
from ROOT import *

ROOT.EnableImplicitMT()
gROOT.SetBatch(True)

def fetch_histogram (file_name, hist_name, hist_title,
                     nbins=100, xlow=100., xhigh=150.):
    """Fetches histogram from a ROOT file.

    Retrieves the value of the HCandMass variable in the events TTree of
    a given ROOT file.

    Args:
        file_name: Name of ROOT input file.
        hist_name: Name of the histogram.
        hist_title: Title of the histogram.
        nbins: Number of bins in histogram.
        xlow: Lower range of histogram.
        xhigh: Upper range of histogram.

    Returns:
        A TH1D histogram containing the values of the HCandMass variable.
    """

    rootfile = TFile(file_name, 'READ')
    hist = TH1D(hist_name, hist_title, nbins, xlow, xhigh)

    for ev in rootfile.events:
        hist.Fill(ev.HCandMass)

    hist.SetDirectory(0)

    return hist


def fit (histogram, process_name, outfile_name):
    """Fits histogram to various models.

    Creates a RooWorkspace containing signal models and RooDataHist. For each model,
    both chi-squared and MLE methods are performed for the fit. The models are:
        Gaussian
        (Non-Relativistic) Breit-Wigner, i.e. Lorentzian or Cauchy
        Voigtian
        CrystalBall

    The RooWorkspace and RooFit results are saved in a ROOT file.

    Args:
        histogram: TH1 histogram containing experimental (or simulated) data.
        process_name: Name of the physics process for identification purposes.
        outfile_name: Name of the ROOT output file.

    Returns:
        (None)
    """
    # Create outfile
    outfile = TFile(outfile_name, 'RECREATE')

    # Create workspace
    w = RooWorkspace('w')

    # Create signal models
    w.factory('Gaussian::g1(mass[100,150],mean1[115,135],sigma1[0.01,30])')
    w.factory('Gaussian::g2(mass,mean2[115,135],sigma2[0.01,30])')
    w.factory('Gaussian::g3(mass,mean3[115,135],sigma3[0.01,30])')
    
    w.factory('SUM::two_gauss_chi2(g2frac[0,1]*g2, g1)')
    w.factory('SUM::three_gauss_chi2(g3frac[0,1]*g3, two_gauss_chi2)')
    w.factory('SUM::two_gauss_mle(g2frac*g2, g1)')
    w.factory('SUM::three_gauss_mle(g3frac*g3, two_gauss_mle)')

    # Create data histogram
    data_hist = RooDataHist('data_' + process_name, process_name,
                            RooArgList(w.var('mass')), histogram)
    w.Import(data_hist)
    
    # Fit signal models & save snapshots
    list_of_models_chi2 = ['two_gauss_chi2',
                           'three_gauss_chi2']
    list_of_models_mle  = ['two_gauss_mle',
                           'three_gauss_mle']
    
    for model in list_of_models_chi2:
        res = w.pdf(model).chi2FitTo(data_hist, RooFit.Save())
        params = w.pdf(model).getParameters(RooArgSet(w.var('mass')))
        w.saveSnapshot('snapshot_' + model, params)
        res.Write('res_' + model)

    for model in list_of_models_mle:
        res = w.pdf(model).fitTo(data_hist, RooFit.Save())
        params = w.pdf(model).getParameters(RooArgSet(w.var('mass')))
        w.saveSnapshot('snapshot_' + model, params)
        res.Write('res_' + model)

    w.writeToFile(outfile_name)
    outfile.Close()
    
    return


def plot_fit_results (workspace, data_name, snapshot_name, model_name, outfile_name):
    """Plots fit results created by the fit function.

    Creates TCanvas and split into two pads. In the first pad, the fitted model
    is plotted on top of the data. In the second pad, the residuals are plotted.
    The resulting TCanvas is saved to the given file path.

    Args:
        workspace: RooWorkspace containing the model
        data_name: Name of the data in workspace
        snapshot_name: Name of the snapshot to load
        model_name: Name of the model to load
        outfile_name: Name of the outfile to write to

    Returns:
        (None)
    """
    # Load snapshot
    workspace.loadSnapshot(snapshot_name)
    
    # Fit histogram
    frame1 = workspace.var('mass').frame(RooFit.Title('Fit of \"{}\"'.format(model_name)))
    workspace.data(data_name).plotOn(frame1)
    workspace.pdf(model_name).plotOn(frame1, RooFit.LineColor(kRed),
                                     RooFit.Name('fit'))
    workspace.pdf(model_name).paramOn(frame1, RooFit.Layout(0.6, 0.88, 0.85))
    frame1.getAttText().SetTextFont(43)
    frame1.getAttText().SetTextSize(21)
    
    # Residual histogram
    hresid = frame1.residHist()
    frame2 = workspace.var('mass').frame(RooFit.Title('Residuals of \"{}\"'.format(model_name)))
    frame2.addPlotable(hresid, 'P')

    # Draw on canvas
    gStyle.SetOptStat(0)
    c = TCanvas('c', 'c')
    c.SetCanvasSize(1500, 1500)

    p1 = TPad('p1', 'p1', 0.05, 0.41, 0.95, 0.95)
    p2 = TPad('p2', 'p2', 0.05, 0.05, 0.95, 0.39)

    p1.cd()
    frame1.Draw()
    p1.Update()
    
    p2.cd()
    frame2.Draw()
    p2.Update()

    c.cd()
    p1.Draw()
    p2.Draw()
    c.Update()
    c.Print(outfile_name)
    
    return
    
    
def signal_fitting ():
    """Fit models to signals.
    """
    # Filenames
    filename_ZH = '/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc10_Wcat.root'
    filename_WH = '/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc11_Wcat.root'
    
    # Get histograms
    hist_ZH = fetch_histogram(filename_ZH, 'ZH_HCandMass', 'HCandMass from mc10 file')
    hist_WH = fetch_histogram(filename_WH, 'WH_HCandMass', 'HCandMass from mc11 file')
    hist_combined = hist_ZH + hist_WH

    # Perform fitting
    fit(hist_ZH, 'ZH', 'ZH_fit.root')
    fit(hist_WH, 'WH', 'WH_fit.root')
    fit(hist_combined, 'ZH_WH_combined', 'ZH_WH_combined_fit.root')
    
    for process in ['ZH', 'WH', 'ZH_WH_combined']:
        
        # Fetch workspace
        f = TFile(process + '_fit.root', 'READ')
        w = f.w

        # Plot    
        output_dir = os.path.join(os.getcwd(), 'signal_fitting', process)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        models = ['two_gauss_chi2', 'three_gauss_chi2']
        models += ['two_gauss_mle', 'three_gauss_mle']

        for m in models:
            savefile = os.path.join(output_dir, m + '.jpg')
            plot_fit_results(w, 'data_' + process, 'snapshot_' + m, m, savefile)
    
    return


if __name__ == '__main__':
    signal_fitting()
