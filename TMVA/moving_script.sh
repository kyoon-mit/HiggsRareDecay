#!/bin/bash

OPTIND=1

while getopts 'd:p:m:' flag
do
    case "${flag}" in
        d) DATE=${OPTARG} ;;
        p) PROD=${OPTARG} ;;
        m) MESON=${OPTARG} ;;
    esac
done

BKGMC=10-15
WGT=w

PFX=/work/submit/kyoon/RareHiggs/Final_BDT_training/${DATE}/${PROD}_${MESON}_MH100-170

mkdir -p ${PFX}/plots
echo ${PFX}/plots
: '
mv dataset/plots/CorrelationMatrixB.png ${PFX}/plots/CorrelationMatrixB_1.png;
mv dataset/plots/CorrelationMatrixS.png ${PFX}/plots/CorrelationMatrixS_1.png;

mv dataset/plots/variables_id_c1.png ${PFX}/plots/variables_id_c1.png;
mv dataset/plots/variables_id_c2.png ${PFX}/plots/variables_id_c2.png;
'

: '
mv dataset/plots/CorrelationMatrixB.png ${PFX}/plots/CorrelationMatrixB_1.png;
mv dataset/plots/CorrelationMatrixS.png ${PFX}/plots/CorrelationMatrixS_1.png;

mv dataset/plots/variables_id_c1.png ${PFX}/plots/variables_id_c1.png;
mv dataset/plots/variables_id_c2.png ${PFX}/plots/variables_id_c2.png;
mv dataset/plots/variables_id_c3.png ${PFX}/plots/variables_id_c3.png;
'
# mv dataset/plots/variables_id_c4.png ${PFX}/plots/variables_id_c4_allvars.png;
# mv dataset/plots/variables_id_c5.png ${PFX}/plots/variables_id_c5_allvars.png;

mv dataset/plots/CorrelationMatrixB.png ${PFX}/plots/CorrelationMatrixB.png;
mv dataset/plots/CorrelationMatrixS.png ${PFX}/plots/CorrelationMatrixS.png;

mv dataset/plots/variables_id_c1.png ${PFX}/plots/variables_id_c1.png;
mv dataset/plots/variables_id_c2.png ${PFX}/plots/variables_id_c2.png;
mv dataset/plots/variables_id_c3.png ${PFX}/plots/variables_id_c3.png;
# mv dataset/plots/variables_id_c4.png ${PFX}/plots_unmodified_variables/variables_id_c8.png;
