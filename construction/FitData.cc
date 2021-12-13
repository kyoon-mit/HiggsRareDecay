#include "FitData.h"

// --- STD ---
#include <iostream>

// --- ROOT ---
#include "TCanvas.h"

// --- RooFit ---
#include "RooFit.h"
#include "RooFitResult.h"

#include "RooRealVar.h"

#include "RooArgList.h"
#include "RooArgSet.h"

#include "RooAbsPdf.h"
#include "RooDataHist.h"

using namespace RHD; // TODO: delete after testing
using namespace RooFit;

namespace RHD
{
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
                                                    int maxTries )
    {
        RooArgSet* params = pdf->getParameters((const RooArgSet*)(0));
        RooFitResult* fitResult;
        int status = 5;
        int ntries = 0;

        // Try fit as many times as needed to converge
        while (++ntries <= maxTries) {
                fitResult = pdf->chi2FitTo(*data,
                                           RooFit::Save(true),
                                           RooFit::Minimizer("Minuit2", "minimize"),
                                           RooFit::SumW2Error(false),
                                           RooFit::PrintLevel(0),
                                           RooFit::Warnings(false),
                                           RooFit::PrintEvalErrors(-1)
                                          );
                status = fitResult->status();
                // If fit status is not 0, randomize parameters
                if (status) {
                    params->assignValueOnly(fitResult->randomizePars());
                } else {
                    break;
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
        return *fitResult;
    }
}
