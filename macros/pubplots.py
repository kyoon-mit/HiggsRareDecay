from ROOT import *
import CMS_lumi, tdrstyle
from os import path

class pubplots:
    """Class to handle stylish plots for HIG-23-005.

    In order to work properly, the modules CMS_lumi and tdrstyle
    must be downloaded from this TWiki page:
    https://twiki.cern.ch/twiki/bin/viewauth/CMS/Internal/FigGuidelines

    The __init__ method sets the global style to the default TDRStyle.

    Attributes:
        data (RooDataHist or RooDataSet): Data.
        sigPDF (RooAbsPdf): Signal PDF.
        bkgPDF (RooAbsPdf): Background PDF.
        combPDF (RooRealSumPdf): Combined signal + background PDF.
        dir (str): The path to the directory where the ROOT files reside.
    """

    def __init__(self):
        tdrstyle.setTDRStyle()
        self.data = None
        self.sigPDF = None
        self.bkgPDF = None
        self.combPDF = None
        self.dir = ''

    def setFindFileDir(self, directory):
        """Method to set the directory to find the ROOT files.

        Args:
            dir (str): The path to the directory where the ROOT files reside.
        """
        self.dir = directory

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

    def getbackgroundPDF(self, backgroundFileName, workspaceName, backgroundPDFName):
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

    def getData(self, dataFileName, workspaceName, dataPDFName):
        """Method to get the data.

        Args:
            dataFileName (str): Name of the ROOT file containing the data.
            workspaceName (str): Name of the RooWorkspace inside the ROOT file.
            dataPDFName (str): Name of the data PDF.
        """
        dataFile = TFile.Open(path.join(self.dir, dataFileName), 'READ')
        w = dataFile.Get(workspaceName)
        self.data = w.pdf(dataPDFName)
        self.data.SetName('data')
    
    def setSignalNevents(self, Nevents):
        """Sets the number of signal events in order to properly scale the signal PDF.

        One would calculate the expected signal events under a certain hypothesis.
        (e.g. m events for an upper limit of CL 95%).

        Args:
            Nevents (float): Number of signal events.
        """
        Nsig = RooRealVar('Nsig', 'Nsig', Nevents, 0.0, 1000.0)
        Nbkg = RooRealVar('Nbkg', 'Nbkg', self.data.sum(True), 0.0, 100000.0)
        self.combPDF = RooRealSumPdf('comb', 'comb',
                                     RooArgList(self.sigPDF, self.bkgPDF),
                                     RooArgList(Nsig, Nbkg), True)