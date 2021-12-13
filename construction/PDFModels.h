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

        /* Make convolutions with Gaussian. */
        RooAbsPdf* makeLaurentConvGaussian ( RooRealVar& ObsVar, int order );
        
        /* Create basic PDFs. */
        RooAbsPdf* makeGaussian ( RooRealVar& ObsVar );

        /* Create series PDFs. Make sure to delete. */
        RooAbsPdf* makeLaurentSeries ( RooRealVar& ObsVar, int order );        

    protected:
        /* Map to store PDF models. */
        std::map<std::string, RooRealVar> _Parameters;
        std::map<std::string, RooArgList> _RooArgLists;
        std::map<std::string, std::unique_ptr<RooAbsPdf>> _PDFs;
        std::map<std::string, RooAddPdf> _RooAddPdfs;
                
        /* Enum to check status of PDF models. */
        enum PDFStatus { none, exists };
        int _statusGaussian = PDFStatus::none;

    private:
        /* Internally store PDFs and paramters. */
        void storeRooRealVar ( std::string const& key,
                                           double value,
                                           double min,
                                           double max );

        void storeRooArgList ( std::string const& key,
                                      RooArgList& list );

        void storeRooGaussian ( std::string const& key,
                                       RooRealVar& ObsVar,
                                       RooRealVar& mu,
                                       RooRealVar& sigma );

        void storeRooPower ( std::string const& key,
                                    RooRealVar& ObsVar,
                                          float power );

        void storeRooAddPdf ( std::string const& key,
                                      RooArgList pdfs,
                                      RooArgList coeffs,
                                            bool recursive=false );
        /*        
        void storeGaussian ( std::string const& key,
                                    RooRealVar& ObsVar,
                                    RooRealVar& mu,
                                    RooRealVar& sigma );
        
        void storeGaussian ( std::string const& key,
                                    RooRealVar& ObsVar,
                                    RooRealVar& mu,
                                    RooRealVar& sigma );
        */
    };
  
}

#endif // PDF_MODELS_H
