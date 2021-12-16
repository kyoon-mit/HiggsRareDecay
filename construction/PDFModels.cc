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
#include "RooBernstein.h"
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


    RooAbsPdf* PDFModels::makeDoubleGaussian ( RooRealVar& ObsVar )
    {
        double xlow = ObsVar.getMin();
        double xhigh = ObsVar.getMax();

        storeRooRealVar("digauss_mu1", xlow, xhigh);
        storeRooRealVar("digauss_mu2", xlow, xhigh);
        storeRooRealVar("digauss_sigma1", 1e-2, (xhigh-xlow)/2.);
        storeRooRealVar("digauss_sigma2", 1e-2, (xhigh-xlow)/2.);
        storeRooGaussian("digauss_pdf1", ObsVar,
                         _Parameters["digauss_mu1"],
                         _Parameters["digauss_sigma1"]);
        storeRooGaussian("digauss_pdf2", ObsVar,
                         _Parameters["digauss_mu2"],
                         _Parameters["digauss_sigma2"]);
        storeRooRealVar("digauss_c1", 0.5, 0., 1.);

        auto args_coeff = RooArgList(_Parameters["digauss_c1"]);
        auto args_power = RooArgList(*_PDFs["digauss_pdf1"],
                                     *_PDFs["digauss_pdf2"]);

        storeRooArgList("digauss_coeffs", args_coeff);
        storeRooArgList("digauss_pdfs", args_power);
        
        RooAddPdf* pdf = new RooAddPdf("digauss", "digauss",
                                       _RooArgLists["digauss_pdfs"],
                                       _RooArgLists["digauss_coeffs"]);
        return pdf;
    }
    

    RooAbsPdf* PDFModels::makeTripleGaussian ( RooRealVar& ObsVar )
    {
        double xlow = ObsVar.getMin();
        double xhigh = ObsVar.getMax();

        storeRooRealVar("trigauss_mu1", xlow, xhigh);
        storeRooRealVar("trigauss_mu2", xlow, xhigh);
        storeRooRealVar("trigauss_mu3", xlow, xhigh);
        storeRooRealVar("trigauss_sigma1", 1e-2, (xhigh-xlow)/2.);
        storeRooRealVar("trigauss_sigma2", 1e-2, (xhigh-xlow)/2.);
        storeRooRealVar("trigauss_sigma3", 1e-2, (xhigh-xlow)/2.);
        storeRooGaussian("trigauss_pdf1", ObsVar,
                         _Parameters["trigauss_mu1"],
                         _Parameters["trigauss_sigma1"]);
        storeRooGaussian("trigauss_pdf2", ObsVar,
                         _Parameters["trigauss_mu2"],
                         _Parameters["trigauss_sigma2"]);
        storeRooGaussian("trigauss_pdf3", ObsVar,
                         _Parameters["trigauss_mu3"],
                         _Parameters["trigauss_sigma3"]);
        storeRooRealVar("trigauss_c1", 0.33, 0., 1.);
        storeRooRealVar("trigauss_c2", 0.33, 0., 1.);

        auto args_coeff = RooArgList(_Parameters["trigauss_c1"],
                                     _Parameters["trigauss_c2"]);
        auto args_power = RooArgList(*_PDFs["trigauss_pdf1"],
                                     *_PDFs["trigauss_pdf2"],
                                     *_PDFs["trigauss_pdf3"]);

        storeRooArgList("trigauss_coeffs", args_coeff);
        storeRooArgList("trigauss_pdfs", args_power);
        
        RooAddPdf* pdf = new RooAddPdf("trigauss", "trigauss",
                                       _RooArgLists["trigauss_pdfs"],
                                       _RooArgLists["trigauss_coeffs"]);
        return pdf;
    }


    RooAbsPdf* PDFModels::makePowerSeries ( RooRealVar& ObsVar,
                                                    int order )
    /*
     * PDF built from expression of the following form:
     * c1*ObsVar^p1 + c2*ObsVar^p2 + ... + (1-c1-c2-...-cn)*ObsVar^pn
     */
    {
        
        std::string formula = ""; // PDF expression
        auto prefix = Form("powsrs%d", order); // power series
        auto plist_name = Form("%s_vars", prefix);
        auto args_param = RooArgList(plist_name);
        args_param.add(ObsVar);
        
        char* coeff_name;
        char* pow_name;
        char* ci_form; // expression of first and middle coeffs
        char* pi_form; // expression of frist and middle powers
        std::string clast_form; // expression of last coeff

        if (order==1) { // if only one term
            pow_name = Form("%s_p1", prefix);
            storeRooRealVar(pow_name, -4, -10., 0.);
            args_param.add(_Parameters[pow_name]);
            formula = "TMath::Power(@0, @1)";
        } else { // if two or more terms
            for (int i=1; i<order; i++) {
                coeff_name = Form("%s_c%d", prefix, i);
                pow_name = Form("%s_p%d", prefix, i);

                storeRooRealVar(coeff_name, 0.1, 0., 1.);
                storeRooRealVar(pow_name, std::max(-10., -std::pow((i+1), 2.)),
                                -std::pow((i+1), 2.), 0.);
                args_param.add(_Parameters[coeff_name]);
                args_param.add(_Parameters[pow_name]);
                
                ci_form = Form("@%d", 2*i-1);
                clast_form += Form("-%s", ci_form);
                pi_form = Form("TMath::Power(@0, @%d)", 2*i);
                formula += Form("%s*%s", ci_form, pi_form);
                formula += "+";
            }

            // final term
            pow_name = Form("%s_p%d", prefix, order);
            storeRooRealVar(pow_name, -std::pow((order+1), 2.),
                            std::max(-10., -std::pow((order+1), 2.)), 0.);
            args_param.add(_Parameters[pow_name]);
            formula += Form("(1%s)*TMath::Power(@0, @%d)",
                            clast_form.c_str(), 2*order-1);
        }

        std::cout << formula << std::endl;
        args_param.Print("v");
        storeRooArgList(plist_name, args_param);
        storeRooGenericPdf(prefix, formula.c_str(), _RooArgLists[plist_name]);
        /*
        RooGenericPdf* pow = new RooGenericPdf(prefix, prefix,
                                               formula.c_str(),
                                               _RooArgLists[plist_name]);
        pow->Print("v");
        */
        return _PDFs[prefix].get();
    }


    RooAbsPdf* PDFModels::makeBernsteinPoly ( RooRealVar& ObsVar,
                                                     int order )
    {
        auto prefix = Form("bern%d", order);
        auto clist_name = Form("%s_coeffs", prefix);
        auto args_coeff = RooArgList(clist_name);
        
        for (int i=0; i<=order; i++) {
            auto coeff_name = Form("%s_c%d", prefix, order);
            storeRooRealVar(coeff_name, 0., 1.);
            args_coeff.add(_Parameters[coeff_name]);
        }

        storeRooArgList(clist_name, args_coeff);
        storeRooBernstein(prefix, ObsVar, _RooArgLists[clist_name]);

        return _PDFs[prefix].get();
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
        auto clist_name = Form("%s_coeffs", prefix);
        auto powlist_name = Form("%s_pdfs", prefix);
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
    void PDFModels::storeRooRealVar ( const char* key,
                                           double value,
                                           double min,
                                           double max )
    {
        _Parameters.insert(std::pair<std::string, RooRealVar>
                                 (key, RooRealVar(key, key, value, min, max)));      
    }

    void PDFModels::storeRooRealVar ( const char* key,
                                           double min,
                                           double max )
    {
        _Parameters.insert(std::pair<std::string, RooRealVar>
                                 (key, RooRealVar(key, key, min, max)));      
    }

    void PDFModels::storeRooArgList ( const char* key,
                                      RooArgList& list )
    {
        _RooArgLists.insert(std::pair<std::string, RooArgList> (key, list));
    }
    
    void PDFModels::storeRooGaussian ( const char* key,
                                       RooRealVar& ObsVar,
                                       RooRealVar& mu,
                                       RooRealVar& sigma )
    {
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (key, std::make_unique<RooGaussian>
                      (RooGaussian(key, key, ObsVar, mu, sigma))
                     )
                    );
        _statusGaussian = PDFStatus::exists;
        std::cout << "Created Gaussian PDF with the following key: ";
        std::cout << key << std::endl;
    }

    void PDFModels::storeRooPower ( const char* key,
                                    RooRealVar& ObsVar,
                                          float power )
    {
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (key, std::make_unique<RooPower>
                      (RooPower(key, key, ObsVar, RooConst(power)))
                     )
                    );
        std::cout << "Created Power PDF with the following key: ";
        std::cout << key << std::endl;
    }

    void PDFModels::storeRooBernstein ( const char* key,
                                        RooRealVar& ObsVar,
                                        RooArgList& coeffs )
    {
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (key, std::make_unique<RooBernstein>
                      (RooBernstein(key, key, ObsVar, coeffs))
                     )
                    );
        std::cout << "Create Bernstein PDF with the following key: ";
        std::cout << key << std::endl;
    }

    void PDFModels::storeRooAddPdf ( const char* key,
                                     RooArgList& pdfs,
                                     RooArgList& coeffs,
                                            bool recursive )
    { // Doesn't work
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (key, std::make_unique<RooAddPdf>
                      (RooAddPdf(key, key, pdfs, coeffs, recursive))
                     )
                    );
        std::cout << "Created RooAddPdf with the following key: ";
        std::cout << key << std::endl;
    }

    void PDFModels::storeRooGenericPdf ( const char* key,
                                         const char* formula,
                                         RooArgList& param_list )
    {
        _PDFs.insert(std::pair<std::string, std::unique_ptr<RooAbsPdf>>
                     (key, std::make_unique<RooGenericPdf>
                      (RooGenericPdf(key, key, formula, param_list))
                     )
                    );
        std::cout << "Created RooGenericPdf with the following key: ";
        std::cout << key << std::endl;
        std::cout << "   and expression: " << std::endl;
        std::cout << "   " << formula;
        std::cout << "   and params: " << std::endl;
        param_list.Print();
    }
}
