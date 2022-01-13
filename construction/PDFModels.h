/**
 * PDFModels.h
 * 
 * Module to create PDF models for background fitting
 */
#ifndef PDF_MODELS_H
#define PDF_MODELS_H

#include <string>
#include <map>
#include <memory>

class RooArgList;

class RooRealVar;

class RooAbsPdf;
class RooAddPdf;
class RooGaussian;

namespace RHD // Rare Higgs Decay
{
    class PDFModels
    {
    public:
        PDFModels () {};
        ~PDFModels () {};

        /* Fetch PDF model.
         * Example keys of some PDFs:
         *   lau3_X_gauss
         */
        RooAbsPdf& getPDF ( std::string key );

        /* List keys of existing PDFs. */
        void listPDFs ();

        /* Set parameter values. */
        void setVal ( const std::string& paramKey, double value );
        void setMultiVals (          const char* prefix,
                                             int lowOrder,
                                             int highOrder,
                            std::vector<double>& values );

        /* Make convolutions with Gaussian. */
        RooAbsPdf* makeExponentialConvGaussian ( RooRealVar& ObsVar,
                                                         int order,
                                                        bool extend=false );
        RooAbsPdf* makePowerConvGaussian ( RooRealVar& ObsVar,
                                                   int order,
                                                  bool extend=false );
        RooAbsPdf* makeBernsteinConvGaussian ( RooRealVar& ObsVar,
                                                       int order,
                                                      bool extend=false );
        RooAbsPdf* makeLaurentConvGaussian ( RooRealVar& ObsVar,
                                                     int order,
                                                    bool extend=false );
        
        /* Create signal PDFs. */
        RooAbsPdf* makeGaussian ( RooRealVar& ObsVar, const char* prefix );
        RooAbsPdf* makeDoubleGaussian ( RooRealVar& ObsVar, bool extend=false );
        RooAbsPdf* makeTripleGaussian ( RooRealVar& ObsVar, bool extend=false );
        RooAbsPdf* makeVoigtian ( RooRealVar& ObsVar, bool extend=false );

        /* Create falling spectrum PDFs. Make sure to delete. */
        RooAbsPdf* makeExponentialSeries ( RooRealVar& ObsVar, int order );
        RooAbsPdf* makePowerSeries ( RooRealVar& ObsVar, int order );
        RooAbsPdf* makeBernsteinPoly ( RooRealVar& ObsVar, int order);
        RooAbsPdf* makeBernsteinFast ( RooRealVar& ObsVar, int order); // see if any difference
        RooAbsPdf* makeLaurentSeries ( RooRealVar& ObsVar, int order );

        /* The following methods return a pointer created with "new"
         * makeDoubleGaussian
         * makeTripleGaussian
         * makeBernsteinPoly
         * makeLaurentSeries
         */

    protected:
        /* Map to store PDF models. */
        std::map<std::string, RooRealVar> _Parameters;
        std::map<std::string, RooArgList> _RooArgLists;
        std::map<std::string, std::unique_ptr<RooAbsPdf>> _PDFs;
        std::map<std::string, RooAddPdf> _RooAddPdfs;
                
    private:
        /* Internally store PDFs and paramters. */
        void storeRooRealVar ( const char* key,
                                    double value,
                                    double min,
                                    double max );

        void storeRooRealVar ( const char* key,
                                    double min,
                                    double max );

        void storeRooArgList ( const char* key,
                               RooArgList& list );

        void storeRooGaussian ( const char* key,
                                RooRealVar& ObsVar,
                                RooRealVar& mu,
                                RooRealVar& sigma );

        void storeRooVoigtian ( const char* key,
                                RooRealVar& ObsVar,
                                RooRealVar& mean,
                                RooRealVar& width,
                                RooRealVar& sigma );

        void storeRooExponential( const char* key,
                                  RooRealVar& ObsVar,
                                  RooRealVar& power );

        void storeRooPower ( const char* key,
                             RooRealVar& ObsVar,
                                   float power );

        void storeRooBernstein ( const char* key,
                                 RooRealVar& ObsVar,
                                 RooArgList& coeffs );

        void storeRooAddPdf ( const char* key,
                              RooArgList& pdfs,
                              RooArgList& coeffs,
                                     bool recursive=false );

        void storeRooGenericPdf ( const char* key,
                                  const char* formula,
                                  RooArgList& param_list );

        void storeRooFFTConvPdf ( const char* key,
                                  RooRealVar& ObsVar,
                                   RooAbsPdf* pdf1,
                                   RooAbsPdf* pdf2 );
    };
  
}

#endif // PDF_MODELS_H
