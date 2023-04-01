#include <string.h>
#include <iostream>
#include <cctype>

using namespace RooFit;

void datacompare_resid(const char* Meson) {

    string date;
    const char* meson;
    if (strcmp(Meson, "Rho") == 0) {date = "230319"; meson = "rho";}
    if (strcmp(Meson, "Phi") == 0) {date = "230319"; meson = "phi";}

    // Files
    const char* Torino_presel_filename = Form("/work/submit/mariadlf/cards_march20/%s_%sGammaLimit/workspace_STAT_%s_GFpreselection_2018.root", date.c_str(), Meson, Meson);
    // const char* MIT_presel_sig_filename = Form("/work/submit/mariadlf/cards_march20/Signal_GFcat__%sCat_2018_workspace.root", Meson);
    // const char* MIT_presel_bkg_filename = Form("/work/submit/mariadlf/cards_march20/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
      const char* MIT_presel_sig_filename_ = Form("/work/submit/mariadlf/cards_march20/WS_MARCH20/Signal_GFcat__%sCat_2018_workspace.root", Meson);
      const char* MIT_presel_bkg_filename_ = Form("/work/submit/mariadlf/cards_march20/WS_MARCH20/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
        const char* MIT_presel_sig_filename__ = Form("/work/submit/mariadlf/cards_march20/WS_MARCH20_pt38/Signal_GFcat__%sCat_2018_workspace.root", Meson);
        const char* MIT_presel_bkg_filename__ = Form("/work/submit/mariadlf/cards_march20/WS_MARCH20_pt38/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
    TFile* Torino_presel = TFile::Open(Torino_presel_filename, "READ");
    // TFile* MIT_presel_sig = TFile::Open(MIT_presel_sig_filename, "READ");
    // TFile* MIT_presel_bkg = TFile::Open(MIT_presel_bkg_filename, "READ");
      TFile* MIT_presel_sig_ = TFile::Open(MIT_presel_sig_filename_, "READ");
      TFile* MIT_presel_bkg_ = TFile::Open(MIT_presel_bkg_filename_, "READ");
        TFile* MIT_presel_sig__ = TFile::Open(MIT_presel_sig_filename__, "READ");
        TFile* MIT_presel_bkg__ = TFile::Open(MIT_presel_bkg_filename__, "READ");

    // Workspaces
    const char* Torino_workspace_name = Form("workspace_STAT_%s_GFpreselection_2018", Meson);
    const char* MIT_workspace_name = "w";

    RooWorkspace* Torino_workspace = (RooWorkspace*) Torino_presel->Get(Torino_workspace_name);
    // RooWorkspace* MIT_workspace_sig = (RooWorkspace*) MIT_presel_sig->Get(MIT_workspace_name);
    // RooWorkspace* MIT_workspace_bkg = (RooWorkspace*) MIT_presel_bkg->Get(MIT_workspace_name);
      RooWorkspace* MIT_workspace_sig_ = (RooWorkspace*) MIT_presel_sig_->Get(MIT_workspace_name);
      RooWorkspace* MIT_workspace_bkg_ = (RooWorkspace*) MIT_presel_bkg_->Get(MIT_workspace_name);
        RooWorkspace* MIT_workspace_sig__ = (RooWorkspace*) MIT_presel_sig__->Get(MIT_workspace_name);
        RooWorkspace* MIT_workspace_bkg__ = (RooWorkspace*) MIT_presel_bkg__->Get(MIT_workspace_name);

    // DataSets & DataHists
    RooDataSet* Torino_ggH_data = (RooDataSet*) Torino_workspace->data("dataset_ggH");
    RooDataSet* Torino_VBF_data = (RooDataSet*) Torino_workspace->data("dataset_VBF");
    RooDataSet* Torino_observed_data = (RooDataSet*) Torino_workspace->data("observed_data");
    // RooDataHist* MIT_ggH_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_ggH");
    // RooDataHist* MIT_VBF_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_VBFH");
    // RooDataHist* MIT_observed_data = (RooDataHist*) MIT_workspace_bkg->data(Form("datahist_%sCat_GFcat", Meson));
    //   RooDataHist* MIT_ggH_data_ = (RooDataHist*) MIT_workspace_sig_->data("datahist_GFcat_ggH");
    //   RooDataHist* MIT_VBF_data_ = (RooDataHist*) MIT_workspace_sig_->data("datahist_GFcat_VBFH");
    //   RooDataHist* MIT_observed_data_ = (RooDataHist*) MIT_workspace_bkg_->data(Form("datahist_%sCat_GFcat", Meson));
        RooDataHist* MIT_ggH_data__ = (RooDataHist*) MIT_workspace_sig__->data("datahist_GFcat_ggH");
        RooDataHist* MIT_VBF_data__ = (RooDataHist*) MIT_workspace_sig__->data("datahist_GFcat_VBFH");
        RooDataHist* MIT_observed_data__ = (RooDataHist*) MIT_workspace_bkg__->data(Form("datahist_%sCat_GFcat", Meson));

    // PDFs
    RooAbsPdf* Torino_ggH_pdf = Torino_workspace->pdf(Form("crystal_ball_%s_GFpreselection_ggH", Meson));
    RooAbsPdf* Torino_VBF_pdf = Torino_workspace->pdf(Form("crystal_ball_%s_GFpreselection_VBF", Meson));
    RooAbsPdf* Torino_bkg_pdf1 = Torino_workspace->pdf("chebychev_GFpreselection_bkg");
    RooAbsPdf* Torino_bkg_pdf2;
      // RooAbsPdf* MIT_ggH_pdf_ = MIT_workspace_sig_->pdf(Form("crystal_ball_%sCat_GFcat_ggH", Meson));
      // RooAbsPdf* MIT_VBF_pdf_ = MIT_workspace_sig_->pdf(Form("crystal_ball_%sCat_GFcat_VBFH", Meson));
      // RooAbsPdf* MIT_bkg_pdf1_ = MIT_workspace_bkg_->pdf(Form("chebychev3_%sCat_GFcat", Meson));
      // RooAbsPdf* MIT_bkg_pdf2_ = MIT_workspace_bkg_->pdf(Form("bern3_%sCat_GFcat", Meson));
        RooAbsPdf* MIT_ggH_pdf__ = MIT_workspace_sig__->pdf(Form("crystal_ball_%sCat_GFcat_ggH", Meson));
        RooAbsPdf* MIT_VBF_pdf__ = MIT_workspace_sig__->pdf(Form("crystal_ball_%sCat_GFcat_VBFH", Meson));
        RooAbsPdf* MIT_bkg_pdf1__ = MIT_workspace_bkg__->pdf(Form("chebychev3_%sCat_GFcat", Meson));
        RooAbsPdf* MIT_bkg_pdf2__ = MIT_workspace_bkg__->pdf(Form("bern3_%sCat_GFcat", Meson));

    // Parameters
    float Torino_ggH_pdf_mean = Torino_workspace->var(Form("dCB_pole_%s_GFpreselection_ggH", Meson))->getValV();
    float Torino_ggH_pdf_width = Torino_workspace->var(Form("dCB_width_%s_GFpreselection_ggH", Meson))->getValV();
    float Torino_VBF_pdf_mean = Torino_workspace->var(Form("dCB_pole_%s_GFpreselection_VBF", Meson))->getValV();
    float Torino_VBF_pdf_width = Torino_workspace->var(Form("dCB_width_%s_GFpreselection_VBF", Meson))->getValV();
        float MIT_ggH_pdf_mean__ = MIT_workspace_sig__->var(Form("cb_mu_%sCat_GFcat_ggH", Meson))->getValV();
        float MIT_ggH_pdf_width__ = MIT_workspace_sig__->var(Form("cb_sigma_%sCat_GFcat_ggH", Meson))->getValV();
        float MIT_VBF_pdf_mean__ = MIT_workspace_sig__->var(Form("cb_mu_%sCat_GFcat_VBFH", Meson))->getValV();
        float MIT_VBF_pdf_width__ = MIT_workspace_sig__->var(Form("cb_sigma_%sCat_GFcat_VBFH", Meson))->getValV();

    std::cout << "*************************************************" << std::endl;
    std::cout << "GFcat: " << Meson << "Cat signal ggH PDF parameters (preselection)" << std::endl;
    std::cout << "Torino (March 19): mean=" << Torino_ggH_pdf_mean << ", sigma=" << Torino_ggH_pdf_width << std::endl;
    std::cout << "MIT (March 20, Pt:38,38, ph=WP90): mean=" << MIT_ggH_pdf_mean__ << ", sigma=" << MIT_ggH_pdf_width__ << std::endl;
    std::cout << "GFcat: " << Meson << "Cat signal VBF PDF parameters (preselection)" << std::endl;
    std::cout << "Torino (March 19): mean=" << Torino_VBF_pdf_mean << ", sigma=" << Torino_VBF_pdf_width << std::endl;
    std::cout << "MIT (March 20, Pt:38,38, ph=WP90): mean=" << MIT_VBF_pdf_mean__ << ", sigma=" << MIT_VBF_pdf_width__ << std::endl;
    std::cout << "*************************************************" << std::endl;

    // RooPlots
    /* RooRealVars */
    RooRealVar* var_Torino = Torino_workspace->var("mesonGammaMass");
        RooRealVar* var_MIT = MIT_workspace_sig__->var("mh");

    /* RooPlot frames */
    RooPlot* ggH_frame_Torino = var_Torino->frame();
    RooPlot* VBF_frame_Torino = var_Torino->frame();
    RooPlot* bkg_frame_Torino = var_Torino->frame();
    RooPlot* ggH_frame_MIT = var_MIT->frame();
    RooPlot* VBF_frame_MIT = var_MIT->frame();
    RooPlot* bkg_frame_MIT = var_MIT->frame();

    RooPlot* resid_ggH_frame_Torino = var_Torino->frame();
    RooPlot* resid_VBF_frame_Torino = var_Torino->frame();
    RooPlot* resid_bkg_frame_Torino = var_Torino->frame();
    RooPlot* resid_ggH_frame_MIT = var_MIT->frame();
    RooPlot* resid_VBF_frame_MIT = var_MIT->frame();
    RooPlot* resid_bkg_frame_MIT = var_MIT->frame();

    /* Plot DataSets & DataHists */
    Torino_ggH_data->plotOn(ggH_frame_Torino, Binning(140));
    Torino_VBF_data->plotOn(VBF_frame_Torino, Binning(140));
    Torino_observed_data->plotOn(bkg_frame_Torino, Binning(70));
        MIT_ggH_data__->plotOn(ggH_frame_MIT, Binning(140));
        MIT_VBF_data__->plotOn(VBF_frame_MIT, Binning(140));
        MIT_observed_data__->plotOn(bkg_frame_MIT, Binning(70));

    /* Draw RooAbsPdfs */
    Torino_ggH_pdf->plotOn(ggH_frame_Torino, LineColor(kBlue), LineStyle(kDashed));
    Torino_VBF_pdf->plotOn(VBF_frame_Torino, LineColor(kBlue), LineStyle(kDashed));
    Torino_bkg_pdf1->plotOn(bkg_frame_Torino, LineColor(kBlue), LineStyle(kDashed));
      // MIT_ggH_pdf_->plotOn(ggH_frame_MIT, LineColor(kRed), LineStyle(kDashed));
      // MIT_VBF_pdf_->plotOn(VBF_frame_MIT, LineColor(kRed), LineStyle(kDashed));
      // MIT_bkg_pdf1_->plotOn(bkg_frame_MIT, LineColor(kRed), LineStyle(kDashed));
        MIT_ggH_pdf__->plotOn(ggH_frame_MIT, LineColor(kRed), LineStyle(kDashed));
        MIT_VBF_pdf__->plotOn(VBF_frame_MIT, LineColor(kRed), LineStyle(kDashed));
        MIT_bkg_pdf1__->plotOn(bkg_frame_MIT, LineColor(kRed), LineStyle(kDashed));

    /* Construct residHists */
    RooHist* hresid_ggH_Torino = ggH_frame_Torino->residHist();
    RooHist* hresid_VBF_Torino = VBF_frame_Torino->residHist();
    RooHist* hresid_bkg_Torino = bkg_frame_Torino->residHist();
    RooHist* hresid_ggH_MIT = ggH_frame_MIT->residHist();
    RooHist* hresid_VBF_MIT = VBF_frame_MIT->residHist();
    RooHist* hresid_bkg_MIT = bkg_frame_MIT->residHist();

    resid_ggH_frame_Torino->addPlotable(hresid_ggH_Torino, "P");
    resid_VBF_frame_Torino->addPlotable(hresid_VBF_Torino, "P");
    resid_bkg_frame_Torino->addPlotable(hresid_bkg_Torino, "P");
    resid_ggH_frame_MIT->addPlotable(hresid_ggH_MIT, "P");
    resid_VBF_frame_MIT->addPlotable(hresid_VBF_MIT, "P");
    resid_bkg_frame_MIT->addPlotable(hresid_bkg_MIT, "P");

    /* Configure RooPlots */
    ggH_frame_Torino->SetTitle(Form("Torino GFcat %sCat ggH signal fit & residuals (preselection)", Meson));
    ggH_frame_Torino->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    ggH_frame_Torino->SetYTitle(Form("events/0.5 GeV"));
    resid_ggH_frame_Torino->SetTitle("");
    resid_ggH_frame_Torino->SetXTitle("");
    resid_ggH_frame_Torino->SetLabelSize(0.07, "XY");

    VBF_frame_Torino->SetTitle(Form("Torino GFcat %sCat VBF signal fit & residuals (preselection)", Meson));
    VBF_frame_Torino->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    VBF_frame_Torino->SetYTitle(Form("events/0.5 GeV"));
    resid_VBF_frame_Torino->SetTitle("");
    resid_VBF_frame_Torino->SetXTitle("");
    resid_VBF_frame_Torino->SetLabelSize(0.07, "XY");

    bkg_frame_Torino->SetTitle(Form("Torino GFcat %sCat background fit & residuals (preselection)", Meson));
    bkg_frame_Torino->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    bkg_frame_Torino->SetYTitle(Form("events/1 GeV"));
    resid_bkg_frame_Torino->SetTitle("");
    resid_bkg_frame_Torino->SetXTitle("");
    resid_bkg_frame_Torino->SetLabelSize(0.07, "XY");
    
    ggH_frame_MIT->SetTitle(Form("MIT GFcat %sCat ggH signal fit & residuals (preselection)", Meson));
    ggH_frame_MIT->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    ggH_frame_MIT->SetYTitle(Form("events/0.5 GeV"));
    resid_ggH_frame_MIT->SetTitle("");
    resid_ggH_frame_MIT->SetXTitle("");
    resid_ggH_frame_MIT->SetLabelSize(0.07, "XY");

    VBF_frame_MIT->SetTitle(Form("MIT GFcat %sCat VBF signal fit & residuals (preselection)", Meson));
    VBF_frame_MIT->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    VBF_frame_MIT->SetYTitle(Form("events/0.5 GeV"));
    resid_VBF_frame_MIT->SetTitle("");
    resid_VBF_frame_MIT->SetXTitle("");
    resid_VBF_frame_MIT->SetLabelSize(0.07, "XY");
    
    bkg_frame_MIT->SetTitle(Form("MIT GFcat %sCat background fit & residuals (preselection)", Meson));
    bkg_frame_MIT->SetXTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    bkg_frame_MIT->SetYTitle(Form("events/1 GeV"));
    resid_bkg_frame_MIT->SetTitle("");
    resid_bkg_frame_MIT->SetXTitle("");
    resid_bkg_frame_MIT->SetLabelSize(0.07, "XY");

    // Chi2
    // float chi2_ggH_Torino;
    // float chi2_VBF_Torino;
    // float chi2_bkg_Torino;
    // float chi2_ggH_MIT;
    // float chi2_VBF_MIT;
    // float chi2_bkg_MIT;
    
    // Draw & Save
    TCanvas c_ggH_resid_Torino("c", "c", 1200, 1200);
    TPad pad1_ggH_Torino("pad1", "pad1", 0, 0.333, 1, 1);
    TPad pad2_ggH_Torino("pad2", "pad2", 0, 0, 1, 0.333);
    TLegend legend_ggH_resid_Torino(0.9, 0.9, 0.5, 0.83);
    // auto text_ggH_param_Torino = new TText(0.6, 0.5, Form("mean=%f width=%f", Torino_ggH_pdf_mean, Torino_ggH_pdf_width));
    // ggH_frame_Torino->addObject(text_ggH_param_Torino);
    TPaveText text_ggH_param_Torino(0.6, 0.46, 0.8, 0.54);
    text_ggH_param_Torino.SetFillStyle(0);
    legend_ggH_resid_Torino.AddEntry(ggH_frame_Torino->findObject(Form("crystal_ball_%s_GFpreselection_ggH_Norm[mesonGammaMass]", Meson)), "Torino DoubleCrystalBall (March 19)", "l");
    legend_ggH_resid_Torino.AddEntry(ggH_frame_Torino->findObject("h_dataset_ggH"), "Torino signal dataset (March 19)", "p");
    text_ggH_param_Torino.AddText(Form("mean=%f width=%f", Torino_ggH_pdf_mean, Torino_ggH_pdf_width));
    pad1_ggH_Torino.cd();
    ggH_frame_Torino->Draw();
    text_ggH_param_Torino.DrawClone();
    legend_ggH_resid_Torino.Draw();
    pad2_ggH_Torino.cd();
    resid_ggH_frame_Torino->Draw();
    c_ggH_resid_Torino.cd();
    pad1_ggH_Torino.Draw();
    pad2_ggH_Torino.Draw();
    c_ggH_resid_Torino.SaveAs(Form("compare/presel_signal_ggH_resid_Torino_GF_%s.png", Meson));
    c_ggH_resid_Torino.Close();

    TCanvas c_ggH_resid_MIT("c", "c", 1200, 1200);
    TPad pad1_ggH_MIT("pad1", "pad1", 0, 0.333, 1, 1);
    TPad pad2_ggH_MIT("pad2", "pad2", 0, 0, 1, 0.333);
    TLegend legend_ggH_resid_MIT(0.9, 0.9, 0.5, 0.83);
    legend_ggH_resid_MIT.AddEntry(ggH_frame_MIT->findObject(Form("crystal_ball_%sCat_GFcat_ggH_Norm[mh]_Range[fit_nll_crystal_ball_%sCat_GFcat_ggH_datahist_GFcat_ggH]_NormRange[fit_nll_crystal_ball_%sCat_GFcat_ggH_datahist_GFcat_ggH]", Meson, Meson, Meson)), "MIT DoubleCrystalBall (March 20) Pt:38,38 ph=WP90", "l");
    legend_ggH_resid_MIT.AddEntry(ggH_frame_MIT->findObject("h_datahist_GFcat_ggH"), "MIT signal datahist (March 20) Pt:38,38 ph=WP90", "p");
    pad1_ggH_MIT.cd();
    ggH_frame_MIT->Draw();
    legend_ggH_resid_MIT.Draw();
    pad2_ggH_MIT.cd();
    resid_ggH_frame_MIT->Draw();
    c_ggH_resid_MIT.cd();
    pad1_ggH_MIT.Draw();
    pad2_ggH_MIT.Draw();
    c_ggH_resid_MIT.SaveAs(Form("compare/presel_signal_ggH_resid_MIT_GF_%s.png", Meson));
    c_ggH_resid_MIT.Close();
    
    TCanvas c_VBF_resid_Torino("c", "c", 1200, 1200);
    TPad pad1_VBF_Torino("pad1", "pad1", 0, 0.333, 1, 1);
    TPad pad2_VBF_Torino("pad2", "pad2", 0, 0, 1, 0.333);
    pad1_VBF_Torino.cd();
    TLegend legend_VBF_resid_Torino(0.9, 0.9, 0.5, 0.83);
    VBF_frame_Torino->Draw();
    legend_VBF_resid_Torino.AddEntry(VBF_frame_Torino->findObject(Form("crystal_ball_%s_GFpreselection_VBF_Norm[mesonGammaMass]", Meson)), "Torino DoubleCrystalBall (March 19)", "l");
    legend_VBF_resid_Torino.AddEntry(VBF_frame_Torino->findObject("h_dataset_VBF"), "Torino signal dataset (March 19)", "p");
    legend_VBF_resid_Torino.Draw();
    pad2_VBF_Torino.cd();
    resid_VBF_frame_Torino->Draw();
    c_VBF_resid_Torino.cd();
    pad1_VBF_Torino.Draw();
    pad2_VBF_Torino.Draw();
    c_VBF_resid_Torino.SaveAs(Form("compare/presel_signal_VBF_resid_Torino_GF_%s.png", Meson));
    c_VBF_resid_Torino.Close();

    TCanvas c_VBF_resid_MIT("c", "c", 1200, 1200);
    TPad pad1_VBF_MIT("pad1", "pad1", 0, 0.333, 1, 1);
    TPad pad2_VBF_MIT("pad2", "pad2", 0, 0, 1, 0.333);
    pad1_VBF_MIT.cd();
    TLegend legend_VBF_resid_MIT(0.9, 0.9, 0.5, 0.83);
    VBF_frame_MIT->Draw();
    legend_VBF_resid_MIT.AddEntry(VBF_frame_MIT->findObject(Form("crystal_ball_%sCat_GFcat_VBFH_Norm[mh]_Range[fit_nll_crystal_ball_%sCat_GFcat_VBFH_datahist_GFcat_VBFH]_NormRange[fit_nll_crystal_ball_%sCat_GFcat_VBFH_datahist_GFcat_VBFH]", Meson, Meson, Meson)), "MIT DoubleCrystalBall (March 20) Pt:38,38 ph=WP90", "l");
    legend_VBF_resid_MIT.AddEntry(VBF_frame_MIT->findObject("h_datahist_GFcat_VBFH"), "MIT signal datahist (March 20) Pt:38,38 ph=WP90", "p");
    legend_VBF_resid_MIT.Draw();
    pad2_VBF_MIT.cd();
    resid_VBF_frame_MIT->Draw();
    c_VBF_resid_MIT.cd();
    pad1_VBF_MIT.Draw();
    pad2_VBF_MIT.Draw();
    c_VBF_resid_MIT.SaveAs(Form("compare/presel_signal_VBF_resid_MIT_GF_%s.png", Meson));
    c_VBF_resid_MIT.Close();

    TCanvas c_data_resid_Torino("c", "c", 1200, 1200);
    TPad pad1_data_Torino("pad1", "pad1", 0, 0.333, 1, 1);
    TPad pad2_data_Torino("pad2", "pad2", 0, 0, 1, 0.333);
    pad1_data_Torino.cd();
    TLegend legend_data_resid_Torino(0.52, 0.19, 0.12, 0.12);
    bkg_frame_Torino->Draw();
    legend_data_resid_Torino.AddEntry(bkg_frame_Torino->findObject("chebychev_GFpreselection_bkg_Norm[mesonGammaMass]"), "Torino Chebychev dof=4 (March 19)", "l");
    legend_data_resid_Torino.AddEntry(bkg_frame_Torino->findObject("h_observed_data"), "Torino background dataset (March 19)", "p");
    legend_data_resid_Torino.Draw();
    pad2_data_Torino.cd();
    resid_bkg_frame_Torino->Draw();
    c_data_resid_Torino.cd();
    pad1_data_Torino.Draw();
    pad2_data_Torino.Draw();
    c_data_resid_Torino.SaveAs(Form("compare/presel_bkg_observed_data_resid_Torino_GF_%s.png", Meson));
    c_data_resid_Torino.Close();
    
    TCanvas c_data_resid_MIT("c", "c", 1200, 1200);
    TPad pad1_data_MIT("pad1", "pad1", 0, 0.333, 1, 1);
    TPad pad2_data_MIT("pad2", "pad2", 0, 0, 1, 0.333);
    pad1_data_MIT.cd();
    TLegend legend_data_resid_MIT(0.52, 0.19, 0.12, 0.12);
    bkg_frame_MIT->Draw();
    legend_data_resid_MIT.AddEntry(bkg_frame_MIT->findObject(Form("chebychev3_%sCat_GFcat_Norm[mh]_Range[fit_nll_chebychev3_%sCat_GFcat_datahist_%sCat_GFcat]_NormRange[fit_nll_chebychev3_%sCat_GFcat_datahist_%sCat_GFcat]", Meson, Meson, Meson, Meson, Meson)), "MIT Chebychev dof=4 (March 20) Pt:38,38 ph=WP90", "l");
    legend_data_resid_MIT.AddEntry(bkg_frame_MIT->findObject(Form("h_datahist_%sCat_GFcat", Meson)), "MIT background datahist (March 20) Pt:38,38 ph=WP90", "p");
    legend_data_resid_MIT.Draw();
    pad2_data_MIT.cd();
    resid_bkg_frame_MIT->Draw();
    c_data_resid_MIT.cd();
    pad1_data_MIT.Draw();
    pad2_data_MIT.Draw();
    c_data_resid_MIT.SaveAs(Form("compare/presel_bkg_observed_data_resid_MIT_GF_%s.png", Meson));
    c_data_resid_MIT.Close();
}
