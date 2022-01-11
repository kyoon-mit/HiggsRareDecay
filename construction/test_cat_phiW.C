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
#include "RooDataSet.h"
#include "RooDataHist.h"

#include "RooFit.h"

using namespace RHD;
using namespace RooFit;

void test_cat_phiW()
{
    auto fitting = FitData("test_cat_phiW_sgn.root", "./", "w");

    // Open file
    const char* file_name = "/home/submit/kyoon/CMSSW_10_2_13/src/HiggsRareDecay/"
                            "data/cat_phi/cat_phiW_signal.root";
    TFile * file = TFile::Open(file_name, "READ");
    auto tree = (TTree*) file->Get("events");

    // Create observable variable
    double xlow = 40., xhigh = 200.;
    int nbins = (int) 2*(xhigh - xlow);
    RooRealVar mH("HCandMass", "HCandMass", xlow, xhigh);

    // Create signal dataset
    RooDataSet cat_phiW_sgn = fitting.makeUnbinnedData("cat_phiW_sgn", mH, tree);

    // Fit to signal
    fitting.performSignalFit(&mH, &cat_phiW_sgn);
}
