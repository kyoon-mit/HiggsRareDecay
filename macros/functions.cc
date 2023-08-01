#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "TROOT.h"
#include "TFile.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TH2Poly.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TSpline.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TLorentzVector.h"
#include "TEfficiency.h"
#include "TVector2.h"
#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/PtEtaPhiM4D.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib> //as stdlib.h      
#include <cstdio>
#include <cmath>
#include <array>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <algorithm>
//#include <boost/algorithm/string/join.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/functional/hash.hpp>
#include <limits>
#include <map>

#include <ROOT/RVec.hxx>
#include <ROOT/RVec.hxx>
#include <ROOT/RDataFrame.hxx>
//#include <ROOT/RDF/RInterface.hxx>

using Vec_b = ROOT::VecOps::RVec<bool>;
using Vec_d = ROOT::VecOps::RVec<double>;
using Vec_f = ROOT::VecOps::RVec<float>;
using Vec_i = ROOT::VecOps::RVec<int>;
using Vec_ui = ROOT::VecOps::RVec<int>;


using stdVec_i = std::vector<int>;
using stdVec_b = std::vector<bool>;
using stdVec_f = std::vector<float>;

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > PtEtaPhiMVector;

bool isKPhiH(const Vec_i& genPart_pdgId, Vec_i& genPart_genPartIdxMother) {
  bool foundMother = false;
  int idxPhifromKPlus = -1;
  int idxPhifromKMinus = -1;
  int idxKPlus = -1;
  int idxKMinus = -1;
  for (int i=genPart_pdgId.size(); i>=0; i--) {
    if (genPart_pdgId[i]==321 &&
        genPart_pdgId[genPart_genPartIdxMother[i]]==333 &&
        genPart_pdgId[genPart_genPartIdxMother[genPart_genPartIdxMother[i]]]==25) {
      foundMother = true;
      idxPhifromKPlus = genPart_genPartIdxMother[i];
      idxKPlus = i;
      break;
    }
  }
  for (int i=genPart_pdgId.size(); i>=0; i--) {
    if (genPart_pdgId[i]==-321 &&
        genPart_pdgId[genPart_genPartIdxMother[i]]==333 &&
        genPart_pdgId[genPart_genPartIdxMother[genPart_genPartIdxMother[i]]]==25) {
      foundMother = true;
      idxPhifromKMinus = genPart_genPartIdxMother[i];
      idxKMinus = i;
      break;
    }
  }
  if (idxPhifromKPlus != idxPhifromKMinus) foundMother = false;
  return foundMother;
}

bool isKPhiPhiH(const Vec_i& genPart_pdgId, Vec_i& genPart_genPartIdxMother) {
  bool foundMother = false;
  int idxPhifromKPlus = -1;
  int idxPhifromKMinus = -1;
  int idxKPlus = -1;
  int idxKMinus = -1;
  for (int i=genPart_pdgId.size(); i>=0; i--) {
    if (genPart_pdgId[i]==321 &&
        genPart_pdgId[genPart_genPartIdxMother[i]]==333 &&
        genPart_pdgId[genPart_genPartIdxMother[genPart_genPartIdxMother[genPart_genPartIdxMother[i]]]]==25) {
      foundMother = true;
      idxPhifromKPlus = genPart_genPartIdxMother[i];
      idxKPlus = i;
      break;
    }
  }
  for (int i=genPart_pdgId.size(); i>=0; i--) {
    if (genPart_pdgId[i]==-321 &&
        genPart_pdgId[genPart_genPartIdxMother[i]]==333 &&
        genPart_pdgId[genPart_genPartIdxMother[genPart_genPartIdxMother[genPart_genPartIdxMother[i]]]]==25) {
      foundMother = true;
      idxPhifromKMinus = genPart_genPartIdxMother[i];
      idxKMinus = i;
      break;
    }
  }
  if (idxPhifromKPlus != idxPhifromKMinus) foundMother = false;
  return foundMother;
}

