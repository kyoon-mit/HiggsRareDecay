#include "TFile.h"
#include "TDirectoryFile.h"
#include "TRandom3.h"

#include "RooFit.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooRealVar.h"

void fitModelToToys ( const char* toyFileName,
                      const char* modelFileName,
                      const char* bkgName,
                      const char* outPrefix,
                              int nToys )
{
    // Open toy file
    TFile* toyFile = TFile::Open(toyFileName, "READ");
    auto toys = (TDirectoryFile*) toyFile->Get("toys");

    // Open model file
    TFile* modelFile = TFile::Open(modelFileName, "READ");
    auto w = (RooWorkspace*) modelFile->Get("w");

    // Create sgn + bkg model
    RooRealVar* HCandMass = w->var("HCandMass");
    RooRealVar* nbkg = w->var("background_multipdf_norm");
    RooAbsPdf* bkg_model = w->pdf(bkgName);
    RooAbsPdf* sgn_model = w->pdf("trigauss");
    RooRealVar r_gen("r", "number of signal events", 0, -2000, 10000);
    r_gen.setConstant();
    RooRealVar bkgfrac("bkgfrac", "bkgfrac", 1, 0., 1.);

    RooAddPdf genmodel("genmodel", "sig+bkg",
                       RooArgList(*bkg_model, *sgn_model),
                       RooArgList(*nbkg, r_gen)
                      );
    RooAddPdf fitmodel("fitmodel", "sig+bkg",
                       RooArgList(*bkg_model, *sgn_model),
                       RooArgList(bkgfrac));

    RooFitResult* fitResult;

    // Freeze certain parameters of the fit model
    RooArgSet* params = fitmodel.getParameters(RooArgSet(bkgfrac, *HCandMass));//r, *nbkg, *HCandMass));
    auto it_params = params->createIterator();
    RooRealVar *par;
    while ((par = (RooRealVar*) it_params->Next())) {
        par->Print();
        par->setConstant();
    }

    // Configure generator
    TRandom3 Generator;
    double nevents_mean = nbkg->getValV() + r_gen.getValV();
    
    for (int i=1; i<=nToys; i++) {
        // Get toy
        // auto toydata = (RooDataHist*) toys->Get(Form("toy_%d", i));

        // Generate toy
        int nevents = Generator.Poisson(nevents_mean);
        RooDataHist* toydata = genmodel.generateBinned(*HCandMass, nevents,
                                                       false, true);

        // Fit model to toy
        fitResult = fitmodel.fitTo(*toydata,
                                   RooFit::Save(true),
                                   RooFit::Minimizer("Minuit2", "minimize"),
                                   RooFit::Warnings(false),
                                   RooFit::PrintEvalErrors(-99)
                                  );
        //sgn_bkg_model.chi2FitTo(*toydata);
        
        // Plot
        RooPlot* xframe = HCandMass->frame(RooFit::Bins(260));
        toydata->plotOn(xframe);
        fitmodel.plotOn(xframe);

        // Save plot
        TCanvas c;
        xframe->Draw();
        c.SaveAs(Form("toy_%d.jpg", i));
    }
}
