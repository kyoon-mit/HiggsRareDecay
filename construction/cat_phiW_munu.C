#include "TString.h"
#include "TFile.h"
#include "TH1F.h"

#include "FitData.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooDataSet.h"
#include "RooDataHist.h"

#include "RooFit.h"

using namespace RHD;
using namespace RooFit;

void cat_phiW_munu()
{
    auto fitting = FitData("cat_phiW_munu.root", "../models", "w");
    const char* DIR_DATA_PREFIX
        = "/home/submit/kyoon/CMSSW_10_2_13/src/HiggsRareDecay/data/cat_phi/cat_phiW_munu";

    /***** SIGNAL FITTING *****/
    // Create observable variable
    double xlow = 100., xhigh = 150.;
    int nbins = (int) 2*(xhigh - xlow); // see if this has effect on signal fitting
    RooRealVar mH("HCandMass", "HCandMass", xlow, xhigh, "GeV");
    mH.setBins(nbins);

    // Create signal histogram & dataset
    auto fn_signal = Form("%s/cat_phiW_munu_signal.root", DIR_DATA_PREFIX);
    TH1F signal = fitting.fetchHistogram({(const char*) fn_signal}, "events", "HCandMass",
                                         "cat_phiW_munu_signal",
                                         "cat_phiW_munu signal",
                                         nbins, xlow, xhigh);
    RooDataHist data_signal = fitting.makeBinnedData("data_signal_cat_phiW_munu", mH, signal);

    // Fit to signal
    fitting.performSignalFit<RooDataHist>(mH, data_signal);

    /***** BACKGROUND FITTING *****/
    // Make background histogram
    auto fn_DYjets     = (const char*) Form("%s/cat_phiW_munu_DYjets.root", DIR_DATA_PREFIX);
    auto fn_Wjets      = (const char*) Form("%s/cat_phiW_munu_Wjets.root", DIR_DATA_PREFIX);
    auto fn_Wgamma     = (const char*) Form("%s/cat_phiW_munu_Wgamma.root", DIR_DATA_PREFIX);
    auto fn_Zgamma     = (const char*) Form("%s/cat_phiW_munu_Zgamma.root", DIR_DATA_PREFIX);
    auto fn_ttbar_llvv = (const char*) Form("%s/cat_phiW_munu_ttbar_llvv.root", DIR_DATA_PREFIX);

    const std::vector<const char*> fn_bkg {fn_DYjets, fn_Wjets, fn_Wgamma, fn_Zgamma, fn_ttbar_llvv};

    xlow = 65., xhigh = 200.;
    nbins = (int) 2*(xhigh - xlow);
    TH1F bkg_comb = fitting.fetchHistogram(fn_bkg, "events", "HCandMass",
                                           "cat_phiW_munu_bkg_comb",
                                           "cat_phiW_munu combined background",
                                           nbins, xlow, xhigh);

    // Create background dataset
    mH.setRange(xlow, xhigh);
    RooDataHist data_bkg_comb = fitting.makeBinnedData("data_bkg_comb_cat_phiW_munu", mH, bkg_comb);
    
    fitting.performMultiLikelihoodFit("bern", mH, data_bkg_comb);
    fitting.performMultiLikelihoodFit("lau", mH, data_bkg_comb);
    fitting.performMultiLikelihoodFit("pow", mH, data_bkg_comb);
    fitting.performMultiLikelihoodFit("exp", mH, data_bkg_comb);

    // Create toy dataset
    fitting.saveToyData(mH, "bern3", "trigauss", .5, 1e+6);
    
    
    // Plot PDFs
    /*
    fitting.plotMultiplePDFs(mH, data_bkg_comb, "bern",
                             {"bern2_X_gauss", "bern3_X_gauss",
                              "bern4_X_gauss"},
                             {kGreen+2, kBlue, kViolet+7});
    fitting.plotMultiplePDFs(mH, data_bkg_comb, "pow_conv_gauss",
                             {"pow2_X_gauss", "pow3_X_gauss",
                              "pow4_X_gauss", "pow5_X_gauss",
                              "pow6_X_gauss"},
                             {kGreen+2, kBlue, kViolet+7, kMagenta+1, kRed});
    fitting.plotMultiplePDFs(mH, data_bkg_comb, "laurent_conv_gauss",
                             {"lau3_X_gauss", "lau4_X_gauss",
                              "lau5_X_gauss", "lau6_X_gauss",
                              "lau7_X_gauss"},
                             {kGreen+2, kBlue, kViolet+7, kMagenta+1, kRed});
    fitting.plotMultiplePDFs(mH, data_bkg_comb, "exp_conv_gauss",
                             {"exp1_X_gauss", "exp2_X_gauss",
                              "exp3_X_gauss", "exp4_X_gauss",
                              "exp5_X_gauss"},
                             {kGreen+2, kBlue, kViolet+7, kMagenta+1, kRed});
    */
}
