#ifndef ANALYSISSUPPORT_macros_TreeReaderWrapper_H
#define ANALYSISSUPPORT_macros_TreeReaderWrapper_H

#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TChain.h>
#include <assert.h>
#include <iostream>
#include <fstream>


template<typename T>
class ReaderData{
public:
    void set(TTreeReader& reader,const char* branchname){
        if(data){
            delete data;
            data=0;
        }
        data = new TTreeReaderValue<T> (reader,branchname);
    }
    T* operator->() { return data->operator(); }
    T& operator*() {  return data->operator*(); }

    const T* operator->() const { return data->operator(); }
    const T& operator*() const {  return data->operator*(); }

    T& val() {return data->operator*();}
    const T& val() const {return data->operator*();}
    TTreeReaderValue<T> * data =0;
};

template<typename T>
class ReaderDataArray{
public:
    void set(TTreeReader& reader,const char* branchname){
        if(data){
            delete data;
            data=0;
        }
        data = new TTreeReaderArray<T> (reader,branchname);
    }

    T* operator->() {  return data->operator(); }
    T& operator*() {return data->operator*(); }


    T &At(std::size_t idx) { return data->At(idx); }
    const T &At(std::size_t idx) const {   return data->At(idx); }
    T &operator[](std::size_t idx) { return (*data)[idx]; }
    const T &operator[](std::size_t idx) const { return (*data)[idx];  }
    std::size_t size() const {
        return data->GetSize();
    }


    using iterator = typename TTreeReaderArray<T>::template Iterator_t<TTreeReaderArray<T>>;
    using const_iterator = typename TTreeReaderArray<T>::template Iterator_t<const TTreeReaderArray<T>>;

    iterator begin() { return data->begin(); }
    iterator end() {  return data->end(); }
    const_iterator begin() const {  return data->begin(); }
    const_iterator end() const { return data->end(); }
    const_iterator cbegin() const { return data->cbegin(); }
    const_iterator cend() const  { return data->cend(); }

    TTreeReaderArray<T> * data =0;
};


class TreeReaderWrapper{
public:
	TreeReaderWrapper(std::string  fileName, std::string  treeName){
    std::cout << " ++  Loading file: "<< fileName <<" and tree: " << treeName <<std::endl;
    if(TString(fileName).EndsWith(".root")){
        file = TFile::Open(fileName.c_str(),"READ");
        if(file == 0)
            throw std::invalid_argument(std::string("TreeReaderWrapper::TreeReaderWrapper() -> could not open file ") + fileName );
        tree = (TTree*)(file->Get(treeName.c_str()) );
        if(tree == 0)
            throw std::invalid_argument("TreeReaderWrapper::TreeReaderWrapper() -> could not open tree");
    } else {

        std::ifstream ifs (fileName);
        if(!ifs.is_open())
            throw std::invalid_argument("TreeReaderWrapper::TreeReaderWrapper could not open file! ");
        std::string line;
        TChain * chain = new TChain(treeName.c_str());
        while ( std::getline (ifs,line) )
         {
            std::cout << " ++  Adding file: "<< line <<" to the tree."<<std::endl;
            int readStatus = chain->Add(line.c_str(),0);
            if(readStatus!=1)
                throw std::invalid_argument("TreeReaderWrapper::TreeReaderWrapper could not open file! ");
         }
        ifs.close();
        tree = chain;
        file = 0;
    }

    reader = new TTreeReader(tree);
    tree->SetBranchStatus("*",0);
    std::cout <<" ++  " << tree->GetEntries() << " entries to process" << std::endl;
  }
  virtual ~TreeReaderWrapper(){
    delete reader;
    if(file){
        file->Close();
        delete file;
    } else if(tree){
        delete tree;
    }

  }

  TTree * getTree() {return tree;}

  void setEventRange(int startEvent = 0, int numEvents = -1){
      if(numEvents >0 )
          reader->SetEntriesRange(startEvent, startEvent  + numEvents );
      else
          reader->SetEntriesRange(startEvent, startEvent-1 );
  }



  //Load the next event from the tree....return false if there are no more events in the tree
  bool nextEvent(int& eventNumber, int reportFrequency = 1000000)
  {
      if(reader->Next()){
          eventNumber= reader->GetCurrentEntry();
          if(eventNumber%reportFrequency == 0)
            std::cout << "Processing event " << eventNumber << std::endl;
          return true;
      }
      return false;
  }




  template<typename T>
  void setBranch(const std::string branchName, const std::string varName, T& var, bool require = false, bool verbose = true) {
    TString tBranchName = (branchName == "" ? varName : branchName + "_" + varName).c_str();
    if(tree->GetBranch(tBranchName)){
      tree->SetBranchStatus(tBranchName,1);
      var.set(*reader,tBranchName);
    }
    else {
      if(require) throw std::invalid_argument(( TString("TreeReaderWrapper::setBranchAddress could not load variable: " ) + tBranchName).Data() );
      if(verbose)std::cout << "-" <<varName <<" ";
    }
  }



private:
  TFile * file;
  TTree * tree;
  TTreeReader * reader;
};


#endif
