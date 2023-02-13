#include "TMVA/correlations.h"
#include "TMVA/variables.h"
#include "TFile.h"

void make_vars_plots ( const char* inFileName )
{
    TMVA::correlations("dataset", inFileName);
    TMVA::variables("dataset", inFileName);
}
