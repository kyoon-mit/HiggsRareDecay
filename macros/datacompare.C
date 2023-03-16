#include <string.h>
#include <iostream>

void datacompare(const char* Meson) {

    string date;
    if (strcmp(Meson, "Rho") == 0) date = "230309";
    if (strcmp(Meson, "Phi") == 0) date = "230310";

    // Files
    const char* Torino_presel_filename = Form("/work/submit/mariadlf/cards_march9/%s_%sGammaLimit/workspace_STAT_%s_GFpreselection_2018.root", date.c_str(), Meson, Meson);
    const char* MIT_presel_sig_filename = Form("/work/submit/mariadlf/cards_march9/WS_MARCH9/Signal_GFcat__%sCat_2018_workspace.root", Meson);
    const char* MIT_presel_bkg_filename = Form("/work/submit/mariadlf/cards_march9/WS_MARCH9/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
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
    std::cout << "GFcat " << Meson << "Cat signal ggH normalizations (without MVA) " << std::endl;
    std::cout << "Torino: " << Torino_ggH_signal_norm << ", MIT: " << MIT_ggH_signal_norm << std::endl;
    std::cout << "GFcat " << Meson << "Cat signal VBF normalizations (without MVA) " << std::endl;
    std::cout << "Torino: " << Torino_VBF_signal_norm << ", MIT: " << MIT_VBF_signal_norm << std::endl;
    std::cout << "GFcat " << Meson << "Cat background normalizations (without MVA) " << std::endl;
    std::cout << "Torino: " << Torino_data_bkg_norm << ", MIT: " << MIT_data_bkg_norm << std::endl;
    std::cout << "*************************************************" << std::endl;

    // Histograms
    /* Book new histograms */
    TH1F* Torino_ggH_signal = new TH1F("Torino_ggH_signal", "Torino", 100., 170., 70);
    TH1F* MIT_ggH_signal = new TH1F("MIT_ggH_signal", "MIT", 100., 170., 70);

    TH1F* Torino_VBF_signal = new TH1F("Torino_VBF_signal", "Torino", 100., 170., 70);
    TH1F* MIT_VBF_signal = new TH1F("MIT_VBF_signal", "MIT", 100., 170., 70);

    TH1F* Torino_comb_signal = new TH1F("Torino_comb_signal", "Torino", 100., 170., 70);
    TH1F* MIT_comb_signal = new TH1F("MIT_comb_signal", "MIT", 100., 170., 70);

    TH1F* Torino_data_bkg = new TH1F("Torino_data_bkg", "Torino", 100., 170., 70);
    TH1F* MIT_data_bkg = new TH1F("MIT_data_bkg", "MIT", 100., 170., 70);

    /* Fill histograms */
    Torino_ggH_signal->Add(Torino_workspace->data("dataset_ggH")->createHistogram("mesonGammaMass", 70)); // Check binning
    MIT_ggH_signal->Add(MIT_workspace_sig->data("datahist_GFcat_ggH")->createHistogram("mh", 70));

    Torino_VBF_signal->Add(Torino_workspace->data("dataset_VBF")->createHistogram("mesonGammaMass", 70));
    MIT_VBF_signal->Add(MIT_workspace_sig->data("datahist_GFcat_VBFH")->createHistogram("mh", 70));  

    Torino_comb_signal->Add(Torino_ggH_signal);
    Torino_comb_signal->Add(Torino_VBF_signal);
    MIT_comb_signal->Add(MIT_ggH_signal);
    MIT_comb_signal->Add(MIT_VBF_signal);
    
    Torino_data_bkg->Add(Torino_workspace->data("observed_data")->createHistogram("mesonGammaMass", 70));
    MIT_data_bkg->Add(MIT_workspace_bkg->data(Form("datahist_%sCat_GFcat", Meson))->createHistogram("mh", 70));
    
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
    Torino_ggH_signal->SetMarkerSize(0.8);
    Torino_ggH_signal->SetMarkerColor(kBlue);
    Torino_VBF_signal->SetMarkerStyle(kFullCircle);
    Torino_VBF_signal->SetMarkerSize(0.8);
    Torino_VBF_signal->SetMarkerColor(kBlue);
    Torino_comb_signal->SetMarkerStyle(kFullCircle);
    Torino_comb_signal->SetMarkerSize(0.8);
    Torino_comb_signal->SetMarkerColor(kBlue);
    Torino_data_bkg->SetMarkerStyle(kFullCircle);
    Torino_data_bkg->SetMarkerSize(0.8);
    Torino_data_bkg->SetMarkerColor(kBlue);
    MIT_ggH_signal->SetMarkerStyle(kFullSquare);
    MIT_ggH_signal->SetMarkerSize(0.8);
    MIT_ggH_signal->SetMarkerColor(kRed);
    MIT_VBF_signal->SetMarkerStyle(kFullSquare);
    MIT_VBF_signal->SetMarkerSize(0.8);
    MIT_VBF_signal->SetMarkerColor(kRed);
    MIT_comb_signal->SetMarkerStyle(kFullSquare);
    MIT_comb_signal->SetMarkerSize(0.8);
    MIT_comb_signal->SetMarkerColor(kRed);
    MIT_data_bkg->SetMarkerStyle(kFullSquare);
    MIT_data_bkg->SetMarkerSize(0.8);
    MIT_data_bkg->SetMarkerColor(kRed);
    
    // Make THStack
    THStack ggH_sig_stack("GFcat_ggH_signal", Form("GFcat %sCat ggH signal compare (no MVA)", Meson));
    THStack VBF_sig_stack("GFcat_VBF_signal", Form("GFcat %sCat VBF signal compare (no MVA)", Meson));
    THStack comb_sig_stack("GFcat_comb_signal", Form("GFcat %sCat combined signal compare (no MVA)", Meson));
    THStack bkg_stack("GFcat_bkg", Form("GFcat %sCat background compare (no MVA)", Meson));

    ggH_sig_stack.Add(Torino_ggH_signal);
    ggH_sig_stack.Add(MIT_ggH_signal);
    VBF_sig_stack.Add(Torino_VBF_signal);
    VBF_sig_stack.Add(MIT_VBF_signal);
    comb_sig_stack.Add(Torino_comb_signal);
    comb_sig_stack.Add(MIT_comb_signal);
    bkg_stack.Add(Torino_data_bkg);
    bkg_stack.Add(MIT_data_bkg);
    
    // Draw & Save
    TCanvas c_ggH;
    ggH_sig_stack.Draw("nostack p0");
    c_ggH.BuildLegend(0.95, 0.95, 0.7, 0.8);
    c_ggH.SaveAs(Form("signal_ggH_compare_GF_%s.png", Meson));
    c_ggH.Close();

    TCanvas c_VBF;
    VBF_sig_stack.Draw("nostack p0");
    c_VBF.BuildLegend(0.95, 0.95, 0.7, 0.8);
    c_VBF.SaveAs(Form("signal_VBF_compare_GF_%s.png", Meson));
    c_VBF.Close();

    TCanvas c_comb;
    comb_sig_stack.Draw("nostack p0");
    c_comb.BuildLegend(0.95, 0.95, 0.7, 0.8);
    c_comb.SaveAs(Form("signal_comb_compare_GF_%s.png", Meson));
    c_comb.Close();
    
    TCanvas c_data;
    bkg_stack.Draw("nostack e1p0");
    c_data.BuildLegend();
    c_data.SaveAs(Form("bkg_observed_data_compare_GF_%s.png", Meson));
    c_data.Close();
}
