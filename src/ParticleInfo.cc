/**
  @file         ParticleInfo.cc
*/

#include <cassert>
#include <TPRegexp.h>
#include <TArrayI.h>

#include "../interface/ParticleInfo.h"

//_____________________________________________________________________________
bool ParticleInfo::isFinal(const int status) {return status == 1;}
bool ParticleInfo::isIntermediate(const int status) {return status == 2;}
bool ParticleInfo::isHadronization(const int status) {return status >= 71 && status <= 79; }
bool ParticleInfo::isDocIntermediate(const int status){return status == 22;}
bool ParticleInfo::isDocAltered(const int status){return status >= 31 && status < 71;}
bool ParticleInfo::isDocOutgoing(const int status){return status == 23 || status == 24;}
bool ParticleInfo::isIncoming(const int status) {return status > 3 && status <= 21;}
//_____________________________________________________________________________
bool ParticleInfo::isDoc(const int status) {return isDocAltered(status) || isDocOutgoing(status) || isDocIntermediate(status); }
//_____________________________________________________________________________
ParticleInfo::ParticleStatus ParticleInfo::getStatus(const int status)
{
  if(isFinal(status)) return FINAL;
  else if(isIntermediate(status)) return INTERMEDIATE;
  else if(isHadronization(status)) return HADRONIZATION;
  else if(isDocAltered(status)) return DOC_ALTERED;
  else if(isDocIntermediate(status)) return DOC_INTERMEDIATE;
  else if(isDocOutgoing(status)) return DOC_OUTGOING;
  else if(isIncoming(status)) return INCOMING;
  else return UNKNOWN;
}
//_____________________________________________________________________________
bool ParticleInfo::isJetSource(int particleID)
{
  if (particleID < 0)   return isJetSource(-particleID);
  return ((particleID >= p_d && particleID <= p_b) || particleID == p_g || particleID == p_gamma || particleID == p_eminus || particleID == p_tauminus);
}
//_____________________________________________________________________________
bool ParticleInfo::isLightQuark(int particleID)
{
  if (particleID < 0)   return isLightQuark(-particleID);
  return (particleID >= p_d && particleID <= p_s /*p_c*/);
}
//_____________________________________________________________________________
bool ParticleInfo::isHeavyQuark(int particleID)
{
  if (particleID < 0)   return isHeavyQuark(-particleID);
  return (particleID == p_c || particleID == p_b || particleID == p_t);
}
//_____________________________________________________________________________
bool ParticleInfo::isQuark(int pdgId)
{
  if (pdgId < 0)  return isQuark(-pdgId);
  return (p_d <= pdgId && pdgId <= p_t);
}
//_____________________________________________________________________________
bool ParticleInfo::isEWKBoson(int particleID)
{
  if (particleID < 0)   return isEWKBoson(-particleID);
  return (particleID >= p_gamma && particleID <= p_Hplus);
}
//_____________________________________________________________________________
bool ParticleInfo::isHadron(int pdgId)
{
  if (pdgId < 0)        return isHadron(-pdgId);
  return (pdgId >= 100 && pdgId < 60000);
}
//_____________________________________________________________________________
bool ParticleInfo::isQuarkOrGluon(int pdgId)
{
  if (pdgId < 0)  return isQuarkOrGluon(-pdgId);
  return (pdgId == p_g || isQuark(pdgId));
}
//_____________________________________________________________________________
bool ParticleInfo::isDecayProduct(int pdgId)
{
  if (pdgId < 0)                return isDecayProduct(-pdgId);
  if (pdgId >= p_d      && pdgId <= p_b     )   return true;
  if (pdgId >= p_eminus && pdgId <= p_nu_tau)   return true;
//if (pdgId == p_g    )                         return true;
//if (pdgId == p_gamma)                         return true;
  assert(pdgId < p_cluster);
  return false;
}
//_____________________________________________________________________________
bool ParticleInfo::isThirdGeneration(int pdgId)
{
  if (pdgId < 0)  return isThirdGeneration(-pdgId);
  return (  pdgId==p_sbottom1 
         || pdgId==p_stop1
         || pdgId==p_sbottom2
         || pdgId==p_stop2
         || pdgId==p_b
         || pdgId==p_t
         );
}
//_____________________________________________________________________________
bool ParticleInfo::isANeutrino(int particleID)
{
  if (particleID < 0)   return isANeutrino(-particleID);
  return (particleID == p_nu_e || particleID == p_nu_mu || particleID == p_nu_tau);
}
//_____________________________________________________________________________
bool ParticleInfo::isInvisible(int particleID)
{
  if (particleID < 0)   return isInvisible(-particleID);
  return (isLSP(particleID) || isANeutrino(particleID));
}
//_____________________________________________________________________________
bool ParticleInfo::isVisible(int particleID)
{
  if (particleID < 0)   return isVisible(-particleID);
  return (!isLSP(particleID) && !isANeutrino(particleID));
}
//_____________________________________________________________________________
bool ParticleInfo::isLepton(int particleID)
{
  if (particleID < 0)   return isLepton(-particleID);
  return (particleID == p_eminus || particleID == p_muminus || particleID == p_tauminus);
}
//_____________________________________________________________________________
bool ParticleInfo::isLeptonOrNeutrino(int particleID)
{
  if (particleID < 0)   return isLeptonOrNeutrino(-particleID);
  return (p_eminus <= particleID && particleID <= p_nu_tau);
}
//_____________________________________________________________________________
bool ParticleInfo::isPion(int particleID)
{
  if (particleID < 0)   return isPion(-particleID);
  return particleID == p_pi0 || particleID == p_piplus;
}
//_____________________________________________________________________________
bool ParticleInfo::isKaon(int particleID)
{
  if (particleID < 0)   return isKaon(-particleID);
  return  particleID == p_klong
      ||  particleID == p_kshort
      ||  particleID == p_k0
      ||  particleID == p_kplus
      ;
}
//_____________________________________________________________________________
bool ParticleInfo::isBSM(int particleID)
{
  if (particleID < 0)   return isBSM(-particleID);
  if (particleID==p_H0 || particleID==p_A0 || particleID==p_Hplus || particleID==p_H30 || particleID==p_A20)
    return true;
  return particleID >= 1000000;
}
//_____________________________________________________________________________
bool ParticleInfo::isSquark(int pdgId)
{
  if (pdgId < 0)  return isSquark(-pdgId);
  return (p_sdownL <= pdgId && pdgId <= p_stop1) || (p_sdownR <= pdgId && pdgId <= p_stop2);
}
//_____________________________________________________________________________
bool ParticleInfo::isSquarkOrGluino(int pdgId)
{
  if (pdgId < 0)  return isSquarkOrGluino(-pdgId);
  return (pdgId == p_gluino) || (p_sdownL <= pdgId && pdgId <= p_stop1) || (p_sdownR <= pdgId && pdgId <= p_stop2);
}
//_____________________________________________________________________________
bool ParticleInfo::isLSP(int pdgId)
{
  if (pdgId < 0)  return isLSP(-pdgId);
  return (pdgId == p_chi10);
}
//_____________________________________________________________________________
bool ParticleInfo::isHadronizationModel(int particleID)
{
  if (particleID < 0)   return isHadronizationModel(-particleID);
  return (particleID >= 88 && particleID <= 93);
}
//_____________________________________________________________________________
TString ParticleInfo::smPartonName(int particleID, bool separateL, bool separateU, bool separateD, bool separateB, bool separateAnti)
{
  if (particleID < 0)       return smPartonName(-particleID, separateU, separateD, separateB, separateL) + (separateAnti ? "bar" : "");
  switch (particleID) {
    case p_d:               if (separateD)    return "d";
    case p_u:               if (separateU)    return "u";
    case p_s:
    case p_c:               return separateB ? "q" : "j";
    case p_b:               return separateB ? "b" : "j";
    case p_t:               return "t";
    case p_eminus:          if (separateL)    return "e";
    case p_muminus:         if (separateL)    return "mu";
    case p_tauminus:        return separateL ? "tau" : "l";
    case p_nu_e:
    case p_nu_mu:
    case p_nu_tau:          return separateL ? "nu"  : "l";
    case p_g:               return "g";
    case p_gamma:           return "y";
    case p_Z0:              return "Z";
    case p_Wplus:           return "W";
    case p_h0:              
    case p_H0:
    case p_A0:
    case p_Hplus:
    case p_H30:
    case p_A20:             return "h";
    default:                return "x";
  }
}
//_____________________________________________________________________________
TString ParticleInfo::smPartonTitle(int particleID, bool separateL, bool separateU, bool separateD, bool separateB, bool separateAnti)
{
  if (particleID < 0)       return smPartonName(-particleID, separateU, separateD, separateB, separateL) + (separateAnti ? "bar" : "");
  switch (particleID) {
    case p_d:               if (separateD)    return "d";
    case p_u:               if (separateU)    return "u";
    case p_s:
    case p_c:               return separateB ? "q" : "j";
    case p_b:               return separateB ? "b" : "j";
    case p_t:               return "t";
    case p_eminus:          if (separateL)    return "e";
    case p_muminus:         if (separateL)    return "#mu";
    case p_tauminus:        return separateL ? "#tau" : "l";
    case p_nu_e:
    case p_nu_mu:
    case p_nu_tau:          return separateL ? "#nu"  : "l";
    case p_g:               return "g";
    case p_gamma:           return "#gamma";
    case p_Z0:              return "Z";
    case p_Wplus:           return "W";
    case p_h0:              
    case p_H0:
    case p_A0:
    case p_Hplus:
    case p_H30:
    case p_A20:             return "h";
    default:                return "x";
  }
}
//_____________________________________________________________________________
TString ParticleInfo::bsmPartonName(int particleID, bool separateU, bool separateB, bool separateLR)
{
  if (particleID < 0)       return bsmPartonName(-particleID, separateU, separateB, separateLR);
  if (separateLR) {
    if (p_sdownL <= particleID && particleID <= p_stop1)
      return bsmPartonName(particleID, separateU, separateB, false) + "L";
    if (p_sdownR <= particleID && particleID <= p_stop2)
      return bsmPartonName(particleID, separateU, separateB, false) + "R";
  }
  switch (particleID) {
    case p_H0:
    case p_A0:
    case p_Hplus:
    case p_H30:
    case p_A20:             return "H";
    case p_supL:
    case p_supR:            if (separateU)  return "U";
    case p_sdownL:
    case p_sdownR:
    case p_sstrangeL:
    case p_sstrangeR:
    case p_scharmL:
    case p_scharmR:         return separateB ? "Q" : "J";
    case p_sbottom1:
    case p_sbottom2:        return separateB ? "B" : "J";
    case p_stop1:
    case p_stop2:           return "T";
    case p_selectronL:
    case p_smuonL:
    case p_stau1:
    case p_selectronR:
    case p_smuonR:
    case p_stau2:           return "L";
    case p_snu_eL:
    case p_snu_muL:
    case p_snu_tauL: 
    case p_snu_eR:
    case p_snu_muR:
    case p_snu_tauR:        return "V";
    case p_gluino:          return "G";
    case p_chi10:
    case p_chi20:
    case p_chi30:
    case p_chi40:
    case p_chi50:           return "N";
    case p_chi1plus:
    case p_chi2plus:        return "C";
    case p_gravitino:       return "Gr";
    default:                return isBSM(particleID) ? "X" : "x";
  }
}
//_____________________________________________________________________________
TString ParticleInfo::flavorName(int particleID)
{
  if (particleID < 0)   return flavorName(-particleID);
  switch (particleID) {
    case 0:             return "unmatched";
    case p_d:
    case p_u:
    case p_s:
    case p_c:           return "udsc";
    case p_b:           return "b";
    case p_g:           return "g";
    default:            return "unknown";
  }
}
//_____________________________________________________________________________
TString ParticleInfo::shortFlavorName(int particleID)
{
  if (particleID < 0)   return shortFlavorName(-particleID);
  switch (particleID) {
    case 0:             return "x";
    case p_d:
    case p_u:
    case p_s:           return "q";
    case p_c:           return "c";
    case p_b:           return "b";
    case p_g:           return "g";
    default:            return "0";
  }
}
//_____________________________________________________________________________
const TString* ParticleInfo::pfTypeNames()
{
  static const TString  NAMES[] = { "x", "h", "e", "mu", "gamma", "h0", "HFhad", "HFEM" };
  return NAMES;
}
//_____________________________________________________________________________
const TString* ParticleInfo::pfTypeTitles()
{
  static const TString  TITLES[] = { "(?)", "h^{#pm}", "e", "#mu", "#gamma", "h_{0}", "HF_{had}", "HF_{EM}" };
  return TITLES;
}
//_____________________________________________________________________________
TString ParticleInfo::multiply(int count, const char label[])
{
  if (count > 2)      return TString::Format("%d%s", count, label);
  TString             multiplied;
  for (int i = 0; i < count; ++i)   multiplied  += label;
  return multiplied;
}
//_____________________________________________________________________________
TString ParticleInfo::nameFor(int pdgId)
{
  if (pdgId < 0) {
    TString               name  = nameFor(-pdgId);
//    if (charge == 0 && TMath::Abs(pdgId) > p_t) return name;

    if      (name.Contains("Plus"))   name.ReplaceAll("Plus", "Minus");
    else if (name.Contains("Minus"))  name.ReplaceAll("Minus", "Plus");
    else if (isdigit(name[0]))        name  = "-" + name;
    else                              name += "Bar";
    return name;
  }

  switch (pdgId) {
    case p_d:             return "d";
    case p_u:             return "u";
    case p_s:             return "s";
    case p_c:             return "c";
    case p_b:             return "b";
    case p_t:             return "t";
    case p_eminus:        return "eMinus";
    case p_nu_e:          return "nuE";
    case p_muminus:       return "muMinus";
    case p_nu_mu:         return "nuMu";
    case p_tauminus:      return "tauMinus";
    case p_nu_tau:        return "nuTau";
    case p_g:             return "g";
    case p_gamma:         return "gamma";
    case p_Z0:            return "Z";
    case p_Wplus:         return "WPlus";
    case p_h0:            return "h0";
    case p_H0:            return "H0";
    case p_A0:            return "A0";
    case p_Hplus:         return "HPlus";
    case p_G:             return "G";
    case p_R0:            return "R0";
    case p_H30:           return "H30";
    case p_A20:           return "A20";
    case p_LQ:            return "LQ";
    case p_cluster:       return "cluster";
    case p_string:        return "string";
    case p_pi0:           return "pi0";
    case p_rho0:          return "rho0";
    case p_klong:         return "KL0";
    case p_piplus:        return "piPlus";
    case p_eta:           return "eta";
    case p_omega:         return "omega";
    case p_rhoplus:       return "rhoPlus";
    case p_kshort:        return "KS0";
    case p_k0:            return "K0";
    case p_kstar0:        return "KStar0";
    case p_kplus:         return "KPlus";
    case p_kstarplus:     return "KStarPlus";
    case p_phi:           return "phi";
    case p_dplus:         return "DPlus";
    case p_d0:            return "D0";
    case p_dsplus:        return "DsPlus";
    case p_bplus:         return "BPlus";
    case p_b0:            return "B0";
    case p_bs0:           return "Bs0";
    case p_bcplus:        return "BcPlus";
    case p_neutron:       return "n";
    case p_proton:        return "p";
    case p_sigmaminus:    return "SigmaMinus";
    case p_lambda0:       return "Lambda0";
    case p_sigma0:        return "Sigma0";
    case p_sigmaplus:     return "SigmaPlus";
    case p_ximinus:       return "XiMinus";
    case p_xi0:           return "Xi0";
    case p_omegaminus:    return "OmegaMinus";
    case p_sigmac0:       return "Sigmac0";
    case p_lambdacplus:   return "LambdacPlus";
    case p_xic0:          return "Xic0";
    case p_sigmacplus:    return "SigmacPlus";
    case p_sigmacpp:      return "Sigmac^{++}";
    case p_xicplus:       return "XicPlus";
    case p_omegac0:       return "Omegac0";
    case p_sigmabminus:   return "SigmaMinusb";
    case p_lambdab0:      return "Lambdab0";
    case p_xibminus:      return "XibMinus";
    case p_sigmab0:       return "Sigmab0";
    case p_sigmabplus:    return "SigmabPlus";
    case p_xib0:          return "Xib0";
    case p_omegabminus:   return "OmegabMinus";
    case p_sdownL:        return "sdownL";
    case p_supL:          return "supL";
    case p_sstrangeL:     return "sstrangeL";
    case p_scharmL:       return "scharmL";
    case p_sbottom1:      return "sbottom1";
    case p_stop1:         return "stop1";
    case p_selectronL:    return "selectronL";
    case p_snu_eL:        return "snuEL";
    case p_smuonL:        return "smuonL";
    case p_snu_muL:       return "snuMuL";
    case p_stau1:         return "stau1";
    case p_snu_tauL:      return "snuTauL";
    case p_gluino:        return "gluino";
    case p_chi10:         return "chi10";
    case p_chi20:         return "chi20";
    case p_chi1plus:      return "chi1Plus";
    case p_chi30:         return "chi30";
    case p_chi40:         return "chi40";
    case p_chi2plus:      return "chi2Plus";
    case p_gravitino:     return "gravitino";
    case p_chi50:         return "chi50";
    case p_sdownR:        return "sdownR";
    case p_supR:          return "supR";
    case p_sstrangeR:     return "sstrangeR";
    case p_scharmR:       return "scharmR";
    case p_sbottom2:      return "sbottom2";
    case p_stop2:         return "stop2";
    case p_selectronR:    return "selectronR";
    case p_snu_eR:        return "snuER";
    case p_smuonR:        return "smuonR";
    case p_snu_muR:       return "snuMuR";
    case p_stau2:         return "stau2";
    case p_snu_tauR:      return "snuTauR";
    default:              return TString::Format("%d", pdgId);
  }
}
//_____________________________________________________________________________
TString ParticleInfo::titleFor(int pdgId)
{
  if (pdgId < 0) {
    TString           title  = titleFor(-pdgId);
//    if (charge == 0 && TMath::Abs(pdgId) > p_t) return title;

    if      (title.Contains("+"))   title.ReplaceAll("+", "-");
    else if (title.Contains("-"))   title.ReplaceAll("-", "+");
    else if (isdigit(title[0]))     title = "-" + title;
    else                            title = "#bar{" + title + "}";
    return title;
  }

  switch (pdgId) {
    case p_d:             return "d";
    case p_u:             return "u";
    case p_s:             return "s";
    case p_c:             return "c";
    case p_b:             return "b";
    case p_t:             return "t";
    case p_eminus:        return "e^{-}";
    case p_nu_e:          return "#nu_{e}";
    case p_muminus:       return "#mu^{-}";
    case p_nu_mu:         return "#nu_{#mu}";
    case p_tauminus:      return "#tau^{-}";
    case p_nu_tau:        return "#nu_{#tau}";
    case p_g:             return "g";
    case p_gamma:         return "#gamma";
    case p_Z0:            return "Z_{0}";
    case p_Wplus:         return "W^{+}";
    case p_h0:            return "h_{0}";
    case p_H0:            return "H_{0}";
    case p_A0:            return "A_{0}";
    case p_Hplus:         return "H^{+}";
    case p_G:             return "G";
    case p_R0:            return "R_{0}";
    case p_H30:           return "H3_{0}";
    case p_A20:           return "A2_{0}";
    case p_LQ:            return "L_{Q}";
    case p_cluster:       return "*";
    case p_string:        return "~";
    case p_pi0:           return "#pi^{0}";
    case p_rho0:          return "#rho^{0}";
    case p_klong:         return "K_{L}^{0}";
    case p_piplus:        return "#pi^{+}";
    case p_eta:           return "#eta";
    case p_omega:         return "#omega";
    case p_rhoplus:       return "#rho^{+}";
    case p_kshort:        return "K_{S}^{0}";
    case p_k0:            return "K_{0}";
    case p_kstar0:        return "K*_{0}";
    case p_kplus:         return "K^{+}";
    case p_kstarplus:     return "K*^{+}";
    case p_phi:           return "#phi";
    case p_dplus:         return "D^{+}";
    case p_d0:            return "D^{0}";
    case p_dsplus:        return "D_{s}^{+}";
    case p_bplus:         return "B^{+}";
    case p_b0:            return "B^{0}";
    case p_bs0:           return "B_{s}^{0}";
    case p_bcplus:        return "B_{c}^{+}";
    case p_neutron:       return "n";
    case p_proton:        return "p";
    case p_sigmaminus:    return "#Sigma^{-}";
    case p_lambda0:       return "#Lambda^{0}";
    case p_sigma0:        return "#Sigma^{0}";
    case p_sigmaplus:     return "#Sigma^{+}";
    case p_ximinus:       return "#Xi^{-}";
    case p_xi0:           return "#Xi^{0}";
    case p_omegaminus:    return "#Omega^{-}";
    case p_sigmac0:       return "#Sigma_{c}^{0}";
    case p_lambdacplus:   return "#Lambda_{c}^{+}";
    case p_xic0:          return "#Xi_{c}^{0}";
    case p_sigmacplus:    return "#Sigma_{c}^{+}";
    case p_sigmacpp:      return "#Sigma_{c}^{++}";
    case p_xicplus:       return "#Xi_{c}^{+}";
    case p_omegac0:       return "#Omega_{c}^{0}";
    case p_sigmabminus:   return "#Sigma^{-}_{b}";
    case p_lambdab0:      return "#Lambda_{b}^{0}";
    case p_xibminus:      return "#Xi_{b}^{-}";
    case p_sigmab0:       return "#Sigma_{b}^{0}";
    case p_sigmabplus:    return "#Sigma_{b}^{+}";
    case p_xib0:          return "#Xi_{b}^{0}";
    case p_omegabminus:   return "#Omega_{b}^{-}";
    case p_sdownL:        return "#tilde{d}_{L}";
    case p_supL:          return "#tilde{u}_{L}";
    case p_sstrangeL:     return "#tilde{s}_{L}";
    case p_scharmL:       return "#tilde{c}_{L}";
    case p_sbottom1:      return "#tilde{b}_{1}";
    case p_stop1:         return "#tilde{t}_{1}";
    case p_selectronL:    return "#tilde{e}_{L}";
    case p_snu_eL:        return "#tilde{#nu}_{eL}";
    case p_smuonL:        return "#tilde{#mu}_{L}";
    case p_snu_muL:       return "#tilde{#nu}_{#muL}";
    case p_stau1:         return "#tilde{#tau}_{1}";
    case p_snu_tauL:      return "#tilde{#nu}_{#tauL}";
    case p_gluino:        return "#tilde{g}";
    case p_chi10:         return "#chi_{1}^{0}";
    case p_chi20:         return "#chi_{2}^{0}";
    case p_chi1plus:      return "#chi_{1}^{+}";
    case p_chi30:         return "#chi_{3}^{0}";
    case p_chi40:         return "#chi_{4}^{0}";
    case p_chi2plus:      return "#chi_{2}^{+}";
    case p_gravitino:     return "#tilde{G}";
    case p_chi50:         return "#chi_{5}^{0}";
    case p_sdownR:        return "#tilde{d}_{R}";
    case p_supR:          return "#tilde{u}_{R}";
    case p_sstrangeR:     return "#tilde{s}_{R}";
    case p_scharmR:       return "#tilde{c}_{R}";
    case p_sbottom2:      return "#tilde{b}_{2}";
    case p_stop2:         return "#tilde{t}_{2}";
    case p_selectronR:    return "#tilde{e}_{R}";
    case p_snu_eR:        return "#tilde{#nu}_{eR}";
    case p_smuonR:        return "#tilde{#mu}_{R}";
    case p_snu_muR:       return "#tilde{#nu}_{#muR}";
    case p_stau2:         return "#tilde{#tau}_{2}";
    case p_snu_tauR:      return "#tilde{#nu}_{#tauR}";
    default:              return TString::Format("%d", pdgId);
  }
}
//_____________________________________________________________________________
TString ParticleInfo::shortTitleFor(int pdgId, bool ignoreLR)
{
  TString         title = nameFor(TMath::Abs(pdgId));
  if (ignoreLR)   title.ReplaceAll("_{L}","").ReplaceAll("_{R}","");
  return title;
}
//_____________________________________________________________________________
TString ParticleInfo::toMathematica(TString text)
{
  static TPRegexp         getTilde    ("#tilde{([^}]+)}");
  static TPRegexp         getSubSuper ("(.+)_{([^}]+)}\\^{([^}]+)}");
  static TPRegexp         getSuperSub ("(.+)_{([^}]+)}\\^{([^}]+)}");
  static TPRegexp         getSub      ("(.+)_{([^}]+)}");
  static TPRegexp         getSuper    ("(.+)\\^{([^}]+)}");
  static TPRegexp         getGreek    ("#[a-zA-Z]+");

  getTilde    .Substitute(text, "OverscriptBox[$1,\\\"~\\\"]"                 , "g");
  getSubSuper .Substitute(text, "SubsuperscriptBox[$1,\\\"$2\\\",\\\"$3\\\"]" , "g");
  getSuperSub .Substitute(text, "SubsuperscriptBox[$1,\\\"$3\\\",\\\"$2\\\"]" , "g");
  getSub      .Substitute(text, "SubscriptBox[$1,\\\"$2\\\"]"                 , "g");
  getSuper    .Substitute(text, "SuperscriptBox[$1,\\\"$2\\\"]"               , "g");
  TArrayI                 position;
  while (getGreek.Match(text, "g", 0, 10, &position)) {
    TString               inGreek     = text(position[0]+1, position[1]-position[0]-1);
    inGreek[0]            = toupper(inGreek[0]);
    getGreek  .Substitute(text, "\\["+inGreek+"]");
  }

  if (text.Contains("Box[")) {
    text.Prepend("\\!\\(\\*");
    text.Append ("\\)");
  }
  
  return text;
}
//_____________________________________________________________________________
bool ParticleInfo::isHeavyFlavor(HadronType type)
{
  return type > LIGHT_BARYON;
}
//_____________________________________________________________________________
bool ParticleInfo::isBHadron(HadronType type)
{
  return type >= BPLUS;
}
//_____________________________________________________________________________
bool ParticleInfo::isCHadron(HadronType type)
{
  return isHeavyFlavor(type) && !isBHadron(type);
}
//_____________________________________________________________________________
ParticleInfo::HadronType ParticleInfo::typeOfHadron(int pdgId, int* numBQuarks, int* numCQuarks)
{
  if (pdgId < 0)            return typeOfHadron(-pdgId);
  if (!isHadron(pdgId))     return OTHER;

  int                       code        = pdgId;          code -= (code / 10000) * 10000;
  const int                 flavor1     = code / 1000;    code -= flavor1 * 1000;
  const int                 flavor2     = code / 100 ;    code -= flavor2 * 100 ;
  const int                 flavor3     = code / 10  ;

  ////cout << "-----------?  " << pdgId << " = " << flavor1 << " " << flavor2 << " " << flavor3 << endl;

  if (numBQuarks)          *numBQuarks  = (flavor1 == p_b) + (flavor2 == p_b) + (flavor3 == p_b);
  if (numCQuarks)          *numCQuarks  = (flavor1 == p_c) + (flavor2 == p_c) + (flavor3 == p_c);

  if      (!flavor1 + !flavor2 + !flavor3 < 2) {        // at most one digit allowed to be zero
    if    (flavor1) {
      if  (!flavor3)        return DIQUARK;
      if  (flavor1 == p_b)  return B_BARYON;
      if  (flavor1 == p_c)  return C_BARYON;
      if  (isLightQuark(flavor1) && isLightQuark(flavor2) && isLightQuark(flavor3))
                            return LIGHT_BARYON;
    } else {
      if  (flavor2 == p_b)  return B_MESON;
      if  (flavor2 == p_c)  return C_MESON;
      if  (isLightQuark(flavor2) && isLightQuark(flavor3))
                            return LIGHT_MESON;
    }
  }

  return OTHER;
}
//_____________________________________________________________________________
ParticleInfo::HadronType ParticleInfo::detailedTypeOfHadron(int pdgId)
{
  if (pdgId < 0)            return detailedTypeOfHadron(-pdgId);
  if (!isHadron(pdgId))     return OTHER;

  switch (pdgId) {
  //case p_pi0:             return PI0;
  //case p_klong:           return KLONG;
    case p_piplus:          return PIPLUS;
    case p_eta:             return ETA0;
  //case p_k0:              return K0;
  //case p_kplus:           return KPLUS;
    case p_klong:
    case p_k0:
    case p_kplus:           return KAON;
    case p_dplus:           return DPLUS;
    case p_d0:              return D0;
    case p_dsplus:          return DSPLUS;
    case p_bplus:           return BPLUS;
    case p_b0:              return B0;
    case p_bs0:             return BS0;
  //case p_bcplus:          return BCPLUS;
    case p_lambdacplus:     return LAMBDACPLUS;
  //case p_xic0:            return XIC0;
  //case p_xicplus:         return XICPLUS;
  //case p_omegac0:         return OMEGAC0;
    case p_lambdab0:        return LAMBDAB0;
  //case p_xibminus:        return XIBMINUS;
  //case p_xib0:            return XIB0;
  //case p_omegabminus:     return OMEGABMINUS;
  }

  return typeOfHadron(pdgId);
}
//_____________________________________________________________________________
const TString& ParticleInfo::hadronTypeName(HadronType type)
{
  static const TString      NAME[numDetailedHadronTypes]
    = { "other"
      , /*"pi0",*/ "piplus", "eta", "kaon", "diquark", "light_meson", "light_baryon"
      , "Dplus" ,"D0" ,"Dsplus", "Lambdacplus", "c_meson", "c_baryon"
      , "Bplus", "B0", "Bsplus", "Lambdab0", "b_meson", "b_baryon" 
      };
  return NAME[type];
}
//_____________________________________________________________________________
const TString& ParticleInfo::hadronTypeTitle(HadronType type)
{
  static const TString      TITLE[numDetailedHadronTypes] 
    = { "other"
      , /*"#pi^{0}",*/ "#pi^{+}", "#eta", "K_{L}^{0}/K_{0}/K^{+}", "diquark", "light meson", "light baryon"
      , "D^{+}", "D^{0}", "D_{s}^{+}", "#Lambda_{c}^{+}", "c meson", "c baryon"
      , "B^{+}", "B^{0}", "B_{s}^{+}", "#Lambda_{b}^{0}", "b meson", "b baryon" 
      };
  return TITLE[type];
}
//_____________________________________________________________________________
TString ParticleInfo::hadronTypeName(HadronType type, int pdgId)
{
  if (type == OTHER)  return TString::Format("%d", TMath::Abs(pdgId));
  return hadronTypeName(type);
}
//_____________________________________________________________________________
TString ParticleInfo::hadronTypeTitle(HadronType type, int pdgId)
{
  if (type == OTHER)  return TString::Format("%d", TMath::Abs(pdgId));
  return hadronTypeTitle(type);
}
