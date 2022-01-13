#include "FitData.h"
#include "PDFModels.h"

#include <boost/filesystem.hpp>

// --- STD ---
#include <iostream>
#include <stdexcept>

// --- ROOT ---
#include "TCanvas.h"
#include "TPad.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TMath.h"
#include "TStyle.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// --- RooFit ---
#include "RooFit.h"
#include "RooFitResult.h"
#include "RooMsgService.h"
#include "RooWorkspace.h"

#include "RooRealVar.h"

#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCategory.h"

#include "RooAbsPdf.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooHist.h"
#include "RooPlot.h"

#include "HiggsAnalysis/CombinedLimit/interface/RooMultiPdf.h"

using namespace RHD; // TODO: delete after testing
using namespace RooFit;
namespace fs = boost::filesystem;

namespace RHD
{
    /* Constructor. */
    FitData::FitData () :
        _SAVEOPTION(false),
        _SAVEDIR("./"),
        _OUTFILENAME("FitData_results.root"),
        _WORKSPACENAME("w")
    {}

    FitData::FitData ( const char* outfileName,
                       const char* saveDir,
                       const char* workspaceName ) :
        _SAVEOPTION(true),
        _SAVEDIR(saveDir),
        _OUTFILENAME(outfileName),
        _WORKSPACENAME(workspaceName)
    {
        // fs::path save_path = fs::canonical(fs::path(_SAVEDIR)) / fs::path(_OUTFILENAME);
        // _SAVEPATHFULL = save_path.string();
        makePaths();

        std::cout << "Recreating ROOT file " << _SAVEPATHFULL << std::endl;
        
        TFile* outfile = TFile::Open(_SAVEPATHFULL.c_str(), "RECREATE");
        std::cout << "Creating workspace \"" << _WORKSPACENAME << "\"" << std::endl;
        RooWorkspace w(_WORKSPACENAME);
        w.Write();
        outfile->Close();
    }


    /* Public methods. */
    void FitData::setSaveOption ( bool saveOption, bool recreate )
    {
        if (!_SAVEOPTION && saveOption) { // i.e. if setting first time
            _SAVEOPTION = true;
            makePaths();
            
            if (recreate) {
                std::cout << "Recreating ROOT file " << _SAVEPATHFULL << std::endl;
                TFile* outfile = TFile::Open(_SAVEPATHFULL.c_str(), "RECREATE");
                std::cout << "Creating workspace \"" << _WORKSPACENAME << "\"" << std::endl;
                RooWorkspace w(_WORKSPACENAME);
                w.Write();
                outfile->Close();
            }   
        } else {
            _SAVEOPTION = saveOption;
        }
    }


    void FitData::saveMultiPdf ( const std::vector<const char*>& pdfNameList,
                                                     const char* multipdfName )
    /*
     * Fetches PDFs in the OUTFILE specified by pdfNameList, adds them in
     * a RooMultiPdf instance, and saves the RooMultiPdf to the WORKSPACE
     * in the OUTFILE.
     */
    {
        if (!_SAVEOPTION) {
            std::cout << "SAVEOPTION is set to false. "
                      << "Not creating RooMultiPdf." << std::endl;
            return;
        } else {
            // Book containers
            RooCategory pdf_index("pdf_index", "pdf_index");
            RooArgList pdf_list;

            // Open OUTFILE
            TFile* outfile = TFile::Open(_SAVEPATHFULL.c_str(), "UPDATE");
            auto wspace = (RooWorkspace*) outfile->Get(_WORKSPACENAME);
            for (auto const& pdf_name: pdfNameList) {
                RooAbsPdf* pdf = wspace->pdf(pdf_name);
                if (pdf) {
                    pdf_list.add(*pdf);
                } else {
                    std::cout << "PDF named " << pdf_name
                              << " not found in file " << _SAVEPATHFULL
                              << " and workspace " << _WORKSPACENAME << std::endl;
                }
            }
            RooMultiPdf multipdf(multipdfName, multipdfName, pdf_index, pdf_list);
            wspace->import(multipdf);
            wspace->writeToFile(_SAVEPATHFULL.c_str());
            outfile->Close();
        }
    }

    
    TH1F FitData::fetchHistogram ( const std::vector<const char*>& fileNames,
                                                       const char* treeName,
                                                       const char* branchName,
                                                       const char* histName,
                                                       const char* histTitle,
                                                               int nbins,
                                                            double xlow,
                                                            double xhigh )
    /*
     * Fetch histograms from data files.
     */
    {
        auto hist = TH1F(histName, histTitle, nbins, xlow, xhigh);

        for (const auto& fname: fileNames) {
            auto file = TFile(fname, "READ");
            TTreeReader reader(treeName, &file);
            TTreeReaderValue<float> var(reader, branchName);
            while (reader.Next()) {
                hist.Fill(*var);
            }
        }

        return hist;
    }

    
    RooDataHist FitData::makeBinnedData ( const char* name,
                                          RooRealVar& ObsVar,
                                                 TH1& hist )
    {
        _DataHistograms.insert(std::pair<std::string, RooDataHist>
                               (name, RooDataHist(name, name, ObsVar, &hist))
                              );
        if (_SAVEOPTION) {
            TFile* saveFile = TFile::Open(_SAVEPATHFULL.c_str(), "UPDATE");
            auto wspace = (RooWorkspace*) saveFile->Get(_WORKSPACENAME);
            wspace->import(_DataHistograms[name]);
            wspace->writeToFile(_SAVEPATHFULL.c_str());
            saveFile->Close();
        }
        return _DataHistograms[name];
    }


