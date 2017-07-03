#ifndef ANALYSISSUPPORT_macros_TREEREADINGWRAPPER_H
#define ANALYSISSUPPORT_macros_TREEREADINGWRAPPER_H

#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <assert.h>
#include <iostream>


class TreeReadingWrapper{
public:
	TreeReadingWrapper(std::string  fileName, std::string  treeName){
    std::cout << "Loading file: "<< fileName <<" and tree: " << treeName <<std::endl;
    file = TFile::Open(fileName.c_str(),"READ");
    assert(file);
    tree = (TTree*)(file->Get(treeName.c_str()) );
    assert(tree);
    std::cout << tree->GetEntries() << " entries to process" << std::endl;
  }
  virtual ~TreeReadingWrapper(){
    file->Close();
    delete file;
  }
  int getEntries() const {return tree->GetEntries();}

  //Load the next event from the tree....return false if there are no more events in the tree
  bool readEvent(unsigned int eventNumber, int reportFrequency = 1000000)
  {
    if(eventNumber >= tree->GetEntries()) return false;
    tree->GetEntry(eventNumber);

    if(eventNumber%reportFrequency == 0)
      std::cout << "Processing event " << eventNumber << std::endl;
    return true;
  }

  template<typename varType>
  void setBranchAddressPre(const std::string branchName, const std::string varName, varType **var, bool require = false, bool verbose = true) {
      TString tBranchName = (branchName == "" ? varName : branchName + "_" + varName).c_str();
    if(tree->GetBranch(tBranchName)){
      tree->SetBranchStatus(tBranchName,1);
      tree->SetBranchAddress(tBranchName,var);
    }
    else {
      if(require) throw std::invalid_argument(( TString("TreeReadingWrapper::setBranchAddress could not load variable: " ) + tBranchName).Data() );
      if(verbose)std::cout << " -" <<tBranchName;
    }
  }
  template<typename varType>
  void setBranchAddress(const std::string varName, varType **var, bool require = false, bool verbose = true) {
	  setBranchAddressPre("",varName,var,require,verbose);
  }


  template<typename varType>
  void setBranchAddressPre(const std::string branchName, const std::string varName, varType *var, bool require = false, bool verbose = true) {
    TString tBranchName = (branchName == "" ? varName : branchName + "_" + varName).c_str();
    if(tree->GetBranch(tBranchName)){
      tree->SetBranchStatus(tBranchName,1);
      tree->SetBranchAddress(tBranchName,var);
    }
    else {
      if(require) throw std::invalid_argument(( TString("TreeReadingWrapper::setBranchAddress could not load variable: " ) + tBranchName).Data() );
      if(verbose)std::cout << " -" <<tBranchName;
    }
  }

  template<typename varType>
  void setBranchAddress(const std::string varName, varType *var, bool require = false, bool verbose = true) {
	  setBranchAddressPre("",varName,var,require,verbose);
  }


private:
  TFile * file;
  TTree * tree;
};


#endif
