#include "BackgroundPDFModels.h"

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

#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooGaussian.h"
#include "HiggsAnalysis/CombinedLimit/interface/HGGRooPdfs.h"

using namespace RHD; // TODO: delete this line after testing
using namespace RooFit;

namespace RHD
{
    // RooAbsPdf& BackgroundPDFModels::getPDF ( std::string key )
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

    
    // void BackgroundPDFModels::listPDFs ()
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

        
    // void BackgroundPDFModels::makeLaurentConvGaussian ( RooRealVar& ObsVar,
    //                                                            int  min_order,
    //                                                            int  max_order )
    // /*
    //  * min_order and max_order are the lowest and highest orders
    //  * of the Laurent Series.
    //  */
    // {
    //     // Make Gaussian if not exist
    //     if (!_statusGaussian) {
    //         this->makeGaussian(ObsVar);
    //     }

    //     // Fetch Gaussian
    //     RooAbsPdf& gauss_pdf = _PDFs["Gaussian"];

    //     char* lau_name;
    //     char* conv_name;
        
    //     // Make Laurent Series
    //     for (int order=min_order; order<=max_order; order++) {
    //         this->makeLaurentSeries(ObsVar, order);
    //         lau_name = Form("lau%d", order);
    //         RooAbsPdf* lau_pdf = _PDFs.find(lau_name)->second.get();

    //         // Make Convolution: Laurent Series (X) Gaussian
    //         conv_name = Form("%s_X_gauss", lau_name);
    //         RooFFTConvPdf lauXgauss_pdf(conv_name, conv_name, ObsVar,
    //                                            *lau_pdf, *gauss_pdf);
            
    //         // Insert into map of PDFs
    //         //storePDF(conv_name, std::make_unique<RooFFTConvPdf>(lauXgauss_pdf));
    //         std::cout << "Created PDF with the following key: ";
    //         std::cout << conv_name << std::endl;
    //     }
    // }

    
    RooAbsPdf* BackgroundPDFModels::makeGaussian ( RooRealVar& ObsVar )
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
    

    RooAbsPdf* BackgroundPDFModels::makeLaurentSeries ( RooRealVar& ObsVar,
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
        RooAbsPdf* pdf = new RooAddPdf(prefix, prefix, _RooArgLists[powlist_name], _RooArgLists[clist_name], true);
        //storeRooAddPdf(prefix,// *args_power, *args_coeff, true);
        //               _RooArgLists[powlist_name],
        //               _RooArgLists[clist_name],
        //               true);

        return pdf;
        // return _PDFs[prefix].get();
    }

    
    /* Private methods for internal bookeeping. */
    void BackgroundPDFModels::storeRooRealVar ( std::string const& key,
                                                            double value,
                                                            double min,
                                                            double max )
    {
        const char* k  = key.c_str();
        _Parameters.insert(std::pair<std::string, RooRealVar>
                                 (k, RooRealVar(k, k, value, min, max)));      
    }

    void BackgroundPDFModels::storeRooArgList ( std::string const& key,
                                                       RooArgList& list )
    {
        const char* k = key.c_str();
        _RooArgLists.insert(std::pair<std::string, RooArgList> (k, list));
    }
    
    void BackgroundPDFModels::storeRooGaussian ( std::string const& key,
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

    void BackgroundPDFModels::storeRooPower ( std::string const& key,
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

    void BackgroundPDFModels::storeRooAddPdf ( std::string const& key,
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
