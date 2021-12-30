#include "FitData.h"
#include "PDFModels.h"

#include <boost/filesystem.hpp>
// --- STD ---
#include <iostream>
#include <stdexcept>

// --- ROOT ---
#include "TCanvas.h"
#include "TFile.h"
#include "TPad.h"
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
    {
        makePath();
    }

    FitData::FitData ( const char* outfileName,
                       const char* saveDir,
                       const char* workspaceName ) :
        _SAVEOPTION(true),
        _SAVEDIR(saveDir),
        _OUTFILENAME(outfileName),
        _WORKSPACENAME(workspaceName)
    {
        makePath();
        std::cout << "Recreating ROOT file " << _SAVEPATHFULL << std::endl;
        
        TFile* outfile = TFile::Open(_SAVEPATHFULL, "RECREATE");
        std::cout << "Creating workspace \"" << _WORKSPACENAME << "\"" << std::endl;
        RooWorkspace w(_WORKSPACENAME);
        w.Write();
        outfile->Close();
    }


    /* Public methods. */
    void FitData::setSaveOption ( bool saveOption )
    {
        if (!_SAVEOPTION && saveOption) { // i.e. if setting first time
            _SAVEOPTION = true;
            makePath();
            std::cout << "Recreating ROOT file \"" << _SAVEPATHFULL << std::endl;
        
            TFile* outfile = TFile::Open(_SAVEPATHFULL, "RECREATE");
            std::cout << "Creating workspace \"" << _WORKSPACENAME << "\"" << std::endl;
            RooWorkspace w(_WORKSPACENAME);
            w.Write();
            outfile->Close();
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
            TFile* outfile = TFile::Open(_SAVEPATHFULL, "UPDATE");
            auto w = (RooWorkspace*) outfile->Get(_WORKSPACENAME);
            for (auto const& pdf_name: pdfNameList) {
                RooAbsPdf* pdf = w->pdf(pdf_name);
                if (pdf) {
                    pdf_list.add(*pdf);
                } else {
                    std::cout << "PDF named " << pdf_name
                              << " not found in file " << _SAVEPATHFULL
                              << " and workspace " << _WORKSPACENAME << std::endl;
                }
            }
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
                               (name, RooDataHist(name, name,
                                                  RooArgList(ObsVar), &hist)
                               )
                              );
        return _DataHistograms[name];
    }

    
    RooFitResult FitData::performChi2Fit (   RooAbsPdf* pdf,
                                           RooDataHist* data,
                                                    int maxTries,
                                                    int retry )
    {
        RooArgSet* params = pdf->getParameters((const RooArgSet*)(0));
        RooFitResult* fitResult;
        int status = 5;
        int ntries = 0;
        int retrycount = retry;
        
        std::cout << "----------- ";
        std::cout << "Chi2 fitting " << pdf->GetTitle() << " to " << data->GetTitle();
        std::cout << " -----------" << std::endl;
        
        // Try fit as many times as needed to converge
        RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
        while (++ntries <= maxTries) {
            std::cout << "trial #" << ntries << std::endl;
            fitResult = pdf->chi2FitTo(*data,
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


    RooFitResult FitData::performLikelihoodFit (   RooAbsPdf* pdf,
                                                 RooDataHist* data,
                                                          int maxTries,
                                                          int retry=0 )
    {
        RooArgSet* params = pdf->getParameters((const RooArgSet*)(0));
        RooFitResult* fitResult;
        int status = 5;
        int ntries = 0;
        int retrycount = retry;
        
        std::cout << "----------- ";
        std::cout << "Likelihood (binned) fitting " << pdf->GetTitle() << " to " << data->GetTitle();
        std::cout << " -----------" << std::endl;
        
        // Try fit as many times as needed to converge
        RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
        while (++ntries <= maxTries) {
            std::cout << "trial #" << ntries << std::endl;
            fitResult = pdf->fitTo(*data,
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


    RooFitResult FitData::performLikelihoodFit (  RooAbsPdf* pdf,
                                                 RooDataSet* data,
                                                         int maxTries,
                                                         int retry=0 )
    {
        RooArgSet* params = pdf->getParameters((const RooArgSet*)(0));
        RooFitResult* fitResult;
        int status = 5;
        int ntries = 0;
        int retrycount = retry;
        
        std::cout << "----------- ";
        std::cout << "Likelihood (unbinned) fitting " << pdf->GetTitle() << " to " << data->GetTitle();
        std::cout << " -----------" << std::endl;
        
        // Try fit as many times as needed to converge
        RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
        while (++ntries <= maxTries) {
            std::cout << "trial #" << ntries << std::endl;
            fitResult = pdf->fitTo(*data,
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
                                                       RooRealVar* ObsVar,
                                                      RooDataHist* data,
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
            if (std::strcmp(pdfType, "bernXgauss") == 0) {
                pdfs.push_back(models.makeBernsteinConvGaussian(*ObsVar, order));
                retry = 1;
                models.setMultiVals(Form("bern%d_c", order),
                                    0, std::min(order, plist1_size),
                                    params1);

            } else if (std::strcmp(pdfType, "lauXgauss") == 0) {
                pdfs.push_back(models.makeLaurentConvGaussian(*ObsVar, order));
                retry = 2;
                models.setMultiVals(Form("lau%d_h", order),
                                    1, std::min(order+1, plist1_size+1),
                                    params1);
                models.setMultiVals(Form("lau%d_l", order),
                                    1, std::min(order+1, plist2_size+1),
                                    params2);
                
            } else if (std::strcmp(pdfType, "powXgauss") == 0) {
                pdfs.push_back(models.makePowerConvGaussian(*ObsVar, order));
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
            if (std::strcmp(pdfType, "powXgauss") == 0) {
                //                models.setVal("gauss_mu", 19.6);
                //                models.setVal("gauss_sigma", 10.);
            }
            RooFitResult fit = performChi2Fit(pdfs.at(order-1), data, 150, retry);
            nlls.push_back(fit.minNll());
            dofs.push_back(fit.covarianceMatrix().GetNcols());
            
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
            plotPDF(ObsVar, pdfs.at(order-1), data);
            order++;
        } // End while-loop

        // Save to workspace and file
        if (_SAVEOPTION) {
            TFile* saveFile = TFile::Open(_SAVEPATHFULL, "UPDATE");
            auto wspace = (RooWorkspace*) saveFile->Get(_WORKSPACENAME);
            for (const auto& pdf: pdfs) {
                RooArgSet* params = (RooArgSet*) pdf->getParameters(*ObsVar);
                wspace->import(*pdf);
                wspace->saveSnapshot(Form("%s_chi2", pdf->GetName()), *params, true);
            }
            saveFile->Close();
        }
    }


    void FitData::performMultiLikelihoodFit (                const char* pdfType,
                                                             RooRealVar* ObsVar,
                                                            RooDataHist* data,
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
            if (std::strcmp(pdfType, "bernXgauss") == 0) {
                pdfs.push_back(models.makeBernsteinConvGaussian(*ObsVar, order));
                retry = 1;
                models.setMultiVals(Form("bern%d_c", order),
                                    0, std::min(order, plist1_size),
                                    params1);

            } else if (std::strcmp(pdfType, "lauXgauss") == 0) {
                pdfs.push_back(models.makeLaurentConvGaussian(*ObsVar, order));
                retry = 2;
                models.setMultiVals(Form("lau%d_h", order),
                                    1, std::min(order+1, plist1_size+1),
                                    params1);
                models.setMultiVals(Form("lau%d_l", order),
                                    1, std::min(order+1, plist2_size+1),
                                    params2);
                
            } else if (std::strcmp(pdfType, "powXgauss") == 0) {
                pdfs.push_back(models.makePowerConvGaussian(*ObsVar, order));
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
            if (std::strcmp(pdfType, "powXgauss") == 0) {
                //                models.setVal("gauss_mu", 19.6);
                //                models.setVal("gauss_sigma", 10.);
            }
            RooFitResult fit = performLikelihoodFit(pdfs.at(order-1), data, 150, retry);
            nlls.push_back(fit.minNll());
            dofs.push_back(fit.covarianceMatrix().GetNcols());
            
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
            plotPDF(ObsVar, pdfs.at(order-1), data);
            order++;
        } // End while-loop

        // Save to workspace and file
        if (_SAVEOPTION) {
            TFile* saveFile = TFile::Open(_SAVEPATHFULL, "UPDATE");
            auto wspace = (RooWorkspace*) saveFile->Get(_WORKSPACENAME);
            for (const auto& pdf: pdfs) {
                RooArgSet* params = (RooArgSet*) pdf->getParameters(*ObsVar);
                wspace->import(*pdf);
                wspace->saveSnapshot(Form("%s_bin_mle", pdf->GetName()), *params, true);
            }
            saveFile->Close();
        }
    } 

    
    /* Evaluate fit.*/
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
    

    /* Plot fit. */
    void FitData::plotPDF (  RooRealVar* ObsVar,
                              RooAbsPdf* pdf,
                            RooDataHist* data )
    {
        // PDF and data plot
        RooPlot* frame1 = ObsVar->frame(RooFit::Title(pdf->GetTitle()));
        data->plotOn(frame1);
        pdf->plotOn(frame1,
                    RooFit::LineColor(kRed),
                    RooFit::LineStyle(kDashed),
                    RooFit::Name("fit"));
        pdf->paramOn(frame1, RooFit::Layout(0.72, 0.88, 0.86));
        frame1->getAttText()->SetTextFont(43);
        frame1->getAttText()->SetTextSize(15);

        // Residuals
        RooPlot* frame2 = ObsVar->frame(RooFit::Title("residuals"));
        RooHist* hresid = frame1->residHist();
        frame2->addPlotable(hresid, "P");

        // Pull
        RooPlot* frame3 = ObsVar->frame(RooFit::Title("pull"));
        RooHist* hpull = frame1->pullHist();
        frame3->addPlotable(hpull, "P");

        // Draw on canvas
        gStyle->SetOptStat(0);
        TCanvas c(pdf->GetName(), pdf->GetTitle());
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

        fs::path save_path = fs::path(_SAVEDIR) / fs::path("plots") /
                             fs::path(Form("%s.jpg", pdf->GetName()));
        c.SaveAs(save_path.c_str());
    }


    /* Private methods. */
    void FitData::makePath()
    {
        fs::path full_path = fs::weakly_canonical(fs::path(_SAVEDIR) /
                                                  fs::path(_OUTFILENAME));
        _SAVEPATHFULL = full_path.c_str();
        // TODO: if not exist create
    }
}
