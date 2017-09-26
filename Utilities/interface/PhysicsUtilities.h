/*
 * PhysicsUtilities.h
 *
 *  Created on: Aug 20, 2014
 *      Author: nmccoll
 */

#ifndef ANALYSISSUPPORT_PHYSICSUTILITIES_H_
#define ANALYSISSUPPORT_PHYSICSUTILITIES_H_


#include <vector>
#include <map>
#include <TVector2.h>
#include <TMath.h>

#include "Types.h"

namespace PhysicsUtilities{
//_____________________________________________________________________________
// Basic getters
//_____________________________________________________________________________
template<typename Thing>  static const Thing& get(Thing* const& thing)                    { return *thing; }
template<typename Thing>  static const Thing& get(Thing        const& thing)              { return  thing; }
//_____________________________________________________________________________
// Angular properties
//_____________________________________________________________________________
inline double etaToTheta(const double& eta) {return 2.0*TMath::ATan(TMath::Exp(-eta));}
inline double thetaToEta(const double& theta) {return -1*TMath::Log(TMath::Tan(theta/2));}

inline double getEta(const double& eta)   { return eta;       }
template<typename Thing>

inline double getEta(const Thing& obj)    { return obj.eta(); }
template<typename Thing>
inline double absEta(const Thing& obj) { return TMath::Abs(obj.eta()); }
//_____________________________________________________________________________
template<typename Thing1, typename Thing2>
inline double deltaEta(const Thing1& obj1, const Thing2& obj2)    { return (getEta(obj1) - getEta(obj2));    }
template<typename Thing1, typename Thing2>
inline double absDeltaEta(const Thing1& obj1, const Thing2& obj2) { return TMath::Abs(deltaEta(obj1, obj2)); }
//_____________________________________________________________________________
inline double getPhi(const double& phi)   { return phi;       }
inline float  getPhi(const float& phi)   { return phi;       }
inline double getPhi(const TVector2& obj) { return obj.Phi(); }
template<typename Thing>
inline double getPhi(const Thing& obj)    { return obj.phi(); }
//_____________________________________________________________________________
inline double deltaPhi(const float phi1, const float phi2) {return TVector2::Phi_mpi_pi(phi1 - phi2);}
inline double deltaPhi(const double phi1, const double phi2) {return TVector2::Phi_mpi_pi(phi1 - phi2);}
template<typename Thing1, typename Thing2>
inline double deltaPhi(const Thing1& obj1, const Thing2& obj2) { return TVector2::Phi_mpi_pi(getPhi(obj1) - getPhi(obj2)); }
template<typename Thing1, typename Thing2>
inline double absDeltaPhi(const Thing1& obj1, const Thing2& obj2) { return TMath::Abs(deltaPhi(obj1, obj2)); }
//_____________________________________________________________________________
inline double deltaR2(double eta1, double phi1, double eta2, double phi2) {
  double deta = eta1 - eta2;
  double dphi = deltaPhi(phi1, phi2);
  return deta*deta + dphi*dphi;
}
inline double deltaR(double eta1, double phi1, double eta2, double phi2) {
  return std::sqrt(deltaR2 (eta1, phi1, eta2, phi2));
}
template<typename T1, typename T2>
inline double deltaR2(const T1 & t1, const T2 & t2) {
  return deltaR2(t1.eta(), t1.phi(), t2.eta(), t2.phi());
}
template<typename T1, typename T2>
inline double deltaR(const T1 & t1, const T2 & t2) {
  return deltaR(t1.eta(), t1.phi(), t2.eta(), t2.phi());
}

//_____________________________________________________________________________
// Count Objects
//_____________________________________________________________________________
/// Count the number of objects, optionally passing a given test.
template<typename Object, typename ObjectPtr>
ASTypes::size countObjectsDeref(const std::vector<ObjectPtr>& objects, double minPT, double maxEta, bool (*test)(const Object&));

/// Count the number of objects passing the given momentum thresholds.
template<typename ObjectPtr>
ASTypes::size countObjectsDeref(const std::vector<ObjectPtr>& objects, double minPT, double maxEta);

/// Count the number of objects, optionally passing a given test.
template<typename Object>
ASTypes::size countObjects(const std::vector<Object>& objects, double minPT = 0, double maxEta = 9999, bool (*test)(const Object&) = 0);

/// Count the number of objects passing the given test.
template<typename Object, typename Analyzer>
ASTypes::size countObjects(const std::vector<Object>& objects, const Analyzer* analyzer, bool (Analyzer::*test)(const Object&) const);

/// Count the number of objects passing the given test and momentum thresholds.
template<typename Object, typename Analyzer>
ASTypes::size countObjects(const std::vector<Object>& objects, const Analyzer* analyzer, bool (Analyzer::*test)(const Object&) const, double minPT, double maxEta);

/// Count the number of objects, optionally passing a given test.
template<typename Property, typename Object>
ASTypes::size countObjectProperties(const std::vector<Object>& objects, const Property& (Object::*getProperty)() const, double minPT = 0, double maxEta = 9999, bool (*test)(const Property&) = 0);

/// Count the number of objects that are not near a veto set.
template<typename Object, typename Other>
ASTypes::size countDistinctObjects(const std::vector<Object>& objects, const std::vector<Other>& vetoNearby, double minDRnearby, double minPT = 0, double maxEta = 9999);


//_____________________________________________________________________________
// Momentum manipulations
//_____________________________________________________________________________
//Add a set of coordinates to a CartLorentzVector
template<typename CoordSystem, typename Number>
ROOT::Math::LorentzVector<CoordSystem>& addTo(ROOT::Math::LorentzVector<CoordSystem>& p4, Number px, Number py, Number pz, Number E, Number scale = 1){
  p4.SetCoordinates ( p4.px() + scale * px
                    , p4.py() + scale * py
                    , p4.pz() + scale * pz
                    , p4.E () + scale * E
                    );
  return p4;
}


template<typename CoordSystem, typename Momentum>
ROOT::Math::LorentzVector<CoordSystem>& addTo(ROOT::Math::LorentzVector<CoordSystem>& p4, const Momentum& addend, double scale = 1)
{
  p4.SetCoordinates ( p4.px() + scale * addend.px()
                    , p4.py() + scale * addend.py()
                    , p4.pz() + scale * addend.pz()
                    , p4.E () + scale * addend.E ()
                    );
  return p4;
}

//Get the cartLorentzVector after scaling
template<typename CoordSystem, typename Tag>
ROOT::Math::LorentzVector<CoordSystem> getP4(const ROOT::Math::DisplacementVector3D<CoordSystem, Tag>& addend, double scale = 1)
{
  return ROOT::Math::LorentzVector<CoordSystem>
                          (            scale  * addend.x()
                          ,            scale  * addend.y()
                          ,            scale  * addend.z()
                          , TMath::Abs(scale) * addend.r()
                          );
}

/// Sums the 4-momenta of objects
template<typename CoordSystem>
ROOT::Math::LorentzVector<CoordSystem> sumMomenta(const std::vector<ROOT::Math::LorentzVector<CoordSystem> >& p4, double scale = 1)
{
  ROOT::Math::LorentzVector<CoordSystem>   sumP4;
  for (ASTypes::size index = 0; index < p4.size(); ++index)
    addTo(sumP4, p4[index], scale);
  return sumP4;
}


/// Sums the 4-momenta of objects passing the given test and momentum thresholds.
template<typename CoordSystem, typename Object, typename Analyzer>
ROOT::Math::LorentzVector<CoordSystem> sumObjects(const std::vector<Object>& objects, const Analyzer* analyzer, bool (Analyzer::*test)(const Object&) const, double minPT, double maxEta, double scale = 1)
{
  const ASTypes::size            numObjects    = objects.size();
  ROOT::Math::LorentzVector<CoordSystem>     sumP4;
  for (ASTypes::size iObject = 0; iObject < numObjects; ++iObject) {
    const Object&       object        = objects[iObject];
    if (get(object).pt()    < minPT )                   continue;
    if (absEta(get(object)) > maxEta)                   continue;
    if (analyzer && test && !(analyzer->*test)(object)) continue;
    addTo(sumP4, get(object).p4(), scale);
  } // end loop over objects

  return sumP4;
}


//_____________________________________________________________________________
// Jet calculations
//_____________________________________________________________________________
//Count number of adjacent jets
template<typename Jet>
double countProximities(const std::vector<Jet>& jets, const ASTypes::size numJets, ASTypes::size index1, ASTypes::size index2, int& numNearer, int* numFurther = 0);

//Count number of adjacent jets
template<typename Jet>
int countNumNearer(const std::vector<Jet>& jets, const ASTypes::size numJets, ASTypes::size index1, ASTypes::size index2);

//_____________________________________________________________________________
// Basic sorting
//_____________________________________________________________________________
template<typename Field, typename Object>
struct lesserAbsFirst : public std::binary_function<const std::pair<Field,Object>&, const std::pair<Field,Object>&, Bool_t> {
  Bool_t operator()(const std::pair<Field,Object>& x, const std::pair<Field,Object>& y) const {
    return std::abs(x.first) < std::abs(y.first);
  }
};

template<typename Field, typename Object>
struct greaterAbsFirst : public std::binary_function<const std::pair<Field,Object>&, const std::pair<Field,Object>&, Bool_t> {
  Bool_t operator()(const std::pair<Field,Object>& x, const std::pair<Field,Object>& y) const {
    return std::abs(x.first) > std::abs(y.first);
  }
};

template<typename Field, typename Object>
struct greaterFirst : public std::binary_function<const std::pair<Field,Object>&, const std::pair<Field,Object>&, Bool_t> {
  Bool_t operator()(const std::pair<Field,Object>& x, const std::pair<Field,Object>& y) const {
    return x.first > y.first;
  }
};

template<typename Field1, typename Field2, typename Pair = std::pair<Field1,Field2> >
struct greaterFirstOrSecond : public std::binary_function<Pair, Pair, Bool_t> {
  Bool_t operator()(const Pair& t1, const Pair& t2) const
  {
    if (t1.first  > t2.first )        return kTRUE  ;
    if (t1.first  < t2.first )        return kFALSE ;
    if (t1.second > t2.second)        return kTRUE  ;
    if (t1.second < t2.second)        return kFALSE ;
    return kFALSE;
  }
};

template<typename T1, typename T2 = T1>
struct AbsDeltaPhi {
  double operator()(const T1 & t1, const T2 & t2) const {
    return std::abs(deltaPhi(t1.phi(), t2.phi()));
  }
};

template<typename Object>
struct greaterPTDeref : public std::binary_function<const Object*, const Object*, Bool_t> {
  Bool_t operator()(const Object* x, const Object* y) const { return x->pt() > y->pt(); }
};
template<typename Object>
struct greaterEDeref : public std::binary_function<const Object*, const Object*, Bool_t> {
  Bool_t operator()(const Object* x, const Object* y) const { return x->energy() > y->energy(); }
};

template<typename ObjectRef>
struct greaterRefPT : public std::binary_function<const ObjectRef&, const ObjectRef&, Bool_t> {
  Bool_t operator()(const ObjectRef& x, const ObjectRef& y) const { return x->pt() > y->pt(); }
};

template<typename Object>
struct greaterPT : public std::binary_function<const Object&, const Object&, Bool_t> {
  Bool_t operator()(const Object& x, const Object& y) const { return x.pt() > y.pt(); }
};
template<typename T1, typename T2 = T1>
struct DeltaR2 {
  double operator()( const T1 & t1, const T2 & t2 ) const {
    return PhysicsUtilities::deltaR2(t1, t2);
  }
};
template<typename T1, typename T2>
struct DeltaR2Deref {
  double operator()( const T1& t1, const T2& t2 ) const { return PhysicsUtilities::deltaR2(t1, *t2);
  }
};

//_____________________________________________________________________________
// Number conversion
//_____________________________________________________________________________
template<typename Target, typename Source>
Target convertTo(Source source, const char name[], bool lenient = false, bool* good = 0);


//_____________________________________________________________________________
// vector destruction
//_____________________________________________________________________________
template<typename Object>
void trash(std::vector<Object*>& objects);

template<typename Key, typename Object>
void trash(std::map<Key,Object*>& objects);


//_____________________________________________________________________________
// Filters a collection into a vector of pointers
//_____________________________________________________________________________
template<typename Obj, typename Func>
std::vector<const Obj*> selObjs(const std::vector<Obj>& objs, Func test){
    std::vector<const Obj*>  outObjs;
    outObjs.reserve(objs.size());
    for(const auto& obj : objs){
        if(test(&obj) ) outObjs.push_back(&obj);
    }
}
template<typename Obj, typename Func>
std::vector<const Obj*> selObjsD(const std::vector<const Obj*>& objs, Func test){
    std::vector<const Obj*>  outObjs;
    outObjs.reserve(objs.size());
    for(const auto* obj : objs){
        if(test(obj) ) outObjs.push_back(obj);
    }
}

template<typename Obj>
std::vector<const Obj*> selObjsMom(const std::vector<Obj>& objs, const double minPT)
{ return selObjs(objs, [&](const Obj* o){return o->pt()  >= minPT; }  );}
template<typename Obj>
std::vector<const Obj*> selObjsMom(const std::vector<Obj>& objs, const double minPT, const double maxETA)
{ return selObjs(objs, [&](const Obj* o){return o->pt()  >= minPT && std::fabs(o->eta()) < maxETA; });}
template<typename Obj, typename Func>
std::vector<const Obj*> selObjsMom(const std::vector<Obj>& objs, const double minPT, const double maxETA, Func test)
{ return selObjs(objs, [&](const Obj* o){return o->pt()  >= minPT && std::fabs(o->eta()) < maxETA && test(o); });}

template<typename Obj>
std::vector<const Obj*> selObjsMomD(const std::vector<const Obj*>& objs, const double minPT)
{ return selObjsD(objs, [&](const Obj* o){return o->pt()  >= minPT; }  );}
template<typename Obj>
std::vector<const Obj*> selObjsMomD(const std::vector<const Obj*>& objs, const double minPT, const double maxETA)
{ return selObjsD(objs, [&](const Obj* o){return o->pt()  >= minPT && std::fabs(o->eta()) < maxETA; });}
template<typename Obj, typename Func>
std::vector<const Obj*> selObjsMomD(const std::vector<const Obj*>& objs, const double minPT, const double maxETA, Func test)
{ return selObjsD(objs, [&](const Obj* o){return o->pt()  >= minPT && std::fabs(o->eta()) < maxETA && test(o); });}


//_____________________________________________________________________________
// Finding objects in a group
//_____________________________________________________________________________
/// Locates the given item in the list.
template<typename Object1, typename Object2>
int findIndex(const Object1* target, const std::vector<Object2>& items, ASTypes::size first = 0, ASTypes::size last = 0);

//_____________________________________________________________________________
template<typename Thing1, typename Thing2, typename Comparator, typename Distance>
int findByDistance(Comparator comparator, Distance ruler, const Thing1& reference, const std::vector<Thing2>& objects,
                          double& bestDistance, double maxDistance, const double minPT = 0, const std::vector<bool>* vetoed = 0, int numObjects = -1);

template<typename Thing1, typename Thing2, typename Comparator, typename Distance>
int findByDistance(Comparator comparator, Distance ruler, const Thing1& reference, const Thing2& objects,
                          double& bestDistance, double maxDistance, const double minPT = 0, const std::vector<bool>* vetoed = 0, int numObjects = -1);

//_____________________________________________________________________________
template<typename Thing1, typename Thing2>
int findNearestDR(const Thing1& reference, const std::vector<Thing2>& objects, double& nearestDR, double maxDeltaR = 1e308,
                         const double minPT = 0, const std::vector<bool>* vetoed = 0, int numObjects = -1);

template<typename Thing1, typename Thing2>
int findNearestDRDeref(const Thing1& reference, const std::vector<Thing2>& objects, double& nearestDR, double maxDeltaR = 1e308,
                              const double minPT = 0, const std::vector<bool>* vetoed = 0, int numObjects = -1);

template<typename Thing1, typename Thing2>
int findNearestDPhi(const Thing1& reference, const std::vector<Thing2>& objects, double& nearestDPhi, double maxDeltaPhi = 1e308,
                           const double minPT = 0, const std::vector<bool>* vetoed = 0, int numObjects = -1);

template<typename T1, typename T2>
struct AbsDeltaPhiDeref { double operator()( const T1& t1, const T2& t2 ) const { return absDeltaPhi(t1, *t2); } };
template<typename Thing1, typename Thing2>
int findNearestDPhiDeref(const Thing1& reference, const std::vector<Thing2>& objects, double& nearestDPhi, double maxDeltaPhi = 1e308,
                                const double minPT = 0, const std::vector<bool>* vetoed = 0, int numObjects = -1);


//_____________________________________________________________________________
template<typename Thing1, typename Thing2>
int findFurthestDR(const Thing1& reference, const std::vector<Thing2>& objects, double& furthestDR, double minDeltaR = -1e308,
                          const double minPT = 0, const std::vector<bool>* vetoed = 0, int numObjects = -1);

template<typename Thing1, typename Thing2>
int findFurthestDPhi(const Thing1& reference, const std::vector<Thing2>& objects, double& furthestDR, double minDeltaR = -1e308,
                            const double minPT = 0, const std::vector<bool>* vetoed = 0, int numObjects = -1);


//_____________________________________________________________________________
template<typename Thing1, typename Thing2>
double getNearestDR(const Thing1& reference, const std::vector<Thing2>& objects, int thisIndex = -1, const double minPT = 0);

//_____________________________________________________________________________
template<typename Number>
int sign(Number x) { return (x < 0 ? -1 : 1); }

//_____________________________________________________________________________
template<class T1, class T2>
std::vector<T1> removeOverlapsDR(const std::vector<T1>& objToRemove, const std::vector<T2> &objToKeep, double distance);

template<class T1, class T2>
std::vector<T1> removeOverlapsDRDeref(const std::vector<T1>& ptrToRemove, const std::vector<T2> &ptrToKeep, double distance);

};


#include "../src/PhysicsUtilities.icc"

#endif /* PHYSICSUTILITIES_H_ */
