// ROOT macro for background fitting
// Lite version

#include <iostream>
#include <string>

#include "TFile.h"
#include "TH1D.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "PDFModels.h"
#include "FitData.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooDataHist.h"

#include "RooFit.h"

using namespace RHD;
using namespace RooFit;

void test()
{
    // Create signal histogram from file
    auto f = FitData();
    TH1F signal = f.fetchHistogram ({"/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc10_Wcat.root",
                                     "/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc11_Wcat.root"},
                                    "events", "HCandMass", "signal", "signal WH and ZH",
                                    100, 100., 150.);

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
    auto m = PDFModels();
    RooAbsPdf* gauss = m.makeGaussian(mH);
    RooAbsPdf* trigauss = m.makeTripleGaussian(mH);
    RooAbsPdf* bern3 = m.makeBernsteinPoly(mH, 3);
    RooAbsPdf* pow3 = m.makePowerSeries(mH, 3);
    RooAbsPdf* lau1_X_gauss = m.makeLaurentConvGaussian(mH, 1);
    RooAbsPdf* lau2_X_gauss = m.makeLaurentConvGaussian(mH, 2);
    RooAbsPdf* lau3_X_gauss = m.makeLaurentConvGaussian(mH, 3);
    RooAbsPdf* lau4_X_gauss = m.makeLaurentConvGaussian(mH, 4);
    RooAbsPdf* lau5_X_gauss = m.makeLaurentConvGaussian(mH, 5);

    auto bkgdata = f.makeBinnedData("bkg_comb", mH, bkg_comb);
    auto fres = f.performChi2Fit(pow3, &bkgdata, 5);

    /*
    RooWorkspace w("w");

    auto signaldata = f.makeBinnedData("signal", mH, signal);
    RooFitResult sfit = f.performChi2Fit(trigauss, &signaldata, 5);

    w.import(*trigauss);
    w.import(signaldata);
    w.saveSnapshot("sig_values", *trigauss->getParameters(signaldata), true);

    auto bkgdata = f.makeBinnedData("bkg_comb", mH, bkg_comb);
    RooFitResult fitresult = f.performChi2Fit(lau3_X_gauss, &bkgdata, 5);

    w.import(*lau3_X_gauss);
    w.import(bkgdata);
    w.saveSnapshot("bkg_values", *lau3_X_gauss->getParameters(bkgdata), true);

    w.Print();

    w.writeToFile("test_workspace.root");
    */
}
