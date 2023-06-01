from pubplots import *

group = 'MIT' # 'TO'
cat = 'GF' # 'VBF'
meson = 'Rho'

if group=='MIT':
    workspaceDir = '/work/submit/mariadlf/cards_march20/WS_MARCH20/'
    if cat=='GF' and meson=='Rho':
        sigFileName = 'Signal_{}cat__{}Cat_2018_workspace.root'.format(cat, meson)
        bkgFileName = 'Bkg_{}cat__{}Cat_2018_workspace.root'.format(cat, meson)
        iPeriod = 4
        iPos = 11
        varName = 'mh'
        varTitle = 'm_{#rho#gamma}'
        blinding = False
        blindLow = 115.
        blindHigh = 135.
        workspaceName = 'w'
        signalPDFName = 'crystal_ball_{}Cat_{}cat_ggH'.format(meson, cat)
        backgroundPDFName = 'chebychev3_{}Cat_{}cat'.format(meson, cat)
        dataName = 'datahist_{}Cat_{}cat'.format(meson, cat)
        signalDataName = 'datahist_{}cat_ggH'.format(cat)
        BR = 9.13e-4
        sigEntry='B(H#rightarrow#rho#gamma) = 9.13#times10^{-4}'
        saveName = 'MIT_{}_{}_WS_MARCH20.png'.format(cat, meson)
        plotMax = 5000
        residMin = -250
        residMax = 250
        legend_xl = .2
        legend_xr = .6
        legend_yb = .1
        legend_yt = .4
    elif cat=='GF' and meson=='Phi':
        sigFileName = 'Signal_{}cat__{}Cat_2018_workspace.root'.format(cat, meson)
        bkgFileName = 'Bkg_{}cat__{}Cat_2018_workspace.root'.format(cat, meson)
        iPeriod = 4
        iPos = 11
        varName = 'mh'
        varTitle = 'm_{#phi#gamma}'
        blinding = False
        blindLow = 115.
        blindHigh = 135.
        workspaceName = 'w'
        signalPDFName = 'crystal_ball_{}Cat_{}cat_ggH'.format(meson, cat)
        backgroundPDFName = 'chebychev3_{}Cat_{}cat'.format(meson, cat)
        dataName = 'datahist_{}Cat_{}cat'.format(meson, cat)
        signalDataName = 'datahist_{}cat_ggH'.format(cat)
        BR = 4.74e-4
        sigEntry='#mathcal{B}(H#rightarrow#phi#gamma) = 4.74#times10^{-4}'
        saveName = 'MIT_{}_{}_WS_MARCH20.png'.format(cat, meson)
        plotMax = 5000
        residMin = -250
        residMax = 250
        legend_xl = .2
        legend_xr = .6
        legend_yb = .1
        legend_yt = .4
    elif cat=='VBF' and meson=='Rho':
        workspaceDir = '/work/submit/mariadlf/cards_march9/WS_MARCH9/'
        sigFileName = 'Signal_{}cat__{}Cat_Run2_workspace.root'.format(cat, meson)
        bkgFileName = 'Bkg_{}cat__{}Cat_Run2_workspace.root'.format(cat, meson)
        iPeriod = 4
        iPos = 11
        varName = 'mh'
        varTitle = 'm_{#rho#gamma}'
        blinding = False
        blindLow = 115.
        blindHigh = 135.
        workspaceName = 'w'
        signalPDFName = 'crystal_ball_{}Cat_{}cat_VBFH'.format(meson, cat)
        backgroundPDFName = 'chebychev2_{}Cat_{}cat'.format(meson, cat)
        dataName = 'datahist_{}Cat_{}cat'.format(meson, cat)
        signalDataName = 'datahist_{}cat_VBFH'.format(cat)
        BR = 1.6e-3
        sigEntry='B(H#rightarrow#rho#gamma) = 1.6#times10^{-3}'
        saveName = 'MIT_{}_{}_WSmva_MARCH9.png'.format(cat, meson)
        plotMax = 300
        residMin = -25
        residMax = 25
        legend_xl = .55
        legend_xr = .95
        legend_yb = .6
        legend_yt = .9
    elif cat=='VBF' and meson=='Phi':
        workspaceDir = '/work/submit/mariadlf/cards_march9/WS_MARCH9/'
        sigFileName = 'Signal_{}cat__{}Cat_Run2_workspace.root'.format(cat, meson)
        bkgFileName = 'Bkg_{}cat__{}Cat_Run2_workspace.root'.format(cat, meson)
        iPeriod = 4
        iPos = 11
        varName = 'mh'
        varTitle = 'm_{#phi#gamma}'
        blinding = False
        blindLow = 115.
        blindHigh = 135.
        workspaceName = 'w'
        signalPDFName = 'crystal_ball_{}Cat_{}cat_VBFH'.format(meson, cat)
        backgroundPDFName = 'chebychev2_{}Cat_{}cat'.format(meson, cat)
        dataName = 'datahist_{}Cat_{}cat'.format(meson, cat)
        signalDataName = 'datahist_{}cat_VBFH'.format(cat)
        BR = 2.0e-3
        sigEntry='B(H#rightarrow#phi#gamma) = 2.0#times10^{-3}'
        saveName = 'MIT_{}_{}_WSmva_MARCH9.png'.format(cat, meson)
        plotMax = 30
        residMin = -15
        residMax = 15
        legend_xl = .55
        legend_xr = .95
        legend_yb = .6
        legend_yt = .9
