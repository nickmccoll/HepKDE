#ifndef ANALYSISSUPPORT_TYPES_H
#define ANALYSISSUPPORT_TYPES_H

#include<iostream>
#include<string>

#include <Math/PtEtaPhiE4D.h>
#include <Math/PtEtaPhiM4D.h>
#include <Math/LorentzVector.h>

namespace ASTypes {
typedef   char                   int8  ;
typedef   short int              int16 ;
typedef   unsigned char          size8 ; // 8 bit 0->255
typedef   unsigned short int     size16; //16 bit 0->65,536
typedef   unsigned int           size  ; //32 bit 0->4,294,967,296
typedef   unsigned long long int size64; //64 bit 0->18,446,744,073,709,551,615


typedef ROOT::Math::PtEtaPhiM4D<double>				                CylLorentzCoord;
typedef ROOT::Math::PtEtaPhiM4D<float> 				                CylLorentzCoordF;
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> >	XYZTLorentzVectorF;
typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> >	PtEtaPhiMLorentzVectorD;
typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> >	PtEtaPhiMLorentzVectorF;
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> >	CartLorentzVector;
typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> >	CylLorentzVector;
typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> >	CylLorentzVectorF;

template<typename Target, typename Source>
Target convertTo(Source source, const char name[], bool lenient = false, bool* good = 0)
{
  Target            converted = static_cast<Target>(source);

  if (static_cast<Source>(converted) != source) {
    const Target    lowest    = !std::numeric_limits<Target>::is_signed
                              ? 0
                              : std::numeric_limits<Target>::has_infinity
                              ? -std::numeric_limits<Target>::infinity()
                              :  std::numeric_limits<Target>::min()
                              ;

    std::string problem = "convertTo(): Source value " + std::to_string((double)  source) + " outside of target range "
                         +"["+std::to_string((double)  lowest)+","+std::to_string((double)  std::numeric_limits<Target>::max())+"]"
                         + " for '" + name +"'";

    if (good)      *good      = false;
    if (lenient) {
      std::cerr << "WARNING: " << problem << std::endl;
      return  ( source > static_cast<Source>(std::numeric_limits<Target>::max())
              ? std::numeric_limits<Target>::max()
              : lowest
              );
    }
    throw std::range_error( problem);
  }

  return converted;
}


};

#endif
