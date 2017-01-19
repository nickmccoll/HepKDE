/**
  @file         ParticleInfo.cc
*/

#include <cassert>
#include <TPRegexp.h>
#include <TArrayI.h>

#include "../interface/ParticleUtilities.h"

//_____________________________________________________________________________
bool ParticleUtilities::findBosonDaughters(const std::vector<reco::GenParticle> &genParticles, const reco::GenParticle* &boson, const reco::GenParticle* &dau1, const reco::GenParticle* &dau2)
{
  for(const reco::GenParticle &p : genParticles) {
    const reco::Candidate* mom = p.mother(0);
    if(mom==nullptr) continue;               // not filled
    if(!isAncestor(boson, mom)) continue;    // not related
    if(p.pdgId()==boson->pdgId()) continue;  // skip the boson
    if(p.status()!=23 && p.status()!=2 && p.status()!=1) continue;
    if(!dau1) dau1 = &p;                     // fill daughter 1
    else if(!dau2) dau2 = &p;                // fill daughter 2
    else break;
  }

  if(dau1 && dau2) return true;

  // if didn't find daughters for input (status 22) boson, look for daughters of boson versions with higher status codes
  std::vector<const reco::GenParticle*> outbosons;
  for(const reco::GenParticle &p : genParticles) {
    if(p.pdgId()==boson->pdgId() && p.status()!=boson->status() && p.mass()==boson->mass()) outbosons.push_back(&p);
  }

  for(size_t i = 0; i < outbosons.size(); ++i) {
    dau1 = 0;
    dau2 = 0;
    if(findBosonDaughters(genParticles, outbosons[i], dau1, dau2)) {
      boson = outbosons[i];
      return true;
    }
  }

  // no luck!
  return false;
}
//_____________________________________________________________________________
void ParticleUtilities::findTauDaughter(const std::vector<reco::GenParticle> &genParticles, const reco::GenParticle* &tau, const reco::GenParticle* &dau)
{
  for(const reco::GenParticle &p : genParticles) {
    const reco::Candidate* mom = p.mother(0);
    if(mom==nullptr) continue;             // not filled
    if(!isAncestor(tau, mom)) continue;    // not related
    if(p.status()!=2 && p.status()!=1) continue;
    if(isA(ParticleInfo::p_tauminus,&p) || isA(ParticleInfo::p_nu_tau,&p) || isA(ParticleInfo::p_Wplus, &p) || isA(ParticleInfo::p_gamma,&p)) continue;    // get the 1st lepton/hadron from the tau decay, status 2 should imply a decayed hadron in this case
    if(!dau) {
      dau = &p;                            // fill daughter
      break;
    }
  }
}

