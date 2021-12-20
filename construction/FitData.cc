#include "FitData.h"

// --- STD ---
#include <iostream>
#include <stdexcept>

// --- ROOT ---
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TMath.h"
#include "TStyle.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// --- RooFit ---
#include "RooFit.h"
#include "RooFitResult.h"

#include "RooRealVar.h"

#include "RooArgList.h"
#include "RooArgSet.h"

#include "RooAbsPdf.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooHist.h"
#include "RooPlot.h"

using namespace RHD; // TODO: delete after testing
using namespace RooFit;

namespace RHD
{
    TH1F FitData::fetchHistogram ( const std::list<const char*>& fileNames,
                                                     const char* treeName,
                                                     const char* branchName,
                                                     const char* histName,
                                                     const char* histTitle,
                                                             int nbins,
                                                          double xlow,
                                                          double xhigh )
    {
        auto hist = TH1F(histName, histTitle, nbins, xlow, xhigh);

        for (const auto& fname: fileNames) {
            auto file = TFile(fname, "READ");

            TTreeReader reader(treeName, &file);
            TTreeReaderValue<float> var(reader, branchName);
            while (reader.Next()) {
                hist.Fill(*var);
            }
        }

        return hist;
    }

    
    RooDataHist FitData::makeBinnedData ( const char* name,
                                          RooRealVar& ObsVar,
                                                 TH1& hist )
    {
        _DataHistograms.insert(std::pair<std::string, RooDataHist>
                               (name, RooDataHist(name, name,
                                                  RooArgList(ObsVar), &hist)
                               )
                              );
        return _DataHistograms[name];
    }

    
    RooFitResult FitData::performChi2Fit (   RooAbsPdf* pdf,
                                           RooDataHist* data,
                                                    int maxTries,
                                                    int retry )
    {
        RooArgSet* params = pdf->getParameters((const RooArgSet*)(0));
        RooFitResult* fitResult;
        int status = 5;
        int ntries = 0;
        int retrycount = retry;
        
        std::cout << "----------- ";
        std::cout << "Fitting " << pdf->GetTitle() << " to " << data->GetTitle();
        std::cout << " -----------" << std::endl;
        
        // Try fit as many times as needed to converge
        while (++ntries <= maxTries) {
            std::cout << "trial #" << ntries << std::endl;
            fitResult = pdf->chi2FitTo(*data,
                                       RooFit::Save(true),
                                       RooFit::Minimizer("Minuit2", "minimize"),
                                       RooFit::PrintLevel(0),
                                       RooFit::Warnings(false),
                                       RooFit::PrintEvalErrors(0)
                                      );
            status = fitResult->status();
            // If fit status is not 0, randomize parameters
            if (status) {
                params->assignValueOnly(fitResult->randomizePars());
            } else {
                // Retry if argument given is > 0
                if (retrycount <= 0) break;
                else {
                    params->assignValueOnly(fitResult->randomizePars());
                    retrycount--;
                }
            }
        }

        // Check final fit status
        std::string message;
        if (status == 0) {
            message = "Chi2 fit has converged.";
        } else if (status == 1) {
            message = "Chi2 fit exited with status = 1: "
                      "Covariance was made pos defined.";
        } else if (status == 2) {
            message = "Chi2 fit exited with status = 2: "
                      "Hesse is invalid.";
        } else if (status == 3) {
            message = "Chi2 fit exited with status = 3: "
                      "Edm is above max.";
        } else if (status == 4) {
            message = "Chi2 fit exited with status = 4: "
                      "Reached call limit.";
        } else if (status == 5) {
            message = "Chi2 fit exited with status = 5: "
                      "Please investigate.";
        } else {
            message = "Something is amiss!";
        }

        std::cout << message << std::endl;
        std::cout << "---------------------------------------------------------\n" << std::endl;
        return *fitResult;
    }

    
    /* Evaluate fit.*/
    double FitData::getWilksProb ( double nllNull,
                                   double nllAlt,
                                      int dofNull,
                                      int dofAlt )
    /*
     * Calculates probability that the likelihood-ratio test statistic is
     * greater than the one observed under the null hypothesis,
     * using Wilks' Theorem.
     * See: https://en.wikipedia.org/wiki/Likelihood-ratio_test
     *  and https://en.wikipedia.org/wiki/Wilks%27_theorem
     *
     * Parameters:
     *   nllNull = negative log-likelihood of null model
     *   nllAlt  = negative log-likelihood of alternative model
     *   dofNull = degree of freedom in null model
     *   dofAlt  = degree of freedom in alternative model
     *   (note: the alternative model has more parameters than the null model)
     */
    {
        if (!(dofAlt > dofNull))
            std::invalid_argument("dofNull must be smaller than dofAlt");
        if (!(nllAlt > 0 && nllNull > 0))
            std::invalid_argument("NLLs must be positive.");
        if (!(nllNull >= nllAlt))
            std::invalid_argument("nllNull must be greater than or equal to nllAlt.");
        return TMath::Prob(2.*(nllNull - nllAlt), dofAlt - dofNull);
    }

    
    double FitData::getFProb ( double rssNull,
                               double rssAlt,
                                  int nParamNull,
                                  int nParamAlt,
                                  int nData)
    /*
     * Calculates the F statistic given by the equation in the following
     * reference, and returns the probability of observing an F value greater
     * than the one observed under the null hypothesis.
     * https://en.wikipedia.org/wiki/F-test#Regression_problems
     *
     * Parameters:
     *   rssNull    = residual sum of squares of null model
     *   rssAlt     = residual sum of squares of alternative model
     *   nParamNull = number of parameters in null model
     *   nParamAlt  = number of parameters in alternative model
     *   nData      = number of points in data
     */
    {
        if (!(rssNull >= 0 && rssAlt >= 0))
            std::invalid_argument("RSS cannot be negative.");
        if (!(rssNull >= rssAlt))
            std::invalid_argument("rssNull must be greater than or equal to rssAlt.");
        if (!(nParamAlt > 0 && nParamNull > 0 && nData > 0))
            std::invalid_argument("Number of parameters and data points must be positive.");
        if (!(nParamAlt > nParamNull))
            std::invalid_argument("nParamAlt must be greater than nParamNull.");
        if (!(nData > nParamAlt))
            std::invalid_argument("nData must be greater than nParamAlt.");

        double F = ((rssNull-rssAlt)/(nParamAlt-nParamNull))/(rssAlt/(nData-nParamAlt));
        return 1 - TMath::FDistI(F, nParamAlt-nParamNull, nData-nParamAlt);
    }
    