    RooDataSet FitData::makeUnbinnedData ( const char* name,
                                           RooRealVar& ObsVar,
                                                TTree* tree )
    /*
     * ObsVar must be the same name as variable in the tree.
     */
    {
        _DataSets.insert(std::pair<std::string, RooDataSet>
                         (name, RooDataSet(name, name, tree, ObsVar))
                        );
        if (_SAVEOPTION) {
            std::cout << _SAVEPATHFULL << std::endl;
            TFile* saveFile = TFile::Open(_SAVEPATHFULL.c_str(), "UPDATE");
            auto wspace = (RooWorkspace*) saveFile->Get(_WORKSPACENAME);
            wspace->import(_DataSets[name]);
            wspace->writeToFile(_SAVEPATHFULL.c_str());
            saveFile->Close();
        }
        return _DataSets[name];
    }

    
    RooFitResult FitData::performChi2Fit (   RooAbsPdf& pdf,
                                           RooDataHist& data,
                                                    int maxTries,
                                                    int retry )
    {
        RooArgSet* params = pdf.getParameters((const RooArgSet*)(0));
        RooFitResult* fitResult;
        int status = 5;
        int ntries = 0;
        int retrycount = retry;
        
        std::cout << "----------- ";
        std::cout << "Chi2 fitting " << pdf.GetTitle() << " to " << data.GetTitle();
        std::cout << " -----------" << std::endl;
        
        // Try fit as many times as needed to converge
        RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
        while (++ntries <= maxTries) {
            std::cout << "trial #" << ntries << std::endl;
            fitResult = pdf.chi2FitTo(data,
                                      RooFit::Save(true),
                                      RooFit::Minimizer("Minuit2", "minimize"),
                                      RooFit::PrintLevel(-1),
                                      RooFit::Warnings(false),
                                      RooFit::PrintEvalErrors(-99)
                                     );
            status = fitResult->status();
            // If fit status is not 0, randomize parameters
            if (status) {
                params->assignValueOnly(fitResult->randomizePars());
            } else {
                // Retry if argument given is > 0
                if (retrycount <= 0) {
                    fitResult->floatParsFinal().Print("s");
                    break;
                } else {
                    params->assignValueOnly(fitResult->randomizePars());
                    retrycount--;
                }
            }
        }

        // Check final fit status
        std::string message;
        if (status == 0) {
            message = "Chi2 fit has converged.";
        } else if (status == 1) {
            message = "Chi2 fit exited with status = 1: "
                      "Covariance was made pos defined.";
        } else if (status == 2) {
            message = "Chi2 fit exited with status = 2: "
                      "Hesse is invalid.";
        } else if (status == 3) {
            message = "Chi2 fit exited with status = 3: "
                      "Edm is above max.";
        } else if (status == 4) {
            message = "Chi2 fit exited with status = 4: "
                      "Reached call limit.";
        } else if (status == 5) {
            message = "Chi2 fit exited with status = 5: "
                      "Please investigate.";
        } else {
            message = "Something is amiss!";
        }

        std::cout << message << std::endl;
        std::cout << "---------------------------------------------------------\n" << std::endl;
        return *fitResult;
    }


