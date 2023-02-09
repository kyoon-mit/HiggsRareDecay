#!/bin/bash

vbflow_rho_dir=/home/submit/kyoon/public_html/plots/VBFlow_Rho/variables
vbflow_phi_dir=/home/submit/kyoon/public_html/plots/VBFlow_Phi/variables

mkdir -p ${vbflow_rho_dir}
mkdir -p ${vbflow_phi_dir}

cp vbflow_rho_vars/* ${vbflow_rho_dir}
cp vbflow_phi_vars/* ${vbflow_phi_dir}
