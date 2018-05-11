#ifndef ANALYSISSUPPORT_TYPES_H
#define ANALYSISSUPPORT_TYPES_H

#include<iostream>
#include<string>
#include <iomanip>
#include <sstream>

#include <Math/PtEtaPhiE4D.h>
#include <Math/PtEtaPhiM4D.h>
#include <Math/LorentzVector.h>

namespace ASTypes {
typedef   char                   int8  ;
typedef   short int              int16 ;
typedef   unsigned char          size8 ; // 8 bit 0->255
typedef   unsigned short int     size16; //16 bit 0->65,536
typedef   unsigned int           size  ; //32 bit 0->4,294,967,296
typedef   unsigned long long     size64; //64 bit 0->18,446,744,073,709,551,615


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


//Class to contain a value and error
template <typename ContType>
class ValAndErr {
public:
    ValAndErr(): val_(0), err_(0) {};
    ValAndErr(ContType val,ContType err ) : val_(val), err_(err) {};
    void set(ContType val,ContType err) { val_ = val; err_ = err;}
    ContType val() const {return val_;}
    ContType err() const {return err_;}
private:
    ContType val_;
    ContType err_;
};
typedef ValAndErr<float> ValAndErrF;

template <typename ContType>
class ValAndAssymErr {
public:
    ValAndAssymErr(): val_(0), errup_(0),errdown_(0) {};
    ValAndAssymErr(ContType val,ContType errup,ContType errdown ) : val_(val), errup_(errup), errdown_(errdown) {};
    void set(ContType val,ContType errup,ContType errdown) { val_ = val; errup_ = errup; errdown_ = errdown;}
    ContType val() const {return val_;}
    ContType err() const {return (errup_+errdown_)/2.0;}
    ContType errup() const {return errup_;}
    ContType errdown() const {return errdown_;}
private:
    ContType val_;
    ContType errup_;
    ContType errdown_;
};
typedef ValAndAssymErr<float> ValAndAssymErrF;


//Conversion
inline std::string flt2Str(double val, int pre = -1){
std::stringstream stream;
if(pre > -1) stream << std::fixed << std::setprecision(pre) << val;
else stream << val;
return stream.str();
}
inline std::string int2Str(int val){
std::stringstream stream;
stream << val;
return stream.str();
}

template<typename CompType>
inline bool strFind(const std::string& checkStr, const CompType& subStrToFind){
    return checkStr.find(subStrToFind) != std::string::npos;
}



};

#endif
