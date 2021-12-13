// ROOT macro for background fitting
// Lite version

#include <iostream>
#include <string>

#include "TFile.h"
#include "TH1D.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "BackgroundPDFModels.h"
#include "BackgroundFitting.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooDataHist.h"

#include "RooFit.h"

using namespace RHD;
using namespace RooFit;

void test()
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
    RooAbsPdf* lau1_X_gauss = m.makeLaurentConvGaussian(mH, 1);
    RooAbsPdf* lau2_X_gauss = m.makeLaurentConvGaussian(mH, 2);
    RooAbsPdf* lau3_X_gauss = m.makeLaurentConvGaussian(mH, 3);
    RooAbsPdf* lau4_X_gauss = m.makeLaurentConvGaussian(mH, 4);
    RooAbsPdf* lau5_X_gauss = m.makeLaurentConvGaussian(mH, 5);

    // Perform fit
    auto f = BackgroundFitting();
    auto datahist = f.makeBinnedData("bkg_comb", mH, bkg_comb);
    RooFitResult fitresult = f.performChi2Fit(lau3_X_gauss, &datahist, 5);

    std::cout << fitresult.status() << std::endl;
    fitresult.Print();

    //gauss->chi2FitTo(datahist,
    //                        RooFit::Minimizer("Minuit2", "minimize"));
    //lau3->chi2FitTo(datahist, RooFit::Minimizer("Minuit2", "minimize"));
    //lau3_X_gauss->chi2FitTo(datahist, RooFit::Minimizer("Minuit2", "minimize"));

//lau2_X_gauss->chi2FitTo(datahist,
//                            RooFit::Minimizer("Minuit2", "minimize"));
//lau3_X_gauss->chi2FitTo(datahist,
//                            RooFit::Minimizer("Minuit2", "minimize"));
//lau4_X_gauss->chi2FitTo(datahist,
//                            RooFit::Minimizer("Minuit2", "minimize"));
//    lau5_X_gauss->chi2FitTo(datahist,
//                            RooFit::Minimizer("Minuit2", "minimize"));
}