    template<typename T>
    RooFitResult FitData::performLikelihoodFit (  RooAbsPdf& pdf,
                                                          T& data,
                                                         int maxTries,
                                                         int retry )
    {
        RooArgSet* params = pdf.getParameters((const RooArgSet*)(0));
        RooFitResult* fitResult;
        int status = 5;
        int ntries = 0;
        int retrycount = retry;
        
        std::cout << "----------- ";
        std::cout << "Likelihood fitting " << pdf.GetTitle() << " to " << data.GetTitle();
        std::cout << " -----------" << std::endl;
        
        // Try fit as many times as needed to converge
        RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
        while (++ntries <= maxTries) {
            std::cout << "trial #" << ntries << std::endl;
            fitResult = pdf.fitTo(data,
                                  RooFit::Save(true),
                                  RooFit::Minimizer("Minuit2", "minimize"),
                                  RooFit::PrintLevel(-1),
                                  RooFit::Warnings(false),
                                  RooFit::PrintEvalErrors(-99)
                                 );
            status = fitResult->status();
            // If fit status is not 0, randomize parameters
            if (status) {
                params->assignValueOnly(fitResult->randomizePars());
            } else {
                // Retry if argument given is > 0
                if (retrycount <= 0) {
                    fitResult->floatParsFinal().Print("s");
                    break;
                } else {
                    params->assignValueOnly(fitResult->randomizePars());
                    retrycount--;
                }
            }
        }

        // Check final fit status
        std::string message;
        if (status == 0) {
            message = "Likelihood fit has converged.";
        } else if (status == 1) {
            message = "Likelihood fit exited with status = 1: "
                      "Covariance was made pos defined.";
        } else if (status == 2) {
            message = "Likelihood fit exited with status = 2: "
                      "Hesse is invalid.";
        } else if (status == 3) {
            message = "Likelihood fit exited with status = 3: "
                      "Edm is above max.";
        } else if (status == 4) {
            message = "Likelihood fit exited with status = 4: "
                      "Reached call limit.";
        } else if (status == 5) {
            message = "Likelihood fit exited with status = 5: "
                      "Please investigate.";
        } else {
            message = "Something is amiss!";
        }

        std::cout << message << std::endl;
        std::cout << "---------------------------------------------------------\n" << std::endl;
        return *fitResult;
    }
    

    void FitData::performMultiChi2Fit (                const char* pdfType,
                                                       RooRealVar& ObsVar,
                                                      RooDataHist& data,
                                        const std::vector<double>& initParamValues1,
                                        const std::vector<double>& initParamValues2,
                                                            double fTestAlpha )
    /*
     * Performs chi2 fit of the pdf type provided.
     * Scans from lowest order of pdf type to higher, until value obtained from
     * performing the F-test is greater than fTestAlpha.
     * To faciliate fitting, provide initParamValues in order of parameters in
     * the parameter list. If size of initParamValues is smaller/bigger than the
     * number of free parameters, only the first few parameters will be set.
     */
    {
        // Make containers for PDF pointers and chi2-distribution variables
        std::vector<RooAbsPdf*> pdfs;
        std::vector<double> nlls;
        std::vector<double> dofs;
        
        auto models = PDFModels();
        
        double fProb = 0;
        int order = 1;
        int plist1_size = initParamValues1.size();
        int plist2_size = initParamValues2.size();
        int retry;

        while (fProb < fTestAlpha) {
            // Set initial parameter values for fit
            std::vector<double> params1(initParamValues1.begin(),
                                        initParamValues1.begin() + std::min(order, plist1_size));
            std::vector<double> params2(initParamValues2.begin(),
                                        initParamValues2.begin() + std::min(order, plist2_size));
        
            // Make PDF according to type given
            if (std::strcmp(pdfType, "expXgauss") == 0) {
                pdfs.push_back(models.makeExponentialConvGaussian(ObsVar, order));
                retry = 1;
                models.setMultiVals(Form("expsrs%d_c", order),
                                    1, std::min(order, plist1_size),
                                    params1);
                models.setMultiVals(Form("expsrs%d_p", order),
                                    1, std::min(order, plist2_size),
                                    params2);
            
            } else if (std::strcmp(pdfType, "bernXgauss") == 0) {
                pdfs.push_back(models.makeBernsteinConvGaussian(ObsVar, order));
                retry = 1;
                models.setMultiVals(Form("bern%d_c", order),
                                    0, std::min(order, plist1_size),
                                    params1);

            } else if (std::strcmp(pdfType, "lauXgauss") == 0) {
                pdfs.push_back(models.makeLaurentConvGaussian(ObsVar, order));
                retry = 2;
                models.setMultiVals(Form("lau%d_h", order),
                                    1, std::min(order+1, plist1_size+1),
                                    params1);
                models.setMultiVals(Form("lau%d_l", order),
                                    1, std::min(order+1, plist2_size+1),
                                    params2);
                
            } else if (std::strcmp(pdfType, "powXgauss") == 0) {
                pdfs.push_back(models.makePowerConvGaussian(ObsVar, order));
                retry = 2;
                if (order > 1) {
                    models.setMultiVals(Form("powsrs%d_c", order),
                                        1, std::min(order+1, plist1_size+1),
                                        params1);
                    models.setMultiVals(Form("powsrs%d_p", order),
                                        1, std::min(order+1, plist2_size+1),
                                        params2);
                }
            }

            // Perform chi2 fit
            RooFitResult fit = performChi2Fit(*(pdfs.at(order-1)), data, 150, retry);
            nlls.push_back(fit.minNll());
            dofs.push_back(fit.covarianceMatrix().GetNcols());

            if (std::strcmp(pdfType, "powXgauss") == 0) {
                if (order < 2) {
                    order++;
                    continue;
                }
            }
            
            if (order >= 2) {
                // Calculate F-test probability using chi2 distribution
                fProb = getWilksProb(nlls.at(order-2), nlls.at(order-1),
                                     dofs.at(order-2), dofs.at(order-1));
                
                std::cout << "###################################################" << std::endl;
                std::cout << "   F-test probability for " << pdfType << "\n"
                          << "   between orders " << order
                          << " and " << (order-1) << " : " << std::endl;
                std::cout << "             " << fProb << "\n" << std::endl;
                std::cout << "###################################################\n" << std::endl;
            }
            
            // Plot individual PDFs
            plotPDF(ObsVar, *(pdfs.at(order-1)), data);
            order++;
        } // End while-loop

        // Save to workspace and file
        if (_SAVEOPTION) {
            TFile* saveFile = TFile::Open(_SAVEPATHFULL.c_str(), "UPDATE");
            auto wspace = (RooWorkspace*) saveFile->Get(_WORKSPACENAME);
            for (const auto& pdf: pdfs) {
                RooArgSet* params = (RooArgSet*) pdf->getParameters(ObsVar);
                wspace->import(*pdf);
                wspace->saveSnapshot(Form("%s_chi2", pdf->GetName()), *params, true);
            }
            wspace->writeToFile(_SAVEPATHFULL.c_str());
            saveFile->Close();
        }
    }


