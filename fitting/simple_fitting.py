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
                            RooArgList(w.var('mass')), histogram, 1.0)
    
    # Fit signal models
    res_gauss_chi2       = w.pdf('gauss_chi2').chi2FitTo(data_hist, RooFit.Save())
    res_breitwigner_chi2 = w.pdf('breitwigner_chi2').chi2FitTo(data_hist, RooFit.Save())
    res_voigtian_chi2    = w.pdf('voigtian_chi2').chi2FitTo(data_hist, RooFit.Save())
    res_crystalball_chi2 = w.pdf('crystalball_chi2').chi2FitTo(data_hist, RooFit.Save())
    res_gauss_mle        = w.pdf('gauss_mle').fitTo(data_hist, RooFit.Save())
    res_breitwigner_mle  = w.pdf('breitwigner_mle').fitTo(data_hist, RooFit.Save())
    res_voigtian_mle     = w.pdf('voigtian_mle').fitTo(data_hist, RooFit.Save())
    res_crystalball_mle  = w.pdf('crystalball_mle').fitTo(data_hist, RooFit.Save())

    # Save fit results
    outfile = TFile(outfile_name, 'RECREATE')
    res_gauss_chi2.Write('res_gauss_chi2')
    res_breitwigner_chi2.Write('res_breitwigner_chi2')
    res_voigtian_chi2.Write('res_voigtian_chi2')
    res_crystalball_chi2.Write('res_crystalball_chi2')
    res_gauss_mle.Write('res_gauss_mle')
    res_breitwigner_mle.Write('res_breitwigner_mle')
    res_voigtian_mle.Write('res_voigtian_mle')
    res_crystalball_mle.Write('res_crystalball_mle')
    outfile.Close()

    return


def test():
    """Test functionality.
    """
    filename = '/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc10_Wcat.root'
    h = fetch_histogram(filename, 'ZH_HCandMass', 'HCandMass from mc10 file')
    fit(h, 'ZH', 'test_ZH_fit.root')

    return


def plot_fit_results():
    pass


if __name__ == '__main__':
    test()
