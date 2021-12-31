/**
 * FitData.h
 * 
 * Module to fit signal or background PDFs to data.
 *
 * TODO: include less std libraries where possible
 */
#ifndef FIT_DATA_H
#define FIT_DATA_H

#include <vector>
#include <map>
#include <memory>
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
        FitData ();
        FitData ( const char* outfileName,
                  const char* saveDir="./",
                  const char* workspaceName="w" );
        ~FitData () {};

        /* Fetch histogram from file. */
        TH1F fetchHistogram ( const std::vector<const char*>& fileNames,
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
        RooFitResult performLikelihoodFit (   RooAbsPdf* pdf,
                                            RooDataHist* data, // binned
                                                     int maxTries,
                                                     int retry=0 );
        RooFitResult performLikelihoodFit (  RooAbsPdf* pdf,
                                            RooDataSet* data, // unbinned
                                                    int maxTries,
                                                    int retry=0 );
        void performMultiChi2Fit (                const char* pdfType,
                                                  RooRealVar* ObsVar,
                                                 RooDataHist* data,
                                   const std::vector<double>& initParamValues1={},
                                   const std::vector<double>& initParamValues2={},
                                                       double fTestAlpha=0.05 );
        void performMultiLikelihoodFit (                const char* pdfType,
                                                        RooRealVar* ObsVar,
                                                       RooDataHist* data, // binned
                                         const std::vector<double>& initParamValues1={},
                                         const std::vector<double>& initParamValues2={},
                                                             double fTestAlpha=0.05 );
        void performMultiLikelihoodFit (                const char* pdfType,
                                                        RooRealVar* ObsVar,
                                                        RooDataSet* data, // unbinned
                                         const std::vector<double>& initParamValues1={},
                                         const std::vector<double>& initParamValues2={},
                                                             double fTestAlpha=0.05 ); //
        void performSignalFit ( RooRealVar* ObsVar,
                                RooDataHist* data );

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
        void plotPDF (  RooRealVar* ObsVar,
                         RooAbsPdf* pdf,
                       RooDataHist* data );
        
        inline void setSaveDir ( const char* dir ) { _SAVEDIR = dir; }

        /* Save to file. */
        inline void setOutfileName ( const char* fileName ) { _OUTFILENAME = fileName; }
        inline void setWorkspaceName ( const char* wspaceName ) { _WORKSPACENAME = wspaceName; }
        void setSaveOption ( bool saveOption );
        void saveMultiPdf ( const std::vector<const char*>& pdfNameList,
                                                const char* multipdfName );
        void saveFitResults (); //

    private:
        /* Method to set save path. */
        void makePath();
        
        /* Map to store data histograms. */
        std::map<std::string, RooDataHist> _DataHistograms;

        /* Internal variables for saving. */
               bool _SAVEOPTION;
        const char* _SAVEDIR;
        const char* _SAVEPATHFULL;
        const char* _OUTFILENAME;
        const char* _WORKSPACENAME;
    };
}

#endif // FIT_DATA_H
