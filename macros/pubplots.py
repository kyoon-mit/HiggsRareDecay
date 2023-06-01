from ROOT import *
import CMS_lumi, tdrstyle
from os import path

class pubplots:
    """Class to handle stylish plots for HIG-23-005.

    In order to work properly, the modules CMS_lumi and tdrstyle
    must be downloaded from this TWiki page:
    https://twiki.cern.ch/twiki/bin/viewauth/CMS/Internal/FigGuidelines

    The __init__ method accepts arguments to pass onto the CMS_lumi setting.
    
    Args:
        iPeriod (int): Calculated as follows.
                       1*(0/1 7 TeV) + 2*(0/1 8 TeV)  + 4*(0/1 13 TeV)
        iPos (int): Alignment for the luminosity text

    Attributes:
        (self explanatory)
    """

    def __init__(self, iPeriod, iPos):
        # Attribute for file I/O
        self.dir = ''

        # Attributes for building/storing the RooFit objects
        self.keyVar = None
        self.data = None
        self.blindData = None
        self.sigPDF = None
        self.bkgPDF = None
        self.combPDF = None
        self.Nsig = None
        self.Nbkg = None
        self.Nevents = 0
        self.blinding = True

        # Attributes for beautifying the plotting area
        self.frameLabelSize = 0.04
        self.frameAxisTitleOffset = 1.6
        self.frameFont = 12
        self.canvasPad1Height = 600
        self.canvasPad2Height = 150
        self.canvasWidth = 800
        self.canvasTopMargin = .08
        self.canvasBottomMargin = .12
        self.canvasLeftMargin = .12
        self.canvasRightMargin = .04
        self.dataMarkerStyle = kFullCircle
        self.dataMarkerSize = .5
        self.dataXErrorBarSize = 0 # TODO
        self.bkg1SigmaErrorFillColor = kGray #kGreen + 1 # Brazilian
        self.bkg2SigmaErrorFillColor = kOrange # Brazilian
        self.bkgFitFillColor = kWhite
        self.bkgFitFillStyle = 0 # Transparent
        self.bkgFitLineColor = kBlue
        self.bkgFitLineStyle = kDashed
        self.bkgFitLineWidth = 2
        self.sigFitFillColor = kWhite
        self.sigFitFillStyle = 0 # Transparent
        self.sigFitLineColor = kRed
        self.sigFitLineStyle = kSolid
        self.sigFitLineWidth = 2

        # Attributes related to the legend
        self.dataEntry = ''
        self.bkgFitEntry = ''
        self.bkgSigma1ErrorEntry = ''
        self.bkgSigma2ErrorEntry = ''
        self.sigEntry = ''
        self.legend_xl = .2
        self.legend_xr = .6
        self.legend_yb = .1
        self.legend_yt = .4
        self.legendTextSize = .03
        
        # Attributes for CMS-style texts
        self.iPeriod = iPeriod
        self.iPos = iPos

    def setFindFileDir(self, directory):
        """Method to set the directory to find the ROOT files.

        Args:
            dir (str): The path to the directory where the ROOT files reside.
        """
        self.dir = directory

    def getKeyVar(self, fileName, workspaceName, varName, varTitle, rangeLow, rangeHigh, blinding='True', blindLow=115, blindHigh=135, unit='GeV'):
        """
        """
        varFile = TFile.Open(path.join(self.dir, fileName), 'READ')
        w = varFile.Get(workspaceName)
        self.keyVar = w.var(varName)
        self.keyVar.SetTitle(varTitle)
        self.keyVar.setUnit(unit)
        self.keyVar.setRange('full', float(rangeLow), float(rangeHigh))
        self.blinding = blinding
        if self.blinding:
            self.keyVar.setRange('left', float(rangeLow), float(blindLow))
            self.keyVar.setBins(int(blindLow-rangeLow), 'left')
            self.keyVar.setRange('right', float(blindHigh), float(rangeHigh))
            self.keyVar.setBins(int(rangeHigh-blindHigh), 'right')

    def getSignalPDF(self, signalFileName, workspaceName, signalPDFName):
        """Method to get the signal PDF.

        Args:
            signalFileName (str): Name of the ROOT file containing the signal PDF.
            workspaceName (str): Name of the RooWorkspace inside the ROOT file.
            signalPDFName (str): Name of the signal PDF.
        """
        signalFile = TFile.Open(path.join(self.dir, signalFileName), 'READ')
        w = signalFile.Get(workspaceName)
        self.sigPDF = w.pdf(signalPDFName)
        self.sigPDF.SetName('sigPDF')

    def getBackgroundPDF(self, backgroundFileName, workspaceName, backgroundPDFName):
        """Method to get the background PDF.

        Args:
            backgroundFileName (str): Name of the ROOT file containing the background PDF.
            workspaceName (str): Name of the RooWorkspace inside the ROOT file.
            backgroundPDFName (str): Name of the background PDF.
        """
        backgroundFile = TFile.Open(path.join(self.dir, backgroundFileName), 'READ')
        w = backgroundFile.Get(workspaceName)
        self.bkgPDF = w.pdf(backgroundPDFName)
        self.bkgPDF.SetName('bkgPDF')

    def getData(self, dataFileName, workspaceName, dataName):
        """Method to get the data.

        Args:
            dataFileName (str): Name of the ROOT file containing the data.
            workspaceName (str): Name of the RooWorkspace inside the ROOT file.
            dataName (str): Name of the data PDF.
        """
        dataFile = TFile.Open(path.join(self.dir, dataFileName), 'READ')
        w = dataFile.Get(workspaceName)
        self.data = w.data(dataName)
        self.data.SetName('data')
        if self.blinding:
            self.blindData = self.data.reduce(RooFit.CutRange('left,right'))
    
    def setSignalNevents(self, Nevents):
        """Sets the number of signal events in order to properly scale the signal PDF.

        One would calculate the expected signal events under a certain hypothesis.
        (e.g. m events for an upper limit of CL 95%).

        Args:
            Nevents (float): Number of signal events.
        """
        self.Nevents = Nevents

    def setSignalBranchingRatio(self, signalFileName, workspaceName, signalDataName,
                                branchingRatio, sigBR=1.):
        """Sets the number of signal events by the branching ratio.

        One would calculate the expected signal events under a certain hypothesis.
        (e.g. m events for an upper limit of CL 95%).

        Args:
            signalFileName (str): Name of the ROOT file containing the signal PDF.
            workspaceName (str): Name of the RooWorkspace inside the ROOT file.
            signalDataName (str): Name of the signal data.
            branchingRatio (float): Branching ratio of the signal.
            sigBR (float): Default BR under which the signal data was created.
        """
        signalFile = TFile.Open(path.join(self.dir, signalFileName), 'READ')
        w = signalFile.Get(workspaceName)
        sigData = w.data(signalDataName)
        data_sum = 0
        try:
            data_sum = sigData.sum(True)
        except:
            data_sum = sigData.sumEntries()
        self.Nevents = data_sum * float(branchingRatio) / float(sigBR)
        print ('Signal N events: {:.2f}'.format(self.Nevents))

    def addLegendEntries(self, dataEntry, bkgFitEntry, bkgSigma1ErrorEntry,
                         bkgSigma2ErrorEntry, sigEntry):
        """Enter texts for the legend.

        Args:
            dataEntry (str):
            bkgFitEntry (str):
            bkgSigma1ErrorEntry (str):
            bkgSigma2ErrorEntry (str):
            sigEntry (str):
        """
        self.dataEntry = dataEntry
        self.bkgFitEntry = bkgFitEntry
        self.bkgSigma1ErrorEntry = bkgSigma1ErrorEntry
        self.bkgSigma2ErrorEntry = bkgSigma2ErrorEntry
        self.sigEntry = sigEntry

    def makePlot(self, saveName, Nbins, rangeLow, rangeHigh, plotMin=0, plotMax=5000,
                 residuals=False, residMin=-250, residMax=250):
        """Create and save the final plot.

        Args:
            saveName (str): Name of the file to which the plot is saved.
            Nbins (int): Number of bins.
            rangeLow (float): Lower range of the plotting region.
            rangeHigh (float): Upper range of the region.
            plotMin (float): The lower limit for the y axis.
                Defaults to 0.
            plotMax (float): The lower limit for the y axis.
                Defaults to 5000.
            residuals (bool, optional): Whether to make plot of residuals.
                Defaults to False.
            residMin (float, optional): If residuals is True, then this sets
                the lower limit for the y axis on the residual plot.
                Defaults to -250.
            residMax (float, optional: If residuals is True, then this sets
                the lower limit for the y axis on the residual plot.
                Defaults to 250.
        """
        tdrstyle.setTDRStyle()

        # Create canvas
        c = TCanvas('c', 'c', self.canvasWidth, self.canvasPad1Height)
        if residuals: c = TCanvas('c', 'c', self.canvasWidth, self.canvasPad1Height + self.canvasPad2Height)
        c.SetFillColor(0)
        c.SetBorderMode(0)
        c.SetFrameFillStyle(0)
        c.SetFrameBorderMode(0)
        c.SetLeftMargin(self.canvasLeftMargin)
        c.SetRightMargin(self.canvasRightMargin)
        c.SetTopMargin(self.canvasTopMargin)
        c.SetBottomMargin(self.canvasBottomMargin)
        c.SetTickx(0)
        c.SetTicky(0)

        # Set labels
        xTitle = '{} [{}]'.format(self.keyVar.GetTitle(), self.keyVar.getUnit())
        yTitle = 'Events/{} {}'.format(int((rangeHigh - rangeLow)/Nbins), self.keyVar.getUnit())

        # Create frame
        plotFrame = self.keyVar.frame()
        plotFrame.SetXTitle(xTitle)
        plotFrame.SetYTitle(yTitle)
        plotFrame.SetLabelSize(self.frameLabelSize, 'XY')
        # plotFrame.GetXaxis().SetTitleSize(self.frameLabelSize)
        # plotFrame.GetXaxis().SetTitleOffset(self.frameAxisTitleOffset-0.5)
        # plotFrame.GetYaxis().SetTitleSize(self.frameLabelSize)
        # plotFrame.GetYaxis().SetTitleOffset(self.frameAxisTitleOffset)

        # Create a combined sig + bkg model
        data_sum = 0
        try:
            data_sum = self.data.sum(True)
        except:
            data_sum = self.data.sumEntries()
        Nsig = RooRealVar('Nsig', 'Nsig', self.Nevents, 0.0, 5000.0)
        Nbkg = RooRealVar('Nbkg', 'Nbkg', data_sum, 0.0, 10000.0)
        Nsig.setConstant()
        Nbkg.setConstant()
        combPDF = RooRealSumPdf('comb', 'comb', RooArgList(self.sigPDF, self.bkgPDF), RooArgList(Nsig, Nbkg), True)

        # Get fit result from bkg pdf
        # TODO: better way to do this other than fitTo?
        fit_status = -1
        max_tries = 10
        tries = 1
        while fit_status != 0:
            if not self.blinding:
                bkgFitResult = self.bkgPDF.fitTo(self.data,
                                                 RooFit.Save(True),
                                                 RooFit.Strategy(1),
                                                 RooFit.PrintLevel(-1))
            else:
                bkgFitResult = self.bkgPDF.fitTo(self.blindData,
                                                 RooFit.Range('full'),
                                                 RooFit.Save(True),
                                                 RooFit.Strategy(1),
                                                 RooFit.PrintLevel(-1))
            fit_status = bkgFitResult.status()
            if tries >= max_tries:
                break
            elif (fit_status != 0):
                params.assignValueOnly(bkgFitResult.randomizePars())
                tries += 1

        # Plot the data
        if not self.blinding:
            self.data.plotOn(plotFrame,
                             RooFit.Binning(Nbins),
                             RooFit.MarkerSize(self.dataMarkerSize),
                             RooFit.MarkerStyle(self.dataMarkerStyle),
                             RooFit.XErrorSize(self.dataXErrorBarSize))
        else:
            self.blindData.plotOn(plotFrame,
                                  RooFit.NormRange('left,right'),
                                  RooFit.MarkerSize(self.dataMarkerSize),
                                  RooFit.MarkerStyle(self.dataMarkerStyle),
                                  RooFit.XErrorSize(self.dataXErrorBarSize),
                                  RooFit.Name('h_blind_data'))

        # Plot sig + bkg model
        combPDF.plotOn(plotFrame,
                       RooFit.LineColor(self.sigFitLineColor),
                       RooFit.LineStyle(self.sigFitLineStyle),
                       RooFit.LineWidth(self.sigFitLineWidth),
                       RooFit.DrawOption('L'),
                       RooFit.Name('comb'),
                       RooFit.MoveToBack())


        ### Plot bkg only model
        combPDF.plotOn(plotFrame,
                       RooFit.FillColor(self.bkgFitFillColor),
                       RooFit.FillStyle(self.bkgFitFillStyle),
                       RooFit.LineColor(self.bkgFitLineColor),
                       RooFit.LineStyle(self.bkgFitLineStyle),
                       RooFit.Components('bkgPDF'),
                       RooFit.DrawOption('L'),
                       RooFit.Name('bkg'),
                       RooFit.MoveToBack())

        ### Plot bkg uncertainty
        self.bkgPDF.plotOn(plotFrame,
                           RooFit.FillColor(self.bkg1SigmaErrorFillColor),
                           RooFit.LineWidth(0),
                           RooFit.VisualizeError(bkgFitResult, 1),
                           RooFit.VisualizeError(bkgFitResult, -1),
                           RooFit.DrawOption('F'),
                           RooFit.Name('bkgPlusMinus1Sigma'),
                           RooFit.MoveToBack())

        # self.bkgPDF.plotOn(plotFrame,
        #                    RooFit.FillColor(self.bkg2SigmaErrorFillColor),
        #                    RooFit.LineWidth(0),
        #                    RooFit.VisualizeError(bkgFitResult, 2),
        #                    RooFit.VisualizeError(bkgFitResult, -2),
        #                    RooFit.DrawOption('F'),
        #                    RooFit.Precision(1e-4),
        #                    RooFit.Name('bkgPlusMinus2Sigma'),
        #                    RooFit.MoveToBack())

        plotFrame.SetMinimum(plotMin)
        plotFrame.SetMaximum(plotMax)

        # (Optional) Make plots for residuals
        if residuals:
            # residFrame.SetLabelSize(self.frameLabelSize*4, 'XY')
            # residFrame.GetXaxis().SetTitleSize(self.frameLabelSize)
            # residFrame.GetXaxis().SetTitleOffset(self.frameAxisTitleOffset-0.5)
            # residFrame.GetYaxis().SetTitleSize(self.frameLabelSize)
            # residFrame.GetYaxis().SetTitleOffset(self.frameAxisTitleOffset)
            plotFrame.SetXTitle('')
            plotFrame.SetLabelSize(0, 'X')

            yTitle = 'Data - Bkg'
            residFrame = self.keyVar.frame()
            residFrame.SetTitle('')
            residFrame.SetXTitle(xTitle)
            residFrame.SetYTitle(yTitle)
            residFrame.addPlotable(plotFrame.residHist('h_data', 'bkg', False), 'P')

            # Plot signal - bkg
            self.sigPDF.plotOn(residFrame,
                               RooFit.LineColor(self.sigFitLineColor),
                               RooFit.LineStyle(self.sigFitLineStyle),
                               RooFit.LineWidth(self.sigFitLineWidth),
                               RooFit.DrawOption('L'),
                               RooFit.Name('residSigPDF'),
                               RooFit.MoveToBack())
            
            # Plot bkg - bkg (i.e. 0)
            const_zero = RooRealVar('c1', 'c1', 0, -1.0, 1.0)
            const_zero.setConstant()
            const_zero.plotOn(residFrame,
                              RooFit.LineColor(self.bkgFitLineColor),
                              RooFit.LineStyle(self.bkgFitLineStyle),
                              RooFit.LineWidth(self.bkgFitLineWidth),
                              RooFit.DrawOption('L'),
                              RooFit.Name('residBkgPDF'),
                              RooFit.MoveToBack())
            
            # Make plots of bkg uncertainty - bkg
            bkgCentral = plotFrame.findObject('bkg')
            bkg1Sigma = plotFrame.findObject('bkgPlusMinus1Sigma')
            # bkg2Sigma = plotFrame.findObject('bkgPlusMinus2Sigma')
            bkg1SigmaX = bkg1Sigma.GetX()
            bkg1SigmaY = bkg1Sigma.GetY()
            # bkg2SigmaX = bkg2Sigma.GetX()
            # bkg2SigmaY = bkg2Sigma.GetY()
            bkgNPoints = bkg1Sigma.GetN()
            bkg1SigmaTGraph = TGraph(bkgNPoints)
            # bkg2SigmaTGraph = TGraph(bkgNPoints)
            params = RooArgList(self.bkgPDF.getParameters(RooArgSet(self.keyVar)))
            for i in range(0, bkgNPoints):
                bkg1SigmaTGraph.SetPoint(i, bkg1SigmaX[i], bkg1SigmaY[i] - bkgCentral.Eval(bkg1SigmaX[i]))
                # bkg2SigmaTGraph.SetPoint(i, bkg2SigmaX[i], bkg2SigmaY[i] - bkgCentral.Eval(bkg2SigmaX[i]))
            bkg1SigmaTGraph.SetFillColor(self.bkg1SigmaErrorFillColor)
            bkg1SigmaTGraph.GetXaxis().SetTitle(xTitle)
            bkg1SigmaTGraph.GetYaxis().SetTitle(yTitle)
            bkg1SigmaTGraph.GetXaxis().SetRangeUser(rangeLow, rangeHigh)
            bkg1SigmaTGraph.SetMinimum(residMin)
            bkg1SigmaTGraph.SetMaximum(residMax)
            # bkg2SigmaTGraph.SetFillColor(self.bkg2SigmaErrorFillColor)
            # bkg2SigmaTGraph.GetXaxis().SetTitle(xTitle)
            # bkg2SigmaTGraph.GetYaxis().SetTitle(yTitle)
            # bkg2SigmaTGraph.SetMinimum(residMin)
            # bkg2SigmaTGraph.SetMaximum(residMax)
            # bkg2SigmaTGraph.GetXaxis().SetRangeUser(rangeLow, rangeHigh)

        # Make Legend
        legend = TLegend(self.legend_xl, self.legend_yb, self.legend_xr, self.legend_yt)
        legend.AddEntry(plotFrame.findObject('h_data'), self.dataEntry, 'pe')
        legend.AddEntry(plotFrame.findObject('bkgPlusMinus1Sigma'), self.bkgSigma1ErrorEntry, 'f')
        # legend.AddEntry(plotFrame.findObject('bkgPlusMinus2Sigma'), self.bkgSigma2ErrorEntry, 'f')
        legend.AddEntry(plotFrame.findObject('bkg'), self.bkgFitEntry, 'l')
        legend.AddEntry(plotFrame.findObject('comb'), self.sigEntry, 'l')
        legend.SetTextAlign(12)
        legend.SetBorderSize(0)
        # legend.SetTextSize(self.legendTextSize)

        # Draw
        if not residuals:
            plotFrame.Draw()
            legend.Draw()
            CMS_lumi.CMS_lumi(c, self.iPeriod, self.iPos)  # Draw lumi and sqrt(s) info
        else:
            pad_height_ratio = 1 - float(self.canvasPad1Height) / (self.canvasPad1Height + self.canvasPad2Height) + tdrStyle.GetPadBottomMargin()
            pad1 = TPad('p1', 'p1', 0, pad_height_ratio, 1, 1)
            pad2 = TPad('p2', 'p2', 0, 0, 1, pad_height_ratio)
            pad1.SetBottomMargin(0.03) #
            pad2.SetTopMargin(0.03) #
            pad1.cd()
            plotFrame.Draw()
            legend.Draw()
            CMS_lumi.CMS_lumi(pad1, self.iPeriod, self.iPos)  # Draw lumi and sqrt(s) info
            pad2.cd()
            # bkg2SigmaTGraph.Draw('A CF')
            bkg1SigmaTGraph.Draw('A CF')
            residFrame.Draw('SAME')            
            c.cd()
            pad1.Draw()
            pad2.Draw()

        # Save
        c.Update()
        c.SaveAs(saveName)
        c.Close()
    
