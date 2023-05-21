from ROOT import *
import CMS_lumi, tdrstyle
from os import path

class pubplots:
    """Class to handle stylish plots for HIG-23-005.

    In order to work properly, the modules CMS_lumi and tdrstyle
    must be downloaded from this TWiki page:
    https://twiki.cern.ch/twiki/bin/viewauth/CMS/Internal/FigGuidelines

    The __init__ method sets the global style to the default TDRStyle.
    It accepts arguments to pass onto the CMS_lumi setting.
    
    Args:
        iPeriod (int): Calculated as follows.
                       1*(0/1 7 TeV) + 2*(0/1 8 TeV)  + 4*(0/1 13 TeV)
        iPos (int): Alignment for the luminosity text

    Attributes:
        keyVar (RooRealVar): Key Variable, i.e. mass of the Higgs candidate.
        data (RooDataHist or RooDataSet): Data.
        sigPDF (RooAbsPdf): Signal PDF.
        bkgPDF (RooAbsPdf): Background PDF.
        combPDF (RooRealSumPdf): Combined signal + background PDF.
        Nsig (RooRealVar):
        Nbkg (RooRealVar): 
        Nevents (int):
        dir (str): The path to the directory where the ROOT files reside.
        bkgFitEntry (str):
        bkgErrorEntry (str):
        sigEntry (str):
        iPeriod (int): iPeriod.
        iPos (int): iPos.
    """

    def __init__(self, iPeriod, iPos):
        tdrstyle.setTDRStyle()

        # Attribute for file I/O
        self.dir = ''

        # Attributes for building/storing the RooFit objects
        self.keyVar = None
        self.data = None
        self.sigPDF = None
        self.bkgPDF = None
        self.combPDF = None
        self.Nsig = None
        self.Nbkg = None
        self.Nevents = 0

        # Attributes for beautifying the plotting area
        self.frameLabelSize = 0.03
        self.frameAxisTitleOffset = 1.8
        self.frameFont = 10
        self.dataMarkerStyle = kFullCircle
        self.dataErrorBarStyle = ''
        self.bkgFitLineWidth = 2
        self.bkgFitLineColor = kBlack
        self.bkgErrorFillColor = kBlue-9
        self.bkgFillColor = kBlue-4
        self.sigFillColor = kOrange+8

        # Attributes related to the legend
        self.dataEntry = ''
        self.bkgFitEntry = ''
        self.bkgErrorEntry = ''
        self.sigEntry = ''
        self.legend_xl = .25
        self.legend_xr = .6
        self.legend_yb = .4
        self.legend_yt = .6
        self.legendTextSize = .02
        
        # Attributes for CMS-style texts
        self.iPeriod = iPeriod
        self.iPos = iPos

    def setFindFileDir(self, directory):
        """Method to set the directory to find the ROOT files.

        Args:
            dir (str): The path to the directory where the ROOT files reside.
        """
        self.dir = directory

    def getKeyVar(self, fileName, workspaceName, varName, varTitle, unit='GeV'):
        """
        """
        varFile = TFile.Open(path.join(self.dir, fileName), 'READ')
        w = varFile.Get(workspaceName)
        self.keyVar = w.var(varName)
        self.keyVar.SetTitle(varTitle)
        self.keyVar.setUnit(unit)

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
    
    def setSignalNevents(self, Nevents):
        """Sets the number of signal events in order to properly scale the signal PDF.

        One would calculate the expected signal events under a certain hypothesis.
        (e.g. m events for an upper limit of CL 95%).

        Args:
            Nevents (float): Number of signal events.
        """
        self.Nevents = Nevents

    def addLegendEntries(self, dataEntry, bkgFitEntry, bkgErrorEntry, sigEntry):
        """Enter texts for the legend.

        Args:
            dataEntry (str):
            bkgFitEntry (str):
            bkgErrorEntry (str):
            sigEntry (str):
        """
        self.dataEntry = dataEntry
        self.bkgFitEntry = bkgFitEntry
        self.bkgErrorEntry = bkgErrorEntry
        self.sigEntry = sigEntry

    def makePlot(self, saveName, Nbins, rangeLow, rangeHigh, residuals=False):
        """Create and save the final plot.

        Args:
            saveName (str): Name of the file to which the plot is saved.
            Nbins (int): Number of bins.
            rangeLow (float): Lower range of the plotting region.
            rangeHigh (float): Upper range of the region.
            residuals (bool, optional): Whether to make plot of residuals.
                Defaults to False.
        """
        # Create canvas
        c_width = 1200
        c_height = 1200
        if residuals: c_height = 1500

        # Create frame
        plotFrame = self.keyVar.frame()
        plotFrame.SetXTitle('{} [{}]'.format(self.keyVar.GetTitle(), self.keyVar.getUnit()))
        plotFrame.SetYTitle('Events/{} GeV'.format(int((rangeHigh - rangeLow)/Nbins)))
        plotFrame.SetLabelSize(self.frameLabelSize, 'XY')
        plotFrame.GetXaxis().SetTitleSize(self.frameLabelSize)
        plotFrame.GetXaxis().SetTitleOffset(self.frameAxisTitleOffset-0.5)
        plotFrame.GetYaxis().SetTitleSize(self.frameLabelSize)
        plotFrame.GetYaxis().SetTitleOffset(self.frameAxisTitleOffset)

        # Create a combined sig + bkg model
        Nsig = RooRealVar('Nsig', 'Nsig', self.Nevents, 0.0, 5000.0)
        Nbkg = RooRealVar('Nbkg', 'Nbkg', self.data.sum(True), 0.0, 10000.0)
        combPDF = RooRealSumPdf('comb', 'comb', RooArgList(self.sigPDF, self.bkgPDF), RooArgList(Nsig, Nbkg), True)

        # Get fit result from bkg pdf (better way to do this other than fitTo?)
        fit_status = -1
        max_tries = 10
        tries = 1
        while fit_status != 0:
            bkgFitResult = self.bkgPDF.fitTo(self.data,
                                             RooFit.Save(True),
                                             RooFit.Strategy(1),
                                             RooFit.PrintLevel(-1))
            fit_status = bkgFitResult.status()
            print (fit_status)
            if tries >= max_tries:
                break
            elif (fit_status != 0):
                params.assignValueOnly(bkgFitResult.randomizePars())
                tries += 1

        # Plot the data
        self.data.plotOn(plotFrame,
                         RooFit.Binning(Nbins))

        # Plot bkg only
        ### Plot bkg line
        self.bkgPDF.plotOn(plotFrame,
                           RooFit.LineColor(self.bkgFitLineColor),
                           RooFit.LineWidth(self.bkgFitLineWidth),
                           RooFit.DrawOption('L'),
                           RooFit.MoveToBack())

        ### Plot bkg uncertainty
        self.bkgPDF.plotOn(plotFrame,
                           RooFit.FillColor(self.bkgErrorFillColor),
                           RooFit.LineColor(self.bkgFitLineColor),
                           RooFit.VisualizeError(bkgFitResult, 1),
                           RooFit.VisualizeError(bkgFitResult, -1),
                           RooFit.DrawOption('F'),
                           RooFit.Name('bkgPlusMinus1Sigma'),
                           RooFit.MoveToBack())

        ### Plot bkg color
        combPDF.plotOn(plotFrame,
                       RooFit.FillColor(self.bkgFillColor),
                       RooFit.LineColor(self.bkgFitLineColor),
                       RooFit.Components('bkgPDF'),
                       RooFit.DrawOption('F'),
                       RooFit.Name('bkg'),
                       RooFit.MoveToBack())

        # Plot sig + bkg model
        combPDF.plotOn(plotFrame,
                       RooFit.FillColor(self.sigFillColor),
                       RooFit.LineColor(self.bkgFitLineColor),
                       RooFit.DrawOption('F'),
                       RooFit.Name('comb'),
                       RooFit.MoveToBack())

        # Make Legend
        legend = TLegend(self.legend_xl, self.legend_yb, self.legend_xr, self.legend_yt)
        legend.AddEntry(plotFrame.findObject('h_data'), self.dataEntry, 'pe')
        legend.AddEntry(plotFrame.findObject('bkgPlusMinus1Sigma'), self.bkgErrorEntry, 'lf')
        legend.AddEntry(plotFrame.findObject('bkg'), self.bkgFitEntry, 'f')
        legend.AddEntry(plotFrame.findObject('comb'), self.sigEntry, 'f')
        legend.SetTextAlign(12)
        legend.SetTextSize(self.legendTextSize)

        # Add residuals
        if residuals:
            residFrame = self.keyVar.frame()
            residFrame.addPlotable(plotFrame.residHist('h_data', 'bkg', True), 'P')
            ####### TODO: ADD uncertainty of the bkgPDF to the residFrame
            residFrame.SetTitle('')
            residFrame.SetYTitle('data/background fit')
            plotFrame.SetXTitle('')

        # Draw
        c = TCanvas('c', 'c', c_width, c_height)
        if not residuals:
            plotFrame.Draw()
            legend.Draw()
            CMS_lumi.CMS_lumi(c, self.iPeriod, self.iPos)  # Draw lumi and sqrt(s) info
        else:
            pad1 = TPad('p1', 'p1', 0, .2, 1, 1)
            pad2 = TPad('p2', 'p2', 0, 0, 1, .2)
            pad1.cd()
            plotFrame.Draw()
            legend.Draw()
            CMS_lumi.CMS_lumi(c, self.iPeriod, self.iPos)  # Draw lumi and sqrt(s) info
            pad2.cd()
            residFrame.Draw()
            c.cd()
            pad1.Draw()
            pad2.Draw()

        # Save
        c.Update()
        c.SaveAs(saveName)
        c.Close()
    
if __name__=="__main__":
    sigFileName = 'Signal_GFcat__RhoCat_2018_workspace.root'
    bkgFileName = 'Bkg_GFcat__RhoCat_2018_workspace.root'

    Plots = pubplots(iPeriod=4, iPos=11)
    Plots.setFindFileDir('/work/submit/mariadlf/cards_march20/WS_MARCH20/')
    Plots.getKeyVar(sigFileName, 'w', 'mh', varTitle='m_{#rho#gamma}')
    Plots.getSignalPDF(sigFileName, 'w', 'crystal_ball_RhoCat_GFcat_ggH')
    Plots.getBackgroundPDF(bkgFileName, 'w', 'bern3_RhoCat_GFcat')
    Plots.getData(bkgFileName, 'w', 'datahist_RhoCat_GFcat')
    Plots.setSignalNevents(25.)
    Plots.addLegendEntries(dataEntry='Data',
                           bkgFitEntry='Background Only Model',
                           bkgErrorEntry='Background Fit #pm 1#sigma',
                           sigEntry='H#rightarrow#rho#gamma BR=...')
    Plots.makePlot('test.png',
                   Nbins=70,
                   rangeLow=100.,
                   rangeHigh=170.,
                   residuals=False)