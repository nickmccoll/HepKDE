#ifndef ANALYSISSUPPORT_TreeWrapperDATA_H
#define ANALYSISSUPPORT_TreeWrapperDATA_H

#include <string>
#include <vector>
#include <assert.h>
#include <typeinfo>
#include <typeindex>
#include <map>
#include <algorithm>

#include "TreeWrapper.h"


//_____________________________________________________________________________
//_____________________________________________________________________________

//imported from:
//https://github.com/cms-sw/cmssw/blob/02d4198c0b6615287fd88e9a8ff650aea994412e/DataFormats/NanoAOD/interface/FlatTable.h
//https://github.com/cms-sw/cmssw/blob/02d4198c0b6615287fd88e9a8ff650aea994412e/DataFormats/Math/interface/libminifloat.h
namespace floatCompressor {
//_____________________________________________________________________________

inline float reduceMantissaToNbitsRounding(const float f, const int bits)  {
    const int shift =(23-bits);
    const uint32_t mask = ((0xFFFFFFFF >> (shift)) << (shift));
    const uint32_t test = (1 << (shift-1));
    const uint32_t maxn = ((1<<bits)-2);
    assert(bits <= 23); // "max mantissa size is 23 bits"

    constexpr uint32_t low23 = (0x007FFFFF); // mask to keep lowest 23 bits = mantissa
    constexpr uint32_t  hi9  = (0xFF800000); // mask to keep highest 9 bits = the rest
    union { float flt; uint32_t i32; } conv;
    conv.flt=f;
    if (conv.i32 & test) { // need to round
        uint32_t mantissa = (conv.i32 & low23) >> shift;
        if (mantissa < maxn) mantissa++;
        conv.i32 = (conv.i32 & hi9) | (mantissa << shift);
    } else {
        conv.i32 &= mask;
    }
    return conv.flt;
}
//_____________________________________________________________________________
inline float maybeMantissaReduce(const float& val, const int bits) {return bits < 1 ? val : reduceMantissaToNbitsRounding(val,bits);}
inline  void maybeMantissaReduceVector(std::vector<float>& vals, const int bits) {
    if(bits>0)  std::transform(vals.begin(), vals.end(), vals.begin(),[&](const float f){return reduceMantissaToNbitsRounding(f,bits); });
}

//template<typename T> T maybeMantissaReduce(const T& val, int bits) {return val;}
//template<> float maybeMantissaReduce<float>(const float& val, const int bits) {return bits < 1 ? val : reduceMantissaToNbitsRounding(val,bits);}
//
////_____________________________________________________________________________
//
//template<typename T> void maybeMantissaReduceVector(std::vector<T>& vals, const int bits) {
//    return;
//}
//template<> void maybeMantissaReduceVector<float>(std::vector<float>& vals, const int bits) {
//    if(bits>0)  std::transform(vals.begin(), vals.end(), vals.begin(),[&](const float f){return reduceMantissaToNbitsRounding(f,bits); });
//}
}
//_____________________________________________________________________________
//_____________________________________________________________________________


//_____________________________________________________________________________
class AbstractTreeVar {
public:
    AbstractTreeVar(const std::string& prefix_, const std::string& name_) : prefix(prefix_), name(name_) {};
    virtual ~AbstractTreeVar() {};
    virtual void book(TreeWrapper * tw) = 0;
    virtual void reset() = 0;
    virtual void processForTree() = 0;
    std::string bookName() const {return prefix == "" ? name : (prefix + "_" + name); }
    const std::string prefix;
    const std::string name;
};


//_____________________________________________________________________________
template<typename Type>
class TreeVar : public AbstractTreeVar {
public:
    TreeVar( Type& value_, const std::string& prefix_, const std::string& name_, const Type defaultValue_)
: AbstractTreeVar(prefix_,name_), defaultValue(defaultValue_), value(value_)  { }
    virtual void processForTree() {}
    virtual void reset() { value = defaultValue;}
    virtual void book(TreeWrapper * tw) {tw->book(bookName().c_str(),value);}
protected:
    const Type        defaultValue;
    Type &            value;
};

//_____________________________________________________________________________
class CompressibleTreeVar : public TreeVar<float> {
public:
    CompressibleTreeVar( float& value_, const std::string& prefix_, const std::string& name_, const float defaultValue_,  const int bitComp_ =-1)
: TreeVar<float>(value_,prefix_,name_,defaultValue_), bitComp(bitComp_)  { }
    virtual void processForTree() {value = floatCompressor::maybeMantissaReduce(value,bitComp);}
protected:
    const int bitComp;

};

//_____________________________________________________________________________
template<typename Type>
class TreeVector : public AbstractTreeVar {
public:
    TreeVector(std::shared_ptr<std::vector<Type>> value_, const std::string& prefix_,const std::string& sizeName_,
            const std::string& name_,const std::string& type_)
: AbstractTreeVar(prefix_,name_),value(value_),
  sizeName(sizeName_),branch(0), type(type_)
{ }
    virtual void processForTree() {
        branch->SetAddress(value->size() ?  &((*value)[0]) : 0);
    }
    virtual void reset() { value->clear();}
    virtual void book(TreeWrapper * tw) {branch = tw->bookArray<Type>(bookName().c_str(),(Type*)(0), type.c_str(),sizeVarName().c_str() );}
    virtual std::string sizeVarName() const {return prefix == "" ? name : (prefix + "_" + sizeName); }
protected:
    std::shared_ptr<std::vector<Type>> value;
    const std::string sizeName;
    TBranch * branch;
    const std::string type;

};
//_____________________________________________________________________________
class CompressibleTreeVector : public TreeVector<float> {
public:
    CompressibleTreeVector(std::shared_ptr<std::vector<float>> value_, const std::string& prefix_,const std::string& sizeName_,
            const std::string& name_,const std::string& type_,  const int bitComp_ =-1)
: TreeVector<float>(value_,prefix_,sizeName_,name_,type_),bitComp(bitComp_)
  { }
    void processForTree() {
        floatCompressor::maybeMantissaReduceVector(*value,bitComp);
        TreeVector::processForTree();
    }
protected:
    const int bitComp;

};

