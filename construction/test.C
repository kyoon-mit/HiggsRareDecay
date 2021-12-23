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
    auto fitting = FitData();
    fitting.setSaveOption(false);

    // Create signal histogram from file
    const std::vector<const char*> filenames_WH_ZH {"/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc10_Wcat.root",
                                                    "/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc11_Wcat.root"};
    TH1F signal = fitting.fetchHistogram (filenames_WH_ZH,
                                          "events", "HCandMass", "signal", "signal WH and ZH",
                                          100, 100., 150.);

    // Create background histograms from file
    double xlow = 40., xhigh = 200.;
    int nbins = (int) xhigh - xlow;

    const std::vector<const char*> filenames_ZW_H {"/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc0_Wcat.root",
                                                   "/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc1_Wcat.root",
                                                   "/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc2_Wcat.root",
                                                   "/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc3_Wcat.root",
                                                   "/home/submit/kyoon/CMSSW_10_6_27/src/Hrare/analysis/outname_mc4_Wcat.root"};

    TH1F bkg_comb = fitting.fetchHistogram (filenames_ZW_H,
                                            "events", "HCandMass", "bkg_comb", "Z/W + H bkg combined",
                                            nbins, xlow, xhigh);

    // Key variable
    RooRealVar mH("mH", "mH", xlow, xhigh, "GeV");

    // Fit to background
    auto bkgdata = fitting.makeBinnedData("bkg_comb", mH, bkg_comb);
    fitting.performMultiFit("bernXgauss", &mH, &bkgdata, {.61, .31, .24, .037, .047});
    fitting.performMultiFit("lauXgauss", &mH, &bkgdata, {.06, .02}, {0.9, 0.75, 0.66});
    fitting.performMultiFit("powXgauss", &mH, &bkgdata, {0.98, 0.7}, {-0.6, -4., -0.5});

    /*
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
    RooAbsPdf* bern1_X_gauss = m.makeBernsteinConvGaussian(mH, 1);
    RooAbsPdf* bern2_X_gauss = m.makeBernsteinConvGaussian(mH, 2);
    RooAbsPdf* bern3_X_gauss = m.makeBernsteinConvGaussian(mH, 3);
    RooAbsPdf* bern4_X_gauss = m.makeBernsteinConvGaussian(mH, 4);
    RooAbsPdf* bern5_X_gauss = m.makeBernsteinConvGaussian(mH, 5);
    RooAbsPdf* bern6_X_gauss = m.makeBernsteinConvGaussian(mH, 6);
    RooAbsPdf* bern7_X_gauss = m.makeBernsteinConvGaussian(mH, 7);
    RooAbsPdf* bern8_X_gauss = m.makeBernsteinConvGaussian(mH, 8);
    RooAbsPdf* bern9_X_gauss = m.makeBernsteinConvGaussian(mH, 9);

    m.setVal("gauss_mu", 16.5);
    m.setVal("gauss_sigma", 7.5);
    m.setMultiVals("bern1_c", 0, 0, {.6});
    m.setMultiVals("bern2_c", 0, 1, {.6, .3});
    m.setMultiVals("bern3_c", 0, 2, {.4, .09, .02});
    m.setMultiVals("bern4_c", 0, 3, {.4, .086, .02, .02});
    m.setMultiVals("bern5_c", 0, 4, {.61, .31, .24, .037, .047});
    m.setMultiVals("bern6_c", 0, 4, {.61, .31, .24, .037, .047});
    m.setMultiVals("bern7_c", 0, 4, {.61, .31, .24, .037, .047});
    m.setMultiVals("bern8_c", 0, 4, {.61, .31, .24, .037, .047});
    m.setMultiVals("bern9_c", 0, 4, {.61, .31, .24, .037, .047});

    auto bkgdata = f.makeBinnedData("bkg_comb", mH, bkg_comb);
    
    auto f1 = f.performChi2Fit(bern1_X_gauss, &bkgdata, 50);
    auto f2 = f.performChi2Fit(bern2_X_gauss, &bkgdata, 100);
    auto f3 = f.performChi2Fit(bern3_X_gauss, &bkgdata, 100, 5);
    auto f4 = f.performChi2Fit(bern4_X_gauss, &bkgdata, 100, 5);
    m.setVal("gauss_mu", 16.43);
    m.setVal("gauss_sigma", 6.88);
    auto f5 = f.performChi2Fit(bern5_X_gauss, &bkgdata, 100, 5);
    auto f6 = f.performChi2Fit(bern6_X_gauss, &bkgdata, 100, 5);
    auto f7 = f.performChi2Fit(bern7_X_gauss, &bkgdata, 100, 5);
    auto f8 = f.performChi2Fit(bern8_X_gauss, &bkgdata, 100, 5);
    auto f9 = f.performChi2Fit(bern9_X_gauss, &bkgdata, 100, 5);

    auto dof1 = f1.covarianceMatrix().GetNcols();
    auto dof2 = f2.covarianceMatrix().GetNcols();
    auto dof3 = f3.covarianceMatrix().GetNcols();
    auto dof4 = f4.covarianceMatrix().GetNcols();
    auto dof5 = f5.covarianceMatrix().GetNcols();
    auto dof6 = f6.covarianceMatrix().GetNcols();
    auto dof7 = f7.covarianceMatrix().GetNcols();
    auto dof8 = f8.covarianceMatrix().GetNcols();
    auto dof9 = f9.covarianceMatrix().GetNcols();

    std::cout << "dofs: " << dof1 << " "
                          << dof2 << " "
                          << dof3 << " "
                          << dof4 << " "
                          << dof5 << " "
                          << dof6 << " "
                          << dof7 << " "
                          << dof8 << " "
                          << dof9 << " " << std::endl;
    std::cout << "minNlls: " << f1.minNll() << " "
                             << f2.minNll() << " "
                             << f3.minNll() << " "
                             << f4.minNll() << " "
                             << f5.minNll() << " "
                             << f6.minNll() << " "
                             << f7.minNll() << " "
                             << f8.minNll() << " "
                             << f9.minNll() << " " << std::endl;

    std::cout << "f1 & f2: " << f.getWilksProb(f1.minNll(), f2.minNll(), dof1, dof2) << std::endl;
    std::cout << "f2 & f3: " <<	f.getWilksProb(f2.minNll(), f3.minNll(), dof2, dof3) << std::endl;
    std::cout << "f3 & f4: " <<	f.getWilksProb(f3.minNll(), f4.minNll(), dof3, dof4) << std::endl;
    std::cout << "f4 & f5: " <<	f.getWilksProb(f4.minNll(), f5.minNll(), dof4, dof5) << std::endl;
    std::cout << "f5 & f6: " << f.getWilksProb(f5.minNll(), f6.minNll(), dof5, dof6) << std::endl;
    std::cout << "f6 & f7: " << f.getWilksProb(f6.minNll(), f7.minNll(), dof6, dof7) << std::endl;
    std::cout << "f7 & f8: " << f.getWilksProb(f7.minNll(), f8.minNll(), dof7, dof8) << std::endl;
    std::cout << "f8 & f9: " << f.getWilksProb(f8.minNll(), f9.minNll(), dof8, dof9) << std::endl;

    f.plotPDF(&mH, bern2_X_gauss, &bkgdata);
    f.plotPDF(&mH, bern3_X_gauss, &bkgdata);
    f.plotPDF(&mH, bern4_X_gauss, &bkgdata);
    f.plotPDF(&mH, bern5_X_gauss, &bkgdata);
    f.plotPDF(&mH, bern6_X_gauss, &bkgdata);
    f.plotPDF(&mH, bern7_X_gauss, &bkgdata);
    f.plotPDF(&mH, bern8_X_gauss, &bkgdata);
    f.plotPDF(&mH, bern9_X_gauss, &bkgdata);

    */
    // auto ftest = f.performFTest(13,2,lau3_X_gauss,lau4_X_gauss,&mH,&bkgdata,"test");
}
