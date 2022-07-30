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
    // options to control used methods
    bool useRandomSplitting = false; // option for cross validation
    bool useLikelihood = true;    // likelihood based discriminant
    bool useLikelihoodKDE = false;    // likelihood based discriminant
    bool useFischer = false;       // Fischer discriminant
    bool useMLP = false;          // Multi Layer Perceptron (old TMVA NN implementation)
    bool useBDT = false;           // Boosted Decision Tree
    bool useBDTG = false;         // BDT with GradBoost
    bool useDL = false;           // TMVA Deep Learning ( CPU or GPU)
    // bool useKeras = false;        // Keras Deep learning
    
    // Open files
    TString fileformat;
    TFile* sgnfile;
    TFile* bkgfile1;
    TFile* bkgfile2;
    TFile* bkgfile3;
    TFile* bkgfile4;
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

    // TFile* sgnfile = TFile::Open("/work/submit/kyoon/RareHiggs/data/cat_rho/cat_rho_ggH/test/test_signal_GFcat_RhoCat_2018.root");
    // TFile* bkgfile = TFile::Open("/work/submit/kyoon/RareHiggs/data/cat_rho/cat_rho_ggH/test/test_background_GFcat_RhoCat_2018.root");
    
    // Initialize the dataset
    TFile* outfile = TFile::Open(outFileName, "RECREATE");    
    TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");
    
    // Add variables to dataset
    const char* idx0 = "index_pair[0]"; // Meson index
    const char* idx1 = "index_pair[1]"; // Photon index
    dataloader->AddVariable("HCandMass", "HCandMass", "GeV/c^2", 'F'); // DON'T USE!!
    dataloader->AddVariable("HCandPT", "HCandPT", "", 'F');
    dataloader->AddVariable("HCandPT__div_HCandMass", "HCandPT__div_HCandMass", "", 'F'); // divide by HCandMass
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
    dataloader->AddVariable("goodMeson_DR__times_sqrtHCandMass", "goodMeson_DR__times_sqrtHCandMass", "", 'F');
    /*
    dataloader->AddVariable("goodPhotons_eta", "goodPhotons_eta", "", 'F');
    dataloader->AddVariable("goodPhotons_mvaID", "goodPhotons_mvaID", "", 'F');
    dataloader->AddVariable("SoftActivityJetNjets5", "SoftActivityJetNjets5", "", 'F');
    dataloader->AddVariable("DeepMETResolutionTune_pt", "DeepMETResolutionTune_pt", "GeV/c", 'F');
    dataloader->AddVariable("goodMeson_mass", "goodMeson_mass", "GeV/c^2", 'F');
    dataloader->AddVariable("goodMeson_iso", "goodMeson_iso", "", 'F');
    dataloader->AddVariable("goodMeson_massErr", "goodMeson_massErr", "GeV/c^2", 'F');
    dataloader->AddVariable("goodMeson_sipPV", "goodMeson_sipPV", "", 'F');
    // dataloader->AddVariable("dPhiGammaMesonCand", "dPhiGammaMesonCand", "", 'F');
    // dataloader->AddVariable("dEtaGammaMesonCand", "dEtaGammaMesonCand", "", 'F');
    // dataloader->AddVariable("dPhiGammaMesonCand/HCandMass", "dPhiGammaMesonCand__div_HCandMass", "", 'F');
    // dataloader->AddVariable("dEtaGammaMesonCand/HCandMass", "dEtaGammaMesonCand__div_HCandMass", "", 'F');
    dataloader->AddVariable("dPhiGammaMesonCand__div_sqrtHCandMass", "dPhiGammaMesonCand__div_sqrtHCandMass", "", 'F');
    dataloader->AddVariable("dEtaGammaMesonCand__div_sqrtHCandMass", "dEtaGammaMesonCand__div_sqrtHCandMass", "", 'F');
    //dataloader->AddVariable("nGoodJets", "nGoodJets", "", 'F');
    dataloader->AddVariable("sigmaHCandMass_Rel2", "sigmaHCandMass_Rel2", "", 'F');
    dataloader->AddVariable("goodPhotons_energyErr", "goodPhotons_energyErr", "", 'F');
    */
    // Set weights
    dataloader->SetWeightExpression("w");

    // Spectator used for split
    // dataloader->AddSpectator("Entry$", "eventID");

    // Apply split
    ////////////////////// TODO: use cross validation /////////////////////////
    const char* trainTreeEventSplitStr = "(Entry$%3) >= 1"; //"(events % 10)>=5";
    const char* testTreeEventSplitStr = "(Entry$%3) < 1"; // "(events % 10)<5";
    
    // Apply cuts
    const char* higgsMass = "HCandMass > 90 && HCandMass <= 170";
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
    dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal", signalWeight, cutSignalTest, "test");

    dataloader->AddTree((TTree*)bkgfile1->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile2->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile3->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile4->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    /*
    dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal", signalWeight, cutSignalTrain, "train")\
;                                                                                                        
    dataloader->AddTree((TTree*)bkgfile->Get("events"), "Background", backgroundWeight, cutBkgTrain, "t\
rain");
    dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal", signalWeight, cutSignalTest, "test");  
    dataloader->AddTree((TTree*)bkgfile->Get("events"), "Background", backgroundWeight, cutBkgTest, "te\
st");
    */
    /*
    dataloader->PrepareTrainingAndTestTree(cutSignal, cutBkg,
                                           "nTrain_Signal=-1"
                                           "nTrain_Background=-1"
                                           "nTest_Signal=0"
                                           ":nTest_Background=0"
                                           ":SplitMode=Random"
                                           ":NormMode=NumEvents"
                                           ":!V");
    */
    
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
    // Use a kernel density estimator to approximate the PDFs
    if (useLikelihoodKDE)
    {
        factory.BookMethod(dataloader,TMVA::Types::kLikelihood, "LikelihoodKDE",
                      "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" );
 
    }
 
    // Fisher discriminant (same as LD)
    if (useFischer)
    {
        factory.BookMethod(dataloader,TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );
    }
 
    // Boosted Decision Trees
    if (useBDT)
    {
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDT",
                      "!V:NTrees=200:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
    }

    if (useBDTG)
    {
        /*
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t50",
                      "!V:NTrees=50:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t100",
                      "!V:NTrees=100:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t150",
                      "!V:NTrees=150:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t200",
                      "!V:NTrees=200:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t250",
                      "!V:NTrees=250:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t300",
                      "!V:NTrees=300:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );        
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t350",
                      "!V:NTrees=350:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        */
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t400",
                      "!V:NTrees=400:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        /*
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t450",
                      "!V:NTrees=450:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t500",
                      "!V:NTrees=500:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t550",
                      "!V:NTrees=550:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t600",
                      "!V:NTrees=600:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t650",
                      "!V:NTrees=650:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t700",
                      "!V:NTrees=700:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );        
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t750",
                      "!V:NTrees=750:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d3_t800",
                      "!V:NTrees=800:BoostType=Grad:Shrinkage=0.30:MaxDepth=3:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        */
        /*
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t50",
                      "!V:NTrees=50:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t100",
                      "!V:NTrees=100:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t150",
                      "!V:NTrees=150:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t200",
                      "!V:NTrees=200:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t250",
                      "!V:NTrees=250:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t300",
                      "!V:NTrees=300:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );        
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t350",
                      "!V:NTrees=350:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        */
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t400",
                      "!V:NTrees=400:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        /*
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t450",
                      "!V:NTrees=450:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t500",
                      "!V:NTrees=500:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t550",
                      "!V:NTrees=550:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t600",
                      "!V:NTrees=600:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t650",
                      "!V:NTrees=650:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t700",
                      "!V:NTrees=700:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );        
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t750",
                      "!V:NTrees=750:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_d4_t800",
                      "!V:NTrees=800:BoostType=Grad:Shrinkage=0.30:MaxDepth=4:SeparationType=GiniIndex:nCuts=20:UseRandomisedTrees:UseNvars=4:UseBaggedBoost:BaggedSampleFraction=0.5" );
        */
    }
 
    // Multi-Layer Perceptron (Neural Network)
    if (useMLP)
    {
        factory.BookMethod(dataloader,TMVA::Types::kMLP, "MLP",
                      "!H:!V:NeuronType=tanh:VarTransform=N:NCycles=100:HiddenLayers=N+5:TestRate=5:UseRegulator" );
    }

    // Deep Neural Network
    if (useDL)
    {
 
        bool useDLGPU = false;
#ifdef R__HAS_TMVAGPU
        useDLGPU = true;
#endif
 
        // Define DNN layout
        TString inputLayoutString = "InputLayout=1|1|16";
        TString batchLayoutString= "BatchLayout=1|128|16";
        TString layoutString ("Layout=DENSE|64|TANH,DENSE|64|TANH,DENSE|64|TANH,DENSE|64|TANH,DENSE|1|LINEAR");
        // Define Training strategies
        // one can catenate several training strategies
        TString training1("LearningRate=1e-3,Momentum=0.9,"
                          "ConvergenceSteps=10,BatchSize=128,TestRepetitions=1,"
                          "MaxEpochs=30,WeightDecay=1e-4,Regularization=None,"
                          "Optimizer=ADAM,ADAM_beta1=0.9,ADAM_beta2=0.999,ADAM_eps=1.E-7," // ADAM default parameters
                          "DropConfig=0.0+0.0+0.0+0.");
        //     TString training2("LearningRate=1e-3,Momentum=0.9"
        //                       "ConvergenceSteps=10,BatchSize=128,TestRepetitions=1,"
        //                       "MaxEpochs=20,WeightDecay=1e-4,Regularization=None,"
        //                       "Optimizer=SGD,DropConfig=0.0+0.0+0.0+0.");
 
        TString trainingStrategyString ("TrainingStrategy=");
        trainingStrategyString += training1; // + "|" + training2;
 
        // General Options.
 
        TString dnnOptions ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=G:"
                            "WeightInitialization=XAVIER");
        dnnOptions.Append (":"); dnnOptions.Append (inputLayoutString);
        dnnOptions.Append (":"); dnnOptions.Append (batchLayoutString);
        dnnOptions.Append (":"); dnnOptions.Append (layoutString);
        dnnOptions.Append (":"); dnnOptions.Append (trainingStrategyString);
 
        TString dnnMethodName = "DNN_CPU";
        if (useDLGPU) {
            dnnOptions += ":Architecture=GPU";
            dnnMethodName = "DNN_GPU";
        } else  {
            dnnOptions += ":Architecture=CPU";
        }
 
        factory.BookMethod(dataloader,TMVA::Types::kDL, dnnMethodName, dnnOptions);
    }
 
    /*
    // Keras deep learning
    if (useKeras) {

        Info("TMVA_Higgs_Classification", "Building deep neural network with keras ");
        // create python script which can be executed
        // create 2 conv2d layer + maxpool + dense
        TMacro m;
        m.AddLine("import tensorflow");
        m.AddLine("from tensorflow.keras.models import Sequential");
        m.AddLine("from tensorflow.keras.optimizers import Adam");
        m.AddLine("from tensorflow.keras.layers import Input, Dense");
        m.AddLine("");
        m.AddLine("model = Sequential() ");
        m.AddLine("model.add(Dense(64, activation='relu',input_dim=7))");
        m.AddLine("model.add(Dense(64, activation='relu'))");
        m.AddLine("model.add(Dense(64, activation='relu'))");
        m.AddLine("model.add(Dense(64, activation='relu'))");
        m.AddLine("model.add(Dense(2, activation='sigmoid'))");
        m.AddLine("model.compile(loss = 'binary_crossentropy', optimizer = Adam(learning_rate = 0.001), metrics = ['accuracy'])");
        m.AddLine("model.save('Higgs_model.h5')");
        m.AddLine("model.summary()");
 
        m.SaveSource("make_higgs_model.py");
        // execute
        auto ret = (TString *)gROOT->ProcessLine("TMVA::Python_Executable()");
        TString python_exe = (ret) ? *(ret) : "python";
        gSystem->Exec(python_exe + " make_higgs_model.py");
 
        if (gSystem->AccessPathName("Higgs_model.h5")) {
            Warning("TMVA_Higgs_Classification", "Error creating Keras model file - skip using Keras");
        } else {
            // book PyKeras method only if Keras model could be created
            Info("TMVA_Higgs_Classification", "Booking tf.Keras Dense model");
            factory.BookMethod(
                               dataloader, TMVA::Types::kPyKeras, "PyKeras",
                               "H:!V:VarTransform=None:FilenameModel=Higgs_model.h5:tf.keras:"
                               "FilenameTrainedModel=Higgs_trained_model.h5:NumEpochs=20:BatchSize=100:"
                               "GpuOptions=allow_growth=True"); // needed for RTX NVidia card and to avoid TF allocates all GPU memory
        }
    }
    */
 
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
