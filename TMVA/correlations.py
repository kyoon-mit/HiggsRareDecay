"""
Correlations
=========================================

This macro provides an example in calculating the correlation of the Higgs candidate
mass and the BDT output.

Usage:
    In the terminal, do `python correlations.py`.
"""
from os import path, environ
from kytools import rootio
import argparse
import json
from matplotlib import pyplot as plt

### Parse arguments
## TODO ##

### Dictionary of global parameters
global global_params
global_params = {'prod': 'GF',
                 'Meson': 'Rho',
                 'json_path': path.abspath(path.join(environ['HRARE_DIR'], 'json/bdtoutput.json')),
                 'json_obj_key': 'maria_jul22_1thread_local_copy'}

### File I/O
print ('Getting list of ROOT files from {}' + global_params['json_path'])
json_file = json.load(open(global_params['json_path']))
filenames = json_file['root_files'][global_params['json_obj_key']]
bkg_filenames, sig_filenames =\
    filenames['{prod}_{Meson}_bkg'.format(**global_params)],\
    filenames['{prod}_{Meson}_sig'.format(**global_params)]
bkg_df, sig_df =\
    rootio.root_to_pandas(bkg_filenames, 'events', ['HCandMass', 'MVAdisc']),\
    rootio.root_to_pandas(sig_filenames, 'events', ['HCandMass', 'MVAdisc'])

### Calculate correlation
bkg_corr = bkg_df.corr(method='pearson')
sig_corr = sig_df.corr(method='pearson')

### Creat a scatter plot
ax_bkg, ax_sig =\
    bkg_df.plot.scatter(x='HCandMass', y='MVAdisc', c='DarkBlue'),\
    sig_df.plot.scatter(x='HCandMass', y='MVAdisc', c='DarkRed')
ax_bkg.set_title('Background')
ax_sig.set_title('Signal')

plt.show() # TODO: save figure

### TODO: create mass bin plot

### Print result
print('\n\nHiggs candidate mass and MVA discriminator correlation:')
print('<Background>\n' + bkg_corr.to_string())
print('<Signal>\n' + sig_corr.to_string())