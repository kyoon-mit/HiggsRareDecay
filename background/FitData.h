/**
 * FitData.h
 * 
 * Module to fit signal or background PDFs to data.
 */
#ifndef FIT_DATA_H
#define FIT_DATA_H

#include <map>
#include <string>

class TH1;

class RooAbsPdf;
class RooDataHist;
class RooFitResult;
class RooRealVar;
class TCanvas;

namespace RHD // Rare Higgs Decay
{
    class FitData
    {
    public:
        FitData () {};
        ~FitData () {};

        /* Make data histogram. */
        RooDataHist makeBinnedData ( const char* name,
                                     RooRealVar& ObsVar,
                                            TH1& hist );
        
        void makeBlindedDataHistogram (); // TODO

        /* Perform fit. */
        RooFitResult performChi2Fit (   RooAbsPdf* pdf,
                                      RooDataHist* data,
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

#endif // FIT_DATA_H
