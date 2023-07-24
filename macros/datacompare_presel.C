#include <string.h>
#include <iostream>

using namespace RooFit;

void datacompare_presel(const char* Meson) {

    const char* meson;
    if (strcmp(Meson, "Rho") == 0) {meson = "rho";}
    if (strcmp(Meson, "Phi") == 0) {meson = "phi";}

    // Files
    const char* Torino_presel_filename = Form("/work/submit/kyoon/RareHiggs/Torino_cards/230724/workspace_STAT_%s_GFpreselection_2018.root", Meson);
    const char* MIT_presel_sig_filename = Form("/work/submit/mariadlf/cards_JUL22/WS_JUL22/Signal_GFcat__%sCat_2018_workspace.root", Meson);
    const char* MIT_presel_bkg_filename = Form("/work/submit/mariadlf/cards_JUL22/workspaces_JUL22/workspace_%s_GFcat_2018.root", Meson);
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
    std::cout << "Torino (July 24): " << Torino_ggH_signal_norm << ", MIT (July 22): " << MIT_ggH_signal_norm << std::endl;
    std::cout << "GFcat " << Meson << "Cat signal VBF normalizations (preselection) " << std::endl;
    std::cout << "Torino (July 24): " << Torino_VBF_signal_norm << ", MIT (July 22): " << MIT_VBF_signal_norm << std::endl;
    std::cout << "GFcat " << Meson << "Cat background normalizations (preselection) " << std::endl;
    std::cout << "Torino (July 24): " << Torino_data_bkg_norm << ", MIT (July 22): " << MIT_data_bkg_norm << std::endl;
    std::cout << "*************************************************" << std::endl;

    // DataSets & DataHists
    // RooDataSet* Torino_ggH_data = (RooDataSet*) Torino_workspace->data("dataset_ggH");
    // RooDataSet* Torino_VBF_data = (RooDataSet*) Torino_workspace->data("dataset_VBF");
    TH1F* Torino_ggH_data= (TH1F*) Torino_presel->Get("h_mH_ggH");
    TH1F* Torino_VBF_data= (TH1F*) Torino_presel->Get("h_mH_VBF");
    RooDataSet* Torino_observed_data = (RooDataSet*) Torino_workspace->data("observed_data");
    RooDataHist* MIT_ggH_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_ggH");
    RooDataHist* MIT_VBF_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_VBFH");
    RooDataHist* MIT_observed_data = (RooDataHist*) MIT_workspace_bkg->data("observed_data");

    // Histograms
    /* Book new histograms */
    TH1F* Torino_ggH_signal = new TH1F("Torino_ggH_signal", "Torino", 500, 110., 160.);
    TH1F* MIT_ggH_signal = new TH1F("MIT_ggH_signal", "MIT (July 22)", 500, 110., 160.);

    TH1F* Torino_VBF_signal = new TH1F("Torino_VBF_signal", "Torino (July 24)", 500, 110., 160.);
    TH1F* MIT_VBF_signal = new TH1F("MIT_VBF_signal", "MIT (July 22)", 500, 110., 160.);

    TH1F* Torino_comb_signal = new TH1F("Torino_comb_signal", "Torino (July 24)", 500, 110., 160.);
    TH1F* MIT_comb_signal = new TH1F("MIT_comb_signal", "MIT (July 22)", 500, 110., 160.);

    TH1F* Torino_data_bkg = new TH1F("Torino_data_bkg", "Torino (July 24)", 50, 110., 160.);
    TH1F* MIT_data_bkg = new TH1F("MIT_data_bkg", "MIT (July 22)", 50, 110., 160.);

    /* Fill histograms */
    // Torino_ggH_signal->Add(Torino_ggH_data->createHistogram("mesonGammaMass", 70)); // Check binning
    Torino_ggH_signal->Add(Torino_ggH_data); // Check binning
    MIT_ggH_signal->Add(MIT_ggH_data->createHistogram("MIT_ggH_signal", *(MIT_workspace_sig->var("mh")), Binning(500, 110, 160)));

    // Torino_VBF_signal->Add(Torino_VBF_data->createHistogram("mesonGammaMass", 70));
    Torino_VBF_signal->Add(Torino_VBF_data);
    MIT_VBF_signal->Add(MIT_VBF_data->createHistogram("MIT_VBF_signal", *(MIT_workspace_sig->var("mh")), Binning(500, 110, 160)));

    Torino_comb_signal->Add(Torino_ggH_signal);
    Torino_comb_signal->Add(Torino_VBF_signal);
    MIT_comb_signal->Add(MIT_ggH_signal);
    MIT_comb_signal->Add(MIT_VBF_signal);
    
    Torino_data_bkg->Add(Torino_observed_data->createHistogram("Torino_data_bkg", *(Torino_workspace->var("mesonGammaMass")), Binning(50, 110, 160)));
    MIT_data_bkg->Add(MIT_observed_data->createHistogram("MIT_data_bkg", *(MIT_workspace_bkg->var("mh")), Binning(50, 110 ,160)));

    /* Normalize histograms */
    // double factor = 1;
    // Torino_ggH_signal->Scale(factor*Torino_ggH_signal_norm/Torino_ggH_signal->Integral());
    // Torino_VBF_signal->Scale(factor*Torino_VBF_signal_norm/Torino_VBF_signal->Integral());
    // Torino_comb_signal->Scale(factor*(Torino_ggH_signal_norm+Torino_VBF_signal_norm)/Torino_comb_signal->Integral());
    // Torino_data_bkg->Scale(factor*Torino_data_bkg_norm/Torino_data_bkg->Integral());
    
    /* Configure histograms */
    Torino_ggH_signal->SetTitle("Torino");
    Torino_VBF_signal->SetTitle("Torino");
    Torino_comb_signal->SetTitle("Torino");
    Torino_data_bkg->SetTitle("Torino");
    MIT_ggH_signal->SetTitle("MIT");
    MIT_VBF_signal->SetTitle("MIT");
    MIT_comb_signal->SetTitle("MIT");
    MIT_data_bkg->SetTitle("MIT");
    Torino_ggH_signal->SetMarkerStyle(kFullCircle);
    Torino_ggH_signal->SetMarkerSize(1.2);
    Torino_ggH_signal->SetMarkerColor(kBlue);
    Torino_VBF_signal->SetMarkerStyle(kFullCircle);
    Torino_VBF_signal->SetMarkerSize(1.2);
    Torino_VBF_signal->SetMarkerColor(kBlue);
    Torino_comb_signal->SetMarkerStyle(kFullCircle);
    Torino_comb_signal->SetMarkerSize(0.8);
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
    MIT_comb_signal->SetMarkerSize(0.8);
    MIT_comb_signal->SetMarkerColor(kRed);
    MIT_data_bkg->SetMarkerStyle(kFullSquare);
    MIT_data_bkg->SetMarkerSize(1.2);
    MIT_data_bkg->SetMarkerColor(kRed);
    
    // Make THStack
    THStack ggH_sig_stack("GFcat_ggH_signal", Form("GFcat %sCat ggH signal compare (preselection)", Meson));
    THStack VBF_sig_stack("GFcat_VBF_signal", Form("GFcat %sCat VBF signal compare (preselection)", Meson));
    THStack comb_sig_stack("GFcat_comb_signal", Form("GFcat %sCat combined signal compare (preselection)", Meson));
    THStack bkg_stack("GFcat_bkg", Form("GFcat %sCat background compare (preselection)", Meson));

    ggH_sig_stack.Add(Torino_ggH_signal);
    ggH_sig_stack.Add(MIT_ggH_signal);
    VBF_sig_stack.Add(Torino_VBF_signal);
    VBF_sig_stack.Add(MIT_VBF_signal);
    comb_sig_stack.Add(Torino_comb_signal);
    comb_sig_stack.Add(MIT_comb_signal);
    bkg_stack.Add(Torino_data_bkg);
    bkg_stack.Add(MIT_data_bkg);
    
    TCanvas c_ggH_hist("c", "c", 1200, 800);
    ggH_sig_stack.Draw("nostack p0");
    ggH_sig_stack.Draw("nostack p0");
    ggH_sig_stack.GetXaxis()->SetTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    ggH_sig_stack.GetYaxis()->SetTitle("Events/0.1 GeV");
    c_ggH_hist.BuildLegend(0.9, 0.9, 0.55, 0.77);
    c_ggH_hist.SaveAs(Form("compare/presel_signal_ggH_hist_compare_GF_%s_JUL24.png", Meson));
    c_ggH_hist.Close();

    TCanvas c_VBF_hist("c", "c", 1200, 800);
    VBF_sig_stack.Draw("nostack p0");
    VBF_sig_stack.GetXaxis()->SetTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    VBF_sig_stack.GetYaxis()->SetTitle("Events/0.1 GeV");
    c_VBF_hist.BuildLegend(0.9, 0.9, 0.55, 0.77);
    c_VBF_hist.SaveAs(Form("compare/presel_signal_VBF_hist_compare_GF_%s_JUL24.png", Meson));
    c_VBF_hist.Close();

    TCanvas c_comb_hist;
    comb_sig_stack.Draw("nostack p0");
    comb_sig_stack.GetXaxis()->SetTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    comb_sig_stack.GetYaxis()->SetTitle("Events/0.1 GeV");
    c_comb_hist.BuildLegend(0.9, 0.9, 0.65, 0.77);
    c_comb_hist.SaveAs(Form("compare/presel_signal_comb_hist_compare_GF_%s_JUL24.png", Meson));
    c_comb_hist.Close();
    
    TCanvas c_data_hist("c", "c", 1200, 800);
    bkg_stack.Draw("nostack e1p0");
    bkg_stack.GetXaxis()->SetTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    bkg_stack.GetYaxis()->SetTitle("Events/1 GeV");
    c_data_hist.BuildLegend(0.47, 0.25, 0.12, 0.12);
    c_data_hist.SaveAs(Form("compare/presel_bkg_observed_data_hist_compare_GF_%s_JUL24.png", Meson));
    c_data_hist.Close();
}
