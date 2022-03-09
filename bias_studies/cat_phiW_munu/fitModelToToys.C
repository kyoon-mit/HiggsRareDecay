#include <string>
#include <vector>

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TRandom3.h"

#include "RooFit.h"
#include "RooFitResult.h"
#include "RooMsgService.h"
#include "RooWorkspace.h"

#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooArgSet.h"
#include "RooArgList.h"

#include "RooDataSet.h"
#include "RooDataHist.h"

#include "RooRealVar.h"

void fitModelToToys ( const char* modelFileName,
                      const char* bkgGenName,
                      const char* bkgFitName,
                      const char* outPrefix,
                              int nToys,
                           double nSignal=0,
                           double nBackground=0 )
{
    // Open model file
    TFile* modelFile = TFile::Open(modelFileName, "READ");
    auto w = (RooWorkspace*) modelFile->Get("w");

    // Create sgn + bkg model
    RooRealVar* HCandMass = w->var("HCandMass");
    RooAbsPdf* bkg_gen_model = w->pdf(bkgGenName);
    RooAbsPdf* bkg_fit_model = w->pdf(bkgFitName);
    RooAbsPdf* sgn_model = w->pdf("digauss");
    RooRealVar* nbkg_gen = w->var("background_multipdf_norm");
    if (nBackground > 0) nbkg_gen->setVal(nBackground);
    RooRealVar* nbkg_fit = (RooRealVar*) nbkg_gen->Clone();
    RooRealVar nsig_gen("nsig_gen", "number of signal events", nSignal, -50000, 50000);
    RooRealVar nsig_fit("nsig_fit", "number of signal events", 0, -80000, 80000);

    RooAddPdf genmodel("genmodel", "sig+bkg",
                       RooArgList(*bkg_gen_model, *sgn_model),
                       RooArgList(*nbkg_gen, nsig_gen)
                      );
    RooAddPdf fitmodel("fitmodel", "sig+bkg",
                       RooArgList(*bkg_fit_model, *sgn_model),
                       RooArgList(*nbkg_fit, nsig_fit)
                      );

    nbkg_gen->setConstant();
    nsig_gen.setConstant();

    RooFitResult* fitResult;

    // Freeze certain parameters of the fit model
    RooArgSet* params = fitmodel.getParameters(RooArgSet(nsig_fit, *nbkg_fit, *HCandMass));
    auto it_params = params->createIterator();
    RooRealVar *par;
    while ((par = (RooRealVar*) it_params->Next())) {
        if (strstr(par->GetName(), "digauss")) { // freeze only signal params
            par->Print();
            par->setConstant();
        }
    }

    // Book histograms
    std::vector<float> nsig_nums; // vector holding fitted signal events
    TH1F nsig_fitresults("nsig_fitresults", "fitted number of signal events",
                         200, -.5*fabs(nSignal), 2*fabs(nSignal));
    TH1F pull_dist("pull_dist", "pull distribution of signal strength",
                   20, -5, 5);

    // Configure generator
    TRandom3 Generator;
    double nevents_mean = nbkg_gen->getValV() + nsig_gen.getValV();

    // Generate and fit to toy
    int maxPlots = 8;
    RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    for (int i=1; i<=nToys; i++) {
        // Generate toy
        int nevents = Generator.Poisson(nevents_mean);
        RooDataHist* toydata = genmodel.generateBinned(*HCandMass, nevents,
                                                       false, true);

        // Fit model to toy
        int status = 5;
        int maxTries = 50;
        int retrycount = 1; // even if fit status = 0, try again
        int ntries = 0;
        while (++ntries <= maxTries) {
            fitResult = fitmodel.fitTo(*toydata,
                                       RooFit::Save(true),
                                       RooFit::Minimizer("Minuit2", "minimize"),
                                       RooFit::Warnings(false),
                                       RooFit::PrintLevel(-1),
                                       RooFit::PrintEvalErrors(-99)
                                       );
            status = fitResult->status();
            if (status) params->assignValueOnly(fitResult->randomizePars());
            else {
                if (retrycount <= 0) {
                    // std::cout << "Exiting fit with status: " << status << std::endl;
                    // fitResult->floatParsFinal().Print("s");
                    break;
                } else {
                    params->assignValueOnly(fitResult->randomizePars());
                    retrycount--;
                }
            }
        }

        if (!status) {
            RooRealVar* nsig_final =
                dynamic_cast<RooRealVar*>(fitResult->floatParsFinal().find(nsig_fit.GetName()));
            nsig_fitresults.Fill(nsig_final->getVal());
            nsig_nums.push_back(nsig_final->getVal());
        }

        if (i <= maxPlots) {
            // Plot
            RooPlot* xframe = HCandMass->frame(RooFit::Bins(260));
            toydata->plotOn(xframe);
            fitmodel.plotOn(xframe, RooFit::LineColor(kRed));
            bkg_fit_model->plotOn(xframe, RooFit::LineColor(kBlue));
            
            // Save plot
            TCanvas c;
            xframe->Draw();
            c.SaveAs(Form("%s_%d.jpg", outPrefix, i));
            c.Close();
        }

    }

    // Draw nsig distribution
    TCanvas c_nsig;
    nsig_fitresults.Draw();   
    c_nsig.SaveAs(Form("%s_nsig.jpg", outPrefix));
    c_nsig.Close();

    // Get pull distribution
    double stddev = nsig_fitresults.GetStdDev();
    for (auto const entry: nsig_nums) {
        double pull = (entry-nSignal)/stddev;
        std::cout << pull << std::endl;
        pull_dist.Fill(pull);
    }

    // Draw pull distribution
    TCanvas c_pull;
    pull_dist.Draw();
    c_pull.SaveAs(Form("%s_pull.jpg", outPrefix));
    c_pull.Close();
}
