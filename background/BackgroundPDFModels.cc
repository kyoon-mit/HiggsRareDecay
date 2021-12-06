#include "BackgroundPDFModels.h"

// --- std libraries ---
#include <math.h>

// --- ROOT ---
#include "TString.h"

// --- RooFit ---
#include "RooFit.h"

#include "RooAbsReal.h"
#include "RooConstVar.h"
#include "RooRealVar.h"

#include "RooAbsPdf.h"
#include "RooGaussian.h"
#include "HiggsAnalysis/CombinedLimit/interface/HGGRooPdfs.h"

using namespace RHD; // TODO: delete this line after testing
using namespace RooFit;

namespace RHD
{

    void BackgroundPDFModels::LaurentConvGaussian ( const char* prefix,
                                                    RooAbsReal& ObsVar,
                                                    RooRealVar& mu,
                                                    RooRealVar& sigma,
                                                           int  max_order )
    /*
     * mu and sigma are variables of the Gaussian;
     * max_order is the highest order of the Laurent Series.
     */
    {
        if (_statusGaussian) {
        }
    }
    
    void BackgroundPDFModels::makeGaussian ( RooAbsReal& ObsVar,
                                             RooRealVar& mu,
                                             RooRealVar& sigma )
    {
        static RooGaussian gauss("Gaussian", "Gaussian", ObsVar, mu, sigma);
        _statusGaussian = PDFStatus::exists;
        _Gaussian = gauss;
    }

    void BackgroundPDFModels::makeLaurentSeries ( const char* prefix,
                                                  RooAbsReal& ObsVar,
                                                         int  order )
    /*
     * For original code, see:
     * https://github.com/MiT-HEP/ChargedHiggs/blob/cmssw_94x/src/BackgroundFitter.cpp#L562-L595
     */
    {
        static RooArgList args_coeff(Form("%s_coeffs", prefix));
        static RooArgList args_power(Form("%s_powers", prefix));
        float central = -4.;

        // Central order (call it "0th" order)
        const char* pow_name = Form("%s_pow0", prefix);
        static RooPower pow0(pow_name, pow_name, ObsVar, RooConst(central));
        args_power.add(pow0);
  
        int nlower = int(ceil(order/2.));
        int nhigher = order - nlower;

        // Even terms
        for (int i=1; i<=nlower; i++) {
            const char* var_name = Form("%s_l%d", prefix, i);
            const char* pow_name = Form("%s_powl%d", prefix, i);
            static RooRealVar var(var_name, var_name, 0.25/order, 1e-6, 1-1e-6);
            static RooPower pow(pow_name, pow_name, ObsVar, RooConst(central-i));
            args_coeff.add(var);
            args_power.add(pow);
        }

        // Odd terms
        for (int i=1; i<=nhigher; i++) {
            const char* var_name = Form("%s_h%d", prefix, i);
            const char* pow_name = Form("%s_powh%d", prefix, i);
            static RooRealVar var(var_name, var_name, 0.25/order, 1e-6, 1-1e-6);
            static RooPower pow(pow_name, pow_name, ObsVar, RooConst(central+i));
            args_coeff.add(var);
            args_power.add(pow);
        }

        static RooAddPdf pdf(prefix, prefix, args_power, args_coeff, true);

        _Laurent = &pdf;
    }
}
