"""
Neutral Isolation
=========================================

This is a module to check whether the neutral isolation is a good variable to use.
For completeness, we compare to the charged isolation variable.

Usage:
    In the terminal, do `python neutral_isolation.py`.
"""

from os import path, environ
from kytools import rootio, draw
import json
from matplotlib import pyplot as plt

### Parse arguments
## TODO ##

### Dictionary of global parameters
global global_params
global_params = {'prod': 'GF',
                 'Meson': 'Rho',
                 'json_path': path.abspath(path.join(environ['HRARE_DIR'], 'json/bdtoutput.json')),
                 'json_obj_key': 'maria_aug10_polarized_threadfixed_local_copy',
                 'file_spec': 'MIT_aug10_pol_threadfixed',
                 'save_path': path.abspath(path.join(environ['HRARE_DIR'], 'sync_studies/output'))}

### File I/O
print ('Getting list of ROOT files from {}' + global_params['json_path'])
json_file = json.load(open(global_params['json_path']))
filenames = json_file['root_files'][global_params['json_obj_key']]
bkg_filenames, sig_filenames =\
    filenames['{prod}_{Meson}_bkg'.format(**global_params)],\
    filenames['{prod}_{Meson}_sig'.format(**global_params)]
bkg_df, sig_df =\
    rootio.root_to_pandas(bkg_filenames, 'events', ['HCandMass', 'goodMeson_isoNeuHad', 'goodMeson_iso', 'PV_npvsGood']),\
    rootio.root_to_pandas(sig_filenames, 'events', ['HCandMass', 'goodMeson_isoNeuHad', 'goodMeson_iso', 'PV_npvsGood'])

### Test 1: Correlation of neutral isolation with number of primary vertices
bkg_corr = bkg_df.corr(method='pearson')
sig_corr = sig_df.corr(method='pearson')

print('\n\nCorrelation matrix:')
print('<Background>\n' + bkg_corr.to_string())
print('<Signal>\n' + sig_corr.to_string())

draw.hist2d_heatmap(bkg_df['PV_npvsGood'], bkg_df['goodMeson_isoNeuHad'], bins=[20,20],
                    title='GF Rho Background (MIT): PV_npvsGood vs. goodMeson_isoNeuHad',
                    x_title='Number of primary vertices',
                    y_title='Neutral isolation',
                    dpi=600,
                    range=[0, 80, 0, 1],
                    aspect=80,
                    save_name=path.join('{save_path}'.format(**global_params), '{prod}_{Meson}_bkg_PV_isoNeu_{file_spec}.png'.format(**global_params)))

draw.hist2d_heatmap(bkg_df['PV_npvsGood'], bkg_df['goodMeson_isoNeuHad'], bins=[20,20],
                    title='GF Rho Signal (MIT): PV_npvsGood vs. goodMeson_isoNeuHad',
                    x_title='Number of primary vertices',
                    y_title='Neutral isolation',
                    dpi=600,
                    range=[0, 80, 0, 1],
                    aspect=80,
                    save_name=path.join('{save_path}'.format(**global_params), '{prod}_{Meson}_sig_PV_isoNeu_{file_spec}.png'.format(**global_params)))

draw.hist2d_heatmap(bkg_df['PV_npvsGood'], bkg_df['goodMeson_iso'], bins=[20,20],
                    title='GF Rho Background (MIT): PV_npvsGood vs. goodMeson_iso',
                    x_title='Number of primary vertices',
                    y_title='Charged isolation',
                    dpi=600,
                    range=[0, 80, 0, 1],
                    aspect=80,
                    save_name=path.join('{save_path}'.format(**global_params), '{prod}_{Meson}_bkg_PV_isoChrg_{file_spec}.png'.format(**global_params)))

draw.hist2d_heatmap(bkg_df['PV_npvsGood'], bkg_df['goodMeson_iso'], bins=[20,20],
                    title='GF Rho Signal (MIT): PV_npvsGood vs. goodMeson_iso',
                    x_title='Number of primary vertices',
                    y_title='Charged isolation',
                    dpi=600,
                    range=[0, 80, 0, 1],
                    aspect=80,
                    save_name=path.join('{save_path}'.format(**global_params), '{prod}_{Meson}_sig_PV_isoChrg_{file_spec}.png'.format(**global_params)))

### Test 2: Correlation of MVA discriminator with neutral isolation
# Here, we make sure that the MVAdisc variable that is stored in the file is up-to-date.