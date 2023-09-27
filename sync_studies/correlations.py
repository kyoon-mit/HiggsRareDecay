"""
Correlations
=========================================

This macro computes the correlation of the MVA discriminator variable with other
input variables.

Usage:
    In the terminal, do `python correlations.py`.
"""
from os import path, environ, makedirs
from kytools import rootio
import json
from matplotlib import pyplot as plt
# TODO: Parse arguments

def get_pandas_dataframes(prod, Meson, json_path, json_obj_key, tree_name, features, mva_disc, **kwargs):
    """Converts the given TTree in ROOT files into pandas.DataFrame's.

    This method takes as input strings which specify the path of the JSON file,
    and the names of the ROOT files stored in the JSON file. It opens the ROOT
    files, reads the values in the specified TTree, and converts them into
    pandas.DataFrame objects. Two dataframes are returned: one for signal and
    another for background.

    Args:
        prod (str): Identifier of the production category (e.g. 'GF', 'VBF', etc.)
        Meson (str): Name of the meson, with the first letter capitalized.
        json_path (str): Full path to the JSON file.
        json_obj_key (str): JSON key to the list of the names of ROOT files.
        tree_name (str): Name of the TTree in the ROOT file.
        features (list(str)): The MVA input variables to include in the pandas.DataFrame.
            It is okay to pass variables that are not used in training.
        mva_disc (str): Name of the MVA discriminator variable in the ROOT file.
        **kwargs (optional): Other arguments that are passed but not used.
            This is to avoid getting an error in the case where **dict is passed and
            there are too many keys.
    
    Returns:
        bkg_df (pandas.DataFrame): DataFrame of the background dataset.
        sig_df (pandas.DataFrame): DataFrame of the signal dataset.
    """
    branches = features + [mva_disc]
    print (f'Getting list of ROOT files from {json_path}')
    json_file = json.load(open(json_path))
    filenames = json_file['root_files'][json_obj_key]
    bkg_filenames, sig_filenames =\
        filenames[f'{prod}_{Meson}_bkg'],\
        filenames[f'{prod}_{Meson}_sig']
    bkg_df, sig_df =\
        rootio.root_to_pandas(bkg_filenames, tree_name, branches),\
        rootio.root_to_pandas(sig_filenames, tree_name, branches)
    return bkg_df, sig_df


def compute_mva_correlation(sig_df, bkg_df, mva_disc, save_path, save_suffix, matrix=True, scatter=True, terminal=False, **kwargs):
    """Computes numerical and graphical correlations from the given pandas.DataFrame's.

    Each and every variable in the pandas.DataFrame object is compared against
    the mva discriminator variable. The results are shown for the background
    and signal dataset each.

    If `matrix` is True, it computes the correlation matrix. The matrix is saved
    as a text file in the specified directory and optionally printed in the terminal.

    If `scatter` is True, scatter plots are created and saved. For each column
    that is common in both bkg_df and sig_df, three scatter plots are shown:
    signal, background, and combined (in different colors).

    If `slices` is True, a histogram of the MVA discrimininator is created in binned ranges
    of each variable.

    Args:
        sig_df (pandas.DataFrame): pandas.DataFrame of the signal dataset.
        bkg_df (pandas.DataFrame): pandas.DataFrame of the background dataset.
        mva_disc (str): Name of the MVA discriminator variable in the pandas.DataFrame objects.
        save_path (str): Directory where the outputs of this method is saved.
        save_suffix (str): Suffix to put in the name of every file that is created.
        matrix (bool, optional): Whether to compute the correlation matrix.
            Defaults to True.
        scatter (bool, optional): Whether to plot the scatter plots.
            Defaults to True.
        terminal (bool, optional): Whether to show results in the terminal.
            Defaults to True.
        **kwargs (optional): Other arguments that are passed but not used.
            This is to avoid getting an error in the case where **dict is passed and
            there are too many keys.
    
    Returns:
        None
    """
    if not path.exists(save_path): makedirs(save_path)
    # Calculate correlation matrix
    if matrix:
        bkg_corr = bkg_df.corr(method='pearson').to_string()
        sig_corr = sig_df.corr(method='pearson').to_string()
        text_file_name = path.join(save_path, f'CORRELATION_MATRIX_{save_suffix}.txt')
        text_input = [f'<Background>\n{bkg_corr}',
                      '\n',
                      f'<Signal>\n{sig_corr}']
        with open(text_file_name, 'wt') as txtfile:
            txtfile.writelines(text_input)
        if terminal:
            print(*text_input, sep='\n')
    # Create scatter plots
    if scatter:
        sig_columns, bkg_columns = list(sig_df), list(bkg_df)
        sig_columns.remove(mva_disc)
        bkg_columns.remove(mva_disc)
        for col in list(set(sig_columns).intersection(bkg_columns)):
            ax_sig = sig_df.plot.scatter(x=col, y=mva_disc, c='Red', s=2.5, alpha=0.15)
            ax_sig.set_title(f'Signal: {save_suffix}\n{mva_disc} vs. {col}')
            plt.savefig(path.join(save_path, f'SCATTER_PLOT_sig_{mva_disc}_VS_{col}_{save_suffix}.png'),
                        dpi=1000, pad_inches=0.05)
            plt.close()
            ax_bkg = bkg_df.plot.scatter(x=col, y=mva_disc, c='Blue', s=2.5, alpha=0.15)
            ax_bkg.set_title(f'Background: {save_suffix}\n{mva_disc} vs. {col}')
            plt.savefig(path.join(save_path, f'SCATTER_PLOT_bkg_{mva_disc}_VS_{col}_{save_suffix}.png'),
                        dpi=1000, pad_inches=0.05)
            plt.close()
            ax_comb = plt.subplot(111)
            ax_comb.scatter(x=bkg_df[col], y=bkg_df[mva_disc], c='Blue', s=2.5, alpha=0.01, label='background')
            ax_comb.scatter(x=sig_df[col], y=sig_df[mva_disc], c='Red', s=2.5, alpha=0.01, label='signal')
            ax_comb.set_title(f'Signal & Background: {save_suffix}\n{mva_disc} vs. {col}')
            ax_comb.set_xlabel(col)
            ax_comb.set_ylabel(mva_disc)
            for lh in ax_comb.legend().legend_handles: lh.set_alpha(1)
            plt.savefig(path.join(save_path, f'SCATTER_PLOT_comb_{mva_disc}_VS_{col}_{save_suffix}.png'),
                        dpi=1000, pad_inches=0.05)
            plt.close()
    return
    