//_____________________________________________________________________________
template<typename Type>
class TreeSizeVar : public TreeVar<unsigned int> {
public:
    TreeSizeVar( unsigned int& value_, const std::string& name, const std::shared_ptr<std::vector<Type>>  vectorVar)
: TreeVar<unsigned int>(value_,"",name,0), vectorVar(vectorVar) { }
    virtual void processForTree() {value = vectorVar->size();}
protected:
    const std::shared_ptr<std::vector<Type>>  vectorVar;
};


//_____________________________________________________________________________
//_____________________________________________________________________________


class TreeWriterData{
public:
    TreeWriterData() : isBooked(false)
{
        ttreeTypes[std::type_index(typeid(char))] = "B";// : an 8 bit signed integer (Char_t)
        ttreeTypes[std::type_index(typeid(unsigned char))] = "b";// : an 8 bit unsigned integer (UChar_t)
        ttreeTypes[std::type_index(typeid(short int ))] = "S";// : a 16 bit signed integer (Short_t)
        ttreeTypes[std::type_index(typeid(unsigned short int))] = "s";// : a 16 bit unsigned integer (UShort_t)
        ttreeTypes[std::type_index(typeid(int))] = "I";// : a 32 bit signed integer (Int_t)
        ttreeTypes[std::type_index(typeid(unsigned int))] = "i";// : a 32 bit unsigned integer (UInt_t)
        ttreeTypes[std::type_index(typeid(float))] = "F";// : a 32 bit floating point (Float_t)
        ttreeTypes[std::type_index(typeid(double))] = "D";// : a 64 bit floating point (Double_t)
        ttreeTypes[std::type_index(typeid(long long))] = "L";// : a 64 bit signed integer (Long64_t)
        ttreeTypes[std::type_index(typeid(unsigned long long))] = "l";// : a 64 bit unsigned integer (ULong64_t)
        ttreeTypes[std::type_index(typeid(bool))] = "O";// : [the letter o, not a zero] a boolean (Bool_t)

};
    virtual ~TreeWriterData() { for(auto d : data) delete d; for(auto * d: groupSizes) delete d; };

    //_____________________________________________________________________________
    template<typename Type>
    void addSingle(Type& value, const std::string prefix, const std::string& name, const int bitComp=-1 ){
        assert(!isBooked);
        if(bitComp){};//for comp warning
        data.push_back(new TreeVar<Type>(value,prefix,name,value));
    }

    //_____________________________________________________________________________

    void addSingle(float& value, const std::string prefix, const std::string& name, const int bitComp=-1 ){
        assert(!isBooked);
        data.push_back(new CompressibleTreeVar(value,prefix,name,value,bitComp));
    }


    //_____________________________________________________________________________
    template<typename Type>
    void addVector(std::shared_ptr<std::vector<Type>> value, const std::string prefix, const std::string& sizeName, const std::string& name,const int bitComp=-1 ){
        assert(!isBooked);
        if(bitComp){};//for comp warning
        auto tttype = ttreeTypes.find(std::type_index(typeid(Type)));
        assert(tttype != ttreeTypes.end());

        auto * newVar = new TreeVector<Type>(value,prefix,sizeName,name,tttype->second);
        addSize(newVar,value);
        data.push_back(newVar);
    }
    //_____________________________________________________________________________
    void addVector(std::shared_ptr<std::vector<float>> value, const std::string prefix, const std::string& sizeName, const std::string& name,const int bitComp=-1 ){
        assert(!isBooked);
        auto tttype = ttreeTypes.find(std::type_index(typeid(float)));
        assert(tttype != ttreeTypes.end());

        auto * newVar = new CompressibleTreeVector(value,prefix,sizeName,name,tttype->second,bitComp);
        addSize(newVar,value);
        data.push_back(newVar);
    }

    //_____________________________________________________________________________
    void book(TreeWrapper * tw){
        for(auto d : data) d->book(tw);
        isBooked = true;
    }
    void processForTree() {for(auto d : data) d->processForTree();}
    void reset(){ for(auto d : data) d->reset();}
protected:
    template<typename Type>

    void addSize(TreeVector<Type>* newVar,std::shared_ptr<std::vector<Type>> value) {
        const auto sizeVarName =  newVar->sizeVarName();
        if (std::find(groupNames.begin(), groupNames.end(), sizeVarName) == groupNames.end())
        {
            groupNames.push_back(sizeVarName);
            groupSizes.push_back(new unsigned int);
            data.push_back(new TreeSizeVar<Type>(*groupSizes.back(),sizeVarName,value));
        }
    }

    bool isBooked;
    std::vector<AbstractTreeVar*> data;
    std::vector<std::string> groupNames;
    std::vector<unsigned int *> groupSizes;

    std::map<std::type_index,std::string> ttreeTypes;







};




#endif
