import ROOT
import os
from array import array
import math

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptFit(1)
#ROOT.SetBatch()

prod = 'VBFlow'
Meson = 'Rho'
year = '2018'

base = '/work/submit/kyoon/RareHiggs/test/MVAoutput/2023/FEB06'
mytree = ROOT.TChain('events')

spec = {'base': base,
        'cat': '', #
        'Meson': Meson,
        'num': [], #
        'mvadir': '', #
        'prod': prod,
        'year': year}

mclist = []
if prod=='VBFlow' and Meson=='Rho':
   spec['mvadir'] = 'VBFlow_rho_mvas/disc'
   spec['cat'] = 'VBFcatlow'
   # mclist = [-66, -65] + list(range(9,20))
   mclist = [-62, -63, -64]
elif prod=='VBFlow' and Meson=='PhiCat':
   spec['mvadir'] = 'VBFlow_phi_mvas/disc'
   spec['cat'] = 'VBFcatlow'
   mclist = [-62, -63, -64]

for num in mclist:
   spec['num'] = num
   filename = '{base}/{mvadir}/{year}/outname_mc{num}_{cat}_{Meson}Cat_{year}.root'.format(**spec)
   mytree.AddFile(filename)

def projection(doTestMVA, mytree, xmin, xmax, rangeLow, rangeHigh, color ):
   nBins = 10
   
   h = ROOT.TH1F('h', ' ', nBins, xmin, xmax)
   if Meson == "Rho": h.GetXaxis().SetTitle("m_{#gamma#rho}^{H} [GeV]")
   if Meson == "Phi": h.GetXaxis().SetTitle("m_{#gamma#phi}^{H} [GeV]")
   
   for ev in mytree:
      if ev.MVAdisc[0]>rangeLow and ev.MVAdisc[0]<rangeHigh:
         h.Fill( ev.HCandMass , ev.w )
      
   h.SetLineColor(color)
   h.SetLineWidth(3)   
   return h

if __name__ == "__main__":

   hMH_bin1 = projection(1 , mytree, 100., 170., -1., -0.5, 1)
   hMH_bin2 = projection(1 , mytree, 100., 170., -0.5, 0.0, 2)
   hMH_bin3 = projection(1 , mytree, 100., 170., 0.0, 0.5, 3 )
   hMH_bin4 = projection(1 , mytree, 100., 170., 0.5, 1.0, 4 )
#   hMH_bin5 = projection(1 , mytree, 100., 170., 0.75, 1., 6 )

   c = ROOT.TCanvas("c", "", 600, 600)
   
   hMH_bin1.Scale(1./hMH_bin1.Integral())
   hMH_bin1.SetMaximum(2*hMH_bin1.GetMaximum())
   hMH_bin1.Draw("hist")
   hMH_bin2.DrawNormalized("hist sames")
   hMH_bin3.DrawNormalized("hist sames")
   hMH_bin4.DrawNormalized("hist sames")
#   hMH_bin5.DrawNormalized("hist sames")

   text = ROOT.TText()
   text.SetTextFont(1)
   text.SetTextColor(1)
   text.SetTextSize(0.03)
   text.SetTextAlign(22)
   text.SetTextAngle(0)
   
   text.DrawText(140, 0.18, "MVA[-1, -0.5]")
   text.SetTextColor(2)
   text.DrawText(140, 0.20, "MVA[-0.5, 0]")
   text.SetTextColor(3)
   text.DrawText(140, 0.22, "MVA[0, 0.5]")
   text.SetTextColor(4)
   text.DrawText(140, 0.24, "MVA[0.5, 1]")
   text.SetTextColor(6)
#   text.DrawText(140, 0.22, "MVA[0.75,1.]")

   text.SetTextColor(1)   
   text.DrawText(110, 0.24, '{}_{}'.format(prod, Meson))

   c.SaveAs('{base}/{mvadir}/MassHiggs_inBinsOfPT_bkg_{cat}_{Meson}Cat.png'.format(**spec))
