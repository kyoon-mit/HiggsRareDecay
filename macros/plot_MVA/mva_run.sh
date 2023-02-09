#!/bin/bash

cat=VBFlow

base='/work/submit/kyoon/RareHiggs/test/MVAoutput/2023'
top_dir=$(printf '%(%^b%d)T\n' -1)
work_dir=${base}/${top_dir}

mva_results_rho=${work_dir}/${cat}_rho_mvas
mva_results_phi=${work_dir}/${cat}_phi_mvas

mvafile_rho=${mva_results_rho}/${cat}_rho_mva.root
mvafile_phi=${mva_results_phi}/${cat}_phi_mva.root

mkdir -p $mva_results_rho
mkdir -p $mva_results_phi

thisdir=/work/submit/kyoon/CMSSW_10_2_13/src/HiggsRareDecay

eval "root -l -b -q '${thisdir}/TMVA/TMVA_${cat}.C(\"${mvafile_rho}\", \"rho\")'"
eval "root -l -b -q '${thisdir}/macros/plot_MVA/make_mva_plots.C(\"${mvafile_rho}\")'"
mv dataset/* ${mva_results_rho}

eval "root -l -b -q '${thisdir}/TMVA/TMVA_${cat}.C(\"${mvafile_phi}\", \"phi\")'"
eval "root -l -b -q '${thisdir}/macros/plot_MVA/make_mva_plots.C(\"${mvafile_phi}\")'"
mv dataset/* ${mva_results_phi}