def sliced_hams(sig_df, bkg_df, x_col, y_col, xbins, save_path, save_suffix, **kwargs):
    """Creates histograms of the variable y in bins of the variable x and plots
    them on top of each other.

    This method is similar to projecting the scatter plot as a histogram to the 
    y axis, but instead of summing over the entire range of x, the data counts
    are summed over sub-ranges of x.

    Three plots are created: histograms that are stacked in steps, bar charts of
    different sample sizes shown in parallel, and normalized bar charts shown in
    parallel.

    The resultings plots are saved in the specified directory.

    Args:
        sig_df (pandas.DataFrame): pandas.DataFrame of the signal dataset.
        bkg_df (pandas.DataFrame): pandas.DataFrame of the background dataset.
        x_col (str): The column name of x, the variable to constrain the sub-ranges.
        y_col (str): The column name of y, or the variable to be plotted in the x-axis
            in the histograms
        xbins (list(float)): The boundaries of the x sub-ranges. The list must have
            at least two elements. For a list [x_1, ..., x_n], the sub-ranges are
            [x_1, x_2), ..., [x_(n-1), x_n).
        save_path (str): Directory where the outputs of this method is saved.
        save_suffix (str): Suffix to put in the name of every file that is created.
        **kwargs (optional): Other arguments that are passed but not used.
            This is to avoid getting an error in the case where **dict is passed and
            there are too many keys.

    Raises:
        ValueError: If the length of xbins is less than 2, or if the values are
            not strictly monotonically increasing.

    Returns:
        None
    """
    if not path.exists(save_path): makedirs(save_path)
    if len(xbins) < 2:
        raise ValueError('The list must have at least two elements.')
    sig_y_values, bkg_y_values = [], []
    sliced_labels = []
    for i in range(1, len(xbins)):
        xlow = xbins[i-1]
        xhigh = xbins[i]
        if xlow >= xhigh:
            raise ValueError('The list must be strictly monotonically increasing.')
        sliced_labels.append(f'[{xlow:.1f}, {xhigh:.1f})')
        sig_sliced_ham = sig_df.loc[(xlow <= sig_df[x_col]) & (sig_df[x_col] < xhigh)]
        bkg_sliced_ham = bkg_df.loc[(xlow <= bkg_df[x_col]) & (bkg_df[x_col] < xhigh)]
        sig_y_values.append(sig_sliced_ham[y_col])
        bkg_y_values.append(bkg_sliced_ham[y_col])
    # TODO: change the following lines to for loop
    # Signal unnormalized bar charts
    plt.hist(sig_y_values, density=False, histtype='bar', label=sliced_labels)
    plt.legend(prop={'size': 10})
    plt.title(f'Signal: {save_suffix}\n{y_col} in bins of {x_col}')
    plt.xlabel(y_col)
    plt.ylabel('Events')
    plt.savefig(path.join(save_path, f'HISTOGRAM_UNNORMED_sig_{y_col}_BINS_{x_col}_{save_suffix}.png'),
                dpi=1000, pad_inches=0.05)
    plt.close()
    # Signal normalized bar charts
    plt.hist(sig_y_values, density=True, histtype='bar', label=sliced_labels)
    plt.legend(prop={'size': 10})
    plt.title(f'Signal: {save_suffix}\nNormalized {y_col} in bins of {x_col}')
    plt.xlabel(y_col)
    plt.ylabel('Events')
    plt.savefig(path.join(save_path, f'HISTOGRAM_NORMED_sig_{y_col}_BINS_{x_col}_{save_suffix}.png'),
                dpi=1000, pad_inches=0.05)
    plt.close()
    # Signal stacked-stepped histograms
    plt.hist(sig_y_values, density=True, histtype='step', label=sliced_labels)
    plt.legend(prop={'size': 10})
    plt.title(f'Signal: {save_suffix}\nNormalized {y_col} in bins of {x_col}')
    plt.xlabel(y_col)
    plt.ylabel('Events')
    plt.savefig(path.join(save_path, f'HISTOGRAM_STEP_NORMED_sig_{y_col}_BINS_{x_col}_{save_suffix}.png'),
                dpi=1000, pad_inches=0.05)
    plt.close()
    # Background unnormalized bar charts
    plt.hist(bkg_y_values, density=False, histtype='bar', label=sliced_labels)
    plt.legend(prop={'size': 10})
    plt.title(f'Background: {save_suffix}\n{y_col} in bins of {x_col}')
    plt.xlabel(y_col)
    plt.ylabel('Events')
    plt.savefig(path.join(save_path, f'HISTOGRAM_UNNORMED_bkg_{y_col}_BINS_{x_col}_{save_suffix}.png'),
                dpi=1000, pad_inches=0.05)
    plt.close()
    # Background normalized bar charts
    plt.hist(bkg_y_values, density=True, histtype='bar', label=sliced_labels)
    plt.legend(prop={'size': 10})
    plt.title(f'Background: {save_suffix}\nNormalized {y_col} in bins of {x_col}')
    plt.xlabel(y_col)
    plt.ylabel('Events')
    plt.savefig(path.join(save_path, f'HISTOGRAM_NORMED_bkg_{y_col}_BINS_{x_col}_{save_suffix}.png'),
                dpi=1000, pad_inches=0.05)
    plt.close()
    # Background stack-stepped bar charts
    plt.hist(bkg_y_values, density=True, histtype='step', label=sliced_labels)
    plt.legend(prop={'size': 10})
    plt.title(f'Background: {save_suffix}\nNormalized {y_col} in bins of {x_col}')
    plt.xlabel(y_col)
    plt.ylabel('Events')
    plt.savefig(path.join(save_path, f'HISTOGRAM_STEP_NORMED_bkg_{y_col}_BINS_{x_col}_{save_suffix}.png'),
                dpi=1000, pad_inches=0.05)
    return

