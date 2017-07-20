#ifndef ANALYSISSUPPORT_macros_TREEREADINGWRAPPER_H
#define ANALYSISSUPPORT_macros_TREEREADINGWRAPPER_H

#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TChain.h>
#include <assert.h>
#include <iostream>
#include <fstream>


class TreeReadingWrapper{
public:
	TreeReadingWrapper(std::string  fileName, std::string  treeName){
    std::cout << " ++  Loading file: "<< fileName <<" and tree: " << treeName <<std::endl;
    if(TString(fileName).EndsWith(".root")){
        file = TFile::Open(fileName.c_str(),"READ");
        assert(file);
        tree = (TTree*)(file->Get(treeName.c_str()) );
        assert(tree);
    } else {

        std::ifstream ifs (fileName);
        if(!ifs.is_open())
            throw std::invalid_argument("TreeReadingWrapper::TreeReadingWrapper could not open file! ");
        std::string line;
        TChain * chain = new TChain(treeName.c_str());
        while ( std::getline (ifs,line) )
         {
            std::cout << " ++  Adding file: "<< line <<" to the tree."<<std::endl;
            int readStatus = chain->Add(line.c_str(),0);
            if(readStatus!=1)
                throw std::invalid_argument("TreeReadingWrapper::TreeReadingWrapper could not open file! ");
         }
        ifs.close();
        tree = chain;
        file = 0;
    }
    tree->SetBranchStatus("*",0);
    std::cout <<" ++  " << tree->GetEntries() << " entries to process" << std::endl;
  }
  virtual ~TreeReadingWrapper(){
    if(file){
        file->Close();
        delete file;
    } else if(tree){
        delete tree;
    }

  }
  int getEntries() const {return tree->GetEntries();}
  TTree * getTree() {return tree;}

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
  int setBranchAddressPre(const std::string branchName, const std::string varName, varType **var, bool require = false, bool verbose = true) {
      TString tBranchName = (branchName == "" ? varName : branchName + "_" + varName).c_str();
    if(tree->GetBranch(tBranchName)){
      tree->SetBranchStatus(tBranchName,1);
      tree->SetBranchAddress(tBranchName,var);
      return 1;
    }
    else {
      if(require) throw std::invalid_argument(( TString("TreeReadingWrapper::setBranchAddress could not load variable: " ) + tBranchName).Data() );
      if(verbose)std::cout << "-" <<varName <<" ";
      return 0;
    }
  }
  template<typename varType>
  int setBranchAddress(const std::string varName, varType **var, bool require = false, bool verbose = true) {
	  return setBranchAddressPre("",varName,var,require,verbose);
  }


  template<typename varType>
  int setBranchAddressPre(const std::string branchName, const std::string varName, varType *var, bool require = false, bool verbose = true) {
    TString tBranchName = (branchName == "" ? varName : branchName + "_" + varName).c_str();
    if(tree->GetBranch(tBranchName)){
      tree->SetBranchStatus(tBranchName,1);
      tree->SetBranchAddress(tBranchName,var);
      return 1;
    }
    else {
      if(require) throw std::invalid_argument(( TString("TreeReadingWrapper::setBranchAddress could not load variable: " ) + tBranchName).Data() );
      if(verbose)std::cout << "-" <<varName <<" ";
      return 0;
    }
  }

  template<typename varType>
  int setBranchAddress(const std::string varName, varType *var, bool require = false, bool verbose = true) {
	  return setBranchAddressPre("",varName,var,require,verbose);
  }



private:
  TFile * file;
  TTree * tree;
};


#endif
