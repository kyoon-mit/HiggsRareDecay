import ROOT
ROOT.TMVA.PyMethodBase.PyInitialize()
ROOT.gInterpreter.Declare('#include "trainedModelClassification.hxx"')

modelName = 'trainedModelClassification'
vars = ['HCandPT',
        'goodPhotons_pt',
        'goodPhotons_eta',
        'goodPhotons_mvaID',
        'goodMeson_pt',
        'goodMeson_iso',
        'goodMeson_DR',
        'goodMeson_mass',
        'dPhiGammaMesonCand',
        'dEtaGammaMesonCand',
        'nGoodJets']

#signalFile = '/work/submit/kyoon/RareHiggs/data/cat_rho/cat_rho_ggH/test/test_mc1027_ggHcat_rhocat_2018.root'
#backgroundFile = ['/work/submit/kyoon/RareHiggs/data/cat_rho/cat_rho_ggH/test/test_mc{}_ggHcat_rhocat_2018.root'.format(mc) for mc in range(10,15)]

signalFile = '/work/submit/mariadlf/Hrare/SEPT24/2018/outname_mc{}_GFcat_RhoCat_2018.root'.format(1027)
backgroundFile = ['/work/submit/mariadlf/Hrare/SEPT24/2018/outname_mc{}_GFcat_RhoCat_2018.root'.format(mc) for mc in range(10, 15)]

"""
# Test PyTorch
ROOT.gInterpreter.Declare('auto sofie_functor = TMVA::Experimental::SofieFunctor<{0},TMVA_SOFIE_{1}::Session>(0);'.format(len(vars), modelName))

df_signal = ROOT.RDataFrame('events', signalFile)
df_background = ROOT.RDataFrame('events', backgroundFile)

h_signal = df_signal.Define('DNN_Value', 'sofie_functor(rdfslot_, {})'.format(', '.join(vars))).Histo1D(('h_sig', '', 100, 0, 1), 'DNN_Value')
h_background = df_background.Define('DNN_Value', 'sofie_functor(rdfslot_, {})'.format(', '.join(vars))).Histo1D(('h_bkg', '', 100, 0, 1), 'DNN_Value')

ROOT.RDF.RunGraphs([h_signal, h_background])

h_signal.SetLineColor(ROOT.kRed)
h_background.SetLineColor(ROOT.kBlue)

c1 = ROOT.TCanvas()
ROOT.gStyle.SetOptStat(0)

h_signal.DrawClone()
h_background.DrawClone('SAME')

c1.SaveAs('torch_example.jpg')
"""

# For comparison with BDTG
xml_file = '/work/submit/kyoon/RareHiggs/test/NOV16/ggH_rho_mH90-170/weights/TMVAClassification_BDTG.weights.xml'
ROOT.gInterpreter.Declare('TMVA::Experimental::RReader model("{0}");'.format(xml_file))
vars = ['HCandPT',
        'goodPhotons_pt__div_HCandPT',
        'goodMeson_pt__div_HCandPT',
        'goodMeson_DR',
        'goodPhotons_eta',
        'goodPhotons_mvaID',
        'SoftActivityJetNjets5',
        'goodMeson_mass',
        'goodMeson_iso',
        'dPhiGammaMesonCand',
        'dEtaGammaMesonCand__div_HCandMass',
        'nGoodJets']

ROOT.gInterpreter.Declare('auto compute_model = TMVA::Experimental::Compute<{0}, float>(model);'.format(len(vars)))

df_signal = (ROOT.RDataFrame('events', signalFile)
             .Define("goodPhotons_pt__div_HCandPT", "(index_pair[1]!= -1) ? goodPhotons_pt[index_pair[1]]/HCandPT: 0.f")
             .Define("goodMeson_pt__div_HCandPT", "(index_pair[0]!= -1) ? goodMeson_pt[index_pair[0]]/HCandPT: 0.f")
             .Redefine("goodMeson_DR", "(index_pair[0]!= -1) ? goodMeson_DR[index_pair[0]]: 0.f")
             .Redefine("goodMeson_mass", "(index_pair[0]!= -1) ? goodMeson_mass[index_pair[0]]: 0.f")
             .Redefine("goodMeson_iso", "(index_pair[0]!= -1) ? goodMeson_iso[index_pair[0]]: 0.f")
             .Redefine("goodPhotons_eta","(index_pair[1]!= -1) ? goodPhotons_eta[index_pair[1]]: 0.f")
             .Redefine("goodPhotons_mvaID","(index_pair[1]!= -1) ? goodPhotons_mvaID[index_pair[1]]: 0.f")
             .Define("dEtaGammaMesonCand__div_HCandMass","(HCandMass>0) ? dEtaGammaMesonCand/HCandMass: 0.f")
             )
df_background = (ROOT.RDataFrame('events', backgroundFile)
                 .Define("goodPhotons_pt__div_HCandPT", "(index_pair[1]!= -1) ? goodPhotons_pt[index_pair[1]]/HCandPT: 0.f")
                 .Define("goodMeson_pt__div_HCandPT", "(index_pair[0]!= -1) ? goodMeson_pt[index_pair[0]]/HCandPT: 0.f")
                 .Redefine("goodMeson_DR", "(index_pair[0]!= -1) ? goodMeson_DR[index_pair[0]]: 0.f")
                 .Redefine("goodMeson_mass", "(index_pair[0]!= -1) ? goodMeson_mass[index_pair[0]]: 0.f")
                 .Redefine("goodMeson_iso", "(index_pair[0]!= -1) ? goodMeson_iso[index_pair[0]]: 0.f")
                 .Redefine("goodPhotons_eta","(index_pair[1]!= -1) ? goodPhotons_eta[index_pair[1]]: 0.f")
                 .Redefine("goodPhotons_mvaID","(index_pair[1]!= -1) ? goodPhotons_mvaID[index_pair[1]]: 0.f")
                 .Define("dEtaGammaMesonCand__div_HCandMass","(HCandMass>0) ? dEtaGammaMesonCand/HCandMass: 0.f")
                 )

h_signal = df_signal.Define('BDTdisc', 'compute_model({})'.format(', '.join(vars))).Histo1D(('h_sig_BDT', '', 100, -1, 1), 'BDTdisc')
h_background  = df_background.Define('BDTdisc', 'compute_model({})'.format(', '.join(vars))).Histo1D(('h_bkg_BDT', '', 100, -1, 1), 'BDTdisc')

#x_signal = TMVA.Experimental.AsTensor(df_signal, vars)
#x_background = TMVA.Experimental.AsTensor(df_background, vars)

#y_signal = model.Compute(x_signal)
#y_background = model.Compute(y_background)

ROOT.RDF.RunGraphs([h_signal, h_background])

h_signal.SetLineColor(ROOT.kRed)
h_background.SetLineColor(ROOT.kBlue)

c2 = ROOT.TCanvas()
ROOT.gStyle.SetOptStat(0)

h_signal.DrawClone()
h_background.DrawClone('SAME')

c2.SaveAs('bdt_example.jpg')