    template<typename T>
    void FitData::performMultiLikelihoodFit (                const char* pdfType,
                                                             RooRealVar& ObsVar,
                                                                      T& data,
                                              const std::vector<double>& initParamValues1,
                                              const std::vector<double>& initParamValues2,
                                                                  double fTestAlpha )
    /*
     * Performs likelihood fit of the pdf type provided.
     * Scans from lowest order of pdf type to higher, until value obtained from
     * performing the F-test is greater than fTestAlpha.
     * To faciliate fitting, provide initParamValues in order of parameters in
     * the parameter list. If size of initParamValues is smaller/bigger than the
     * number of free parameters, only the first few parameters will be set.
     */
    {
        // Make containers for PDF pointers and chi2-distribution variables
        std::vector<RooAbsPdf*> pdfs;
        std::vector<double> nlls;
        std::vector<double> dofs;
        
        auto models = PDFModels();
        
        double fProb = 0;
        int order = 1;
        int plist1_size = initParamValues1.size();
        int plist2_size = initParamValues2.size();
        int retry;

        while (fProb < fTestAlpha) {
            // Set initial parameter values for fit
            std::vector<double> params1(initParamValues1.begin(),
                                        initParamValues1.begin() + std::min(order, plist1_size));
            std::vector<double> params2(initParamValues2.begin(),
                                        initParamValues2.begin() + std::min(order, plist2_size));
        
            // Make PDF according to type given
            if (std::strcmp(pdfType, "expXgauss") == 0) {
                pdfs.push_back(models.makeExponentialConvGaussian(ObsVar, order));
                retry = 1;
                models.setMultiVals(Form("expsrs%d_c", order),
                                    1, std::min(order, plist1_size),
                                    params1);
                models.setMultiVals(Form("expsrs%d_p", order),
                                    1, std::min(order, plist2_size),
                                    params2);
                
            } else if (std::strcmp(pdfType, "bernXgauss") == 0) {
                pdfs.push_back(models.makeBernsteinConvGaussian(ObsVar, order));
                retry = 1;
                models.setMultiVals(Form("bern%d_c", order),
                                    0, std::min(order, plist1_size),
                                    params1);

            } else if (std::strcmp(pdfType, "lauXgauss") == 0) {
                pdfs.push_back(models.makeLaurentConvGaussian(ObsVar, order));
                retry = 2;
                models.setMultiVals(Form("lau%d_h", order),
                                    1, std::min(order+1, plist1_size+1),
                                    params1);
                models.setMultiVals(Form("lau%d_l", order),
                                    1, std::min(order+1, plist2_size+1),
                                    params2);
                
            } else if (std::strcmp(pdfType, "powXgauss") == 0) {
                pdfs.push_back(models.makePowerConvGaussian(ObsVar, order));
                retry = 2;
                models.setMultiVals(Form("powsrs%d_c", order),
                                    1, std::min(order+1, plist1_size+1),
                                    params1);
                models.setMultiVals(Form("powsrs%d_p", order),
                                    1, std::min(order+1, plist2_size+1),
                                    params2);
            }

            // Perform fit
            RooFitResult fit = performLikelihoodFit(*(pdfs.at(order-1)), data, 150, retry);
            nlls.push_back(fit.minNll());
            dofs.push_back(fit.covarianceMatrix().GetNcols());

            if (std::strcmp(pdfType, "powXgauss") == 0) {
                if (order < 2) {
                    order++;
                    continue;
                }
            }
            
            if (order >= 2) {
                // Calculate F-test probability using chi2 distribution
                fProb = getWilksProb(nlls.at(order-2), nlls.at(order-1),
                                     dofs.at(order-2), dofs.at(order-1));
                
                std::cout << "###################################################" << std::endl;
                std::cout << "   F-test probability for " << pdfType << "\n"
                          << "   between orders " << order
                          << " and " << (order-1) << " : " << std::endl;
                std::cout << "             " << fProb << "\n" << std::endl;
                std::cout << "###################################################\n" << std::endl;
            }
            
            // Plot individual PDFs
            plotPDF(ObsVar, *(pdfs.at(order-1)), data);
            if ((std::strcmp("powXgauss", pdfType) == 0) && order >= 4) break; //
            order++;
        } // End while-loop

        // Save to workspace and file
        if (_SAVEOPTION) {
            TFile* saveFile = TFile::Open(_SAVEPATHFULL.c_str(), "UPDATE");
            auto wspace = (RooWorkspace*) saveFile->Get(_WORKSPACENAME);
            for (const auto& pdf: pdfs) {
                RooArgSet* params = (RooArgSet*) pdf->getParameters(ObsVar);
                wspace->import(*pdf);
                wspace->saveSnapshot(Form("%s_bin_mle", pdf->GetName()), *params, true);
            }
            wspace->writeToFile(_SAVEPATHFULL.c_str());
            saveFile->Close();
        }
    }


