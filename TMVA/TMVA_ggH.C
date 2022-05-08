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
    bool useRandomSplitting = false; // for cross validation
    bool useLikelihood = true;    // likelihood based discriminant
    bool useLikelihoodKDE = false;    // likelihood based discriminant
    bool useFischer = true;       // Fischer discriminant
    bool useMLP = false;          // Multi Layer Perceptron (old TMVA NN implementation)
    bool useBDT = true;           // Boosted Decision Tree
    bool useDL = false;            // TMVA Deep learning ( CPU or GPU)
    bool useKeras = false;        // Keras Deep learning
    bool usePyTorch = false;      // PyTorch Deep learning
    
    // Open files
    TString fileformat;
    TFile* sgnfile;
    TFile* bkgfile1;
    TFile* bkgfile2;
    TFile* bkgfile3;
    TFile* bkgfile4;
    if ( std::strcmp(channel, "phi") == 0 ) {
        fileformat = "/home/submit/kyoon/CMSSW_10_2_13/src/HiggsRareDecay/data/cat_phi/cat_phi_ggH/outname_mc%d_GFcat_PhiCat_2018.root";
        sgnfile = TFile::Open(Form(fileformat, 1017), "READ");
    } else if ( std::strcmp(channel, "rho") == 0 ) {
        fileformat = "/home/submit/kyoon/CMSSW_10_2_13/src/HiggsRareDecay/data/cat_rho/cat_rho_ggH/outname_mc%d_GFcat_RhoCat_2018.root";
        sgnfile = TFile::Open(Form(fileformat, 1027), "READ");
    }
    bkgfile1 = TFile::Open(Form(fileformat, 6), "READ");
    bkgfile2 = TFile::Open(Form(fileformat, 7), "READ");
    bkgfile3 = TFile::Open(Form(fileformat, 8), "READ");
    bkgfile4 = TFile::Open(Form(fileformat, 9), "READ");

    // Initialize the factory
    TFile* outfile = TFile::Open(outFileName, "RECREATE");
    TMVA::Factory factory("TMVAClassification", outfile, "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
    
    TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");
    
    // Add variables to dataset
    const char* idx0 = "events.index_pair[0]"; // Meson index
    const char* idx1 = "events.index_pair[1]"; // Photon index
    // dataloader->AddVariable("HCandMass", "HCandMass", "", 'F'); // DON'T!!!!!
    dataloader->AddVariable("HCandPT_div := HCandPT/HCandMass", 'F'); // divide by HCandMass
    dataloader->AddVariable(Form("goodPhotons_pt[%s]", idx1), "goodPhotons_pt", "", 'F');
    dataloader->AddVariable(Form("goodPhotons_eta[%s]", idx1), "goodPhotons_eta", "", 'F');
    // dataloader->AddVariable(Form("goodPhotons_pfRelIso03_all[%s]", idx1), "goodPhotons_pfRelIso03_all", "", 'F');
    // dataloader->AddVariable(Form("goodPhotons_hoe[%s]", idx1), "goodPhotons_hoe", "", 'F');
    // dataloader->AddVariable(Form("goodPhotons_r9[%s]", idx1), "goodPhotons_r9", "", 'F');
    // dataloader->AddVariable(Form("goodPhotons_sieie[%s]", idx1), "goodPhotons_sieie", "", 'F');
    dataloader->AddVariable("SoftActivityJetNjets5", "SoftActivityJetNjets5", "", 'I');
    dataloader->AddVariable("DeepMETResolutionTune_pt", "DeepMETResolutionTune_pt", "", 'F');
    dataloader->AddVariable(Form("goodMeson_DR[%s]", idx0), "goodMeson_DR", "", 'F');
    dataloader->AddVariable(Form("goodMeson_mass[%s]", idx0), "goodMeson_mass", "", 'F');
    dataloader->AddVariable(Form("goodMeson_pt[%s]", idx0), "goodMeson_pt", "", 'F');
    dataloader->AddVariable(Form("goodMeson_iso[%s]", idx0), "goodMeson_iso", "", 'F');
    dataloader->AddVariable(Form("leadTrkPt := TMath::Max(goodMeson_trk1_pt[%s], goodMeson_trk2_pt[%s])", idx0, idx0), 'F');
    dataloader->AddVariable(Form("subLeadTrkPt := TMath::Min(goodMeson_trk1_pt[%s], goodMeson_trk2_pt[%s])", idx0, idx0), 'F');
    dataloader->AddVariable(Form("leadTrkEta := TMath::Max(goodMeson_trk1_eta[%s], goodMeson_trk2_eta[%s])", idx0, idx0), 'F');
    dataloader->AddVariable(Form("subLeadTrkEta := TMath::Min(goodMeson_trk1_eta[%s], goodMeson_trk2_eta[%s])", idx0, idx0), 'F');
    dataloader->AddVariable(Form("goodMeson_vtx_prob[%s]", idx0), "goodMeson_vtx_prob", "", 'F');
    dataloader->AddVariable("goodMeson_massErr[events.index_pair[0]]", "goodMeson_massErr", "", 'F');
    dataloader->AddVariable("abs(dPhiGammaMesonCand)", "dPhiGammaMesonCand", "", 'F');
    dataloader->AddVariable("abs(dEtaGammaMesonCand)", "dEtaGammaMesonCand", "", 'F');
    dataloader->AddVariable("nGoodJets", "nGoodJets", "", 'F');
    /* For VBF
    dataloader->AddVariable("mJJ", "mJJ", "", 'F');
    dataloader->AddVariable("abs(dEtaJJ)", "dEtaJJ", "", 'F');
    dataloader->AddVariable("abs(dPhiJJ)", "dPhiJJ", "", 'F');
    dataloader->AddVariable("Y1Y2", "Y1Y2", "", 'F');
    dataloader->AddVariable("deltaJetMeson", "deltaJetMeson", "", 'F');
    dataloader->AddVariable("deltaJetPhoton", "deltaJetPhoton", "", 'F');
    dataloader->AddVariable("jet1Pt", "jet1Pt", "", 'F');
    dataloader->AddVariable("jet2Pt", "jet2Pt", "", 'F');
    */

    // Set weights
    dataloader->SetWeightExpression("w");

    // Spectator used for split
    dataloader->AddSpectator("Entry$", "eventID");

    // Apply split
    ////////////////////// TODO: use cross validation /////////////////////////
    const char* trainTreeEventSplitStr = "(events % 10)>=5";
    const char* testTreeEventSplitStr = "(events % 10)<5";
    
    // Apply cuts
    const char* higgsMass = "";
    //const char* nanRemove = " && !TMath::IsNaN(goodMeson_massErr[events.index_pair[0]])";
    const char* nanRemove = "!TMath::IsNaN(goodMeson_massErr[events.index_pair[0]])";
    
    TCut cutSignal = Form("%s %s", /*trainTreeEventSplitStr,*/ higgsMass, nanRemove);
    TCut cutBkg = Form("%s %s", /*trainTreeEventSplitStr,*/ higgsMass, nanRemove);
    
    // Register trees
    ///////////////////// TODO: add weight per event basis /////////////////////
    double signalWeight = 1.0;
    double backgroundWeight = 1.0;
    //dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal",
    //                    signalWeight, cutTrainSignal, "train");
    //dataloader->AddTree((TTree*)sgnfile->Get("events"), "Signal",
    //                    signalWeight, cutTestSignal, "test");
    dataloader->AddSignalTree((TTree*)sgnfile->Get("events"), signalWeight);
    dataloader->AddBackgroundTree((TTree*)bkgfile1->Get("events"), backgroundWeight);
    dataloader->AddBackgroundTree((TTree*)bkgfile2->Get("events"), backgroundWeight);
    dataloader->AddBackgroundTree((TTree*)bkgfile3->Get("events"), backgroundWeight);
    dataloader->AddBackgroundTree((TTree*)bkgfile4->Get("events"), backgroundWeight);
    dataloader->PrepareTrainingAndTestTree(cutSignal, cutBkg,
                                           "nTest_Signal="
                                           ":nTest_Background=1"
                                           ":SplitMode=Random"
                                           ":NormMode=NumEvents"
                                           ":!V");
    // Use cross validation
    int numFolds = 5;
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
        cv.BookMethod(TMVA::Types::kLikelihood, "Likelihood",
                      "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );
    }
    // Use a kernel density estimator to approximate the PDFs
    if (useLikelihoodKDE)
    {
        cv.BookMethod(TMVA::Types::kLikelihood, "LikelihoodKDE",
                      "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" );
 
    }
 
    // Fisher discriminant (same as LD)
    if (useFischer)
    {
        cv.BookMethod(TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );
    }
 
    // Boosted Decision Trees
    if (useBDT)
    {
        cv.BookMethod(TMVA::Types::kBDT, "BDT",
                      "!V:NTrees=200:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
    }
 
    // Multi-Layer Perceptron (Neural Network)
    if (useMLP)
    {
        cv.BookMethod(TMVA::Types::kMLP, "MLP",
                      "!H:!V:NeuronType=tanh:VarTransform=N:NCycles=100:HiddenLayers=N+5:TestRate=5:!UseRegulator" );
    }

    // Deep Neural Network
    if (useDL)
    {
 
        bool useDLGPU = false;
#ifdef R__HAS_TMVAGPU
        useDLGPU = true;
#endif
 
        // Define DNN layout
        TString inputLayoutString = "InputLayout=1|1|7";
        TString batchLayoutString= "BatchLayout=1|128|7";
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
 
        cv.BookMethod(TMVA::Types::kDNN, dnnMethodName, dnnOptions);
    }
 
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
 
    /*
      ## Train Methods
      Here we train all the previously booked methods.
    */
 
    //factory.TrainAllMethods();
 
    /*
      ## Test  all methods
      Now we test and evaluate all methods using the test data set
    */
    
    //factory.TestAllMethods();
    //factory.EvaluateAllMethods();
    cv.Evaluate();
 
    // after we get the ROC curve and we display
    // auto c1 = factory.GetROCCurve(dataloader);
    // c1->Draw();
}
