#ifndef ANALYSISSUPPORT_macros_BASETUPLEANALYZER_H
#define ANALYSISSUPPORT_macros_BASETUPLEANALYZER_H

#include "TreeReadingWrapper.h"

typedef   unsigned char        size8 ; // 8 bit 0->255
typedef   unsigned short int   size16; //16 bit 0->65536
typedef   unsigned int         size  ; //32 bit 0->4294967296

class BaseTupleAnalyzer : public TreeReadingWrapper{
public:
  BaseTupleAnalyzer(std::string  fileName, std::string  treeName) : TreeReadingWrapper(fileName,treeName), eventNumber(0){
  }
  virtual ~BaseTupleAnalyzer(){}

  virtual void analyze(int reportFrequency = 1000000) {
    while(readEvent(eventNumber, reportFrequency)){runAEvent();eventNumber++;}
  }
  virtual void runAEvent() = 0;
  int     eventNumber; //current event number
};


#endif