    void FitData::manuallyFitParams (  const char* pdfType,
                                       int order,
                                       const char* plotTitle,
                                       RooRealVar& ObsVar,
                                      RooDataHist& data,
                                      const std::vector<double>& initParamValues1,
                                      const std::vector<double>& initParamValues2 )
     /*
     * Set parameters in given PDF and plot.
     */
    {
        auto models = PDFModels();

        // Lambda function to plot
        auto lambda_plot = [this]( const char* title,
                                    RooAbsPdf* pdf,
                                  RooDataHist& data,
                                   RooRealVar& x )
        {
            // PDF and data plot
            RooPlot* frame = x.frame(RooFit::Title(title));
            data.plotOn(frame);
            pdf->plotOn(frame,
                        RooFit::LineColor(kRed),
                        RooFit::LineStyle(kDashed),
                        RooFit::Name(pdf->GetName()));
            pdf->paramOn(frame, RooFit::Layout(0.72, 0.88, 0.86));
            frame->getAttText()->SetTextFont(43);
            frame->getAttText()->SetTextSize(15);

            // Draw on canvas
            gStyle->SetOptStat(0);
            TCanvas c(pdf->GetName(), pdf->GetTitle());
            c.SetCanvasSize(640, 480);

            c.cd();
            frame->Draw();
            c.Update();

            fs::path save_path = fs::path(_SAVEDIR) / fs::path("plots_manual") /
                             fs::path(Form("%s.jpg", title));
            c.SaveAs(save_path.c_str());
        };

        // Set initial parameter values for fit
        int plist1_size = initParamValues1.size();
        int plist2_size = initParamValues2.size();
        std::vector<double> params1(initParamValues1.begin(),
                                    initParamValues1.begin() + std::min(order, plist1_size));
        std::vector<double> params2(initParamValues2.begin(),
                                    initParamValues2.begin() + std::min(order, plist2_size));
        
        // Make PDF according to type given
        if (std::strcmp(pdfType, "expXgauss") == 0) {
            auto pdf = models.makeExponentialConvGaussian(ObsVar, order);
            models.setMultiVals(Form("expsrs%d_c", order),
                                1, std::min(order, plist1_size),
                                params1);
            models.setMultiVals(Form("expsrs%d_p", order),
                                1, std::min(order, plist2_size),
                                params2);
            lambda_plot(plotTitle, pdf, data, ObsVar);
            
        } else if (std::strcmp(pdfType, "bernXgauss") == 0) {
            auto pdf = models.makeBernsteinConvGaussian(ObsVar, order);
            models.setMultiVals(Form("bern%d_c", order),
                                0, std::min(order, plist1_size),
                                params1);
            lambda_plot(plotTitle, pdf, data, ObsVar);
            
        } else if (std::strcmp(pdfType, "lauXgauss") == 0) {
            auto pdf = models.makeLaurentConvGaussian(ObsVar, order);
            models.setMultiVals(Form("lau%d_h", order),
                                1, std::min(order+1, plist1_size+1),
                                params1);
            models.setMultiVals(Form("lau%d_l", order),
                                1, std::min(order+1, plist2_size+1),
                                params2);
            lambda_plot(plotTitle, pdf, data, ObsVar);

        } else if (std::strcmp(pdfType, "powXgauss") == 0) {
            auto pdf = models.makePowerConvGaussian(ObsVar, order);
            models.setMultiVals(Form("powsrs%d_c", order),
                                1, std::min(order+1, plist1_size+1),
                                params1);
            models.setMultiVals(Form("powsrs%d_p", order),
                                1, std::min(order+1, plist2_size+1),
                                params2);
            lambda_plot(plotTitle, pdf, data, ObsVar);
            
        }
    }


