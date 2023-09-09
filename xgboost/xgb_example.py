"""
XGBoost Example
=========================================

This example is intended to demonstrate the usage of XGBoost to train a classifier
for the GF channel.

Usage:
    In the terminal, do `python xgb_example.py`.
"""
from kytools import rootio
import uproot
import numpy as np
import pandas as pd
import xgboost as xgb

"""
Function definitions
"""

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
            df_list.append(events.arrays(branches, library='pd'))
    df_ = pd.concat(df_list, ignore_index=True)
    return df_

def define_variables(dataframe, signal):
    """Make manipulations to the input Pandas DataFrame as needed.

    Columns of new variables are created. A target column of integer is also created
    with +1 for signal data and -1 for background data.
    Rows with NaN values are filtered out.

    Args:
        dataframe (pandas.DataFrame): Pandas DataFrame object.
        signal (bool): Indicates whether the input dataframe represents signal or background.

    Returns:
        df (pandas.DataFrame): Pandas DataFrame with more columns.
    """
    # TODO: Store file names, branch names, and definitions as variables.
    df_ = dataframe.dropna(axis='index', how='any', inplace=False).copy()
    df_['HCandPT__div_HCandMass'] = df_['HCandPT']/df_['HCandMass']
    df_['goodMeson_pt__div_HCandMass'] = df_['meson_pt']/df_['HCandMass']
    df_['goodPhoton_pt__div_HCandMass'] = df_['photon_pt']/df_['HCandMass']
    df_['target'] = int(signal)

    return df_

def split_data(signal, background, frac, features, random_state=24421):
    """Splits the data into train and test samples with features and target.
    
    The input datasets must contain a column named 'features'.
    The provided datasets are concatenated and its rows shuffled.
    The combined dataset is split according to the frac.
    The function will select the target column and the columns that are provided through the list.
    
    Args:
        signal (pandas.DataFrame): Signal dataset.
        background (pandas.DataFrame): Background dataset.
        frac (float): Fraction of the training samples.
        features (list(str)): List of the columns to select.
        random_state (int): Random seed.
        
    Returns:
        dtrain (xgboost.DMatrix): Training dataset.
        dtest (xgboost.DMatrix): Testing dataset.
    """
    # Split signal and background datasets with the same ratio
    sig_train_ = signal.sample(frac=frac, random_state=random_state)
    bkg_train_ = background.sample(frac=frac, random_state=random_state)
    sig_test_ = signal.drop(sig_train_.index)
    bkg_test_ = background.drop(bkg_train_.index)
    
    # Combine into training and testing samples
    df_train_ = pd.concat([sig_train_, bkg_train_])
    df_test_ = pd.concat([sig_test_, bkg_test_])
    df_train_ = df_train_.sample(frac=1, random_state=random_state, ignore_index=True)
    df_test_ = df_test_.sample(frac=1, random_state=random_state, ignore_index=True)
    
    # Split into features and target
    X_train_ = df_train_[features]
    y_train_ = df_train_['target']
    X_test_ = df_test_[features]
    y_test_ = df_test_['target']
    
    # Create DMatrices
    dtrain_ = xgb.DMatrix(X_train_, y_train_)
    dtest_ = xgb.DMatrix(X_test_, y_test_)
    
    return dtrain_, dtest_

"""
The main part
"""

# Open files
# TODO: LOAD from Json
fname_format = '/work/submit/mariadlf/JUNE20/2018/outname_mc{}_GFcat_RhoCat_2018.root'
bkg_filenames = [fname_format.format(i) for i in range(10,20)]
sig_filenames = [fname_format.format(1027)]
branches = ['HCandMass',
            'HCandPT',
            'meson_pt',
            'photon_pt',
            'goodMeson_iso',
            'sigmaHCandMass_Rel2',
            'w']

# Convert data in ROOT files to Pandas DataFrames
df_bkg = rootio.root_to_pandas(bkg_filenames, 'events', branches)
df_sig = rootio.root_to_pandas(sig_filenames, 'events', branches)

# Add extra columns
df_bkg = define_variables(df_bkg, signal=False)
df_sig = define_variables(df_sig, signal=True)

# Create train and test datasets
train_ratio = 2./3. # The split is done randomly
features = ['HCandPT__div_HCandMass',
            'goodMeson_pt__div_HCandMass',
            'goodPhoton_pt__div_HCandMass']
dtrain, dtest = split_data(df_sig, df_bkg, train_ratio, features)
watchlist = [(dtest, "eval"), (dtrain, "train")] # specify validations set to watch performance

# Define parameters for the BDT model
params = {'objective': 'binary:logistic', # for binary classification problems
          'max_depth': 3,                 # maximum depth of each tree
          'eta': 0.1,                     # learning rate
          'gamma': 0,                     # minimum loss reduction required to make a further partition
          'subsample': 0.8,               # fraction of samples used for fitting the trees
          'colsample_bytree': 0.8,        # fraction of features used for fitting the trees
          'eval_metric': 'logloss',       # metric to evaluate during training
          'seed': 123                     # random seed for reproducibility
}
num_rounds = 100 # number of boosting rounds (trees)

# Train the model
my_bdt_model = xgb.train(params, dtrain, num_boost_round=num_rounds, evals=watchlist)

# Evaluate the model
preds = bst.predict(dtest)
labels = dtest.get_label()
print(
    "error=%f"
    % (
        sum(1 for i in range(len(preds)) if int(preds[i] > 0.5) != labels[i])
        / float(len(preds))
    )
)

# TODO: Calculate metrics and make plots
# TODO: Export model