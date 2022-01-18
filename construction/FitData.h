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
        RooDataSet makeUnbinnedData ( const char* name,
                                      RooRealVar& ObsVar,
                                           TTree* tree );
        void saveToyData ( const char* signalName,
                           const char* backgroundName,
                                double signalRate,
                                double backgroundRate );
        void makeBlindedDataHistogram (); // TODO

        /* Perform fit. */
        RooFitResult performChi2Fit (   RooAbsPdf& pdf,
                                      RooDataHist& data,
                                               int maxTries,
                                               int retry=0 );
        template<typename T>
        RooFitResult performLikelihoodFit ( RooAbsPdf& pdf,
                                                    T& data,
                                                   int maxTries,
                                                   int retry=0 );
        void performMultiChi2Fit (                const char* pdfType,
                                                  RooRealVar& ObsVar,
                                                 RooDataHist& data,
                                   const std::vector<double>& initParamValues1={},
                                   const std::vector<double>& initParamValues2={},
                                                       double fTestAlpha=0.05 );
        template<typename T>
        void performMultiLikelihoodFit (                const char* pdfType,
                                                        RooRealVar& ObsVar,
                                                                 T& data,
                                         const std::vector<double>& initParamValues1={},
                                         const std::vector<double>& initParamValues2={},
                                                             double fTestAlpha=0.05 );
        void manuallyFitParams (                const char* pdfType,
                                                        int order,
                                                const char* plotTitle,
                                                RooRealVar& ObsVar,
                                               RooDataHist& data,
                                 const std::vector<double>& initParamValues1={},
                                 const std::vector<double>& initParamValues2={} );
        template<typename T>
        void performSignalFit ( RooRealVar& ObsVar,
                                         T& data,
                                       bool use_toys=false );

        /* Evaluate fit. */
        template<typename T>
        double getGoodnessOfFitBC ( RooRealVar& ObsVar,
                                     RooAbsPdf& pdf,
                                             T& data,
                                           bool use_toys=false );
        template<typename T>
        std::vector<double> getBCTestResult ( RooRealVar& ObsVar,
                                               RooAbsPdf& pdf,
                                                       T& data );
        double getWilksProb ( double nllNull,
                              double nllAlt,
                                 int dofNull,
                                 int dofAlt ); // Wilks's Theorem
        double getFProb ( double rssNull,
                          double rssAlt,
                             int nParamNull,
                             int nParamAlt,
                             int nData );

        /* Plot fit. */
        template<typename T>
        void plotPDF (  RooRealVar& ObsVar,
                         RooAbsPdf& pdf,
                                 T& data );
        template<typename T>
        void plotMultiplePDFs (                     RooRealVar& ObsVar,
                                                             T& data,
                                                    const char* plotName,
                                const std::vector<const char*>& pdfNames,
                                        const std::vector<int>& colorScheme );
        
        /* Save to file. */
        inline void setSaveDir ( const char* dir ) { _SAVEDIR = dir; }
        inline void setOutfileName ( const char* fileName ) { _OUTFILENAME = fileName; }
        inline void setWorkspaceName ( const char* wspaceName ) { _WORKSPACENAME = wspaceName; }
        void setSaveOption ( bool saveOption, bool recreate=false );
        void saveMultiPdf ( const std::vector<const char*>& pdfNameList,
                                                const char* multipdfName );
        void saveFitResults (); //

    private:
        /* Method to set save path. */
        void makePaths();
        
        /* Map to store data histograms. */
        std::map<std::string, RooDataHist> _DataHistograms;
        std::map<std::string, RooDataSet> _DataSets;

        /* Internal variables for saving. */
               bool _SAVEOPTION;
        const char* _SAVEDIR;
        const char* _OUTFILENAME;
        const char* _WORKSPACENAME;
        std::string _SAVEPATHFULL;
        std::string _PLOTPATH;
    };
}

#endif // FIT_DATA_H
