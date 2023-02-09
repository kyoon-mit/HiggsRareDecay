"""
A script for merging ROOT files.
"""

import os
import subprocess

date = 'JAN22' #2023
prod_mode = 'VBFcatlow' #'VBF'
Meson_list = ['Rho', 'Phi']

year_list, mc_list = [], []

spec = {'date': date,
        'mc': 0,
        'Meson': '',
        'prod': prod_mode,
        'suffix': 'all',
        'year': None}

if prod_mode == 'VBFcatlow':
    year_list = ['2018'] #[12016, 22016, 2017, 2018]
    mc_list = [1010, 1017, 1020, 1027]
    mc_list += list(range(9, 20))
    mc_list += [-65, -66]
    spec['suffix'] = 2018

top_dir = '/work/submit/mariadlf/Hrare/' + date
command_list = []

for mc in mc_list:
    spec['mc'] = mc
    for Meson in Meson_list:
        input_files = []
        spec['Meson'] = Meson
        for year in year_list:
            spec['year'] = str(year)
            this_path = '{year}/outname_mc{mc}_{prod}_{Meson}Cat_{year}.root'.format(**spec)
            input_files.append(os.path.join(top_dir, this_path))
        output_dir = '/work/submit/kyoon/RareHiggs/data/2023/{date}'.format(**spec)
        output_file = 'outname_mc{mc}_{prod}_{Meson}Cat_{suffix}.root'.format(**spec)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        bash_command = 'hadd {} {}'.format(os.path.join(output_dir, output_file), # TARGET
                                           ' '.join(input_files))                 # SOURCES
        command_list.append(bash_command)
        
for cmd in command_list:
    subprocess.run(cmd, shell=True, check=True)