if __name__=='__main__':
    ### Common Parameters
    prod = 'GF'
    Meson = 'Rho'
    json_path = path.abspath(path.join(environ['HRARE_DIR'], 'json/bdtoutput.json'))
    save_path = path.abspath(path.join(environ['HRARE_DIR'], 'sync_studies/output'))

    ### TO Parameters
    TO_params = {
        'prod': prod,
        'Meson': Meson,
        'json_path': json_path,
        'save_path': path.join(save_path, 'TO_mva'),
        'save_suffix': 'sync_studies_giulio_sep12_mva_sync',
        'json_obj_key': 'sync_studies_giulio_sep12_mva_sync_copy',
        'tree_name': 'tree_output_forMVA',
        'features': ['_photonEt__div_mesonGammaMass', '_bestCouplePt__div_mesonGammaMass',
                     '_firstTrkIsoCh', '_coupleIso0', 'mesonGammaMass', 'mesonMass'],
        'mva_disc': 'MVAdisc_sync_studies'
    }

    ### MIT Parameters
    MIT_params = {
        'prod': prod,
        'Meson': Meson,
        'json_path': json_path,
        'save_path': path.join(save_path, 'MIT_mva'),
        'save_suffix': 'sync_studies_maria_aug10_pol_threadfixed',
        'json_obj_key': 'sync_studies_maria_aug10_polarized_threadfixed_local_copy',
        'tree_name': 'events',
        'features': ['photon_pt__div_HCandMass', 'meson_pt__div_HCandMass',
                     'photon_eta', 'photon_mvaID', 'DeepMETResolutionTune_pt',
                     'meson_iso', 'meson_trk1_eta', 'dEtaGammaMesonCand__div_HCandMass',
                     'nGoodJets', 'HCandPT__div_HCandMass', 'HCandMass'],
        'mva_disc': 'MVAdisc'
    }

    ### Get pandas.DataFrame's
    TO_params['bkg_df'], TO_params['sig_df'] = get_pandas_dataframes(**TO_params)
    MIT_params['bkg_df'], MIT_params['sig_df'] = get_pandas_dataframes(**MIT_params)

    ### Compute correlation
    compute_mva_correlation(terminal=False, **TO_params)
    compute_mva_correlation(terminal=False, **MIT_params)

    ### Create histograms of MVA discriminator in bins of the Higgs mass
    sliced_hams(x_col='mesonGammaMass', y_col=TO_params['mva_disc'],
                xbins=[100., 110., 120., 130., 140., 150.],
                **TO_params)
    sliced_hams(x_col='HCandMass', y_col=MIT_params['mva_disc'],
                xbins=[100., 110., 120., 130., 140., 150.],
                **MIT_params)