    template<typename T>
    void FitData::performSignalFit ( RooRealVar& ObsVar,
                                              T& data )
    /*
     * Performs likelihood fit to the signal data provided.
     * Explores various pdf types for the fitting model.
     */
    {
        std::vector<RooAbsPdf*> pdfs;
        auto models = PDFModels();

        pdfs.push_back(models.makeDoubleGaussian(ObsVar));
        pdfs.push_back(models.makeTripleGaussian(ObsVar));
        pdfs.push_back(models.makeVoigtian(ObsVar));

        for (auto const& pdf: pdfs) {
            performLikelihoodFit<T>(*pdf, data, 100);
            plotPDF<T>(ObsVar, *pdf, data);
            double prob_GoF = getGoodnessOfFitBC<T>(ObsVar, *pdf, data);
            std::cout << "Goodness of fit: " << prob_GoF << std::endl;
        }

        if (_SAVEOPTION) {
            TFile* saveFile = TFile::Open(_SAVEPATHFULL.c_str(), "UPDATE");
            auto wspace = (RooWorkspace*) saveFile->Get(_WORKSPACENAME);
            for (const auto& pdf: pdfs) {
                RooArgSet* params = (RooArgSet*) pdf->getParameters(ObsVar);
                wspace->import(*pdf);
                wspace->saveSnapshot(Form("%s_bin_mle", pdf->GetName()), *params, true);
            }
            wspace->writeToFile(_SAVEPATHFULL.c_str());
            saveFile->Close();
        }
    }

    
    /* Evaluate fit.*/
    template<typename T>
    double FitData::getGoodnessOfFitBC ( RooRealVar& ObsVar,
                                          RooAbsPdf& pdf,
                                                  T& data,
                                                bool use_toys )
    /*
     * Calculates goodness-of-fit probability using Baker and Cousins'
     * test statistic which is based on the lkielihood ratio with respect to
     * the saturated model.
     * See https://twiki.cern.ch/twiki/bin/viewauth/CMS/StatComGOF
     */
    {
        // Convert RooAbsPdf to TF1
        RooArgSet* obs = pdf.getObservables(ObsVar);
        RooArgSet* params = pdf.getParameters(data);
        TF1* fit = pdf.asTF(*obs, *params, *obs);

        // Set TF1 parameter values
        std::string vars_str =  params->contentsString();
        std::vector<std::string> vars_list;
        size_t pos = 0;
        std::string token;
        while ((pos = vars_str.find(",")) != std::string::npos) {
            token = vars_str.substr(0, pos);
            vars_list.push_back(token);
            vars_str.erase(0, pos + 1);
        }
        vars_list.push_back(vars_str);
        for (auto const& var: vars_list) {
            const char* var_c = var.c_str()
            fit->SetParameter(var_c, params->getRealValue(var_c));
            std::cout << var << " : " << fit->GetParameter(var_c) << std::endl;
        }
        //params->createIterator();
        //for (auto it = params->begin(); it != params->end(); ++it) {
        //    std::cout << it.GetName() << " : " << it.getValV() << std::endl;
        //}
        
        // Create TH1 from data (which is either RooDataHist or RooDataSet)        
        TH1* hist = data.createHistogram(data.GetName(),
                                         ObsVar,
                                         Binning(ObsVar.getBinning()));

        // Get Baker-Cousins test statistic
        double BC_t = hist->Chisquare(fit, "L");

        // Get asymptotic probability
        double asym_prob = TMath::Prob(BC_t, hist->GetNbinsX() - params->getSize());

        // Plot for sanity
        TCanvas c;
        hist->Draw();
        fit->Draw("SAME C");
        fs::path plot_name = fs::path(_PLOTPATH) / fs::path(Form("GoF_%s.jpg", pdf.GetName()));
        c.SaveAs(plot_name.c_str());

        // Get Monte Carlo probability
        if (use_toys) {
            int ntrials = 1000;
            int trial = 0;
            int ndata = data.sumEntries();
            TRandom3 Generator;
            while (trial++ <= ntrials) {
                int nevents = Generator.Poisson(ndata);
                RooDataHist* binned_toy = pdf.generateBinned(*obs, nevents, false, true);
                TH1* htoy = binned_toy->createHistogram("htoy",
                                                        ObsVar,
                                                        Binning(ObsVar.getBinning()));
                auto r = htoy->Fit(fit, "Q L S");
            }
        }
        
        return asym_prob;
    }
    