    /* Plot fit. */
    TCanvas FitData::plotPDF (  RooRealVar* ObsVar,
                                 RooAbsPdf* pdf,
                               RooDataHist* data )
    {
        // PDF and data plot
        RooPlot* frame1 = ObsVar->frame(RooFit::Title(pdf->GetTitle()));
        data->plotOn(frame1);
        pdf->plotOn(frame1,
                    RooFit::LineColor(kRed),
                    RooFit::LineStyle(kDashed),
                    RooFit::Name("fit"));
        pdf->paramOn(frame1, RooFit::Layout(0.6, 0.88, 0.85));
        frame1->getAttText()->SetTextFont(43);
        frame1->getAttText()->SetTextSize(21);

        // Residuals
        RooPlot* frame2 = ObsVar->frame(RooFit::Title("residuals"));
        RooHist* hresid = frame1->residHist();
        frame2->addPlotable(hresid, "P");

        // Pull
        RooPlot* frame3 = ObsVar->frame(RooFit::Title("pull"));
        RooHist* hpull = frame1->pullHist();
        frame3->addPlotable(hpull, "P");

        // Draw on canvas
        gStyle->SetOptStat(0);
        TCanvas c(pdf->GetName(), pdf->GetTitle());
        c.SetCanvasSize(1500, 2000);

        TPad p1("p1", "p1", .05, .53, .95, .95);
        TPad p2("p2", "p2", .05, .29, .95, .51);
        TPad p3("p3", "p3", .05, .05, .95, .27);

        p1.cd();
        frame1->Draw();
        p1.Update();

        p2.cd();
        frame2->Draw();
        p2.Update();

        p3.cd();
        frame3->Draw();
        p3.Update();

        c.cd();
        p1.Draw();
        p2.Draw();
        p3.Draw();
        c.Update();

        // TEST
        c.SaveAs(Form("%s.jpg", pdf->GetName()));

        return c.Clone();
    }
}