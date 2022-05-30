### Python3 ###

from ROOT import *
import numpy as np
import array as arr

def ttree_to_numpy_ggH(infile_names):
    nvars = 12
    transformed_array = np.empty((0, nvars))
    spectator_array = np.empty((0, 1))

    for fname in infile_names:
        infile = TFile.Open(fname, "READ");
        events = infile.Get("events");
        
        for entry in events:
            meson_idx = entry.index_pair[0]
            photon_idx = entry.index_pair[1]

            data = []
            spectator = []

            try:
                data.append(entry.HCandMass)
                data.append(entry.HCandPT)
                data.append(entry.goodPhotons_pt[photon_idx])
                data.append(entry.goodPhotons_eta[meson_idx])
                data.append(entry.SoftActivityJetNjets5)
                data.append(entry.DeepMETResolutionTune_pt)
                data.append(entry.goodMeson_DR[meson_idx])
                data.append(entry.goodMeson_mass[meson_idx])
                data.append(entry.goodMeson_pt[meson_idx])
                data.append(entry.goodMeson_iso[meson_idx])
                data.append(entry.nGoodJets)
                data.append(entry.goodMeson_massErr[meson_idx])
                spectator.append(entry.w)
            except:
                continue
        
            if not any(np.isnan(data)):
                transformed_array = np.concatenate((transformed_array, [data]), axis=0)
                spectator_array = np.concatenate((spectator_array, [spectator]), axis=0)

    return transformed_array, spectator_array

def gaussian_transform(array):
    return

def zca_whitening(array):
    sigma = np.cov(array, rowvar=False)
    U, S, V = np.linalg.svd(sigma)
    epsilon = 1e-5
    ZCAMatrix = np.dot(U, np.dot(np.diag(1.0/np.sqrt(S + epsilon)), U.T))
    whitened_array = np.dot(array, ZCAMatrix)
    return whitened_array

def write_numpy_to_ntuple(array, labels, outfilename):
    ntuple = TNtuple("events", "events", ":".join(labels))
    for row in array:
        ntuple.Fill(arr.array("f", row))
    outfile = TFile.Open(outfilename, "RECREATE")
    ntuple.Write()
    return

if __name__ == "__main__":
    infile_format = "/work/submit/kyoon/RareHiggs/data/cat_phi/cat_phi_ggH/outname_mc{}_GFcat_PhiCat_2018.root"
    outfile_format = "/work/submit/kyoon/RareHiggs/data/cat_phi/cat_phi_ggH/test/test_{}_GFcat_PhiCat_2018.root\
"
    labels = ["HCandMass",
              "HCandPT",
              "goodPhotons_pt",
              "goodPhotons_eta",
              "SoftActivityJetNjets5",
              "DeepMETResolutionTune_pt",
              "goodMeson_DR",
              "goodMeson_mass",
              "goodMeson_pt",
              "goodMeson_iso",
              "nGoodJets",
              "goodMeson_massErr",
              "w"]

    bkg_mc = [6, 7, 8, 9]
    sgn_mc = 1017

    bkgfile_names = [infile_format.format(mc) for mc in bkg_mc]
    sgnfile_names = [infile_format.format(sgn_mc)]
    
    bkg_transformed, bkg_spectator = ttree_to_numpy_ggH(bkgfile_names)
    bkg_transformed = zca_whitening(bkg_transformed)
    sgn_transformed, sgn_spectator = ttree_to_numpy_ggH(sgnfile_names)
    sgn_transformed = zca_whitening(sgn_transformed)

    bkg_array = np.concatenate((bkg_transformed, bkg_spectator), axis=1)
    sgn_array = np.concatenate((sgn_transformed, sgn_spectator), axis=1)
    
    write_numpy_to_ntuple(bkg_array, labels, outfile_format.format("background"))
    write_numpy_to_ntuple(sgn_array, labels, outfile_format.format("signal"))
