"""
rootpdf
=========================================

A module to create PDF models in ROOT.
"""
from ROOT import RooRealVar, RooArgList
from ROOT import RooGaussian, RooBernstein

class Models:
    def __init__(self):
        print('{}kytools: You have imported rootpdf, a module to easily create PDFs in ROOT.{}'.format('\033[1;34m', '\033[0m'))
        self.__varlist = []
        self.__pdflist = []

    def __var(self, name, title, value, low, high, unit=''):
        """Internal method for creating a RooRealVar.

        Args:
            name (str): Name of the variable.
            title (str): Display title.
            value (float): Nominal value.
            low (float): Lower limit.
            high (float): Upper limit.
            unit (str, optional): Units of this value.
        """
        var = RooRealVar(name, title, value, low, high, unit)
        self.__varlist.append(var)
        print ('{}kytools: Created variable with name {}.{}'.format('\033[0;36m', name, '\033[0m'))
        return var

    def gaussian(self, x, mean, mean_low, mean_high,
                 sigma, sigma_low, sigma_high, suffix):
        """Creates and returns a RooGaussian.

        Args:
            x (RooRealVar): Key variable.
            mean (float): Nominal mean value.
            mean_low (float): Lower limit of the mean.
            mean_high (float): Upper limit of the mean.
            sigma (float): Nominal sigma value.
            sigma_low (float): Lower limit of the mean.
            sigma_high (float): Upper limit of the mean.
            suffix (str): Suffix for the name of the variables.
        """
        mu = self.__var('gauss_mu_{}'.format(suffix), 'gauss_mu', mean, mean_low, mean_high)
        sigma = self.__var('gauss_sigma_{}'.format(suffix), 'gauss_sigma', sigma, sigma_low, sigma_high)
        pdfname = 'gauss'
        pdf = RooGaussian(pdfname, pdfname, x, mu, sigma)
        self.__pdflist.append(pdf)
        print ('{}kytools: Created gaussian with name {}.{}'.format('\033[0;35m', pdfname, '\033[0m'))
        return pdf
    
    def crystalball(self, x, mean, mean_low, mean_high,
                    width=1., width_low=0., width_high=3.,
                    aL=1., aL_low=0., aL_high=5.,
                    aR=1., aR_low=0., aR_high=5.,
                    nL=5., nL_low=2., nL_high=50.,
                    nR=2., nR_low=0., nR_high=5.):
        """Creates and returns a RooCrystalBall

        """
        return
    
    def bernstein(self, x, degree, suffix, coeff_vals):
        """Creates and returns a Bernstein polynomial of degree n.

        Args:
            x (RooRealVar): Key variable.
            degree (int): Degree of the polynomial
            suffix (str): Suffix for the name of the variables.
            coeff_vals (list of tuple(float) or list of list(float)): For each
                Bernstein coefficient, provide the nominal value, lower limit, and
                upper limit as a tuple or list. For example, if I create a Bernstein
                polynomial of 2nd order, I would provide for this argument,
                    coeffs=[(.2, 0., .5), (.1, 0., 1.), (.1, 0., 1.)]
        """
        coefflist = RooArgList('bern_coeffs')
        if not len(coeff_vals) == (degree+1):
            raise Exception('Length of coeff_vals must be equal to the degrees of freedom.')
        for i in range(0, degree+1):
            if not len(coeff_vals[i]) == 3:
                raise Exception('Each item in coeff_vals must be a tuple or list of length 3.')
            coeff = RooRealVar('bern_c{}_{}'.format(i, suffix), 'bern0', coeff_vals[i][0], coeff_vals[i][1], coeff_vals[i][2])
            self.__varlist.append(coeff)
            coefflist.add(coeff)
        pdfname = 'bern{}_{}'.format(degree, suffix)
        pdf = RooBernstein(pdfname, pdfname, x, coefflist)
        self.__pdflist.append(pdf)
        print ('{}kytools: Created bernstein polynomial of degree {} (d.o.f. = {}) with name {}.{}'.format('\033[0;35m', degree, degree+1, pdfname, '\033[0m'))
        return pdf