#!/bin/bash

rho_webdir=/home/submit/kyoon/public_html/plots/${PROD}_Rho/variables
phi_webdir=/home/submit/kyoon/public_html/plots/${PROD}_Phi/variables

mkdir -p ${rho_webdir}
mkdir -p ${phi_webdir}

base='/work/submit/kyoon/RareHiggs/test/MVAoutput/2023'
top_dir=$(printf '%(%^b%d)T\n' -1)
work_dir=${base}/${top_dir}

cp ${work_dir}/${PROD}_rho_vars/* ${rho_webdir}
cp ${work_dir}/${PROD}_phi_vars/* ${phi_webdir}
