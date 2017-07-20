#ifndef ANALYSISSUPPORT_TreeWriter_H
#define ANALYSISSUPPORT_TreeWriter_H

#include <string>
#include <vector>
#include <assert.h>

#include "TreeWriterData.h"

class TFile;
class TreeWriter : public TreeWriterData {
  public:
  TreeWriter(TString fileName, TString treeName, TString treeTitle);
  // TreeWriter will take ownership!!
  // This is why the file is needed too
  TreeWriter(TFile * file_, TTree * tree_, TDirectory * dir_ =0);

  void book();
  void fillTree();
  void write();


  virtual ~TreeWriter();

  private:
  TFile * file;
  TreeWrapper * tree;
  TDirectory * dir=0;


};

#endif
