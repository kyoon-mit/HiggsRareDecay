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
shift $((OPTIND-1))

BKGMC=6-10
WGT=w

PFX=/work/submit/kyoon/RareHiggs/test/${DATE}/${PROD}_${MESON}
mkdir -p ${PFX}
# PFX=${PFX}/corr1_${PROD}_${MESON}_bkgmc:${BKGMC}_weight:${WGT}
# PFX=${PFX}/corr_unmodified_variables_${PROD}_${MESON}_bkgmc:${BKGMC}_weight:${WGT}
PFX=${PFX}/training_2of3_events__10_vars__all_mH__${PROD}_${MESON}_bkgmc:${BKGMC}_weight:${WGT}
# PFX=${PFX}/training_all_events_${PROD}_${MESON}_bkgmc:${BKGMC}_weight:${WGT}

CMD="root -l -b -q 'TMVA_${PROD}.C(\"${PFX}.root\", \"${MESON}\")' > ${PFX}.out"
# CMD="root -l -b -q 'TMVA_${PROD}.C(\"${PFX}_1.root\", \"${MESON}\")' > ${PFX}_1.out"
# CMD="root -l -b -q 'TMVA_${PROD}.C(\"${PFX}_2.root\", \"${MESON}\")' > ${PFX}_2.out"

echo "---------------- Evaluating -----------------"
echo "${CMD}"
echo "---------------------------------------------"
eval $CMD
