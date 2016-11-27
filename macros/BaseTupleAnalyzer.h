#ifndef ANALYSISSUPPORT_macros_BASETUPLEANALYZER_H
#define ANALYSISSUPPORT_macros_BASETUPLEANALYZER_H

#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <assert.h>
#include <iostream>

typedef   unsigned char        size8 ; // 8 bit 0->255
typedef   unsigned short int   size16; //16 bit 0->65536
typedef   unsigned int         size  ; //32 bit 0->4294967296

class BaseTupleAnalyzer{
public:
  BaseTupleAnalyzer(std::string  fileName, std::string  treeName) : eventNumber(0){
    std::clog << "Loading file: "<< fileName <<" and tree: " << treeName <<std::endl;
    file = TFile::Open(fileName.c_str(),"READ");
    assert(file);
    tree = (TTree*)(file->Get(treeName.c_str()) );
    assert(tree);
    std::clog << tree->GetEntries() << " entries to process" << std::endl;
  }
  virtual ~BaseTupleAnalyzer(){
    file->Close();
    delete file;
  }
  virtual void analyze(int reportFrequency = 1000000) {
    while(nextEvent(reportFrequency)){runAEvent();eventNumber++;}
  }
  virtual void runAEvent() = 0;
  //Load the next event from the tree....return false if there are no more events in the tree
  bool nextEvent(int reportFrequency = 1000000)
  {
    if(eventNumber >= tree->GetEntries()) return false;
    tree->GetEntry(eventNumber);

    if(eventNumber%reportFrequency == 0)
      std::clog << "Processing event " << eventNumber << std::endl;

    return true;
  }
  template<typename varType>
  void setBranchAddress(const std::string varName, varType **var, bool require = false, bool verbose = true) {
    TString tBranchName = varName.c_str();
    if(tree->GetBranch(tBranchName)){
      tree->SetBranchStatus(tBranchName,1);
      tree->SetBranchAddress(tBranchName,var);
    }
    else {
      if(require) throw std::invalid_argument(( TString("BaseTupleAnalyzer::setBranchAddress could not load variable: " ) + tBranchName).Data() );
      if(verbose)std::clog << " -" <<tBranchName;
    }
  }
  template<typename varType>
  void setBranchAddress(const std::string varName, varType *var, bool require = false, bool verbose = true) {
    TString tBranchName = varName.c_str();
    if(tree->GetBranch(tBranchName)){
      tree->SetBranchStatus(tBranchName,1);
      tree->SetBranchAddress(tBranchName,var);
    }
    else {
      if(require) throw std::invalid_argument(( TString("BaseTupleAnalyzer::setBranchAddress could not load variable: " ) + tBranchName).Data() );
      if(verbose)std::clog << " -" <<tBranchName;
    }
  }


  TFile * file;
  TTree * tree;
  int     eventNumber; //current event number
};


#endif
