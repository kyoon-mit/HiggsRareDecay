#include "PDFModels.h"

// --- std libraries ---
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

// --- ROOT ---
#include "TString.h"

// --- RooFit ---
#include "RooFit.h"

#include "RooAbsReal.h"
#include "RooConstVar.h"
#include "RooRealVar.h"

#include "RooArgList.h"

#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooGaussian.h"
#include "RooFFTConvPdf.h"
#include "HiggsAnalysis/CombinedLimit/interface/HGGRooPdfs.h"

using namespace RHD; // TODO: delete after  testing
using namespace RooFit;

namespace RHD
{
    // RooAbsPdf& PDFModels::getPDF ( std::string key )
    // /*
    //  * Get PDF in private member data, _PDFs, according to its key.
    //  */
    // {
    //     if (_PDFs.find(key) == _PDFs.end()) {
    //         throw std::runtime_error("PDF with the following key not found: " + key);
    //     } else {
    //         return _PDFs[key];
    //     }
    // }

    
    // void PDFModels::listPDFs ()
    // /*
    //  * List all existing PDFs.
    //  */
    // {
    //     std::vector<std::string> keys{};
    //     for (const auto &it: _PDFs) {
    //         keys.emplace_back(it.first);
    //     }
    //     std::sort(keys.begin(), keys.end());
    //     for (const auto &key: keys) {
    //         std::cout << key << '\n';
    //     }
    // }

        
    RooAbsPdf* PDFModels::makeLaurentConvGaussian ( RooRealVar& ObsVar,
                                                            int order )
    {
        // Make Gaussian if not exist
        RooAbsPdf* gauss_pdf;
        if (!_statusGaussian) {
            gauss_pdf = makeGaussian(ObsVar);
        } else {
            gauss_pdf = _PDFs["Gaussian"].get();
        }

        // Make Laurent Series
        RooAbsPdf* lau_pdf = makeLaurentSeries(ObsVar, order);

        // Make Convolution: Laurent Series (X) Gaussian
        auto conv_name = Form("lau%d_X_gauss", order);
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (conv_name, std::make_unique<RooFFTConvPdf>
                      (RooFFTConvPdf(conv_name, conv_name,
                                     ObsVar, *lau_pdf, *gauss_pdf))
                     )
                    );

        return _PDFs[conv_name].get();
    }

    
    RooAbsPdf* PDFModels::makeGaussian ( RooRealVar& ObsVar )
    {
        double xlow = ObsVar.getMin();
        double xhigh = ObsVar.getMax();
        
        storeRooRealVar("gauss_mu", (xhigh+xlow)/2., xlow, xhigh);
        storeRooRealVar("gauss_sigma", (xhigh-xlow)/10., 1e-2, (xhigh-xlow)/2.);
        storeRooGaussian("Gaussian", ObsVar,
                          _Parameters["gauss_mu"],
                          _Parameters["gauss_sigma"]);

        return _PDFs["Gaussian"].get();
    }
    

    RooAbsPdf* PDFModels::makeLaurentSeries ( RooRealVar& ObsVar,
                                                      int order )
    /*
     * For original code, see:
     * https://github.com/MiT-HEP/ChargedHiggs/blob/cmssw_94x/src/BackgroundFitter.cpp#L562-L595
     */
    {
        // Name and central order
        auto prefix = Form("lau%d", order);
        float central = -4.;        

        // Make RooArgLists
        auto powlist_name = Form("%s_pdfs", prefix);
        auto clist_name = Form("%s_coeffs", prefix);
        auto args_coeff = RooArgList(clist_name);
        auto args_power = RooArgList(powlist_name);
        
        // Loop setup
        char* pow_name;
        char* var_name;
        int nlower = int(ceil(order/2.));
        int nhigher = order - nlower;
        
        // Make central order (call it "0th" order)
        pow_name = Form("%s_pow0", prefix);
        storeRooPower(pow_name, ObsVar, central);
        args_power.add(*_PDFs[pow_name]);
            
        // Even terms
        for (int i=1; i<=nlower; i++) {
            var_name = Form("%s_l%d", prefix, i);
            pow_name = Form("%s_powl%d", prefix, i);
            storeRooRealVar(var_name, 0.25/order, 1e-6, 1-1e-6);
            storeRooPower(pow_name, ObsVar, central-i);
            args_coeff.add(_Parameters[var_name]);
            args_power.add(*_PDFs[pow_name]);
        }

        // Odd terms
        for (int i=1; i<=nhigher; i++) {
            var_name = Form("%s_h%d", prefix, i);
            pow_name = Form("%s_powh%d", prefix, i);
            storeRooRealVar(var_name, 0.25/order, 1e-6, 1-1e-6);
            storeRooPower(pow_name, ObsVar, central+i);
            args_coeff.add(_Parameters[var_name]);
            args_power.add(*_PDFs[pow_name]);
        }

        storeRooArgList(clist_name, args_coeff);
        storeRooArgList(powlist_name, args_power);
        
        // PDF
        RooAddPdf* pdf = new RooAddPdf(prefix, prefix,
                                       _RooArgLists[powlist_name],
                                       _RooArgLists[clist_name], true);
                                      // This is the only method that works

        return pdf;
    }

    
    /* Private methods for internal bookeeping. */
    void PDFModels::storeRooRealVar ( std::string const& key,
                                                  double value,
                                                  double min,
                                                  double max )
    {
        const char* k  = key.c_str();
        _Parameters.insert(std::pair<std::string, RooRealVar>
                                 (k, RooRealVar(k, k, value, min, max)));      
    }

    void PDFModels::storeRooArgList ( std::string const& key,
                                             RooArgList& list )
    {
        const char* k = key.c_str();
        _RooArgLists.insert(std::pair<std::string, RooArgList> (k, list));
    }
    
    void PDFModels::storeRooGaussian ( std::string const& key,
                                              RooRealVar& ObsVar,
                                              RooRealVar& mu,
                                              RooRealVar& sigma )
    {
        const char* k = key.c_str();
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (k, std::make_unique<RooGaussian>
                      (RooGaussian(k, k, ObsVar, mu, sigma))
                     )
                    );
        _statusGaussian = PDFStatus::exists;
        std::cout << "Created Gaussian PDF with the following key: ";
        std::cout << key << std::endl;
    }

    void PDFModels::storeRooPower ( std::string const& key,
                                           RooRealVar& ObsVar,
                                                 float power )
    {
        const char* k = key.c_str();
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (k, std::make_unique<RooPower>
                      (RooPower(k, k, ObsVar, RooConst(power)))
                     )
                    );
        std::cout << "Created Power PDF with the following key: ";
        std::cout << key << std::endl;
    }

    void PDFModels::storeRooAddPdf ( std::string const& key,
                                             RooArgList pdfs,
                                             RooArgList coeffs,
                                                   bool recursive )
    {
        const char* k = key.c_str();
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (k, std::make_unique<RooAddPdf>
                      (RooAddPdf(k, k, pdfs, coeffs, recursive))
                     )
                    );
        std::cout << "Created RooAddPdf with the following key: ";
        std::cout << key << std::endl;
    }
}