if __name__=="__main__":
    ### Warning: only try one example at a time ###
    ### TEST 1 ###
    """
    sigFileName = 'Signal_GFcat__RhoCat_2018_workspace.root'
    bkgFileName = 'Bkg_GFcat__RhoCat_2018_workspace.root'

    Plots = pubplots(iPeriod=4, iPos=11) # This can change
    Plots.setFindFileDir('/work/submit/mariadlf/cards_march20/WS_MARCH20/')
    Plots.getKeyVar(sigFileName,
                    workspaceName='w',
                    varName='mh',
                    rangeLow=100.,
                    rangeHigh=170.,
                    varTitle='m_{#rho#gamma}')
    Plots.getSignalPDF(sigFileName, 'w', 'crystal_ball_RhoCat_GFcat_ggH')
    Plots.getBackgroundPDF(bkgFileName, 'w', 'bern3_RhoCat_GFcat')
    Plots.getData(bkgFileName, 'w', 'datahist_RhoCat_GFcat')
    Plots.setSignalNevents(25.)
    Plots.addLegendEntries(dataEntry='Data',
                           bkgFitEntry='Background Only Model',
                           bkgSigma1ErrorEntry='Background #pm 1#sigma',
                           bkgSigma2ErrorEntry='Background #pm 2#sigma',
                           sigEntry='H#rightarrow#rho#gamma BR=...')

    Plots.makePlot('test_.pdf', # Better quality with pdf
                   Nbins=70,
                   rangeLow=100.,
                   rangeHigh=170.,
                   residuals=True)
    """

    ### TEST 2: MIT workspace ###
    """
    sigFileName = 'workspace_Rho_GFcat_2018.root'
    bkgFileName = sigFileName # identical file

    Plots = pubplots(iPeriod=4, iPos=11) # This can change
    Plots.setFindFileDir('/work/submit/kyoon/CMSSW_10_6_27/src/hig-23-005/datacards/MIT_all/workspaces')
    Plots.getKeyVar(sigFileName,
                    workspaceName='w',
                    varName='mh',
                    varTitle='m_{#rho#gamma}',
                    rangeLow=100.,
                    rangeHigh=170.)
    Plots.getSignalPDF(sigFileName, 'w', 'crystal_ball_RhoCat_GFcat_ggH')
    Plots.getBackgroundPDF(bkgFileName, 'w', 'bern3_RhoCat_GFcat')
    Plots.getData(bkgFileName, 'w', 'observed_data')
    Plots.setSignalNevents(25.)
    Plots.addLegendEntries(dataEntry='Data',
                           bkgFitEntry='Background Only Model',
                           bkgSigma1ErrorEntry='Background #pm 1#sigma',
                           bkgSigma2ErrorEntry='Background #pm 2#sigma',
                           sigEntry='H#rightarrow#rho#gamma BR=...')

    Plots.makePlot('test1.png',
                   Nbins=70,
                   rangeLow=100.,
                   rangeHigh=170.,
                   residuals=True)
    """

    ### TEST 3: Torino workspace ###
    """
    sigFileName = 'workspace_STAT_Rho_GFcat_bdt0_2018.root'
    bkgFileName = sigFileName # identical file

    Plots = pubplots(iPeriod=4, iPos=11) # This can change
    Plots.setFindFileDir('/work/submit/kyoon/CMSSW_10_6_27/src/hig-23-005/datacards/TO_ggh/workspaces')
    Plots.getKeyVar(sigFileName,
                    workspaceName='workspace_STAT_Rho_GFcat_bdt0_2018',
                    varName='mesonGammaMass',
                    rangeLow=100.,
                    rangeHigh=170.,
                    varTitle='m_{#rho#gamma}')
    Plots.getSignalPDF(sigFileName, 'workspace_STAT_Rho_GFcat_bdt0_2018', 'crystal_ball_Rho_GFcat_bdt0_ggH')
    Plots.getBackgroundPDF(bkgFileName, 'workspace_STAT_Rho_GFcat_bdt0_2018', 'bernstein_GFcat_bdt0_bkg')
    Plots.getData(bkgFileName, 'workspace_STAT_Rho_GFcat_bdt0_2018', 'observed_data')
    Plots.setSignalNevents(25.)
    Plots.addLegendEntries(dataEntry='Data',
                           bkgFitEntry='Background Only Model',
                           bkgSigma1ErrorEntry='Background #pm 1#sigma',
                           bkgSigma2ErrorEntry='Background #pm 2#sigma',
                           sigEntry='H#rightarrow#rho#gamma BR=...')
    Plots.legend_xl = .55
    Plots.legend_xr = .95
    Plots.legend_yb = .6
    Plots.legend_yt = .9
    Plots.makePlot('test2.pdf',
                   Nbins=70,
                   rangeLow=100.,
                   rangeHigh=170.,
                   residuals=True,
                   residMin=-120,
                   residMax=120)
    """