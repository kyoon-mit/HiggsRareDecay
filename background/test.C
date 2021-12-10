// ROOT macro for background fitting
// Lite version

#include <iostream>
#include <string>

#include "TFile.h"
#include "TH1D.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "BackgroundPDFModels.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooDataHist.h"

#include "RooFit.h"

using namespace RHD;
using namespace RooFit;

void fit_01()
{
    // Create background histograms from file
    Float_t xlow = 40., xhigh = 200.;
    Int_t nbins = (Int_t) xhigh - xlow;
    auto bkg_comb = TH1D("bkg_comb", "Z/W + H bkg combined", nbins, xlow, xhigh);

    for (int i=0; i<5; i++) {
        // Read file
        char str[100];
        snprintf(str, sizeof(str),
                 "/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc%d_Wcat.root", i);
        auto filename = std::string(str);
        auto file = TFile(filename.c_str(), "READ");
    
        // Save HCandMass values into bkg_comb histogram
        TTreeReader reader("events", &file);
        TTreeReaderValue<Float_t> HCandMass(reader, "HCandMass");
        while (reader.Next()) {
            bkg_comb.Fill(*HCandMass);
        }
    }

    // Key variable
    RooRealVar mH("mH", "mH", xlow, xhigh, "GeV");

    // Make model(s)
    auto m = BackgroundPDFModels();
    RooAbsPdf* gauss = m.makeGaussian(mH);
    RooAbsPdf* lau3 = m.makeLaurentSeries(mH, 3);
    RooAbsPdf* lau3_X_gauss = m.makeLaurentConvGaussian(mH, 3);
    //m.makeLaurentConvGaussian(mH, 1, 1);
    //auto gauss = m.getPDF("Gaussian");
    //RooAbsPdf* lau1_X_gauss = m.getPDF("lau1_X_gauss");
    //RooAbsPdf* lau2_X_gauss = m.getPDF("lau2_X_gauss");
    //RooAbsPdf* lau3_X_gauss = m.getPDF("lau3_X_gauss");
    //RooAbsPdf* lau4_X_gauss = m.getPDF("lau4_X_gauss");
    //std::shared_ptr<RooAbsPdf> lau5_X_gauss = m.getPDF("lau5_X_gauss");

    // Perform fit
    RooDataHist datahist("bkg_comb", "bkg_comb", RooArgList(mH), &bkg_comb);

    gauss->chi2FitTo(datahist,
                            RooFit::Minimizer("Minuit2", "minimize"));
    lau3->chi2FitTo(datahist, RooFit::Minimizer("Minuit2", "minimize"));
    lau3_X_gauss->chi2FitTo(datahist, RooFit::Minimizer("Minuit2", "minimize"));

    delete lau3;
//lau2_X_gauss->chi2FitTo(datahist,
//                            RooFit::Minimizer("Minuit2", "minimize"));
//lau3_X_gauss->chi2FitTo(datahist,
//                            RooFit::Minimizer("Minuit2", "minimize"));
//lau4_X_gauss->chi2FitTo(datahist,
//                            RooFit::Minimizer("Minuit2", "minimize"));
//    lau5_X_gauss->chi2FitTo(datahist,
//                            RooFit::Minimizer("Minuit2", "minimize"));
}
