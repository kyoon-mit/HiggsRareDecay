#!/bin/bash

while getopts 'd:p:m:' flag
do
    case "${flag}" in
        d) DATE=${OPTARG} ;;
        p) PROD=${OPTARG} ;;
        m) MESON=${OPTARG} ;;
    esac
done
shift $((OPTIND-1))

BKGMC=6-9
WGT=w

PFX=/work/submit/kyoon/RareHiggs/test/${DATE}/${PROD}_${MESON}

mkdir ${PFX}/plots_unmodified_variables;

: '
mv dataset/plots/CorrelationMatrixB.png ${PFX}/plots/CorrelationMatrixB_2.png;
mv dataset/plots/CorrelationMatrixS.png ${PFX}/plots/CorrelationMatrixS_2.png;

mv dataset/plots/variables_id_c1.png ${PFX}/plots/variables_id_c4.png;
mv dataset/plots/variables_id_c2.png ${PFX}/plots/variables_id_c5.png;
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

mv dataset/plots/CorrelationMatrixB.png ${PFX}/plots_unmodified_variables/CorrelationMatrixB_2.png;
mv dataset/plots/CorrelationMatrixS.png ${PFX}/plots_unmodified_variables/CorrelationMatrixS_2.png;

mv dataset/plots/variables_id_c1.png ${PFX}/plots_unmodified_variables/variables_id_c5.png;
mv dataset/plots/variables_id_c2.png ${PFX}/plots_unmodified_variables/variables_id_c6.png;
mv dataset/plots/variables_id_c3.png ${PFX}/plots_unmodified_variables/variables_id_c7.png;
mv dataset/plots/variables_id_c4.png ${PFX}/plots_unmodified_variables/variables_id_c8.png;
