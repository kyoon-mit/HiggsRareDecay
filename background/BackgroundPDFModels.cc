#include "BackgroundPDFModels.h"

// --- std libraries ---
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

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

    RooAbsPdf* BackgroundPDFModels::getPDF ( std::string key )
    /*
     * Get PDF in private member data, _PDFs, according to its key.
     */
    {
        if (_PDFs.find(key) == _PDFs.end()) {
            std::cout << "PDF with the following key not found: ";
            std::cout << key << std::endl;
            return nullptr;
        } else {
            return (RooAbsPdf*) _PDFs.find(key)->second;
        }
    }

    void BackgroundPDFModels::listPDFs ()
    /*
     * List all existing PDFs.
     */
    {
        std::vector<std::string> keys{};
        for (const auto &it: _PDFs) {
            keys.emplace_back(it.first);
        }
        std::sort(keys.begin(), keys.end());
        for (const auto &key: keys) {
            std::cout << key << '\n';
        }
    }

    void BackgroundPDFModels::makeLaurentConvGaussian ( RooRealVar& ObsVar,
                                                        RooRealVar& mu,
                                                        RooRealVar& sigma,
                                                               int  max_order )
    /*
     * mu and sigma are variables of the Gaussian;
     * max_order is the highest order of the Laurent Series.
     */
    {
        // Make Gaussian if not exist
        if (!_statusGaussian) {
            this->makeGaussian(ObsVar, mu, sigma);
        }

        // Fetch Gaussian
        RooAbsPdf* gauss_pdf = _PDFs.find("Gaussian")->second;

        // Make Laurent Series
        for (int order=1; order<=max_order; order++) {
            const char* lau_name = Form("lau%d", order);
            this->makeLaurentSeries(lau_name, ObsVar, order);
            RooAbsPdf* lau_pdf = _PDFs.find(lau_name)->second;

            // Make Convolution: Laurent Series (X) Gaussian
            const char* conv_name = Form("%s_X_gauss", lau_name);
            RooFFTConvPdf lauXgauss_pdf(conv_name, conv_name, ObsVar,
                                        *lau_pdf, *gauss_pdf);
            _PDFs.insert(std::pair<std::string, RooAbsPdf*>(conv_name,
                                                            &lauXgauss_pdf));
        }
    }


    
    /* Private methods */
    void BackgroundPDFModels::makeGaussian ( RooRealVar& ObsVar,
                                             RooRealVar& mu,
                                             RooRealVar& sigma )
    {
        static RooGaussian gauss_pdf("Gaussian", "Gaussian", ObsVar, mu, sigma);
        _statusGaussian = PDFStatus::exists;
        _PDFs.insert(std::pair<std::string, RooAbsPdf*>("Gaussian", &gauss_pdf));
    }

    void BackgroundPDFModels::makeLaurentSeries ( const char* prefix,
                                                  RooRealVar& ObsVar,
                                                         int  order )
    /*
     * For original code, see:
     * https://github.com/MiT-HEP/ChargedHiggs/blob/cmssw_94x/src/BackgroundFitter.cpp#L562-L595
     */
    {
        static RooArgList args_coeff(Form("%s_coeffs", prefix));
        static RooArgList args_power(Form("%s_powers", prefix));
        const char* pow_name;
        const char* var_name;
        float central = -4.;

        // Central order (call it "0th" order)
        pow_name = Form("%s_pow0", prefix);
        static RooPower pow0(pow_name, pow_name, ObsVar, RooConst(central));
        args_power.add(pow0);
        _PDFs.insert(std::pair<std::string, RooAbsPdf*>(pow_name, &pow0));
  
        int nlower = int(ceil(order/2.));
        int nhigher = order - nlower;

        // Even terms
        for (int i=1; i<=nlower; i++) {
            var_name = Form("%s_l%d", prefix, i);
            pow_name = Form("%s_powl%d", prefix, i);
            static RooRealVar var(var_name, var_name, 0.25/order, 1e-6, 1-1e-6);
            static RooPower pow(pow_name, pow_name, ObsVar, RooConst(central-i));
            args_coeff.add(var);
            args_power.add(pow);
            _Parameters.insert(std::pair<std::string, RooRealVar*>(var_name, &var));
            _PDFs.insert(std::pair<std::string, RooAbsPdf*>(pow_name, &pow));
        }

        // Odd terms
        for (int i=1; i<=nhigher; i++) {
            var_name = Form("%s_h%d", prefix, i);
            pow_name = Form("%s_powh%d", prefix, i);
            static RooRealVar var(var_name, var_name, 0.25/order, 1e-6, 1-1e-6);
            static RooPower pow(pow_name, pow_name, ObsVar, RooConst(central+i));
            args_coeff.add(var);
            args_power.add(pow);
            _Parameters.insert(std::pair<std::string, RooRealVar*>(var_name, &var));
            _PDFs.insert(std::pair<std::string, RooAbsPdf*>(pow_name, &pow));
        }
        
        // PDF
        static RooAddPdf lau_pdf(prefix, prefix, args_power, args_coeff, true);
        _PDFs.insert(std::pair<std::string, RooAbsPdf*>(prefix, &lau_pdf));
    }
}
