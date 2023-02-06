#include "TMVA/mvas.h"
#include "TMVA/variables.h"
#include "TFile.h"

void make_mva_plots ( const char* inFileName )
{
    TMVA::mvas("dataset", inFileName, TMVA::kCompareType);
    TMVA::efficiencies("dataset", inFileName);
}
