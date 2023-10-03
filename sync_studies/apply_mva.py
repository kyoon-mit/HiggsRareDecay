'''
Apply MVA
=========================================

This is a customized macro to test MVA models or reproduce the same results.

Usage:
    In the terminal, do `python apply_mva.py`.
'''
from os import path, environ
from kytools import rootio
import json
import ROOT
import argparse

poolSize = ROOT.GetThreadPoolSize()
print(f"Pool size = {poolSize}")

parser = argparse.ArgumentParser()
parser.add_argument('BDTID')
args = parser.parse_args()

### Parameters
bdtid = args.BDTID
prod = 'GF'
Meson = 'Rho'
json_path = path.abspath(path.join(environ['HRARE_DIR'], 'json/bdtoutput.json'))
# save_path = path.abspath(path.join(environ['HRARE_DIR'], 'sync_studies/mva_test_files'))
save_path = '/work/submit/kyoon/RareHiggs/test/2023/mva_test_files'
file_spec = 'maria_sep29_wp80'
tree_name = 'events'
mva_defines_spec = 'MIT_mva_defines'
mva_redefines_spec = 'MIT_mva_redefines'
mva_weights_spec = ('tests', 'MIT_gf_rho_oct1_wp80_8vars_mh115-135')
mva_nvars = 8
mva_col_name = f'MVAdisc_{bdtid}'

### File I/O
print (f'Getting list of ROOT files from {json_path}')
json_file = json.load(open(json_path))
filenames = json_file['root_files'][file_spec]
bkg_filenames, sig_filenames =\
    filenames[f'{prod}_{Meson}_bkg'],\
    filenames[f'{prod}_{Meson}_sig']

### Apply new columns and MVA
mva_defines = json_file['mva_defines'][mva_defines_spec]
mva_redefines = json_file['mva_defines'][mva_redefines_spec]
mva_weights = json_file['mva_weights'][mva_weights_spec[0]][mva_weights_spec[1]][bdtid]
sig_rdf = rootio.root_to_rdf(sig_filenames, tree_name, mva_defines, mva_redefines)
bkg_rdf = rootio.root_to_rdf(bkg_filenames, tree_name, mva_defines, mva_redefines)
#ROOT.RDF.Experimental.AddProgressbar(rdf)

process_string = f'''
TMVA::Experimental::RReader model("{mva_weights}");
computeModel = TMVA::Experimental::Compute<{mva_nvars}, float>(model);
'''
ROOT.gInterpreter.ProcessLine(process_string)
variables = ROOT.model.GetVariableNames()

sig_rdf_mva = sig_rdf.Define(mva_col_name, ROOT.computeModel, list(variables))
sig_rdf_mva = sig_rdf_mva.Redefine(mva_col_name, f'{mva_col_name}[0]')
bkg_rdf_mva = bkg_rdf.Define(mva_col_name, ROOT.computeModel, list(variables))
bkg_rdf_mva = bkg_rdf_mva.Redefine(mva_col_name, f'{mva_col_name}[0]')
sig_outfile_name = path.join(save_path, f'{mva_weights_spec[1]}_{bdtid}_sig.root')
bkg_outfile_name = path.join(save_path, f'{mva_weights_spec[1]}_{bdtid}_bkg.root')
sig_rdf_mva.Snapshot(tree_name, sig_outfile_name, ['HCandMass', mva_col_name])
print(f'Saved BDT result on signal to:\n\t{sig_outfile_name}')
bkg_rdf_mva.Snapshot(tree_name, bkg_outfile_name, ['HCandMass', mva_col_name])
print(f'Saved BDT result on background to:\n\t{bkg_outfile_name}')