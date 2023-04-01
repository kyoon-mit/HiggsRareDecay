#include <string.h>
#include <iostream>

using namespace RooFit;

void datacompare_mva(const char* Meson) {

    string date;
    const char* meson;
    if (strcmp(Meson, "Rho") == 0) {date = "230319"; meson = "rho";}
    if (strcmp(Meson, "Phi") == 0) {date = "230319"; meson = "phi";}

    // Files
    const char* Torino_presel_filename = Form("/work/submit/mariadlf/cards_march20/%s_%sGammaLimit/workspace_STAT_%s_GFcat_bdt0_2018.root", date.c_str(), Meson, Meson);
    const char* MIT_presel_sig_filename = Form("/work/submit/mariadlf/cards_march20/WSmva_MARCH20/Signal_GFcat__%sCat_2018_workspace.root", Meson);
    const char* MIT_presel_bkg_filename = Form("/work/submit/mariadlf/cards_march20/WSmva_MARCH20/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
        const char* MIT_presel_sig_filename__ = Form("/work/submit/mariadlf/cards_march20/WSmva_MARCH20_pt38/Signal_GFcat__%sCat_2018_workspace.root", Meson);
        const char* MIT_presel_bkg_filename__ = Form("/work/submit/mariadlf/cards_march20/WSmva_MARCH20_pt38/Bkg_GFcat__%sCat_2018_workspace.root", Meson);
    TFile* Torino_presel = TFile::Open(Torino_presel_filename, "READ");
    TFile* MIT_presel_sig = TFile::Open(MIT_presel_sig_filename, "READ");
    TFile* MIT_presel_bkg = TFile::Open(MIT_presel_bkg_filename, "READ");
        TFile* MIT_presel_sig__ = TFile::Open(MIT_presel_sig_filename__, "READ");
        TFile* MIT_presel_bkg__ = TFile::Open(MIT_presel_bkg_filename__, "READ");

    // Workspaces
    const char* Torino_workspace_name = Form("workspace_STAT_%s_GFcat_bdt0_2018", Meson);
    const char* MIT_workspace_name = "w";

    RooWorkspace* Torino_workspace = (RooWorkspace*) Torino_presel->Get(Torino_workspace_name);
    RooWorkspace* MIT_workspace_sig = (RooWorkspace*) MIT_presel_sig->Get(MIT_workspace_name);
    RooWorkspace* MIT_workspace_bkg = (RooWorkspace*) MIT_presel_bkg->Get(MIT_workspace_name);
        RooWorkspace* MIT_workspace_sig__ = (RooWorkspace*) MIT_presel_sig__->Get(MIT_workspace_name);
        RooWorkspace* MIT_workspace_bkg__ = (RooWorkspace*) MIT_presel_bkg__->Get(MIT_workspace_name);

    // Normalizations
    float Torino_ggH_signal_norm = Torino_workspace->var(Form("crystal_ball_%s_GFcat_bdt0_ggH_norm", Meson))->getValV();
    float Torino_VBF_signal_norm = Torino_workspace->var(Form("crystal_ball_%s_GFcat_bdt0_VBF_norm", Meson))->getValV();
    float Torino_data_bkg_norm = Torino_workspace->var(Form("multipdf_%s_GFcat_bdt0_bkg_norm", Meson))->getValV();
    float MIT_ggH_signal_norm = MIT_workspace_sig->var(Form("crystal_ball_%sCat_GFcat_ggH_norm", Meson))->getValV();
    float MIT_VBF_signal_norm = MIT_workspace_sig->var(Form("crystal_ball_%sCat_GFcat_VBFH_norm", Meson))->getValV();
    float MIT_data_bkg_norm = MIT_workspace_bkg->var(Form("multipdf_%sCat_GFcat_bkg_norm", Meson))->getValV();
        float MIT_ggH_signal_norm__ = MIT_workspace_sig__->var(Form("crystal_ball_%sCat_GFcat_ggH_norm", Meson))->getValV();
        float MIT_VBF_signal_norm__ = MIT_workspace_sig__->var(Form("crystal_ball_%sCat_GFcat_VBFH_norm", Meson))->getValV();
        float MIT_data_bkg_norm__ = MIT_workspace_bkg__->var(Form("multipdf_%sCat_GFcat_bkg_norm", Meson))->getValV();

    /* For now, print out normalizations */
    std::cout << "*************************************************" << std::endl;
    std::cout << "GFcat " << Meson << "Cat signal ggH normalizations (MVA) " << std::endl;
    std::cout << "Torino (March 20): " << Torino_ggH_signal_norm << /*", MIT (March 9): " << MIT_ggH_signal_norm <<*/ ", MIT (March 20): " << MIT_ggH_signal_norm << ", MIT (March 20, photon working point): " << MIT_ggH_signal_norm__ << std::endl;
    std::cout << "GFcat " << Meson << "Cat signal VBF normalizations (MVA) " << std::endl;
    std::cout << "Torino (March 20): " << Torino_VBF_signal_norm << /*", MIT (March 9): " << MIT_VBF_signal_norm <<*/ ", MIT (March 20): " << MIT_VBF_signal_norm << ", MIT (March 20, photon WP): " << MIT_VBF_signal_norm__ << std::endl;
    std::cout << "GFcat " << Meson << "Cat background normalizations (MVA) " << std::endl;
    std::cout << "Torino (March 20): " << Torino_data_bkg_norm << /*", MIT (March 9): " << MIT_data_bkg_norm <<*/ ", MIT (March 20): " << MIT_data_bkg_norm << ", MIT (March 20, photon WP): " << MIT_data_bkg_norm__ << std::endl;
    std::cout << "*************************************************" << std::endl;

    // DataSets & DataHists
    RooDataSet* Torino_ggH_data = (RooDataSet*) Torino_workspace->data("dataset_ggH");
    RooDataSet* Torino_VBF_data = (RooDataSet*) Torino_workspace->data("dataset_VBF");
    RooDataSet* Torino_observed_data = (RooDataSet*) Torino_workspace->data("observed_data");
    RooDataHist* MIT_ggH_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_ggH");
    RooDataHist* MIT_VBF_data = (RooDataHist*) MIT_workspace_sig->data("datahist_GFcat_VBFH");
    RooDataHist* MIT_observed_data = (RooDataHist*) MIT_workspace_bkg->data(Form("datahist_%sCat_GFcat", Meson));
        RooDataHist* MIT_ggH_data__ = (RooDataHist*) MIT_workspace_sig__->data("datahist_GFcat_ggH");
        RooDataHist* MIT_VBF_data__ = (RooDataHist*) MIT_workspace_sig__->data("datahist_GFcat_VBFH");
        RooDataHist* MIT_observed_data__ = (RooDataHist*) MIT_workspace_bkg__->data(Form("datahist_%sCat_GFcat", Meson));

    // Histograms
    /* Book new histograms */
    TH1F* Torino_ggH_signal = new TH1F("Torino_ggH_signal", "Torino (March 19)", 100., 170., 70);
    TH1F* MIT_ggH_signal = new TH1F("MIT_ggH_signal", "MIT (March 20) Pt:40,40 ph=WP90", 100., 170., 70);
    //   TH1F* MIT_ggH_signal_ = new TH1F("MIT_ggH_signal_", "MIT (March 20) Pt:40,40 ph=WP90", 100., 170., 70);
        TH1F* MIT_ggH_signal__ = new TH1F("MIT_ggH_signal__", "MIT (March 20) Pt:38,38 ph=WP80", 100., 170., 70);

    TH1F* Torino_VBF_signal = new TH1F("Torino_VBF_signal", "Torino (March 19)", 100., 170., 70);
    TH1F* MIT_VBF_signal = new TH1F("MIT_VBF_signal", "MIT (March 20) Pt:40,40 ph=WP90", 100., 170., 70);
    //   TH1F* MIT_VBF_signal_ = new TH1F("MIT_VBF_signal_", "MIT (March 20) Pt:40,40 ph=WP90", 100., 170., 70);
        TH1F* MIT_VBF_signal__ = new TH1F("MIT_VBF_signal__", "MIT (March 20) Pt:38,38 ph=WP90", 100., 170., 70);

    TH1F* Torino_comb_signal = new TH1F("Torino_comb_signal", "Torino (March 19)", 100., 170., 70);
    TH1F* MIT_comb_signal = new TH1F("MIT_comb_signal", "MIT (March 20) Pt:40,40 ph=WP90", 100., 170., 70);
    //   TH1F* MIT_comb_signal_ = new TH1F("MIT_comb_signal_", "MIT (March 20) Pt:40,40 ph=WP90", 100., 170., 70);
        TH1F* MIT_comb_signal__ = new TH1F("MIT_comb_signal__", "MIT (March 20) Pt:38,38 ph=WP90", 100., 170., 70);

    TH1F* Torino_data_bkg = new TH1F("Torino_data_bkg", "Torino (March 19)", 100., 170., 70);
    TH1F* MIT_data_bkg = new TH1F("MIT_data_bkg", "MIT (March 20) Pt:40,40 ph=WP90", 100., 170., 70);
    //   TH1F* MIT_data_bkg_ = new TH1F("MIT_data_bkg_", "MIT (March 20) Pt:40,40 ph=WP90", 100., 170., 70);
        TH1F* MIT_data_bkg__ = new TH1F("MIT_data_bkg__", "MIT (March 20) Pt:38,38 ph=WP90", 100., 170., 70);

    /* Fill histograms */
    Torino_ggH_signal->Add(Torino_ggH_data->createHistogram("mesonGammaMass", 70)); // Check binning
    MIT_ggH_signal->Add(MIT_ggH_data->createHistogram("mh", 70));
    //   MIT_ggH_signal_->Add(MIT_ggH_data_->createHistogram("mh", 70));
        MIT_ggH_signal__->Add(MIT_ggH_data__->createHistogram("mh", 70));

    Torino_VBF_signal->Add(Torino_VBF_data->createHistogram("mesonGammaMass", 70));
    MIT_VBF_signal->Add(MIT_VBF_data->createHistogram("mh", 70));
    //   MIT_VBF_signal_->Add(MIT_VBF_data_->createHistogram("mh", 70));  
        MIT_VBF_signal__->Add(MIT_VBF_data__->createHistogram("mh", 70));  

    Torino_comb_signal->Add(Torino_ggH_signal);
    Torino_comb_signal->Add(Torino_VBF_signal);
    MIT_comb_signal->Add(MIT_ggH_signal);
    MIT_comb_signal->Add(MIT_VBF_signal);
    //   MIT_comb_signal_->Add(MIT_ggH_signal_);
    //   MIT_comb_signal_->Add(MIT_VBF_signal_);
        MIT_comb_signal__->Add(MIT_ggH_signal__);
        MIT_comb_signal__->Add(MIT_VBF_signal__);
    
    Torino_data_bkg->Add(Torino_observed_data->createHistogram("mesonGammaMass", 70));
    MIT_data_bkg->Add(MIT_observed_data->createHistogram("mh", 70));
    //   MIT_data_bkg_->Add(MIT_observed_data_->createHistogram("mh", 70));
        MIT_data_bkg__->Add(MIT_observed_data__->createHistogram("mh", 70));
    
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
    MIT_ggH_signal->SetMarkerStyle(kFullTriangleUp);
    MIT_ggH_signal->SetMarkerSize(1.2);
    MIT_ggH_signal->SetMarkerColor(kGreen);
    MIT_VBF_signal->SetMarkerStyle(kFullTriangleUp);
    MIT_VBF_signal->SetMarkerSize(1.2);
    MIT_VBF_signal->SetMarkerColor(kGreen);
    MIT_comb_signal->SetMarkerStyle(kFullTriangleUp);
    MIT_comb_signal->SetMarkerSize(1.2);
    MIT_comb_signal->SetMarkerColor(kGreen);
    MIT_data_bkg->SetMarkerStyle(kFullTriangleUp);
    MIT_data_bkg->SetMarkerSize(1.2);
    MIT_data_bkg->SetMarkerColor(kGreen);
        MIT_ggH_signal__->SetMarkerStyle(kFullSquare);
        MIT_ggH_signal__->SetMarkerSize(1.2);
        MIT_ggH_signal__->SetMarkerColor(kRed);
        MIT_VBF_signal__->SetMarkerStyle(kFullSquare);
        MIT_VBF_signal__->SetMarkerSize(1.2);
        MIT_VBF_signal__->SetMarkerColor(kRed);
        MIT_comb_signal__->SetMarkerStyle(kFullSquare);
        MIT_comb_signal__->SetMarkerSize(1.2);
        MIT_comb_signal__->SetMarkerColor(kRed);
        MIT_data_bkg__->SetMarkerStyle(kFullSquare);
        MIT_data_bkg__->SetMarkerSize(1.2);
        MIT_data_bkg__->SetMarkerColor(kRed);
    
    // Make THStack
    THStack ggH_sig_stack("GFcat_ggH_signal", Form("GFcat %sCat ggH signal compare (MVA)", Meson));
    THStack VBF_sig_stack("GFcat_VBF_signal", Form("GFcat %sCat VBF signal compare (MVA)", Meson));
    THStack comb_sig_stack("GFcat_comb_signal", Form("GFcat %sCat combined signal compare (MVA)", Meson));
    THStack bkg_stack("GFcat_bkg", Form("GFcat %sCat background compare (MVA)", Meson));

    ggH_sig_stack.Add(Torino_ggH_signal);
    ggH_sig_stack.Add(MIT_ggH_signal);
    //   ggH_sig_stack.Add(MIT_ggH_signal_);
        ggH_sig_stack.Add(MIT_ggH_signal__);
    VBF_sig_stack.Add(Torino_VBF_signal);
    VBF_sig_stack.Add(MIT_VBF_signal);
    //   VBF_sig_stack.Add(MIT_VBF_signal_);
        VBF_sig_stack.Add(MIT_VBF_signal__);
    comb_sig_stack.Add(Torino_comb_signal);
    comb_sig_stack.Add(MIT_comb_signal);
    //   comb_sig_stack.Add(MIT_comb_signal_);
        comb_sig_stack.Add(MIT_comb_signal__);
    bkg_stack.Add(Torino_data_bkg);
    bkg_stack.Add(MIT_data_bkg);
    //   bkg_stack.Add(MIT_data_bkg_);
        bkg_stack.Add(MIT_data_bkg__);    
    
    // Draw & Save
    TCanvas c_ggH_hist("c", "c", 1200, 800);
    ggH_sig_stack.Draw("nostack p0");
    ggH_sig_stack.GetXaxis()->SetTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    ggH_sig_stack.GetYaxis()->SetTitle("Events/1 GeV");
    c_ggH_hist.BuildLegend(0.9, 0.9, 0.55, 0.77);
    c_ggH_hist.SaveAs(Form("compare/mva_signal_ggH_hist_compare_GF_%s.png", Meson));
    c_ggH_hist.Close();

    TCanvas c_VBF_hist("c", "c", 1200, 800);
    VBF_sig_stack.Draw("nostack p0");
    VBF_sig_stack.GetXaxis()->SetTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    VBF_sig_stack.GetYaxis()->SetTitle("Events/1 GeV");
    c_VBF_hist.BuildLegend(0.9, 0.9, 0.55, 0.77);
    c_VBF_hist.SaveAs(Form("compare/mva_signal_VBF_hist_compare_GF_%s.png", Meson));
    c_VBF_hist.Close();

    // TCanvas c_comb_hist;
    // comb_sig_stack.Draw("nostack p0");
    // comb_sig_stack.GetXaxis()->SetTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    // comb_sig_stack.GetYaxis()->SetTitle("Events/1 GeV");
    // c_comb_hist.BuildLegend(0.95, 0.9, 0.7, 0.75);
    // c_comb_hist.SaveAs(Form("compare/mva_signal_comb_hist_compare_GF_%s.png", Meson));
    // c_comb_hist.Close();
    
    TCanvas c_data_hist("c", "c", 1200, 800);
    bkg_stack.Draw("nostack e1p0");
    bkg_stack.GetXaxis()->SetTitle(Form("m_{#%s#gamma} [GeV/c^{2}]", meson));
    bkg_stack.GetYaxis()->SetTitle("Events/1 GeV");
    c_data_hist.BuildLegend(0.9, 0.9, 0.55, 0.77);
    c_data_hist.SaveAs(Form("compare/mva_bkg_observed_data_hist_compare_GF_%s.png", Meson));
    c_data_hist.Close();
}