elif group=='TO':
    workspaceDir = '/work/submit/kyoon/CMSSW_10_6_27/src/hig-23-005/datacards/TO_ggh/workspaces/'
    if cat=='GF' and meson=='Rho':
        sigFileName = 'workspace_STAT_{1}_{0}cat_bdt0_2018.root'.format(cat, meson)
        bkgFileName = sigFileName
        iPeriod = 4
        iPos = 11
        varName = 'mesonGammaMass'
        varTitle = 'm_{#rho#gamma}'
        blinding = False
        blindLow = 120.
        blindHigh = 130.
        workspaceName = 'workspace_STAT_{}_{}cat_bdt0_2018'.format(meson, cat)
        signalPDFName = 'crystal_ball_{}_{}cat_bdt0_ggH'.format(meson, cat)
        backgroundPDFName = 'chebychev_{}cat_bdt0_bkg'.format(cat)
        dataName = 'observed_data'
        signalDataName = 'dataset_ggH'.format(cat)
        BR = 6.98e-4
        sigEntry='B(H#rightarrow#rho#gamma) = 6.98#times10^{-4}'
        saveName = 'TO_{}_{}_bdt0.png'.format(cat, meson)
        plotMax = 390
        residMin = -45
        residMax = 45
        legend_xl = .55
        legend_xr = .95
        legend_yb = .6
        legend_yt = .9
    elif cat=='GF' and meson=='Phi':
        sigFileName = 'workspace_STAT_{1}_{0}cat_bdt0_2018.root'.format(cat, meson)
        bkgFileName = sigFileName
        iPeriod = 4
        iPos = 11
        varName = 'mesonGammaMass'
        varTitle = 'm_{#phi#gamma}'
        blinding = False
        blindLow = 120.
        blindHigh = 130.
        workspaceName = 'workspace_STAT_{}_{}cat_bdt0_2018'.format(meson, cat)
        signalPDFName = 'crystal_ball_{}_{}cat_bdt0_ggH'.format(meson, cat)
        backgroundPDFName = 'chebychev_{}cat_bdt0_bkg'.format(cat)
        dataName = 'observed_data'
        signalDataName = 'dataset_ggH'.format(cat)
        BR = 3.76e-4
        sigEntry='B(H#rightarrow#phi#gamma) = 3.76#times10^{-4}'
        saveName = 'TO_{}_{}_bdt0.png'.format(cat, meson)
        plotMax = 70
        residMin = -25
        residMax = 25
        legend_xl = .55
        legend_xr = .95
        legend_yb = .6
        legend_yt = .9

Plots = pubplots(iPeriod, iPos)
Plots.setFindFileDir(workspaceDir)
Plots.getKeyVar(sigFileName, workspaceName, varName, varTitle, 100., 170., blinding, blindLow, blindHigh)
Plots.getSignalPDF(sigFileName, workspaceName, signalPDFName)
Plots.getBackgroundPDF(bkgFileName, workspaceName, backgroundPDFName)
Plots.getData(bkgFileName, workspaceName, dataName)
Plots.setSignalBranchingRatio(sigFileName, workspaceName, signalDataName, BR)
Plots.addLegendEntries(dataEntry='Data',
                       bkgFitEntry='Background Fit',
                       bkgSigma1ErrorEntry='Background #pm 1#sigma',
                       bkgSigma2ErrorEntry='Background #pm 2#sigma',
                       sigEntry=sigEntry)
Plots.legend_xl = legend_xl
Plots.legend_xr = legend_xr
Plots.legend_yb = legend_yb
Plots.legend_yt = legend_yt
Plots.makePlot(saveName, 70, 100., 170., 0, plotMax, False, residMin, residMax)
