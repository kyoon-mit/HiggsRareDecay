from pubplots import *

group = 'TO' # 'TO'
cat = 'GF' # 'VBF'
meson = 'Rho'

if group=='MIT':
    workspaceDir = '/work/submit/kyoon/CMSSW_10_6_27/src/hig-23-005/datacards/MIT_all/workspaces'
    if cat=='GF' and meson=='Rho':
        sigFileName = 'workspace_{}_{}cat_2018.root'.format(meson, cat)
        bkgFileName = 'workspace_{}_{}cat_2018.root'.format(meson, cat)
        iPeriod = 4
        iPos = 11
        varName = 'mh'
        varTitle = 'm_{#rho#gamma}'
        blinding = False
        workspaceName = 'w'
        signalPDFName = 'crystal_ball_{}Cat_{}cat_ggH'.format(meson, cat)
        signalNormName = 'crystal_ball_{}Cat_{}cat_ggH_norm'.format(meson, cat)
        backgroundPDFName = 'chebychev3_{}Cat_{}cat'.format(meson, cat)
        backgroundNormName = 'multipdf_{}Cat_{}cat_bkg_norm'.format(meson, cat)
        dataName = 'observed_data'
        # signalDataName = 'N/A'
        BR = 9.13e-4
        sigEntry='B(H#rightarrow#rho#gamma) = 9.13#times10^{-4}'
        saveName = 'MIT_{}_{}.png'.format(cat, meson)
        plotMax = 1500
        residMin = -100
        residMax = 100
        legend_xl = .55
        legend_xr = .95
        legend_yb = .6
        legend_yt = .9
    elif cat=='GF' and meson=='Phi':
        sigFileName = 'workspace_{}_{}cat_2018.root'.format(meson, cat)
        bkgFileName = 'workspace_{}_{}cat_2018.root'.format(meson, cat)
        iPeriod = 4
        iPos = 11
        varName = 'mh'
        varTitle = 'm_{#phi#gamma}'
        blinding = False
        workspaceName = 'w'
        signalPDFName = 'crystal_ball_{}Cat_{}cat_ggH'.format(meson, cat)
        signalNormName = 'crystal_ball_{}Cat_{}cat_ggH_norm'.format(meson, cat)
        backgroundPDFName = 'chebychev3_{}Cat_{}cat'.format(meson, cat)
        backgroundNormName = 'multipdf_{}Cat_{}cat_bkg_norm'.format(meson, cat)
        dataName = 'observed_data'
        # signalDataName = 'N/A'
        BR = 4.74e-4
        sigEntry='B(H#rightarrow#phi#gamma) = 4.74#times10^{-4}'
        saveName = 'MIT_{}_{}.png'.format(cat, meson)
        plotMax = 200
        residMin = -40
        residMax = 40
        legend_xl = .55
        legend_xr = .95
        legend_yb = .6
        legend_yt = .9
    elif cat=='VBF' and meson=='Rho':
        sigFileName = 'workspace_{}_{}cat_Run2.root'.format(meson, cat)
        bkgFileName = 'workspace_{}_{}cat_Run2.root'.format(meson, cat)
        iPeriod = 4
        iPos = 11
        varName = 'mh'
        varTitle = 'm_{#rho#gamma}'
        blinding = False
        workspaceName = 'w'
        signalPDFName = 'crystal_ball_{}Cat_{}cat_VBFH'.format(meson, cat)
        signalNormName = 'crystal_ball_{}Cat_{}cat_VBFH_norm'.format(meson, cat)
        backgroundPDFName = 'chebychev1_{}Cat_{}cat'.format(meson, cat)
        backgroundNormName = 'multipdf_{}Cat_{}cat_bkg_norm'.format(meson, cat)
        dataName = 'observed_data'
        # signalDataName = 'N/A'
        BR = 1.6e-3
        sigEntry='B(H#rightarrow#rho#gamma) = 1.6#times10^{-3}'
        saveName = 'MIT_{}_{}.png'.format(cat, meson)
        plotMax = 20
        residMin = -10
        residMax = 10
        legend_xl = .55
        legend_xr = .95
        legend_yb = .6
        legend_yt = .9
    elif cat=='VBF' and meson=='Phi':
        sigFileName = 'workspace_{}_{}cat_Run2.root'.format(meson, cat)
        bkgFileName = 'workspace_{}_{}cat_Run2.root'.format(meson, cat)
        iPeriod = 4
        iPos = 11
        varName = 'mh'
        varTitle = 'm_{#rho#gamma}'
        blinding = False
        workspaceName = 'w'
        signalPDFName = 'crystal_ball_{}Cat_{}cat_VBFH'.format(meson, cat)
        signalNormName = 'crystal_ball_{}Cat_{}cat_VBFH_norm'.format(meson, cat)
        backgroundPDFName = 'chebychev1_{}Cat_{}cat'.format(meson, cat)
        backgroundNormName = 'multipdf_{}Cat_{}cat_bkg_norm'.format(meson, cat)
        dataName = 'observed_data'
        # signalDataName = 'N/A'
        BR = 2.0e-3
        sigEntry='B(H#rightarrow#rho#gamma) = 1.6#times10^{-3}'
        saveName = 'MIT_{}_{}.png'.format(cat, meson)
        plotMax = 20
        residMin = -10
        residMax = 10
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
        workspaceName = 'workspace_STAT_{}_{}cat_bdt0_2018'.format(meson, cat)
        signalPDFName = 'crystal_ball_{}_{}cat_bdt0_ggH'.format(meson, cat)
        signalNormName = 'crystal_ball_{}_{}cat_bdt0_ggH_norm'.format(meson, cat)
        backgroundPDFName = 'chebychev_{}cat_bdt0_bkg'.format(cat)
        backgroundNormName = 'multipdf_{}_{}cat_bdt0_bkg_norm'.format(meson, cat)
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
        workspaceName = 'workspace_STAT_{}_{}cat_bdt0_2018'.format(meson, cat)
        signalPDFName = 'crystal_ball_{}_{}cat_bdt0_ggH'.format(meson, cat)
        signalNormName = 'crystal_ball_{}_{}cat_bdt0_ggH_norm'.format(meson, cat)
        backgroundPDFName = 'chebychev_{}cat_bdt0_bkg'.format(cat)
        backgroundNormName = 'multipdf_{}_{}cat_bdt0_bkg_norm'.format(meson, cat)
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
Plots.getKeyVar(sigFileName, workspaceName, varName, varTitle, 100., 170., blinding)
Plots.getSignalPDF(sigFileName, workspaceName, signalPDFName, signalNormName)
Plots.getBackgroundPDF(bkgFileName, workspaceName, backgroundPDFName, backgroundNormName)
#Plots.getData(bkgFileName, workspaceName, dataName)
Plots.generateData()
Plots.setSignalBranchingRatio(BR)
Plots.addLegendEntries(dataEntry='Data',
                       bkgFitEntry='Background Fit',
                       bkgSigma1ErrorEntry='Background #pm 1#sigma',
                       bkgSigma2ErrorEntry='Background #pm 2#sigma',
                       sigEntry=sigEntry)
Plots.legend_xl = legend_xl
Plots.legend_xr = legend_xr
Plots.legend_yb = legend_yb
Plots.legend_yt = legend_yt
Plots.makePlot(saveName, 70, 100., 170., 0, plotMax, True, residMin, residMax)
