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
    // Open signal file
    auto fn_signal = Form("%s/cat_phiW_munu_signal.root", DIR_DATA_PREFIX);
    TFile * file = TFile::Open(fn_signal, "READ");
    auto tree = (TTree*) file->Get("events");

    // Create observable variable
    double xlow = 100., xhigh = 150.;
    int nbins = (int) 2*(xhigh - xlow);
    RooRealVar mH("HCandMass", "HCandMass", xlow, xhigh, "GeV");
    mH.setBins(nbins);

    // Create signal dataset
    RooDataSet data_signal = fitting.makeUnbinnedData("data_signal_cat_phiW_munu", mH, tree);

    // Fit to signal
    fitting.performSignalFit<RooDataSet>(mH, data_signal);

    /***** BACKGROUND FITTING *****/
    // Make background histogram
    auto fn_DYjets     = (const char*) Form("%s/cat_phiW_munu_DYjets.root", DIR_DATA_PREFIX);
    auto fn_Wjets      = (const char*) Form("%s/cat_phiW_munu_Wjets.root", DIR_DATA_PREFIX);
    auto fn_Wgamma     = (const char*) Form("%s/cat_phiW_munu_Wgamma.root", DIR_DATA_PREFIX);
    auto fn_Zgamma     = (const char*) Form("%s/cat_phiW_munu_Zgamma.root", DIR_DATA_PREFIX);
    auto fn_ttbar_llvv = (const char*) Form("%s/cat_phiW_munu_ttbar_llvv.root", DIR_DATA_PREFIX);

    const std::vector<const char*> fn_bkg {fn_DYjets, fn_Wjets, fn_Wgamma, fn_Zgamma, fn_ttbar_llvv};

    xlow = 40., xhigh = 200.;
    nbins = (int) 2*(xhigh - xlow);
    TH1F bkg_comb = fitting.fetchHistogram(fn_bkg, "events", "HCandMass",
                                           "cat_phiW_munu_bkg_comb",
                                           "cat_phiW_munu combined background",
                                           nbins, xlow, xhigh);

    // Create background dataset
    mH.setRange(xlow, xhigh);
    RooDataHist data_bkg_comb = fitting.makeBinnedData("data_bkg_comb_cat_phiW_munu", mH, bkg_comb);
    
    fitting.performMultiLikelihoodFit("bernXgauss", mH, data_bkg_comb,
                                      {.61, .31, .24, .037, .047});
    fitting.performMultiLikelihoodFit("lauXgauss", mH, data_bkg_comb);
    fitting.performMultiLikelihoodFit("powXgauss", mH, data_bkg_comb,
                                      {1, .7},
                                      {-1.7, -.1});
    fitting.performMultiLikelihoodFit("expXgauss", mH, data_bkg_comb,
                                      {.42, .10, .61, 1.},
                                      {-.02, -.35, -.47, -.4});


    // Plot PDFs
    fitting.plotMultiplePDFs(mH, data_bkg_comb, "bern_conv_gauss_extend",
                             {"bern2_X_gauss_extend", "bern3_X_gauss_extend",
                              "bern4_X_gauss_extend"},
                             {kGreen+2, kBlue, kViolet+7});
    fitting.plotMultiplePDFs(mH, data_bkg_comb, "pow_conv_gauss_extend",
                             {"pow2_X_gauss", "pow3_X_gauss"},
                             {kBlue, kRed});
    fitting.plotMultiplePDFs(mH, data_bkg_comb, "laurent_conv_gauss_extend",
                             {"lau3_X_gauss_extend", "lau4_X_gauss_extend",
                              "lau5_X_gauss_extend", "lau6_X_gauss_extend",
                              "lau7_X_gauss_extend"},
                             {kGreen+2, kBlue, kViolet+7, kMagenta+1, kRed});
    fitting.plotMultiplePDFs(mH, data_bkg_comb, "exp_conv_gauss_extend",
                             {"exp1_X_gauss_extend", "exp2_X_gauss_extend",
                              "exp3_X_gauss_extend", "exp4_X_gauss_extend",
                              "exp5_X_gauss_extend"},
                             {kGreen+2, kBlue, kViolet+7, kMagenta+1, kRed});
}
