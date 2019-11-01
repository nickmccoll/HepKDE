#include "TSystem.h"
#include "TRandom3.h"
#include "TH1F.h"
#include "TFile.h"

#include "../interface/KDEProducer1D.h"
void testMacro()
{
  
  ///Let's use a Gaussian random number generator to make some data:
  std::vector<double> vals;
  std::vector<double> weights;
  
  //Everything will be weight one
  auto randomEvent = new TRandom3(12345);
  for(unsigned int iE = 0; iE < 500; ++iE){
    vals.push_back(randomEvent->Gaus());
    weights.push_back(1);
  }
    
  //Now let's build a histogram of the data that we can use to test the KDE:
  TH1F* h_input = new TH1F("inputHist","Input",100,-4,4);
  for(unsigned int iE = 0; iE < vals.size(); ++iE) {
    h_input->Fill(vals[iE],weights[iE]);
  }
  //And fill up one with much better statistics
  TH1F* h_test = new TH1F("testHist","True distribution",100,-4,4);
  h_test->FillRandom("gaus", 10000000);
  
  //Now run the KDE producer, we will set a x2 bandwidth scaling and a bandwidth cutoff of 1.5
  std:unique_ptr<HepKDE::KDEProducer1D> pdfProd(new HepKDE::KDEProducer1D(&vals,&weights,2,100,-4,4,1.5));
  
  //Get the non-Adaptive KDE
  auto nmKDE = pdfProd->getPDF("nominalKDE","Nominal KDE",100,-4,4);
  
  //Get the Adaptive KDE
  auto adKDE = pdfProd->getAPDF("adaptiveKDE","Nominal KDE",100,-4,4);
  
  //Write your outputs:
  TFile * fo = new TFile("testOutput.root","recreate");
  h_input->Write();
  h_test->Write();
  nmKDE->Write();
  adKDE->Write();
  fo->Close();
  
}