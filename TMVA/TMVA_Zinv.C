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

void TMVA_Zinv ( const char* outFileName,
                const char* channel )
{
    // options to control used methods
    bool useRandomSplitting = false; // option for cross validation
    bool useLikelihood = true;    // likelihood based discriminant
    bool useLikelihoodKDE = false;    // likelihood based discriminant
    bool useFischer = false;       // Fischer discriminant
    bool useMLP = false;          // Multi Layer Perceptron (old TMVA NN implementation)
    bool useBDT = false;           // Boosted Decision Tree
    bool useBDTG = true;         // BDT with GradBoost
    bool useDL = false;            // TMVA Deep Learning ( CPU or GPU)
    // bool useKeras = false;        // Keras Deep learning
    
    // Open files
    TString fileformat;
    TFile* sgnfile1;
    TFile* sgnfile2;
    TFile* sgnfile3;
    TFile* sgnfile4;
    TFile* bkgfile1;
    TFile* bkgfile2;
    TFile* bkgfile3;
    TFile* bkgfile4;
    TFile* bkgfile5;
    TFile* bkgfile6;
    if ( std::strcmp(channel, "phi") == 0 ) {        
        fileformat = "/work/submit/mariadlf/Hrare/NOV25/2018/outname_mc%d_Zinvcat_PhiCat_2018.root";
        sgnfile1 = TFile::Open(Form(fileformat, 1011), "READ");
        sgnfile2 = TFile::Open(Form(fileformat, 1012), "READ");
        sgnfile3 = TFile::Open(Form(fileformat, 1015), "READ");
        sgnfile4 = TFile::Open(Form(fileformat, 1016), "READ");
    } else if ( std::strcmp(channel, "rho") == 0 ) {
        fileformat = "/work/submit/mariadlf/Hrare/NOV25/2018/outname_mc%d_Zinvcat_RhoCat_2018.root";
        sgnfile1 = TFile::Open(Form(fileformat, 1021), "READ");
        sgnfile2 = TFile::Open(Form(fileformat, 1022), "READ");
        sgnfile3 = TFile::Open(Form(fileformat, 1025), "READ");
        sgnfile4 = TFile::Open(Form(fileformat, 1026), "READ");
    }
    bkgfile1 = TFile::Open(Form(fileformat, 10), "READ");
    bkgfile2 = TFile::Open(Form(fileformat, 11), "READ");
    bkgfile3 = TFile::Open(Form(fileformat, 12), "READ");
    bkgfile4 = TFile::Open(Form(fileformat, 13), "READ");
    bkgfile5 = TFile::Open(Form(fileformat, 14), "READ");
    bkgfile6 = TFile::Open(Form(fileformat, 15), "READ");

    // Initialize the dataset
    TFile* outfile = TFile::Open(outFileName, "RECREATE");
    TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

    // Add variables to dataset
    // dataloader->AddVariable("HCandMass", "HCandMass", "", 'F'); // DON'T USE!!

    // dataloader->AddVariable("HCandPT", "HCandPT", "", 'F');
    //// dataloader->AddVariable("HCandPT/HCandMass", "HCandPT__div_HCandMass", "", 'F');
    //// dataloader->AddVariable("HCandPT__div_sqrtHCandMass", "HCandPT__div_sqrtHCandMass", "", 'F');
    /// dataloader->AddVariable("goodPhotons_pt", "goodPhotons_pt", "", 'F');
    //// dataloader->AddVariable("goodPhotons_pt/HCandPT", "goodPhotons_pt__div_HCandPT", "", 'F');
    //// dataloader->AddVariable("goodPhotons_pt/HCandMass", "goodPhotons_pt__div_HCandMass", "", 'F');
    //// dataloader->AddVariable("goodPhotons_pt__div_sqrtHCandMass", "goodPhotons_pt__div_sqrtHCandMass", "", 'F');
    /// dataloader->AddVariable("goodMeson_pt", "goodMeson_pt", "", 'F');
    //// dataloader->AddVariable("goodMeson_pt/HCandPT", "goodMeson_pt__div_HCandPT", "", 'F');
    //// dataloader->AddVariable("goodMeson_pt/HCandMass", "goodMeson_pt__div_HCandMass", "", 'F');
    //// dataloader->AddVariable("goodMeson_pt__div_sqrtHCandMass", "goodMeson_pt__div_sqrtHCandMass", "", 'F');
    /// dataloader->AddVariable("goodMeson_mass", "goodMeson_mass", "", 'F');
    dataloader->AddVariable("goodMeson_DR", "goodMeson_DR", "", 'F');
    //// dataloader->AddVariable("goodMeson_DR/HCandMass", "goodMeson_DR__times_HCandMass", "", 'F');
    //// dataloader->AddVariable("goodMeson_DR__times_sqrtHCandMass", "goodMeson_DR__times_sqrtHCandMass", "", 'F');
    dataloader->AddVariable("dPhiGammaMesonCand", "dPhiGammaMesonCand", "", 'F');
    //// dataloader->AddVariable("dPhiGammaMesonCand/HCandMass", "dPhiGammaMeson__div_HCandMass", "", 'F');
    //// dataloader->AddVariable("dPhiGammaMesonCand__div_sqrtHCandMass", "dPhiGammaMesonCand__div_sqrtHCandMass", "", 'F');
    dataloader->AddVariable("dEtaGammaMesonCand", "dEtaGammaMesonCand", "", 'F');
    //// dataloader->AddVariable("dEtaGammaMesonCand/HCandMass", "dEtaGammaMesonCand__div_HCandMass", "", 'F');
    //// dataloader->AddVariable("dEtaGammaMesonCand__div_sqrtHCandMass", "dEtaGammaMesonCand__div_sqrtHCandMass", "", 'F');
    /// dataloader->AddVariable("nGoodPhotons", "nGoodPhotons", "", 'F');
    /// dataloader->AddVariable("goodPhotons_eta", "goodPhotons_eta", "", 'F');
    /// dataloader->AddVariable("goodPhotons_pfRelIso03_all", "goodPhotons_pfRelIso03_all", "", 'F');
    //// dataloader->AddVariable("goodPhotons_pfRelIso03_chg", "goodPhotons_pfRelIso03_chg", "", 'F');
    /// dataloader->AddVariable("goodPhotons_hoe", "goodPhotons_hoe", "", 'F');
    /// dataloader->AddVariable("goodPhotons_r9", "goodPhotons_r9", "", 'F');
    /// dataloader->AddVariable("goodPhotons_sieie", "goodPhotons_sieie", "", 'F');
    /// dataloader->AddVariable("goodPhotons_mvaID", "goodPhotons_mvaID", "", 'F');
    /// dataloader->AddVariable("sigmaHCandMass_Rel2", "sigmaHCandMass_Rel2", "", 'F');
    /// dataloader->AddVariable("SoftActivityJetNjets5", "SoftActivityJetNjets5", "", 'F');
    //// dataloader->AddVariable("MET_pt", "MET_pt", "", 'F');
    dataloader->AddVariable("DeepMETResolutionTune_pt", "DeepMETResolutionTune_pt", "", 'F');
    //// dataloader->AddVariable("PV_npvsGood", "PV_npvsGood", "", 'F');
    //// dataloader->AddVariable("goodMeson", "goodMeson", "", 'F');
    dataloader->AddVariable("goodMeson_iso", "goodMeson_iso", "", 'F');
    /// dataloader->AddVariable("goodMeson_trk1_pt", "goodMeson_trk1_pt", "", 'F');
    /// dataloader->AddVariable("goodMeson_trk2_pt", "goodMeson_trk2_pt", "", 'F');
    // dataloader->AddVariable("goodMeson_trk1_eta", "goodMeson_trk1_eta", "", 'F');
    dataloader->AddVariable("goodMeson_trk2_eta", "goodMeson_trk2_eta", "", 'F');
    /// dataloader->AddVariable("goodMeson_vtx_chi2dof", "goodMeson_vtx_chi2dof", "", 'F');
    dataloader->AddVariable("goodMeson_vtx_prob", "goodMeson_vtx_prob", "", 'F');
    //dataloader->AddVariable("goodMeson_massErr", "goodMeson_massErr", "", 'F');
    /// dataloader->AddVariable("goodMeson_sipPV", "goodMeson_sipPV", "", 'F');
    dataloader->AddVariable("nGoodJets", "nGoodJets", "", 'F');
    /// dataloader->AddVariable("dPhiGammaMET", "dPhiGammaMET", "", 'F');
    dataloader->AddVariable("dPhiMesonMET", "dPhiMesonMET", "", 'F');
    dataloader->AddVariable("ptRatioMEThiggs", "ptRatioMEThiggs", "", 'F');
    /// dataloader->AddVariable("nbtag", "nbtag", "", 'F');
       
    // Set weights
    dataloader->SetWeightExpression("w");

    // Spectator used for split
    // dataloader->AddSpectator("Entry$", "eventID");

    // Apply split
    ////////////////////// TODO: use cross validation /////////////////////////
    const char* trainTreeEventSplitStr = "(Entry$ % 3) != 1";
    const char* testTreeEventSplitStr = "(Entry$ % 3) == 1";
    
    // Apply cuts
    const char* higgsMass = "HCandMass > 100 && HCandMass < 170";
    const char* nanRemove = "!TMath::IsNaN(goodMeson_massErr) && w < 1000"; // && !TMath::IsNaN(sigmaHCandMass_Rel2)";
    
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
    dataloader->AddTree((TTree*)sgnfile1->Get("events"), "Signal", signalWeight, cutSignalTrain, "train");
    dataloader->AddTree((TTree*)sgnfile2->Get("events"), "Signal", signalWeight, cutSignalTrain, "train");
    dataloader->AddTree((TTree*)sgnfile3->Get("events"), "Signal", signalWeight, cutSignalTrain, "train");
    dataloader->AddTree((TTree*)sgnfile4->Get("events"), "Signal", signalWeight, cutSignalTrain, "train");
    dataloader->AddTree((TTree*)bkgfile1->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)bkgfile2->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)bkgfile3->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)bkgfile4->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)bkgfile5->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    if (bkgfile6) dataloader->AddTree((TTree*)bkgfile6->Get("events"), "Background", backgroundWeight, cutBkgTrain, "train");
    dataloader->AddTree((TTree*)sgnfile1->Get("events"), "Signal", signalWeight, cutSignalTest, "test");
    dataloader->AddTree((TTree*)sgnfile2->Get("events"), "Signal", signalWeight, cutSignalTest, "test");
    dataloader->AddTree((TTree*)sgnfile3->Get("events"), "Signal", signalWeight, cutSignalTest, "test");
    dataloader->AddTree((TTree*)sgnfile4->Get("events"), "Signal", signalWeight, cutSignalTest, "test");
    dataloader->AddTree((TTree*)bkgfile1->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile2->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile3->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile4->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    dataloader->AddTree((TTree*)bkgfile5->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
    if (bkgfile6) dataloader->AddTree((TTree*)bkgfile6->Get("events"), "Background", backgroundWeight, cutBkgTest, "test");
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

    TMVA::Factory factory("TMVAClassification", outfile, "!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");

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
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG",
                           "!V:VarTransform=D:NTrees=115:BoostType=Grad:Shrinkage=0.065:MaxDepth=3:SeparationType=GiniIndex:nCuts=12:UseRandomisedTrees:UseNvars=9:UseBaggedBoost:BaggedSampleFraction=0.8:PruneMethod=NoPruning" );
        /*
        factory.BookMethod(dataloader,TMVA::Types::kBDT, "BDTG_pruning",
                           "!V:VarTransform=P,D:NTrees=100:BoostType=Grad:Shrinkage=0.068:MaxDepth=4:SeparationType=GiniIndex:nCuts=12:UseRandomisedTrees:UseNvars=20:UseBaggedBoost:BaggedSampleFraction=0.8:PruneMethod=CostComplexity:PruneStrength=110" );
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
        TString inputLayoutString = "InputLayout=1|1|23";
        TString batchLayoutString= "BatchLayout=1|128|23";
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
    //cv.Evaluate();
 
    // after we get the ROC curve and we display
    // auto c1 = factory.GetROCCurve(dataloader);
    // c1->Draw();
}
