from ROOT import *
from warnings import warn
import argparse

def getConfusionMatrix(testTree, cut):
    """Gets the 2x2 confusion matrix.

    Args:
        testTree (TTree): TTree of the BDT response of the test events.
        cut (float): cut value of the BDT

    Returns:
        list(float): List of the confusion matrix entries whose indices correspond to 
        [TP, FP, FN, TN].
    """
    confusionMatrixEntries = [0, 0, 0, 0]

    for event in testTree:
        # 0 for 'Signal', 1 for 'Background'
        if event.BDTG >= cut: # Predicted Positive
            if event.className == 'Signal': # Actual Positive
                confusionMatrixEntries[0] += 1 # True Positive
            elif event.className == 'Background': # Actual Negative
                confusionMatrixEntries[1] += 1 # False Positive
            else:
                warn('Event does not fall into either category.', stacklevel=2)
        else:
            if event.className == 'Signal': # Actual Positive
                confusionMatrixEntries[2] += 1 # False Negative
            elif event.className == 'Background': # Actual Negative
                confusionMatrixEntries[3] += 1 # True Negative
            else:
                warn('Event does not fall into either category.', stacklevel=2)

    return confusionMatrixEntries

def getBDTResponse(fileName):
    """Gets useful response from the BDT.

    Args:
        fileName (str): Name of the ROOT file containing the BDT response.
    
    Returns:

    """
    BDTFile = TFile.Open(fileName, 'READ')
    testTree = BDTFile.Get('dataset/TestTree')
    cuts = [round(i*0.1 - 1., 1) for i in range(0, 21)] # [-1.0, -0.9, ... , 0.9, 1.0]
    print('Cut\t||\tTP \tFP \tFN \tTN \tTPR \t TNR \tPPV \tNPV \tACC \tF1')
    for cut in cuts:
        conf = getConfusionMatrix(testTree, cut)
        TP, FP, FN, TN = conf[0], conf[1], conf[2], conf[3]
        try: TPR = round(TP/(TP+FN), 2) # True positive rate, i.e. Recall
        except ZeroDivisionError: TPR = float('Inf')
        try: TNR = round(TN/(TN+FP), 2) # True negative rate
        except ZeroDivisionError: TNR = float('Inf')
        try: PPV = round(TP/(TP+FP), 2) # Positive Predictive Value, i.e. Precision
        except ZeroDivisionError: PPV = float('Inf')
        try: NPV = round(TN/(TN+FN), 2) # Negative Predictive Value
        except ZeroDivisionError: NPV = float('Inf')
        try: ACC = round((TP+TN)/(TP+FP+FN+TN), 2) # Accuracy
        except ZeroDivisionError: ACC = float('Inf')
        try: F1  = round(2*PPV*TPR/(PPV+TPR), 2) # F1 Score
        except ZeroDivisionError: F1 = float('Inf')
        print('{}\t||\t{} \t{} \t{} \t{} \t{} \t{} \t{} \t{} \t{} \t{}'.format(cut, TP, FP, FN, TN, TPR, TNR, PPV, NPV, ACC, F1))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', type=str, required=True)
    args = parser.parse_args()
    getBDTResponse(args.f)
