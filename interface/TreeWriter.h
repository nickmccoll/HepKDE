#ifndef ANALYSISSUPPORT_TreeWriter_H
#define ANALYSISSUPPORT_TreeWriter_H

#include <string>
#include <vector>
#include <assert.h>

#include "TreeWriterData.h"
#include "Types.h"
using namespace AnalysisSupportTypes;

class TFile;
class TreeWriter : public TreeWriterData {
  public:
  TreeWriter(TString fileName, TString treeName, TString treeTitle);

  void book();
  void fillTree();
  void write();


  virtual ~TreeWriter();

  private:
  TFile * file;
  TreeWrapper * tree;


};

#endif
