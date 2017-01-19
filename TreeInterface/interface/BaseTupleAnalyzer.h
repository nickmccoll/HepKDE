#ifndef ANALYSISSUPPORT_macros_BASETUPLEANALYZER_H
#define ANALYSISSUPPORT_macros_BASETUPLEANALYZER_H

#include "TreeReadingWrapper.h"

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
