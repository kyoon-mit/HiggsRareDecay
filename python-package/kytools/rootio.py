import uproot
import pandas as pd
import xgboost as xgb

def root_to_pandas(filenames, treename, branches):
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