Vec_d getGenPhiGenKsDitrackCM(const Vec_i& genPart_pdgId,
                              const Vec_i& genPart_genPartIdxMother,
                              const Vec_d& genPart_pt,
                              const Vec_d& genPart_eta,
                              const Vec_d& genPart_phi,
                              const Vec_d& genPart_mass) {
  bool foundMother = false;
  int idxPhifromKPlus = -1;
  int idxPhifromKMinus = -1;
  int idxKPlus = -1;
  int idxKMinus = -1;
  for (int i=genPart_pdgId.size(); i>=0; i--) {
    if (genPart_pdgId[i]==321 &&
        genPart_pdgId[genPart_genPartIdxMother[i]]==333 &&
          (genPart_pdgId[genPart_genPartIdxMother[genPart_genPartIdxMother[i]]]==25 ||
           genPart_pdgId[genPart_genPartIdxMother[genPart_genPartIdxMother[genPart_genPartIdxMother[i]]]]==25)) {
      foundMother = true;
      idxPhifromKPlus = genPart_genPartIdxMother[i];
      idxKPlus = i;
      break;
    }
  }
  for (int i=genPart_pdgId.size(); i>=0; i--) {
    if (genPart_pdgId[i]==-321 &&
        genPart_pdgId[genPart_genPartIdxMother[i]]==333 &&
          (genPart_pdgId[genPart_genPartIdxMother[genPart_genPartIdxMother[i]]]==25 ||
           genPart_pdgId[genPart_genPartIdxMother[genPart_genPartIdxMother[genPart_genPartIdxMother[i]]]]==25)) {
      foundMother = true;
      idxPhifromKMinus = genPart_genPartIdxMother[i];
      idxKMinus = i;
      break;
    }
  }

  Vec_d res(16, 0);
  if (idxPhifromKPlus != idxPhifromKMinus) foundMother = false;
  if (foundMother) {
    res[0] = genPart_pt[idxPhifromKPlus];
    res[1] = genPart_eta[idxPhifromKPlus];
    res[2] = genPart_phi[idxPhifromKPlus];
    res[3] = genPart_mass[idxPhifromKPlus];
    res[4] = genPart_pt[idxKPlus];
    res[5] = genPart_eta[idxKPlus];
    res[6] = genPart_phi[idxKPlus];
    res[7] = genPart_mass[idxKPlus];
    res[8] = genPart_pt[idxKMinus];
    res[9] = genPart_eta[idxKMinus];
    res[10] = genPart_phi[idxKMinus];
    res[11] = genPart_mass[idxKMinus];

    TLorentzVector p_kplus;
    TLorentzVector p_kminus;
    TLorentzVector p_ditrack;
    p_kplus.SetPtEtaPhiM(res[4], res[5], res[6], res[7]);
    p_kminus.SetPtEtaPhiM(res[8], res[9], res[10], res[11]);
    p_ditrack = p_kplus + p_kminus;

    res[12] = p_ditrack.Pt();
    res[13] = p_ditrack.Eta();
    res[14] = p_ditrack.Phi();
    res[15] = p_ditrack.M();
  }
  return res;
}

// float getRhoPolarizationAngle

float getPhiPolarizationAngle(double phi_pt,
                              double phi_eta,
                              double phi_phi,
                              double phi_m,
                              double k_pt,
                              double k_eta,
                              double k_phi,
                              double k_m) {
  TLorentzVector p_phi;
  TLorentzVector p_kplus;
  p_phi.SetPtEtaPhiM(phi_pt, phi_eta, phi_phi, phi_m);
  p_kplus.SetPtEtaPhiM(k_pt, k_eta, k_phi, k_m);
  TVector3 phiBoost = p_phi.BoostVector();
  p_kplus.Boost(-phiBoost);
  float theta = p_phi.Vect().Angle(p_kplus.Vect());

  return theta;
}
#endif