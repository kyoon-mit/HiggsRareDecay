#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"

void makePullDist (const char* filename, const char* outprefix, float rGen)
{
    // Open file and tree
    TFile* file = TFile::Open(filename, "READ");
    TTree* tree_fit_sb = (TTree*) file->Get("tree_fit_sb");

    // Set branch address
    double r, rHiErr, rLoErr;
    tree_fit_sb->SetBranchAddress("r", &r);
    tree_fit_sb->SetBranchAddress("rHiErr", &rHiErr);
    tree_fit_sb->SetBranchAddress("rLoErr", &rLoErr);

    // Create histogram
    TH1F hr("hr", "r", 80, -20, 20);
    TH1F hpull("hpull",
               Form("(r-%f)/(0.5*(rHiErr+rLoErr))>>h(20,-5,5)", rGen),
               80, -20, 20);

    // Read entries and fill histogram
    long nentries = tree_fit_sb->GetEntries();
    for (long i=0; i<nentries; i++) {
        tree_fit_sb->GetEntry(i);
        hr.Fill(r);
        hpull.Fill((r-rGen)/(.5*(rHiErr+rLoErr)));
    }

    TCanvas c;
    //hpull.Fit("gaus");
    hr.Draw();
    //hpull.Draw();
    c.SaveAs(Form("%s_r.jpg", outprefix));

    TCanvas d;
    d.cd();
    hpull.Draw();
    d.SaveAs(Form("%s_pull.jpg", outprefix));
}
