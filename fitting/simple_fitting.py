from ROOT import *

ROOT.EnableImplicitMT()
gROOT.SetBatch(True)

def fetch_histogram (file_name, hist_name, hist_title,
                     nbins=50, xlow=100., xhigh=150.):
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
    w = RooWorkspace('workspace_' + process_name)

    # Create signal models
    w.factory('Gaussian::gauss_chi2(mass[100,150],mean_gc[115,135],sigma_gc[0.01,20])')
    w.factory('Gaussian::gauss_mle(mass[100,150],mean_gm[115,135],sigma_gm[0.01,20])')
    w.factory('BreitWigner::breitwigner_chi2'
              '(mass[100,150],mean_bc[115,135],width_bc[0.01,20])')
    w.factory('BreitWigner::breitwigner_mle'
              '(mass[100,150],mean_bm[115,135],width_bm[0.01,20])')
    w.factory('Voigtian::voigtian_chi2'
              '(mass[100,150],mean_vc[115,135],sigma_vc[0.01,20],width_vc[0.01,20])')
    w.factory('Voigtian::voigtian_mle'
              '(mass[100,150],mean_vm[115,135],sigma_vm[0.01,20],width_vm[0.01,20])')
    w.factory('CrystalBall::crystalball_chi2'
              '(mass[100,150],x0_cc[115,135],sigmaL_cc[0.01,20],sigmaR_cc[0.01,20],'
              'alphaL_cc[0,10],nL_cc[0,10],alphaR_cc[0,10],nR_cc[0,10])')
    w.factory('CrystalBall::crystalball_mle'
              '(mass[100,150],x0_cm[115,135],sigmaL_cm[0.01,20],sigmaR_cm[0.01,20],'
              'alphaL_cm[0,10],nL_cm[0,10],alphaR_cm[0,10],nR_cm[0,10])')

    # Create data histogram
    data_hist = RooDataHist('data_' + process_name, process_name,
                            RooArgList(w.var('mass')), histogram)
    w.Import(data_hist)
    
    # Fit signal models & save snapshots
    list_of_models_chi2 = ['gauss_chi2',
                           'breitwigner_chi2',
                           'voigtian_chi2',
                           'crystalball_chi2']
    list_of_models_mle  = ['gauss_mle',
                           'breitwigner_mle',
                           'voigtian_mle',
                           'crystalball_mle']
    
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
    
    # res_gauss_chi2       = w.pdf('gauss_chi2').chi2FitTo(data_hist, RooFit.Save())
    # res_breitwigner_chi2 = w.pdf('breitwigner_chi2').chi2FitTo(data_hist, RooFit.Save())
    # res_voigtian_chi2    = w.pdf('voigtian_chi2').chi2FitTo(data_hist, RooFit.Save())
    # res_crystalball_chi2 = w.pdf('crystalball_chi2').chi2FitTo(data_hist, RooFit.Save())
    # res_gauss_mle        = w.pdf('gauss_mle').fitTo(data_hist, RooFit.Save())
    # res_breitwigner_mle  = w.pdf('breitwigner_mle').fitTo(data_hist, RooFit.Save())
    # res_voigtian_mle     = w.pdf('voigtian_mle').fitTo(data_hist, RooFit.Save())
    # res_crystalball_mle  = w.pdf('crystalball_mle').fitTo(data_hist, RooFit.Save())

    w.writeToFile(outfile_name)
    outfile.Close()
    
    return


def test ():
    """Test functionality.
    """
    filename = '/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc10_Wcat.root'
    h = fetch_histogram(filename, 'ZH_HCandMass', 'HCandMass from mc10 file')
    fit(h, 'ZH', 'test_ZH_fit.root')

    f = TFile('test_ZH_fit.root', 'READ')
    w = f.workspace_ZH

    models = ['gauss_chi2', 'breitwigner_chi2', 'voigtian_chi2', 'crystalball_chi2']
    models += ['gauss_mle', 'breitwigner_mle', 'voigtian_mle', 'crystalball_mle']

    for m in models:
        plot_fit_results(w, 'data_ZH', 'snapshot_' + m, m, m + '.jpg')
    
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


if __name__ == '__main__':
    test()