    double FitData::getWilksProb ( double nllNull,
                                   double nllAlt,
                                      int dofNull,
                                      int dofAlt )
    /*
     * Calculates probability that the likelihood-ratio test statistic is
     * greater than the one observed under the null hypothesis,
     * using Wilks' Theorem.
     * See: https://en.wikipedia.org/wiki/Likelihood-ratio_test
     *  and https://en.wikipedia.org/wiki/Wilks%27_theorem
     *
     * Parameters:
     *   nllNull = negative log-likelihood of null model
     *   nllAlt  = negative log-likelihood of alternative model
     *   dofNull = degree of freedom in null model
     *   dofAlt  = degree of freedom in alternative model
     *   (note: the alternative model has more parameters than the null model)
     */
    {
        if (!(dofAlt > dofNull))
            std::invalid_argument("dofNull must be smaller than dofAlt");
        if (!(nllAlt > 0 && nllNull > 0))
            std::invalid_argument("NLLs must be positive.");
        if (!(nllNull >= nllAlt))
            std::invalid_argument("nllNull must be greater than or equal to nllAlt.");
        return TMath::Prob(2.*(nllNull - nllAlt), dofAlt - dofNull);
    }

    
    double FitData::getFProb ( double rssNull,
                               double rssAlt,
                                  int nParamNull,
                                  int nParamAlt,
                                  int nData)
    /*
     * Calculates the F statistic given by the equation in the following
     * reference, and returns the probability of observing an F value greater
     * than the one observed under the null hypothesis.
     * https://en.wikipedia.org/wiki/F-test#Regression_problems
     *
     * Parameters:
     *   rssNull    = residual sum of squares of null model
     *   rssAlt     = residual sum of squares of alternative model
     *   nParamNull = number of parameters in null model
     *   nParamAlt  = number of parameters in alternative model
     *   nData      = number of points in data
     */
    {
        if (!(rssNull >= 0 && rssAlt >= 0))
            std::invalid_argument("RSS cannot be negative.");
        if (!(rssNull >= rssAlt))
            std::invalid_argument("rssNull must be greater than or equal to rssAlt.");
        if (!(nParamAlt > 0 && nParamNull > 0 && nData > 0))
            std::invalid_argument("Number of parameters and data points must be positive.");
        if (!(nParamAlt > nParamNull))
            std::invalid_argument("nParamAlt must be greater than nParamNull.");
        if (!(nData > nParamAlt))
            std::invalid_argument("nData must be greater than nParamAlt.");

        double F = ((rssNull-rssAlt)/(nParamAlt-nParamNull))/(rssAlt/(nData-nParamAlt));
        return 1 - TMath::FDistI(F, nParamAlt-nParamNull, nData-nParamAlt);
    }

    /*
    void FitData::getChi2 ( RooRealVar* ObsVar,
                            RooAbsPdf* pdf,
                            RooDataHist* )
    */
    

