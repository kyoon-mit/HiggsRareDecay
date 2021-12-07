/**
 * BackgroundPDFModels.h
 * 
 * Module to create PDF models for background fitting
 */
#ifndef BACKGROUND_PDF_MODELS_H
#define BACKGROUND_PDF_MODELS_H

#include <string>
#include <map>

class RooAbsReal;
class RooRealVar;

class RooGaussian;
class RooAbsPdf;

namespace RHD // Rare Higgs Decay
{
    class BackgroundPDFModels
    {
    public:
        BackgroundPDFModels () {};
        ~BackgroundPDFModels () {};

        /* Fetch PDF model. */
        RooAbsPdf* getPDF ( std::string key );

        /* List keys of existing PDFs. */
        void listPDFs ();

        /* Make convolutions with Gaussian. */
        void makeLaurentConvGaussian ( RooRealVar& ObsVar,
                                       RooRealVar& mu,
                                       RooRealVar& sigma,
                                              int  max_order );
        
    private:
        /* Map to store PDF models. */
        std::map<std::string, RooAbsPdf*> _PDFs;
        std::map<std::string, RooRealVar*> _Parameters;
        
        /* Enum to check status of PDF models. */
        enum PDFStatus { none, exists };
        int _statusGaussian = PDFStatus::none;

        /* Create and store basic PDFs. */
        void makeGaussian ( RooRealVar& ObsVar,
                            RooRealVar& mu,
                            RooRealVar& sigma );
        void makeLaurentSeries ( const char* prefix,
                                 RooRealVar& ObsVar,
                                        int  order );
    };
  
}

#endif // BACKGROUND_PDF_MODELS_H
