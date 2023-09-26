import uproot
import pandas as pd
from ROOT import RDataFrame

def root_to_pandas(filenames, treename, branches):
    """Converts ROOT NTuple to Pandas DataFrame format.

    Along with the pandas DataFrame, a Numpy array of labels is returned. The length
    of this array is the number of rows. The values are 1 if signal=True, and -1 if
    signal=False.

    Args:
        filename (list(str)): List containing names of the ROOT files.
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

def root_to_rdf(filename, treename, defines=[], redefines=[]):
    """Converts ROOT NTuple to ROOT RDataFrame format.

    If `defines` is provided, the RDataFrame will contain new column definitions
    along with the old.

    Args:
        filename (str or ROOT.std.vector): Name of the ROOT file.
        treename (str): Name of the TTree.
        defines (list((str, str)), optional): List containing new column definitions.
            Defaults to [].
        redefines (list((str, str)), optional): List containing column redefinitions.
            Useful for converting types. Defaults to [].

    Returns:
        rdf (ROOT.RDataFrame): ROOT RDataFrame object.
    """
    rdf_ = RDataFrame(treename, filename)
    for def_ in defines:
        rdf_ = rdf_.Define(def_[0], def_[1])
    for redef_ in redefines:
        rdf_ = rdf_.Redefine(redef_[0], redef_[1])
    return rdf_