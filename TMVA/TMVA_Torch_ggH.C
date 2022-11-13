#include <string.h>

#include "TROOT.h"
#include "TMVA/CrossValidation.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Factory.h"
#include "TMVA/Types.h"
#include "TFile.h"
#include "TCollection.h"
#include "TCut.h"
#include "TObjArray.h"
#include "TString.h"
#include "TTree.h"

void TMVA_ggH ( const char* outFileName,
                const char* channel )
{
    (TMVA::gConfig().GetVariablePlotting()).fMaxNumOfAllowedVariablesForScatterPlots = 25;
    
    // options to control used methods
    bool useRandomSplitting = false; // option for cross validation
    bool useLikelihood = true;    // likelihood based discriminant
    bool useTorch = true;        // TMVA with PyTorch Deep Learning
    bool useBDTG = true;         // BDT with GradBoost
    
    // Open files
    TString fileformat;
    TFile* sgnfile;
    TFile* bkgfile1;
    TFile* bkgfile2;
    TFile* bkgfile3;
    TFile* bkgfile4;
    TFile* bkgfile5;
    if ( std::strcmp(channel, "phi") == 0 ) {
        fileformat = "/work/submit/kyoon/RareHiggs/data/cat_phi/cat_phi_ggH/test/test_mc%d_GFcat_PhiCat_2018.root";
        // fileformat = "/work/submit/mariadlf/JUNE7/2018/outname_mc%d_GFcat_PhiCat_2018.root";
        sgnfile = TFile::Open(Form(fileformat, 1017), "READ");
    } else if ( std::strcmp(channel, "rho") == 0 ) {
        fileformat = "/work/submit/kyoon/RareHiggs/data/cat_rho/cat_rho_ggH/test/test_mc%d_GFcat_RhoCat_2018.root";
        // fileformat = "/work/submit/mariadlf/JUNE7/2018/outname_mc%d_GFcat_RhoCat_2018.root";
        sgnfile = TFile::Open(Form(fileformat, 1027), "READ");
    }
    bkgfile1 = TFile::Open(Form(fileformat, 6), "READ");
    bkgfile2 = TFile::Open(Form(fileformat, 7), "READ");
    bkgfile3 = TFile::Open(Form(fileformat, 8), "READ");
    bkgfile4 = TFile::Open(Form(fileformat, 9), "READ");
    bkgfile5 = TFile::Open(Form(fileformat, 10), "READ");

    // TFile* sgnfile = TFile::Open("/work/submit/kyoon/RareHiggs/data/cat_rho/cat_rho_ggH/test/test_signal_GFcat_RhoCat_2018.root");
    // TFile* bkgfile = TFile::Open("/work/submit/kyoon/RareHiggs/data/cat_rho/cat_rho_ggH/test/test_background_GFcat_RhoCat_2018.root");
    
    // Initialize the dataset
    TFile* outfile = TFile::Open(outFileName, "RECREATE");    
    TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");
    
    // Add variables to dataset
    const char* idx0 = "index_pair[0]"; // Meson index
    const char* idx1 = "index_pair[1]"; // Photon index

    dataloader->AddVariable("HCandMass", "HCandMass", "GeV/c^2", 'F'); // USE ONLY
    Dataloader->AddVariable("HCandPT", "HCandPT", "", 'F');
    dataloader->AddVariable("HCandPT__div_HCandMass", "HCandPT__div_HCandMass", "", 'F');
    dataloader->AddVariable("HCandPT__div_sqrtHCandMass", "HCandPT__div_sqrtHCandMass", "", 'F');
    dataloader->AddVariable("goodPhotons_pt", "goodPhotons_pt", "", 'F');
    dataloader->AddVariable("goodPhotons_pt__div_HCandPT", "goodPhotons_pt__div_HCandPT", "", 'F');
    dataloader->AddVariable("goodPhotons_pt__div_HCandMass", "goodPhotons_pt__div_HCandMass", "", 'F');
    dataloader->AddVariable("goodPhotons_pt__div_sqrtHCandMass", "goodPhotons_pt__div_sqrtHCandMass", "", 'F');
    dataloader->AddVariable("goodMeson_pt", "goodMeson_pt", "", 'F');
    dataloader->AddVariable("goodMeson_pt__div_HCandPT", "goodMeson_pt__div_HCandPT", "", 'F');
    dataloader->AddVariable("goodMeson_pt__div_HCandMass", "goodMeson_pt__div_HCandMass", "", 'F');
    dataloader->AddVariable("goodMeson_pt__div_sqrtHCandMass", "goodMeson_pt__div_sqrtHCandMass", "", 'F');
    dataloader->AddVariable("goodMeson_DR", "goodMeson_DR", "", 'F');
    dataloader->AddVariable("goodMeson_DR * HCandMass", "goodMeson_DR__times_HCandMass", "", 'F');
    dataloader->AddVariable("goodMeson_DR__times_sqrtHCandMass", "goodMeson_DR__times_sqrtHCandMass", "", 'F');
        
    dataloader->AddVariable("dPhiGammaMesonCand", "dPhiGammaMesonCand", "", 'F');
    dataloader->AddVariable("dEtaGammaMesonCand", "dEtaGammaMesonCand", "", 'F');
    dataloader->AddVariable("dPhiGammaMesonCand/HCandMass", "dPhiGammaMesonCand__div_HCandMass", "", 'F');
    dataloader->AddVariable("dEtaGammaMesonCand/HCandMass", "dEtaGammaMesonCand__div_HCandMass", "", 'F');
    dataloader->AddVariable("dPhiGammaMesonCand__div_sqrtHCandMass", "dPhiGammaMesonCand__div_sqrtHCandMass", "", 'F');
    dataloader->AddVariable("dEtaGammaMesonCand__div_sqrtHCandMass", "dEtaGammaMesonCand__div_sqrtHCandMass", "", 'F');
    */
    // Set weights
    dataloader->SetWeightExpression("w");

    // Spectator used for split
    // dataloader->AddSpectator("Entry$", "eventID");

    // Apply split
    ////////////////////// TODO: use cross validation /////////////////////////
    const char* trainTreeEventSplitStr = "(Entry$ % 3) > 0"; //"(events % 10)>=5";
    const char* testTreeEventSplitStr = "(Entry$ % 3) == 0"; // "(events % 10)<5";
    
    // Apply cuts
    const char* higgsMass = /* "HCandMass > 40"; */ "HCandMass > 115 && HCandMass <= 135";
    const char* nanRemove = "!TMath::IsNaN(goodMeson_massErr) && !TMath::IsNaN(sigmaHCandMass_Rel2)";
    
    TCut cutSignalTrain = Form("%s && %s && %s", trainTreeEventSplitStr, higgsMass, nanRemove);
    TCut cutBkgTrain = Form("%s && %s && %s", trainTreeEventSplitStr, higgsMass, nanRemove);
    TCut cutSignalTest = Form("%s && %s && %s", testTreeEventSplitStr, higgsMass, nanRemove);
    TCut cutBkgTest = Form("%s && %s && %s", testTreeEventSplitStr, higgsMass, nanRemove);
    
    // Register trees
    ///////////////////// TODO: add weight per event basis /////////////////////
    double signalWeight = 1.0;
    double backgroundWeight = 1.0;
    //dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal",
    //                    signalWeight, cutTrainSignal, "train");
    //dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal",
    //                    signalWeight, cutTestSignal, "test");
    
    dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal", signalWeight, cutSignalTrain, "train");
    dataloader->AddTree((TTree*)bkgfile1->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)bkgfile2->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)bkgfile3->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)bkgfile4->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)bkgfile5->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    
    dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal", signalWeight, cutSignalTest, "test");

    dataloader->AddTree((TTree*)bkgfile1->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile2->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile3->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile4->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile5->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    
    TMVA::Factory factory("TMVAClassification", outfile, "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");

    /*
    // Use cross validation
    int numFolds = 3;
    const char* analysisType = "Classification";
    const char* splitType = (useRandomSplitting) ? "Random" : "Deterministic";
    const char* splitExpr = (!useRandomSplitting) ? "int(fabs([eventID]))%int([NumFolds])" : "";
    TString cvOptions = Form("!V"
                            ":!Silent"
                            ":ModelPersistence"
                            ":AnalysisType=%s"
                            ":SplitType=%s"
                            ":NumFolds=%i"
                            ":SplitExpr=%s",
                            analysisType, splitType, numFolds, splitExpr);
    TMVA::CrossValidation cv("TMVACrossValidation", dataloader, outfile, cvOptions);
    */
    // Prepare training and test trees
    TString prepareOptions = "NormMode=None";
    prepareOptions+=":SplitMode=random:!V";
    prepareOptions+=":MixMode=Random";
    dataloader->PrepareTrainingAndTestTree("", prepareOptions);

    /*
        ## Booking Methods
 
        Here we book the TMVA methods. We book first a Likelihood based on KDE (Kernel Density Estimation), a Fischer discriminant, a BDT
        and a shallow neural network
 
    */
 
    // Likelihood ("naive Bayes estimator")
    if (useLikelihood)
    {
        factory.BookMethod(dataloader,TMVA::Types::kLikelihood, "Likelihood",
                           "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );
    }

    if (useBDTG)
    {
        // factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d2_t50",
        //               "!V:NTrees=50:BoostType=Grad:Shrinkage=0.15:MaxDepth=2:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=8:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG",
                           "!V:NTrees=100:BoostType=Grad:Shrinkage=0.2:MaxDepth=4:SeparationType=GiniIndex:nCuts=15:UseRandomisedTrees:UseNvars=10:UseBaggedBoost:BaggedSampleFraction=0.5:PruneMethod=CostComplexity:PruneStrength=30" );
        // factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d2_1",
        //                   "!V:NTrees=100:BoostType=Grad:Shrinkage=0.2:MaxDepth=2:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=8:UseBaggedBoost:BaggedSampleFraction=0.2:PruneMethod=NoPruning" );
        // factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3",
        //                   "!V:NTrees=600:BoostType=Grad:Shrinkage=0.1:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=8:UseBaggedBoost:BaggedSampleFraction=0.6:PruneMethod=CostComplexity:PruneStrength=60" );
        // factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG__rho",
        //                  "!V:NTrees=100:BoostType=Grad:Shrinkage=0.1:MaxDepth=2:SeparationType=GiniIndex:nCuts=30:UseRandomisedTrees:UseNvars=8:UseBaggedBoost:BaggedSampleFraction=0.15" );
    }
    
    /*
      ## Train Methods
      Here we train all the previously booked methods.
    */
 
    factory.TrainAllMethods();
 
    /*
      ## Test  all methods
      Now we test and evaluate all methods using the test data set
    */
    
    factory.TestAllMethods();
    factory.EvaluateAllMethods();
    // cv.Evaluate();
 
    // after we get the ROC curve and we display
    // auto c1 = factory.GetROCCurve(dataloader);
    // c1->Draw();
}
