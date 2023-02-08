import ROOT

global NVARS
NVARS = 14

def MVADataFrame (df, prodMode, testEventIndex, MVACut=-1.0):
    """ Helper function to apply MVA values.
    
    Args
        df (ROOT.RDT): RDataFrame object generated from the file containing the events.
        prodMode (str): production mode (e.g. 'GF', 'VBF', 'VBFlow', etc.)
        MVACut (float): cut to apply on the MVA discriminator value
        decayChannel (str): meson decay channel

    Returns:
        ROOT.RDT: RDataFrame containing the MVA discriminator values ('MVAdisc').
    """
    variables = ROOT.model.GetVariableNames()
    
    dfNew = (df
             .Define("HCandPT__div_HCandMass", "(HCandMass>0) ? HCandPT/HCandMass: 0.f")
             .Redefine("photon_pt", "(index_pair[1]!= -1) ? goodPhotons_pt[index_pair[1]]: 0.f")
             .Define("photon_pt__div_HCandPT", "(index_pair[1]!= -1) ? goodPhotons_pt[index_pair[1]]/HCandPT: 0.f")
#             .Define("goodPhotons_pt__div_sqrtHCandMass", "(index_pair[1]!= -1) ? goodPhotons_pt[index_pair[1]]/sqrt(HCandMass): 0.f")
             .Define("photon_eta","(index_pair[1]!= -1) ? goodPhotons_eta[index_pair[1]]: 0.f")
             .Define("photon_mvaID","(index_pair[1]!= -1) ? goodPhotons_mvaID[index_pair[1]]: 0.f")
             .Redefine("meson_pt", "(index_pair[0]!= -1) ? goodMeson_pt[index_pair[0]]: 0.f")
             .Define("meson_pt__div_HCandPT", "(index_pair[0]!= -1) ? goodMeson_pt[index_pair[0]]/HCandPT: 0.f")
#             .Define("goodMeson_pt__div_sqrtHCandMass", "(index_pair[0]!= -1) ? goodMeson_pt[index_pair[0]]/sqrt(HCandMass): 0.f")
#             .Define("goodMeson_DR__times_sqrtHCandMass", "(index_pair[0]!= -1) ? goodMeson_DR[index_pair[0]]*sqrt(HCandMass): 0.f")
             .Redefine("goodMeson_mass", "(index_pair[0]!= -1) ? goodMeson_mass[index_pair[0]]: 0.f")
             .Define("meson_iso", "(index_pair[0]!= -1) ? goodMeson_iso[index_pair[0]]: 0.f")
#             .Define("goodMeson_trk1_iso", "({0}!= -1) ? (goodMeson_trk1_pt[{0}]/goodMeson_pt[{0}]) * goodMeson_iso: 0.f".format("index_pair[0]"))
             .Redefine("goodMeson_sipPV", "(index_pair[0]!= -1) ? goodMeson_sipPV[index_pair[0]]: 0.f")
             .Define("meson_trk1_eta", "(index_pair[0]!= -1) ? goodMeson_trk1_eta[index_pair[0]]: 0.f")
             .Redefine("goodMeson_vtx_prob", "(index_pair[0]!= -1) ? goodMeson_vtx_prob[index_pair[0]]: 0.f")
#             .Define("dEtaGammaMesonCand__div_HCandMass", "(HCandMass>0) ? dEtaGammaMesonCand/HCandMass: 0.f")
#             .Redefine("SoftActivityJetNjets5", "SoftActivityJetNjets5*1.0f")
             .Redefine("event", "event*1.0f")
             .Redefine("MVAdisc", ROOT.computeModel, list(variables))
            )

    # dfNew = dfNew.Define("MVAdisc_testonly", "(abs(event-testEventIndex)<0.1) ? MVAdisc: 0.f")
    # return dfNew.Histo1D(('MVAdisc', 'MVAdisc', 40, -1, 1), 'MVAdisc')
    return dfNew

def applyMVA (prodMode, decayChannel, data=False):
    """ Applies MVA weights to MC or data events. """

    decayChannel = decayChannel.lower()
    #prodMode = prodMode.upper()
    rootFileFormat = '/work/submit/kyoon/RareHiggs/data/2023/{date}/outname_mc{num}_{cat}_{Decay}cat_{year}.root'
    xmlFileFormat = '/work/submit/kyoon/RareHiggs/test/MVAoutput/2023/{traindate}/{prod}_{decay}_mvas/weights/TMVAClassification_BDTG.weights.xml'
    outFileFormat = '/work/submit/kyoon/RareHiggs/test/MVAoutput/2023/{traindate}/{prod}_{decay}_mvas/disc/outname_mc{num}_{prod}cat_{Decay}Cat_{year}.root'

    specification = {'BDTfolder': 'Final_BDT_training',
                     'BDTsuffix': 'MH100-170',
                     'cat': '', #
                     'date': 'JAN22',
                     'decay': decayChannel,
                     'Decay': decayChannel.capitalize(),
                     'num': 0, # will loop over                   
                     'prod': prodMode,
                     'traindate': 'FEB06',
                     'year': 0, # will loop over
                    }

    # Loop over num and year
    nums, years = [], []
    nVars = 0
    if prodMode=='GF' and decayChannel=='phi':
        if data==False:
            nums, years = [10, 11, 12, 13, 14, 1017], [2018]
            nVars = NVARS
    elif prodMode=='GF' and decayChannel=='rho':
        if data==False:
            nums, years = [10, 11, 12, 13, 14, 1027], [2018]
            nVars = NVARS
    elif prodMode=='VBF' and decayChannel=='phi':
        True
    elif prodMode=='VBF' and decayChannel=='rho':
        True
    elif prodMode=='VBFlow' and decayChannel=='phi':
        specification['cat'] = 'VBFcatlow'
        if data==False:
            nums, years = list(range(9, 20)) + [-65, -66, 1010], ['all']
            nVars = 13
    elif prodMode=='VBFlow' and decayChannel=='rho':
        specification['cat'] = 'VBFcatlow'
        if data==False:
            nums, years = list(range(9, 20)) + [-65, -66, 1020], ['all']
            nVars = 13

    # Setup model
    xmlFileName = xmlFileFormat.format(**specification)
    ROOT.gInterpreter.ProcessLine('''
    TMVA::Experimental::RReader model("{0}");
    computeModel = TMVA::Experimental::Compute<{1}, float>(model);
    '''.format(xmlFileName, nVars))

    for yr in years:
        for n in nums:
            specification['num'], specification['year'] = n, yr
            rootFileName = rootFileFormat.format(**specification)
            df = ROOT.RDataFrame('events', rootFileName)
            outFileName = outFileFormat.format(**specification)
            dfMVA = MVADataFrame(df, prodMode, -1.)
            dfMVA.Snapshot('events', outFileName)
            print('Creating file:\n' + outFileName)

    return

if __name__ == '__main__':
    applyMVA('VBFlow', 'rho', 0)
