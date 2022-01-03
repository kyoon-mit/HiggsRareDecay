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

void test_multipdf()
{
    auto fitting = FitData();
    fitting.setSaveOption(true);

    const char* file_format = "/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc%d_Wcat.root";

    double xlow = 40., xhigh = 200.;
    int nbins = (int) xhigh - xlow;

    // Create signal histogram from file
    const std::vector<const char*> filenames_WH_ZH {Form(file_format, 10),
                                                    Form(file_format, 11)};

    TH1F signal = fitting.fetchHistogram (filenames_WH_ZH,
                                          "events", "HCandMass", "signal", "signal WH and ZH",
                                          nbins*2, xlow, xhigh);

    // Create background histograms from file
    const std::vector<const char*> filenames_ZW_H {Form(file_format, 0),
                                                   Form(file_format, 1),
                                                   Form(file_format, 2),
                                                   Form(file_format, 3),
                                                   Form(file_format, 4)};

    TH1F bkg_comb = fitting.fetchHistogram (filenames_ZW_H,
                                            "events", "HCandMass", "bkg_comb", "Z/W + H bkg combined",
                                            nbins, xlow, xhigh);

    // Key variable
    // RooRealVar mH_sgn("mH_sgn", "mH_sgn", xlow, xhigh, "GeV");
    RooRealVar mH("mH", "mH", xlow, xhigh, "GeV");

    // Fit to signal
    // auto sgndata = fitting.makeBinnedData("signal", mH_sgn, signal);
    RooDataHist sgndata("signal", "signal", mH, &signal);
    fitting.performSignalFit(&mH, &sgndata);

    // Fit to background
    auto bkgdata = fitting.makeBinnedData("bkg_comb", mH, bkg_comb);
    fitting.performMultiLikelihoodFit("bernXgauss", &mH, &bkgdata, {.61, .31, .24, .037, .047});
    fitting.performMultiLikelihoodFit("lauXgauss", &mH, &bkgdata);
    fitting.performMultiLikelihoodFit("powXgauss", &mH, &bkgdata, {}, {}, 100);

    // Save multipdf
    fitting.saveMultiPdf({"bern7_X_gauss", "lau4_X_gauss", "pow4_X_gauss"}, "bkgmultipdf");
}
