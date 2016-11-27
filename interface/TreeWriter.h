#ifndef ANALYSISSUPPORT_TreeWriter_H
#define ANALYSISSUPPORT_TreeWriter_H

#include <string>
#include <vector>
#include <assert.h>

#include "TreeWriterData.h"

class TFile;

typedef   unsigned char        size8 ; // 8 bit 0->255
typedef   unsigned short int   size16; //16 bit 0->65536
typedef   unsigned int         size  ; //32 bit 0->4294967296

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
