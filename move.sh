mes=rho
prod=ggH
MES=Rho
PROD=GF
for mc in {6,7,8,9,10,1027}
do
mv /work/submit/kyoon/RareHiggs/data/cat_${mes}/cat_${mes}_${prod}/test/test_mc${mc}_${PROD}cat_${MES}Cat_2018.root /work/submit/kyoon/RareHiggs/data/cat_${mes}/cat_${mes}_${prod}/test/test_mc${mc}_${prod}cat_${mes}cat_2018.root
done
