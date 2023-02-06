#!/bin/bash

batch=${1}
cat=vbflow

base='/work/submit/kyoon/RareHiggs/test/2023'
top_dir=$(printf '%(%^b%d)T\n' -1)
work_dir=${base}/${top_dir}

mkdir $work_dir

varsdir_rho=${work_dir}/${cat}_rho_vars
varsdir_phi=${work_dir}/${cat}_phi_vars

varsfile_rho=${work_dir}/${cat}_rho_vars_batch${batch}.root
varsfile_phi=${work_dir}/${cat}_phi_vars_batch${batch}.root

mkdir -p ${varsdir_rho}
mkdir -p ${varsdir_phi}

eval "root -l -b -q 'make_vars_plots.C(\"${varsfile_rho}\")'"
mv dataset/plots/CorrelationMatrixS.png ${varsdir_rho}/CorrelationMatrixS_${batch}.png
mv dataset/plots/CorrelationMatrixB.png ${varsdir_rho}/CorrelationMatrixB_${batch}.png
mv dataset/plots/variables_id_c1.png    ${varsdir_rho}/variables_id_c$((2*batch-1)).png
mv dataset/plots/variables_id_c2.png    ${varsdir_rho}/variables_id_c$((2*batch)).png

eval "root -l -b -q 'make_vars_plots.C(\"${varsfile_phi}\")'"
mv dataset/plots/CorrelationMatrixS.png ${varsdir_phi}/CorrelationMatrixS_${batch}.png
mv dataset/plots/CorrelationMatrixB.png ${varsdir_phi}/CorrelationMatrixB_${batch}.png
mv dataset/plots/variables_id_c1.png    ${varsdir_phi}/variables_id_c$((2*batch-1)).png
mv dataset/plots/variables_id_c2.png    ${varsdir_phi}/variables_id_c$((2*batch)).png
