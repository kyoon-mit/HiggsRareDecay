/**
 * FitData.h
 * 
 * Module to fit signal or background PDFs to data.
 */
#ifndef FIT_DATA_H
#define FIT_DATA_H

#include <list>
#include <map>
#include <string>

class TH1;
class TH1F;

class RooAbsPdf;
class RooDataHist;
class RooDataSet;
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

        /* Fetch histogram from file. */
        TH1F fetchHistogram ( const std::list<const char*>& fileNames,
                                                const char* treeName,
                                                const char* branchName,
                                                const char* histName,
                                                const char* histTitle,
                                                        int nbins,
                                                     double xlow,
                                                     double xhigh );

        /* Make data histogram. */
        RooDataHist makeBinnedData ( const char* name,
                                     RooRealVar& ObsVar,
                                            TH1& hist );
        
        void makeBlindedDataHistogram (); // TODO

        /* Perform fit. */
        RooFitResult performChi2Fit (   RooAbsPdf* pdf,
                                      RooDataHist* data,
                                               int maxTries,
                                               int retry=0 );

        /* Evaluate fit. */
        void getGoodnessOfFit (); //
        double getWilksProb ( double nllNull,
                              double nllAlt,
                                 int dofNull,
                                 int dofAlt ); // Wilks's Theorem
        double getFProb ( double rssNull,
                          double rssAlt,
                             int nParamNull,
                             int nParamAlt,
                             int nData );
        double performFTest (      double chi2,
                                      int ndof,
                               RooAbsPdf* pdfNull,
                               RooAbsPdf* pdfTest,
                              RooRealVar* mass,
                              RooDataSet* data,
                              std::string name );

        /* Plot fit. */
        TCanvas plotPDF (  RooRealVar* ObsVar,
                            RooAbsPdf* pdf,
                          RooDataHist* data );
        
        inline void setSaveDir ( const char* dir ) { _SAVEDIR = dir; }

        /* Save results. */
        void saveFitResults (); //

    private:
        /* Map to store data histograms. */
        std::map<std::string, RooDataHist> _DataHistograms;

        /* Directory to save outputs. */
        const char* _SAVEDIR;
    };
}

#endif // FIT_DATA_H
