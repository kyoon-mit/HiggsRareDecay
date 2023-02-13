#!/bin/bash

PROD=VBF

rho_webdir=/home/submit/kyoon/public_html/plots/${PROD}_Rho/mva
phi_webdir=/home/submit/kyoon/public_html/plots/${PROD}_Phi/mva

mkdir -p ${rho_webdir}
mkdir -p ${phi_webdir}

base='/work/submit/kyoon/RareHiggs/test/MVAoutput/2023'
top_dir=$(printf '%(%^b%d)T\n' -1)
work_dir=${base}/${top_dir}

cp ${work_dir}/${PROD}_rho_mvas/plots/*png ${rho_webdir}
cp ${work_dir}/${PROD}_phi_mvas/plots/*png ${phi_webdir}
