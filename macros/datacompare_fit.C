#include <string.h>
#include <iostream>
#include <cctype>

using namespace RooFit;

void datacompare_fit(const char* Meson) {

    string date;
    const char* meson;
    if (strcmp(Meson, "Rho") == 0) {date = "230309"; meson = "rho";}
    if (strcmp(Meson, "Phi") == 0) {date = "230310"; meson = "phi";}

    // Files
    const char* Torino_presel_filename = Form("/work/submit/mariadlf/cards_march9/%s_%sGammaLimit/workspace_STAT_%s_GFpreselection_2018.root", date.c_str(), Meson, Meson);
    const char* MIT_presel_sig_filename = Form("/work/submit/mariadlf/cards_march9/WS_MARCH9/Signal_GFcat__%sCat_2018_workspace.root", Meson);
    const char* MIT_presel_bkg_filename = Form("/work/submit/mariadlf/cards_march9/WS_MARCH9/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
      const char* MIT_presel_sig_filename_ = Form("/work/submit/mariadlf/cards_march16/WS_MARCH16/Signal_GFcat__%sCat_2018_workspace.root", Meson);
      const char* MIT_presel_bkg_filename_ = Form("/work/submit/mariadlf/cards_march16/WS_MARCH16/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
        const char* MIT_presel_sig_filename__ = Form("/work/submit/mariadlf/cards_march16/WS_MARCH16_phWP/Signal_GFcat__%sCat_2018_workspace.root", Meson);
        const char* MIT_presel_bkg_filename__ = Form("/work/submit/mariadlf/cards_march16/WS_MARCH16_phWP/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
    TFile* Torino_presel = TFile::Open(Torino_presel_filename, "READ");
    TFile* MIT_presel_sig = TFile::Open(MIT_presel_sig_filename, "READ");
    TFile* MIT_presel_bkg = TFile::Open(MIT_presel_bkg_filename, "READ");
      TFile* MIT_presel_sig_ = TFile::Open(MIT_presel_sig_filename_, "READ");
      TFile* MIT_presel_bkg_ = TFile::Open(MIT_presel_bkg_filename_, "READ");
        TFile* MIT_presel_sig__ = TFile::Open(MIT_presel_sig_filename__, "READ");
        TFile* MIT_presel_bkg__ = TFile::Open(MIT_presel_bkg_filename__, "READ");

    // Workspaces
    const char* Torino_workspace_name = Form("workspace_STAT_%s_GFpreselection_2018", Meson);
    const char* MIT_workspace_name = "w";

    RooWorkspace* Torino_workspace = (RooWorkspace*) Torino_presel->Get(Torino_workspace_name);
    RooWorkspace* MIT_workspace_sig = (RooWorkspace*) MIT_presel_sig->Get(MIT_workspace_name);
    RooWorkspace* MIT_workspace_bkg = (RooWorkspace*) MIT_presel_bkg->Get(MIT_workspace_name);
      RooWorkspace* MIT_workspace_sig_ = (RooWorkspace*) MIT_presel_sig_->Get(MIT_workspace_name);
      RooWorkspace* MIT_workspace_bkg_ = (RooWorkspace*) MIT_presel_bkg_->Get(MIT_workspace_name);
        RooWorkspace* MIT_workspace_sig__ = (RooWorkspace*) MIT_presel_sig__->Get(MIT_workspace_name);
        RooWorkspace* MIT_workspace_bkg__ = (RooWorkspace*) MIT_presel_bkg__->Get(MIT_workspace_name);

    // Normalizations
    float Torino_ggH_signal_norm = Torino_workspace->var(Form("crystal_ball_%s_GFpreselection_ggH_norm", Meson))->getValV();
    float Torino_VBF_signal_norm = Torino_workspace->var(Form("crystal_ball_%s_GFpreselection_VBF_norm", Meson))->getValV();
    float Torino_data_bkg_norm = Torino_workspace->var(Form("multipdf_%s_GFpreselection_bkg_norm", Meson))->getValV();
    float MIT_ggH_signal_norm = MIT_workspace_sig->var(Form("crystal_ball_%sCat_GFcat_ggH_norm", Meson))->getValV();
    float MIT_VBF_signal_norm = MIT_workspace_sig->var(Form("crystal_ball_%sCat_GFcat_VBFH_norm", Meson))->getValV();
    float MIT_data_bkg_norm = MIT_workspace_bkg->var(Form("multipdf_%sCat_GFcat_bkg_norm", Meson))->getValV();
      float MIT_ggH_signal_norm_ = MIT_workspace_sig_->var(Form("crystal_ball_%sCat_GFcat_ggH_norm", Meson))->getValV();
      float MIT_VBF_signal_norm_ = MIT_workspace_sig_->var(Form("crystal_ball_%sCat_GFcat_VBFH_norm", Meson))->getValV();
      float MIT_data_bkg_norm_ = MIT_workspace_bkg_->var(Form("multipdf_%sCat_GFcat_bkg_norm", Meson))->getValV();
        float MIT_ggH_signal_norm__ = MIT_workspace_sig__->var(Form("crystal_ball_%sCat_GFcat_ggH_norm", Meson))->getValV();
        float MIT_VBF_signal_norm__ = MIT_workspace_sig__->var(Form("crystal_ball_%sCat_GFcat_VBFH_norm", Meson))->getValV();
        float MIT_data_bkg_norm__ = MIT_workspace_bkg__->var(Form("multipdf_%sCat_GFcat_bkg_norm", Meson))->getValV();

    /* For now, print out normalizations */
    std::cout << "*************************************************" << std::endl;
    std::cout << "GFcat " << Meson << "Cat signal ggH normalizations (preselection) " << std::endl;
    std::cout << "Torino: " << Torino_ggH_signal_norm << ", MIT (March 9): " << MIT_ggH_signal_norm << ", MIT (March 16): " << MIT_ggH_signal_norm_ << ", MIT (March 16, photon working point): " << MIT_ggH_signal_norm__ << std::endl;
    std::cout << "GFcat " << Meson << "Cat signal VBF normalizations (preselection) " << std::endl;
    std::cout << "Torino: " << Torino_VBF_signal_norm << ", MIT (March 9): " << MIT_VBF_signal_norm << ", MIT (March 16): " << MIT_VBF_signal_norm_ << ", MIT (March 16, photon WP): " << MIT_VBF_signal_norm__ << std::endl;
    std::cout << "GFcat " << Meson << "Cat background normalizations (preselection) " << std::endl;
    std::cout << "Torino: " << Torino_data_bkg_norm << ", MIT (March 9): " << MIT_data_bkg_norm << ", MIT (March 16): " << MIT_data_bkg_norm_ << ", MIT (March 16, photon WP): " << MIT_data_bkg_norm__ << std::endl;
    std::cout << "*************************************************" << std::endl;

    // DataSets & DataHists
    RooDataSet* Torino_ggH_data = (RooDataSet*) Torino_workspace->data("dataset_ggH");
    RooDataSet* Torino_VBF_data = (RooDataSet*) Torino_workspace->data("dataset_VBF");
    RooDataSet* Torino_observed_data = (RooDataSet*) Torino_workspace->data("observed_data");
    RooDataHist* MIT_ggH_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_ggH");
    RooDataHist* MIT_VBF_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_VBFH");
    RooDataHist* MIT_observed_data = (RooDataHist*) MIT_workspace_bkg->data(Form("datahist_%sCat_GFcat", Meson));
      RooDataHist* MIT_ggH_data_ = (RooDataHist*) MIT_workspace_sig_->data("datahist_GFcat_ggH");
      RooDataHist* MIT_VBF_data_ = (RooDataHist*) MIT_workspace_sig_->data("datahist_GFcat_VBFH");
      RooDataHist* MIT_observed_data_ = (RooDataHist*) MIT_workspace_bkg_->data(Form("datahist_%sCat_GFcat", Meson));
        RooDataHist* MIT_ggH_data__ = (RooDataHist*) MIT_workspace_sig__->data("datahist_GFcat_ggH");
        RooDataHist* MIT_VBF_data__ = (RooDataHist*) MIT_workspace_sig__->data("datahist_GFcat_VBFH");
        RooDataHist* MIT_observed_data__ = (RooDataHist*) MIT_workspace_bkg__->data(Form("datahist_%sCat_GFcat", Meson));

    // PDFs
    RooAbsPdf* Torino_ggH_pdf = Torino_workspace->pdf(Form("crystal_ball_%s_GFpreselection_ggH", Meson));
    RooAbsPdf* Torino_VBF_pdf = Torino_workspace->pdf(Form("crystal_ball_%s_GFpreselection_VBF", Meson));
    RooAbsPdf* Torino_bkg_pdf1 = Torino_workspace->pdf("chebychev_GFpreselection_bkg");
    RooAbsPdf* Torino_bkg_pdf2;
    RooAbsPdf* MIT_ggH_pdf = MIT_workspace_sig__->pdf(Form("crystal_ball_%sCat_GFcat_ggH", Meson));
    RooAbsPdf* MIT_VBF_pdf = MIT_workspace_sig__->pdf(Form("crystal_ball_%sCat_GFcat_VBFH", Meson));
    RooAbsPdf* MIT_bkg_pdf1 = MIT_workspace_bkg__->pdf(Form("chebychev3_%sCat_GFcat", Meson));
    RooAbsPdf* MIT_bkg_pdf2 = MIT_workspace_bkg__->pdf(Form("bern3_%sCat_GFcat", Meson));
    
    // RooPlots
    /* RooRealVars */
    RooRealVar* var_Torino = Torino_workspace->var("mesonGammaMass");
    RooRealVar* var_MIT = MIT_workspace_sig->var("mh");

    /* RooPlot frames */
    RooPlot* ggH_frame_Torino = var_Torino->frame();
    RooPlot* VBF_frame_Torino = var_Torino->frame();
    RooPlot* bkg_frame_Torino = var_Torino->frame();
    RooPlot* ggH_frame_MIT = var_MIT->frame();
    RooPlot* VBF_frame_MIT = var_MIT->frame();
    RooPlot* bkg_frame_MIT = var_MIT->frame();

    /* Draw RooAbsPdfs */
    Torino_ggH_pdf->plotOn(ggH_frame_Torino, LineColor(kBlue), LineStyle(kDashed));
    Torino_VBF_pdf->plotOn(VBF_frame_Torino, LineColor(kBlue), LineStyle(kDashed));
    Torino_bkg_pdf1->plotOn(bkg_frame_Torino, LineColor(kBlue), LineStyle(kDashed));
    MIT_ggH_pdf->plotOn(ggH_frame_MIT, LineColor(kRed), LineStyle(kDashed));
    MIT_VBF_pdf->plotOn(VBF_frame_MIT, LineColor(kRed), LineStyle(kDashed));
    MIT_bkg_pdf1->plotOn(bkg_frame_MIT, LineColor(kRed), LineStyle(kDashed));

    /* Configure RooPlots */
    ggH_frame_Torino->SetTitle(Form("GFcat %sCat ggH signal PDF compare (preselection)", Meson));
    ggH_frame_Torino->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    ggH_frame_Torino->SetYTitle(Form(""));
    // ggH_frame_Torino
    VBF_frame_Torino->SetTitle(Form("GFcat %sCat VBF signal PDF compare (preselection)", Meson));
    VBF_frame_Torino->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    VBF_frame_Torino->SetYTitle(Form(""));
    // VBF_frame_Torino
    bkg_frame_Torino->SetTitle(Form("GFcat %sCat background PDF compare (preselection)", Meson));
    bkg_frame_Torino->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    bkg_frame_Torino->SetYTitle(Form(""));
    // bkg_frame_Torino
    ggH_frame_MIT->SetTitle(Form("GFcat %sCat ggH signal PDF compare (preselection)", Meson));
    ggH_frame_MIT->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    ggH_frame_MIT->SetYTitle(Form(""));
    VBF_frame_MIT->SetTitle(Form("GFcat %sCat VBF signal PDF compare (preselection)", Meson));
    VBF_frame_MIT->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    VBF_frame_MIT->SetYTitle(Form(""));
    // VBF_frame_MIT
    bkg_frame_MIT->SetTitle(Form("GFcat %sCat background PDF compare (preselection)", Meson));
    bkg_frame_MIT->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    bkg_frame_MIT->SetYTitle(Form(""));
    // bkg_frame_MIT
    
    // Draw & Save
    TCanvas c_ggH_fit("c", "c", 1200, 800);
    TLegend legend_ggH_fit(0.9, 0.9, 0.5, 0.83);
    ggH_frame_MIT->Draw();
    ggH_frame_Torino->Draw("SAME");
    legend_ggH_fit.AddEntry(ggH_frame_Torino->findObject(Form("crystal_ball_%s_GFpreselection_ggH_Norm[mesonGammaMass]", Meson)), "Torino DoubleCrystalBall", "l");
    legend_ggH_fit.AddEntry(ggH_frame_MIT->findObject(Form("crystal_ball_%sCat_GFcat_ggH_Norm[mh]", Meson)), "MIT DoubleCrystalBall (March 16) Pt:38,38 ph=WP80", "l");
    legend_ggH_fit.Draw();
    c_ggH_fit.SaveAs(Form("compare/presel_signal_ggH_fit_compare_GF_%s.png", Meson));
    c_ggH_fit.Close();

    TCanvas c_VBF_fit("c", "c", 1200, 800);
    TLegend legend_VBF_fit(0.9, 0.9, 0.5, 0.83);
    VBF_frame_MIT->Draw();
    VBF_frame_Torino->Draw("SAME");
    legend_VBF_fit.AddEntry(VBF_frame_Torino->findObject(Form("crystal_ball_%s_GFpreselection_VBF_Norm[mesonGammaMass]", Meson)), "Torino DoubleCrystalBall", "l");
    legend_VBF_fit.AddEntry(VBF_frame_MIT->findObject(Form("crystal_ball_%sCat_GFcat_VBFH_Norm[mh]", Meson)), "MIT DoubleCrystalBall (March 16) Pt:38,38 ph=WP80", "l");
    legend_VBF_fit.Draw();
    c_VBF_fit.SaveAs(Form("compare/presel_signal_VBF_fit_compare_GF_%s.png", Meson));
    c_VBF_fit.Close();
    
    TCanvas c_data_fit("c", "c", 1200, 800);
    TLegend legend_data_fit(0.52, 0.19, 0.12, 0.12);
    bkg_frame_MIT->Draw();
    bkg_frame_Torino->Draw("SAME");
    legend_data_fit.AddEntry(bkg_frame_Torino->findObject("chebychev_GFpreselection_bkg_Norm[mesonGammaMass]"), "Torino Chebychev dof=4", "l");
    legend_data_fit.AddEntry(bkg_frame_MIT->findObject(Form("chebychev3_%sCat_GFcat_Norm[mh]", Meson)), "MIT Chebychev dof=4 (March 16) Pt:38,38 ph=WP80", "l");
    legend_data_fit.Draw();
    c_data_fit.SaveAs(Form("compare/presel_bkg_observed_data_fit_compare_GF_%s.png", Meson));
    c_data_fit.Close();
}
