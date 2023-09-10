from kytools import rootpdf
from ROOT import RooRealVar

pdfs = rootpdf.Models()
x = RooRealVar('mh', 'mh', 125, 100, 170)
gauss = pdfs.gaussian(x, 125, 115, 135, 5, 1, 10, suffix='mysuffix')
bern = pdfs.bernstein(x, degree=2, suffix='mysuffix', coeff_vals=[(0.2, 0., 0.5), (0.1, 0., 1.), (0.1, 0., 1.)])

signal = gauss.generate(x, 1000) # RooDataSet
background = bern.generateBinned(x, 20000) # RooDataHist

fits = rootpdf.FitIt(x, 'w')
fits.add_pdf(bern, 'bern')
fits.add_pdf(gauss, 'gauss')
fits.add_data(signal, 'signal')
fits.add_data(background, 'background')

fits.fit(data_key='signal', pdf_key='gauss')
fits.fit(data_key='background', pdf_key='bern')

fits.wsave("wtest.root")