/**
 * BackgroundFitting.h
 * 
 * Module to fit background PDFs to data.
 */
#ifndef BACKGROUND_FITTING_H
#define BACKGROUND_FITTING_H

#include <map>
#include <string>

class TH1;

class RooDataHist;
class RooFitResult;
class TCanvas;

namespace RHD // Rare Higgs Decay
{
    class BackgroundFitting
    {
    public:
        BackgroundFitting () {};
        ~BackgroundFitting () {};

        /* Make data histogram. */
        RooDataHist makeBinnedData ( const char* name,
                                     RooRealVar& ObsVar,
                                            TH1& hist );
        
        void makeBlindedDataHistogram (); // TODO

        /* Perform fit. */
        void performChi2Fit (    RooAbsPdf* pdf,
                               RooDataHist* data,
                              RooFitResult* fitResult,
                                        int maxTries );

        /* Evaluate fit. */
        void getGoodnessOfFit ();
        void performFTest ();

        /* Plot fit. */
        TCanvas plotFitResults ();

        /* Save results. */
        void saveFitResults ();

    private:
        /* Map to store data histograms. */
        std::map<std::string, RooDataHist> _DataHistograms;
    };
}

#endif // BACKGROUND_FITTING_H