    /* Plot fit. */
    template<typename T>
    void FitData::plotPDF ( RooRealVar& ObsVar,
                             RooAbsPdf& pdf,
                                     T& data )
    {
        // PDF and data plot
        RooPlot* frame1 = ObsVar.frame(RooFit::Title(pdf.GetTitle()));
        data.plotOn(frame1);
        pdf.plotOn(frame1,
                   RooFit::LineColor(kRed),
                   RooFit::LineStyle(kDashed),
                   RooFit::Name("fit"));
        pdf.paramOn(frame1, RooFit::Layout(0.72, 0.88, 0.86));
        frame1->getAttText()->SetTextFont(43);
        frame1->getAttText()->SetTextSize(15);

        // Residuals
        RooPlot* frame2 = ObsVar.frame(RooFit::Title("residuals"));
        RooHist* hresid = frame1->residHist();
        frame2->addPlotable(hresid, "P");

        // Pull
        RooPlot* frame3 = ObsVar.frame(RooFit::Title("pull"));
        RooHist* hpull = frame1->pullHist();
        frame3->addPlotable(hpull, "P");

        // Draw on canvas
        gStyle->SetOptStat(0);
        TCanvas c(pdf.GetName(), pdf.GetTitle());
        c.SetCanvasSize(1500, 2000);

        TPad p1("p1", "p1", .05, .53, .95, .95);
        TPad p2("p2", "p2", .05, .29, .95, .51);
        TPad p3("p3", "p3", .05, .05, .95, .27);

        p1.cd();
        frame1->Draw();
        p1.Update();

        p2.cd();
        frame2->Draw();
        p2.Update();

        p3.cd();
        frame3->Draw();
        p3.Update();

        c.cd();
        p1.Draw();
        p2.Draw();
        p3.Draw();
        c.Update();

        if (_SAVEOPTION) {
            fs::path plot_name = fs::path(_PLOTPATH) / fs::path(Form("%s.jpg", pdf.GetName()));
            c.SaveAs(plot_name.c_str());
        }
    }


    template<typename T>
    void FitData::plotMultiplePDFs (  RooRealVar& ObsVar,
                                               T& data,
                                      const char* plotName,
                                      const std::vector<const char*>& pdfNames,
                                              const std::vector<int>& colorScheme )
    /*
     * Searches for pdfs in OUTFILE and plots them on a single canvas.
     * Saves the canvas in the plots directory.
     * If SAVEOPTION = false, none of these will occur.
     */
    {
        if (!_SAVEOPTION) return;
        
        // Open file
        TFile* saveFile = TFile::Open(_SAVEPATHFULL.c_str(), "READ");
        auto wspace = (RooWorkspace*) saveFile->Get(_WORKSPACENAME);
        
        // PDF and data plot
        TCanvas c;
        RooPlot* frame = ObsVar.frame(RooFit::Title(plotName));
        data->plotOn(frame);

        // Check if pdfNames and colorScheme have same size
        if (pdfNames.size() != colorScheme.size()) {
            throw std::invalid_argument("pdfNames and colorScheme must be of same size.");
        }

        // Legend
        TLegend legend(.72, .64, .86, .88);
        legend.SetFillColor(kWhite);
        legend.SetLineColor(kWhite);

        for (int i=0; i<pdfNames.size(); i++) {
            const char* pdf_name = pdfNames.at(i);
            RooAbsPdf* pdf = wspace->pdf(pdf_name);
            if (!pdf) {
                std::cout << "PDF of name " << pdf_name << " not found" << std::endl;
            } else {
                pdf->plotOn(frame,
                            RooFit::LineColor(colorScheme.at(i)),
                            RooFit::LineStyle(kDashed),
                            RooFit::Name(pdf_name));
                legend.AddEntry(frame->findObject(pdf_name), pdf_name, "L");
            }
        }

        // Draw on canvas
        gStyle->SetOptStat(0);
        c.SetCanvasSize(1500, 780);
        frame->Draw();
        legend.Draw();
        c.Update();

        if (_SAVEOPTION) {
            fs::path plot_name = fs::path(_PLOTPATH) / fs::path(Form("%s.jpg", plotName));
            c.SaveAs(plot_name.c_str());
        }
    }


    /* Private methods. */
    void FitData::makePaths ()
    {
        fs::path save_path = fs::canonical(fs::path(_SAVEDIR)) / fs::path(_OUTFILENAME);
        _SAVEPATHFULL = save_path.string();

        fs::path plot_path = fs::canonical(fs::path(_SAVEDIR)) / fs::path("plots");
        _PLOTPATH = plot_path.string();

        if (!fs::exists(_PLOTPATH)) fs::create_directory(_PLOTPATH);
    }


    // Explicit template instantiation (if necessary)
    template RooFitResult FitData::performLikelihoodFit<RooDataSet>( RooAbsPdf&,
                                                                     RooDataSet&,
                                                                     int,
                                                                     int );
    template RooFitResult FitData::performLikelihoodFit<RooDataHist>( RooAbsPdf&,
                                                                      RooDataHist&,
                                                                      int,
                                                                      int );
    template void FitData::performMultiLikelihoodFit<RooDataSet>( const char*,
                                                                  RooRealVar&,
                                                                  RooDataSet&,
                                                                  const std::vector<double>&,
                                                                  const std::vector<double>&,
                                                                  double );
    template void FitData::performMultiLikelihoodFit<RooDataHist>( const char*,
                                                                   RooRealVar&,
                                                                   RooDataHist&,
                                                                   const std::vector<double>&,
                                                                   const std::vector<double>&,
                                                                   double );
    // TODO: finish this list
}
