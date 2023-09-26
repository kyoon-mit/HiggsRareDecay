'''
Apply MVA
=========================================

This is a module to test MVA models or reproduce the same results.

Usage:
    In the terminal, do `python apply_mva.py`.
'''
from os import path, environ
from kytools import rootio
import json
import ROOT

poolSize = ROOT.GetThreadPoolSize()
print(f"Pool size = {poolSize}")

### Parameters
prod = 'GF'
Meson = 'Rho'
json_path = path.abspath(path.join(environ['HRARE_DIR'], 'json/bdtoutput.json'))
save_path = path.abspath(path.join(environ['HRARE_DIR'], 'sync_studies/output'))
file_spec = 'giulio_sep12_mva_sync_copy'
tree_name = 'tree_output_forMVA'
mva_defines_spec = 'TO_mva_defines'
mva_redefines_spec = 'TO_mva_redefines'
mva_weights_spec = ('sync_studies', 'TO_gf_rho_sep12')
mva_nvars = 4
mva_col_name = 'MVAdisc_sync_studies'

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
mva_weights = json_file['mva_weights'][mva_weights_spec[0]][mva_weights_spec[1]]
sig_filenames_C = ROOT.std.vector('string')()
bkg_filenames_C = ROOT.std.vector('string')()
print('Converting the following files to RDataFrame:')
for fname in sig_filenames:
    print(f'\t{fname}')
    sig_filenames_C.push_back(fname)
for fname in bkg_filenames:
    print(f'\t{fname}')
    bkg_filenames_C.push_back(fname)
sig_rdf = rootio.root_to_rdf(sig_filenames_C, tree_name, mva_defines, mva_redefines)
bkg_rdf = rootio.root_to_rdf(bkg_filenames_C, tree_name, mva_defines, mva_redefines)
#ROOT.RDF.Experimental.AddProgressbar(rdf)

process_string = f'''
TMVA::Experimental::RReader model("{mva_weights}");
computeModel = TMVA::Experimental::Compute<{mva_nvars}, float>(model);
'''
ROOT.gInterpreter.ProcessLine(process_string)
variables = ROOT.model.GetVariableNames()

sig_rdf_mva = sig_rdf.Define(mva_col_name, ROOT.computeModel, list(variables))
bkg_rdf_mva = bkg_rdf.Define(mva_col_name, ROOT.computeModel, list(variables))
sig_outfile_name = path.join(save_path, f'{file_spec}_sig.root')
bkg_outfile_name = path.join(save_path, f'{file_spec}_bkg.root')
sig_rdf_mva.Snapshot(tree_name, sig_outfile_name)
print(f'Saved BDT result on signal to:\n\t{sig_outfile_name}')
bkg_rdf_mva.Snapshot(tree_name, bkg_outfile_name)
print(f'Saved BDT result on background to:\n\t{bkg_outfile_name}')