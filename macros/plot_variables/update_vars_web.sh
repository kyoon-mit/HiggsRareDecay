#!/bin/bash

vbflow_rho_webdir=/home/submit/kyoon/public_html/plots/VBFlow_Rho/variables
vbflow_phi_webdir=/home/submit/kyoon/public_html/plots/VBFlow_Phi/variables

mkdir -p ${vbflow_rho_dir}
mkdir -p ${vbflow_phi_dir}

base='/work/submit/kyoon/RareHiggs/test/'
top_dir=$(printf '%(%^b%d)T\n' -1)
work_dir=${base}/${top_dir}

cp ${work_dir}/vbflow_rho_vars/* ${vbflow_rho_webdir}
cp ${work_dir}/vbflow_phi_vars/* ${vbflow_phi_webdir}
