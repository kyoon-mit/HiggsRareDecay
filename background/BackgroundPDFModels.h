/**
 * BackgroundPDFModels.h
 * 
 * Module to create PDF models for background fitting
 */
#ifndef BACKGROUND_PDF_MODELS_H
#define BACKGROUND_PDF_MODELS_H

#include <string>

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

		/* Make convolutions with Gaussian. */
		void LaurentConvGaussian ( const char* prefix,
								   RooAbsReal& ObsVar,
								   RooRealVar& mu,
								   RooRealVar& sigma,
								      	  int  max_order );
		
	private:
		/* Dictionary to store PDF models. */
	    RooGaussian _Gaussian;
		const RooAbsPdf* _Laurent;
		// TODO: dictionary of PDFs

		/* Enum to check status of PDF models. */
		enum PDFStatus { none, exists };
		int _statusGaussian = PDFStatus::none;

		/* Create and store basic PDFs. */
		void makeGaussian ( RooAbsReal& ObsVar,
							RooRealVar& mu,
							RooRealVar& sigma );
		void makeLaurentSeries ( const char* prefix,
								 RooAbsReal& ObsVar,
								        int  order );
	};
  
}

#endif // BACKGROUND_PDF_MODELS_H
