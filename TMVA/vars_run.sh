#!/bin/bash

batch=${1}
cat=vbflow

varsdir_rho=${cat}_rho_vars
varsdir_phi=${cat}_phi_vars

varsfile_rho=${cat}_rho_vars_batch${batch}.root
varsfile_phi=${cat}_phi_vars_batch${batch}.root

mkdir -p ${varsdir_rho}
mkdir -p ${varsdir_phi}

#eval "root -l -b -q 'TMVA_VBFlow.C(\"${varsfile_rho}\", \"${meson}\")'"
eval "root -l -b -q 'make_plots.C(\"${varsfile_rho}\")'"
mv dataset/plots/CorrelationMatrixS.png ${varsdir_rho}/CorrelationMatrixS_${batch}.png
mv dataset/plots/CorrelationMatrixB.png ${varsdir_rho}/CorrelationMatrixB_${batch}.png
mv dataset/plots/variables_id_c1.png    ${varsdir_rho}/variables_id_c$((2*batch-1)).png
mv dataset/plots/variables_id_c2.png    ${varsdir_rho}/variables_id_c$((2*batch)).png

#eval "root -l -b -q 'TMVA_VBFlow.C(\"${varsfile_phi}\", \"${meson}\")'"
eval "root -l -b -q 'make_plots.C(\"${varsfile_phi}\")'"
mv dataset/plots/CorrelationMatrixS.png ${varsdir_phi}/CorrelationMatrixS_${batch}.png
mv dataset/plots/CorrelationMatrixB.png ${varsdir_phi}/CorrelationMatrixB_${batch}.png
mv dataset/plots/variables_id_c1.png    ${varsdir_phi}/variables_id_c$((2*batch-1)).png
mv dataset/plots/variables_id_c2.png    ${varsdir_phi}/variables_id_c$((2*batch)).png
