import uproot
import numpy as np
import pandas as pd
import xgboost as xgb

#-------------------------------------------------------------------------------
def convert_root_to_pandas(filenames, treename, branches):
"""Converts ROOT NTuple to Pandas DataFrame format.

Along with the pandas DataFrame, a Numpy array of labels is returned. The length
of this array is the number of rows. The values are 1 if signal=True, and -1 if
signal=False.

Args:
    filename (str): Name of the ROOT file.
    treename (str): Name of the TTree.
    branches (list(str)): List of the branches to convert.

Returns:
    df (pandas.DataFrame): Pandas DataFrame of the chosen variables.
"""
    df_list = []
    for f_ in filenames:
        with uproot.open('{}:{}'.format(f_, treename)) as events:
            df_list.append(events.array(branches, library='pd'))
    df = pandas.concat(df_list, ignore_index=True)
    return df

#-------------------------------------------------------------------------------
def manipulate_dataframe(dataframe):
"""Make manipulations to the input Pandas DataFrame as needed.

New columns are created. Rows with NaN values are filtered out.

Args:
    dataframe (pandas.DataFrame): Pandas DataFrame object.

Returns:
    df (pandas.DataFrame): Pandas DataFrame with more columns.
"""
    

    return df


#-------------------------------------------------------------------------------
def create_train_test_sets(signaldata, signalweights, bkgdata, bkgweights):
    labels = np.full(nevents, int(int(signal)*2 - 1))


#-------------------------------------------------------------------------------
def train_xgboost_model(params, dtrain, dtest):
"""

Args:
    param (dict): Booster parameters.
"""

#-------------------------------------------------------------------------------
if __name__=='__main__':
    ###### Test ######
    fname_format = '/work/submit/mariadlf/JUNE20/2018/outname_mc{}_GFcat_RhoCat_2018.root'
    bkg_filenames = [fname_format.format(i) for i in range(10,20)]
    sgn_filenames = [fname_format.format(i) for i in (1027)]
    branches = ['HCandMass',
                'HCandPT',
                'goodMeson_pt',
                'goodMeson_iso',
                'sigmaHCandMass_Rel2',
                'w']

    df_bkg = convert_root_to_pandas(bkg_filenames, 'events', branches)
    df_sig = convert_root_to_pandas(sig_filenames, 'events', branches)

    # Manipulate DataFrames
