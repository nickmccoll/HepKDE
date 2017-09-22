#ifndef ANALYSISSUPPORT_UTILITIES_INTERFACE_TOBJECTHELPER_H_
#define ANALYSISSUPPORT_UTILITIES_INTERFACE_TOBJECTHELPER_H_
#include <string>
#include <iostream>
#include "TFile.h"
#include <TH1.h>
#include <TH2.h>
#include <TGraphAsymmErrors.h>
#include "Types.h"


namespace TObjectHelper {
//Gets object from file and takes ownership (e.g. TH1)
template <typename ObjType>
std::unique_ptr<ObjType> getObject(TFile* file, const std::string& objName, const bool verbose =false ) {
    if(verbose){
        std::cout << " ++  getting object "<<objName<<std::endl;
    }
    ObjType * obj = 0;
    file->GetObject(objName.c_str(), obj);
    if(obj == 0){
        throw std::invalid_argument(std::string("TObjectHelper::getObject() -> could not open object ") + objName );
    }
    obj->SetDirectory(0);
    return std::unique_ptr<ObjType>(obj);
}

//Gets object but ownership is taken automatically (e.g. TGraph)
template <typename ObjType>
std::unique_ptr<ObjType> getObjectNoOwn(TFile* file, const std::string& objName, const bool verbose =false ) {
    if(verbose){
        std::cout << " ++  getting object "<<objName<<std::endl;
    }
    ObjType * obj = 0;
    file->GetObject(objName.c_str(), obj);
    if(obj == 0){
        throw std::invalid_argument(std::string("TObjectHelper::getObject() -> could not open object ") + objName );
    }
    return std::unique_ptr<ObjType>(obj);
}


TFile * getFile(const std::string& filename, std::string option = "READ", const bool verbose = false);

//Containers of TObjects, they own the retreived object...feel free to delete the file after
class Hist1DContainer {
public:
    Hist1DContainer(TFile* file, const std::string& objName, const bool verbose =false );
    ~Hist1DContainer(){}
    ASTypes::ValAndErrF getBinContentByBinNumber(const ASTypes::size ibin) const;
    //Ignores under and overflows!
    ASTypes::ValAndErrF getBinContentByValue(const float xval) const;
private:
    std::unique_ptr<TH1> hist;
};

class Hist2DContainer {
public:
    Hist2DContainer(TFile* file, const std::string& objName, const bool verbose =false );
    ~Hist2DContainer(){}
    ASTypes::ValAndErrF getBinContentByBinNumber(const ASTypes::size ibinx,const ASTypes::size ibiny ) const;
    //Ignores under and overflows!
    ASTypes::ValAndErrF getBinContentByValue(const float xval,const float yval) const;
private:
    std::unique_ptr<TH2> hist;
};
class GraphAEContainer {
public:
    GraphAEContainer(TFile* file, const std::string& objName, const bool verbose =false );
    ~GraphAEContainer(){}

    ASTypes::ValAndAssymErrF getBinContentByBinNumber(const ASTypes::size ibin) const;
    //Ignores under and overflows!
    float eval(float xval) const;
private:
    std::unique_ptr<TGraphAsymmErrors> hist;
    ASTypes::size nBins;
    float min;
    float max;
};


}
#endif /* FRAMEWORK_PROCESSORS_INTERFACE_EVENTWEIGHTS_H_ */
