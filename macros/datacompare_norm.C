#include <string.h>
#include <iostream>

using namespace RooFit;

void datacompare_norm(const char* Meson) {

    string date;
    if (strcmp(Meson, "Rho") == 0) date = "230309";
    if (strcmp(Meson, "Phi") == 0) date = "230310";

    // Files
    const char* Torino_presel_filename = Form("/work/submit/mariadlf/cards_march9/%s_%sGammaLimit/workspace_STAT_%s_GFpreselection_2018.root", date.c_str(), Meson, Meson);
    const char* MIT_presel_sig_filename = Form("/work/submit/mariadlf/cards_march16/WS_MARCH16_phWP/Signal_GFcat__%sCat_2018_workspace.root", Meson);
    const char* MIT_presel_bkg_filename = Form("/work/submit/mariadlf/cards_march16/WS_MARCH16_phWP/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
    TFile* Torino_presel = TFile::Open(Torino_presel_filename, "READ");
    TFile* MIT_presel_sig = TFile::Open(MIT_presel_sig_filename, "READ");
    TFile* MIT_presel_bkg = TFile::Open(MIT_presel_bkg_filename, "READ");

    // Workspaces
    const char* Torino_workspace_name = Form("workspace_STAT_%s_GFpreselection_2018", Meson);
    const char* MIT_workspace_name = "w";

    RooWorkspace* Torino_workspace = (RooWorkspace*) Torino_presel->Get(Torino_workspace_name);
    RooWorkspace* MIT_workspace_sig = (RooWorkspace*) MIT_presel_sig->Get(MIT_workspace_name);
    RooWorkspace* MIT_workspace_bkg = (RooWorkspace*) MIT_presel_bkg->Get(MIT_workspace_name);

    // Normalizations
    float Torino_ggH_signal_norm = Torino_workspace->var(Form("crystal_ball_%s_GFpreselection_ggH_norm", Meson))->getValV();
    float Torino_VBF_signal_norm = Torino_workspace->var(Form("crystal_ball_%s_GFpreselection_VBF_norm", Meson))->getValV();
    float Torino_data_bkg_norm = Torino_workspace->var(Form("multipdf_%s_GFpreselection_bkg_norm", Meson))->getValV();
    float MIT_ggH_signal_norm = MIT_workspace_sig->var(Form("crystal_ball_%sCat_GFcat_ggH_norm", Meson))->getValV();
    float MIT_VBF_signal_norm = MIT_workspace_sig->var(Form("crystal_ball_%sCat_GFcat_VBFH_norm", Meson))->getValV();
    float MIT_data_bkg_norm = MIT_workspace_bkg->var(Form("multipdf_%sCat_GFcat_bkg_norm", Meson))->getValV();

    /* For now, print out normalizations */
    std::cout << "*************************************************" << std::endl;
    std::cout << "GFcat " << Meson << "Cat signal ggH normalizations (preselection) " << std::endl;
    std::cout << "Torino: " << Torino_ggH_signal_norm << ", MIT (March 16, photon working point): " << MIT_ggH_signal_norm << std::endl;
    std::cout << "GFcat " << Meson << "Cat signal VBF normalizations (preselection) " << std::endl;
    std::cout << "Torino: " << Torino_VBF_signal_norm << ", MIT (March 16, photon WP): " << MIT_VBF_signal_norm << std::endl;
    std::cout << "GFcat " << Meson << "Cat background normalizations (preselection) " << std::endl;
    std::cout << "Torino: " << Torino_data_bkg_norm << ", MIT (March 16, photon WP): " << MIT_data_bkg_norm << std::endl;
    std::cout << "*************************************************" << std::endl;

    // DataSets & DataHists
    RooDataSet* Torino_ggH_data = (RooDataSet*) Torino_workspace->data("dataset_ggH");
    RooDataSet* Torino_VBF_data = (RooDataSet*) Torino_workspace->data("dataset_VBF");
    RooDataSet* Torino_observed_data = (RooDataSet*) Torino_workspace->data("observed_data");
    RooDataHist* MIT_ggH_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_ggH");
    RooDataHist* MIT_VBF_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_VBFH");
    RooDataHist* MIT_observed_data = (RooDataHist*) MIT_workspace_bkg->data(Form("datahist_%sCat_GFcat", Meson));

    // PDFs
    // RooAbsPdf* Torino_ggH_pdf = Torino_workspace->pdf(Form("crystal_ball_%s_GFpreselection_ggH", Meson));
    // RooAbsPdf* Torino_VBF_pdf = Torino_workspace->pdf(Form("crystal_ball_%s_GFpreselection_VBF", Meson));
    // RooAbsPdf* Torino_bkg_pdf1 = Torino_workspace->pdf("chebychev_GFpreselection_bkg");
    // RooAbsPdf* Torino_bkg_pdf2;
    // RooAbsPdf* MIT_ggH_pdf = MIT_workspace_sig->pdf(Form("crystal_ball_%sCat_GFcat_ggH", Meson));
    // RooAbsPdf* MIT_VBF_pdf = MIT_workspace_sig->pdf(Form("crystal_ball_%sCat_GFcat_VBFH", Meson));
    // RooAbsPdf* MIT_bkg_pdf1 = MIT_workspace_bkg->pdf(Form("chebychev3_%sCat_GFcat", Meson));
    // RooAbsPdf* MIT_bkg_pdf2 = MIT_workspace_bkg->pdf(Form("bern3_%sCat_GFcat", Meson));
    
    // // RooPlots
    // RooRealVar* x = MIT_workspace_sig->var("mh");
    // RooPlot* ggH_frame = x->frame();
    // RooPlot* VBF_frame = x->frame();
    // RooPlot* bkg_frame = x->frame();

    // Torino_ggH_pdf->plotOn(ggH_frame, LineColor(kRed), LineStyle(kDashed));
    // MIT_ggH_pdf->plotOn(ggH_frame, LineColor(kBlue), LineStyle(kDashed));
    // Torino_VBF_pdf->plotOn(VBF_frame, LineColor(kRed), LineStyle(kDashed));
    // MIT_VBF_pdf->plotOn(VBF_frame, LineColor(kBlue), LineStyle(kDashed));
    // Torino_bkg_pdf1->plotOn(bkg_frame, LineColor(kRed), LineStyle(kDashed));
    // MIT_bkg_pdf1->plotOn(bkg_frame, LineColor(kBlue), LineStyle(kDashed));

    // Histograms
    /* Book new histograms */
    TH1F* Torino_ggH_signal = new TH1F("Torino_ggH_signal", "Torino", 100., 170., 70);
    TH1F* MIT_ggH_signal = new TH1F("MIT_ggH_signal", "MIT (March 16) Pt:38,38 ph=WP80", 100., 170., 70);

    TH1F* Torino_VBF_signal = new TH1F("Torino_VBF_signal", "Torino", 100., 170., 70);
    TH1F* MIT_VBF_signal = new TH1F("MIT_VBF_signal", "MIT (March 16) Pt:38,38 ph=WP80", 100., 170., 70);

    TH1F* Torino_comb_signal = new TH1F("Torino_comb_signal", "Torino", 100., 170., 70);
    TH1F* MIT_comb_signal = new TH1F("MIT_comb_signal", "MIT (March 16) Pt:38,38 ph=WP80", 100., 170., 70);

    TH1F* Torino_data_bkg = new TH1F("Torino_data_bkg", "Torino", 100., 170., 70);
    TH1F* MIT_data_bkg = new TH1F("MIT_data_bkg", "MIT (March 16) Pt:38,38 ph=WP80", 100., 170., 70);

    /* Fill histograms */
    Torino_ggH_signal->Add(Torino_ggH_data->createHistogram("mesonGammaMass", 70)); // Check binning
    MIT_ggH_signal->Add(MIT_ggH_data->createHistogram("mh", 70));

    Torino_VBF_signal->Add(Torino_VBF_data->createHistogram("mesonGammaMass", 70));
    MIT_VBF_signal->Add(MIT_VBF_data->createHistogram("mh", 70));

    Torino_comb_signal->Add(Torino_ggH_signal);
    Torino_comb_signal->Add(Torino_VBF_signal);
    MIT_comb_signal->Add(MIT_ggH_signal);
    MIT_comb_signal->Add(MIT_VBF_signal);
    
    Torino_data_bkg->Add(Torino_observed_data->createHistogram("mesonGammaMass", 70));
    MIT_data_bkg->Add(MIT_observed_data->createHistogram("mh", 70));

    /* Normalize histograms */
    double factor = 1;
    Torino_ggH_signal->Scale(factor/Torino_ggH_signal->Integral());
    Torino_VBF_signal->Scale(factor/Torino_VBF_signal->Integral());
    Torino_comb_signal->Scale(factor/Torino_comb_signal->Integral());
    Torino_data_bkg->Scale(factor/Torino_data_bkg->Integral());
    MIT_ggH_signal->Scale(factor/MIT_ggH_signal->Integral());
    MIT_VBF_signal->Scale(factor/MIT_VBF_signal->Integral());
    MIT_comb_signal->Scale(factor/MIT_comb_signal->Integral());
    MIT_data_bkg->Scale(factor/MIT_data_bkg->Integral());
    
    /* Configure histograms */
    // Torino_ggH_signal->SetTitle("Torino");
    // Torino_VBF_signal->SetTitle("Torino");
    // Torino_comb_signal->SetTitle("Torino");
    // Torino_data_bkg->SetTitle("Torino");
    // MIT_ggH_signal->SetTitle("MIT");
    // MIT_VBF_signal->SetTitle("MIT");
    // MIT_comb_signal->SetTitle("MIT");
    // MIT_data_bkg->SetTitle("MIT");
    Torino_ggH_signal->SetMarkerStyle(kFullCircle);
    Torino_ggH_signal->SetMarkerSize(1.2);
    Torino_ggH_signal->SetMarkerColor(kBlue);
    Torino_VBF_signal->SetMarkerStyle(kFullCircle);
    Torino_VBF_signal->SetMarkerSize(1.2);
    Torino_VBF_signal->SetMarkerColor(kBlue);
    Torino_comb_signal->SetMarkerStyle(kFullCircle);
    Torino_comb_signal->SetMarkerSize(1.2);
    Torino_comb_signal->SetMarkerColor(kBlue);
    Torino_data_bkg->SetMarkerStyle(kFullCircle);
    Torino_data_bkg->SetMarkerSize(1.2);
    Torino_data_bkg->SetMarkerColor(kBlue);
    MIT_ggH_signal->SetMarkerStyle(kFullSquare);
    MIT_ggH_signal->SetMarkerSize(1.2);
    MIT_ggH_signal->SetMarkerColor(kRed);
    MIT_VBF_signal->SetMarkerStyle(kFullSquare);
    MIT_VBF_signal->SetMarkerSize(1.2);
    MIT_VBF_signal->SetMarkerColor(kRed);
    MIT_comb_signal->SetMarkerStyle(kFullSquare);
    MIT_comb_signal->SetMarkerSize(1.2);
    MIT_comb_signal->SetMarkerColor(kRed);
    MIT_data_bkg->SetMarkerStyle(kFullSquare);
    MIT_data_bkg->SetMarkerSize(1.2);
    MIT_data_bkg->SetMarkerColor(kRed);
    
    // Make THStack
    THStack ggH_sig_stack("GFcat_ggH_signal", Form("GFcat %sCat ggH signal compare (preselection, normalized)", Meson));
    THStack VBF_sig_stack("GFcat_VBF_signal", Form("GFcat %sCat VBF signal compare (preselection, normalized)", Meson));
    THStack comb_sig_stack("GFcat_comb_signal", Form("GFcat %sCat combined signal compare (preselection, normalized)", Meson));
    THStack bkg_stack("GFcat_bkg", Form("GFcat %sCat background compare (preselection, normalized)", Meson));

    ggH_sig_stack.Add(Torino_ggH_signal);
    ggH_sig_stack.Add(MIT_ggH_signal);
    VBF_sig_stack.Add(Torino_VBF_signal);
    VBF_sig_stack.Add(MIT_VBF_signal);
    comb_sig_stack.Add(Torino_comb_signal);
    comb_sig_stack.Add(MIT_comb_signal);
    bkg_stack.Add(Torino_data_bkg);
    bkg_stack.Add(MIT_data_bkg);
    
    // Draw & Save
    // TCanvas c_ggH_fit("c", "c", 1200, 800);
    // ggH_frame->Draw();
    // c_ggH_fit.BuildLegend(0.9, 0.9, 0.65, 0.75);
    // c_ggH_fit.SaveAs(Form("compare/presel_normalized_signal_ggH_fit_compare_GF_%s.png", Meson));
    // c_ggH_fit.Close();

    // TCanvas c_VBF_fit("c", "c", 1200, 800);
    // VBF_frame->Draw();
    // c_VBF_fit.BuildLegend(0.9, 0.9, 0.65, 0.75);
    // c_VBF_fit.SaveAs(Form("compare/presel_normalized_signal_VBF_fit_compare_GF_%s.png", Meson));
    // c_VBF_fit.Close();
    
    // TCanvas c_data_fit("c", "c", 1200, 800);
    // bkg_frame->Draw();
    // c_data_fit.BuildLegend(0.37, 0.27, 0.12, 0.12);
    // c_data_fit.SaveAs(Form("compare/presel_normalized_bkg_observed_data_fit_compare_GF_%s.png", Meson));
    // c_data_fit.Close();

    //
    TCanvas c_ggH_hist("c", "c", 1200, 800);
    ggH_sig_stack.Draw("nostack p0");
    c_ggH_hist.BuildLegend(0.9, 0.9, 0.65, 0.75);
    c_ggH_hist.SaveAs(Form("compare/presel_normalized_signal_ggH_hist_compare_GF_%s.png", Meson));
    c_ggH_hist.Close();

    TCanvas c_VBF_hist("c", "c", 1200, 800);
    VBF_sig_stack.Draw("nostack p0");
    c_VBF_hist.BuildLegend(0.9, 0.9, 0.65, 0.75);
    c_VBF_hist.SaveAs(Form("compare/presel_normalized_signal_VBF_hist_compare_GF_%s.png", Meson));
    c_VBF_hist.Close();

    // TCanvas c_comb_hist;
    // comb_sig_stack.Draw("nostack p0");
    // c_comb_hist.BuildLegend(0.95, 0.9, 0.7, 0.75);
    // c_comb_hist.SaveAs(Form("compare/presel_normalized_signal_comb_hist_compare_GF_%s.png", Meson));
    // c_comb_hist.Close();
    
    TCanvas c_data_hist("c", "c", 1200, 800);
    bkg_stack.Draw("nostack e1p0");
    c_data_hist.BuildLegend(0.37, 0.27, 0.12, 0.12);
    c_data_hist.SaveAs(Form("compare/presel_normalized_bkg_observed_data_hist_compare_GF_%s.png", Meson));
    c_data_hist.Close();
}
