
#include "../interface/TObjectHelper.h"
#include <iostream>

using ASTypes::ValAndErrF;
using ASTypes::size;
namespace TObjectHelper {
TFile * getFile(const std::string& filename,std::string option, const bool verbose) {
    if(verbose){
        std::cout << " ++  Opening "<<filename<<std::endl;
    }
    TFile* file = TFile::Open(filename.c_str(),option.c_str());
    if(file == 0) throw std::invalid_argument(std::string("TObjectHelper::getFile() -> could not open file ") + filename );
    return file;
}

Hist1DContainer::Hist1DContainer(TFile* file, const std::string& objName, const bool verbose) :
    hist(getObject<TH1>(file,objName,verbose)){}
ASTypes::ValAndErrF Hist1DContainer::getBinContentByBinNumber(ASTypes::size  ibin) const {
    return ValAndErrF(hist->GetBinContent(ibin),hist->GetBinError(ibin));
}
ASTypes::ValAndErrF Hist1DContainer:: getBinContentByValue(const float xval) const {
    size ibin = hist->GetXaxis()->FindFixBin(xval);
    if(ibin == 0) ibin = 1;
    else if (ibin >   size(hist->GetNbinsX()) ) ibin =  hist->GetNbinsX() ;
    return getBinContentByBinNumber(ibin);
}

Hist2DContainer::Hist2DContainer(TFile* file, const std::string& objName, const bool verbose) :
    hist(getObject<TH2>(file,objName,verbose)){}

ASTypes::ValAndErrF Hist2DContainer::getBinContentByBinNumber(const ASTypes::size ibinx,const ASTypes::size ibiny ) const {
    return ValAndErrF(hist->GetBinContent(ibinx,ibiny),hist->GetBinError(ibinx,ibiny));
}
ASTypes::ValAndErrF Hist2DContainer:: getBinContentByValue(const float xval,const float yval)const {
    size ibinx = hist->GetXaxis()->FindFixBin(xval);
    if(ibinx == 0) ibinx = 1;
    else if (ibinx >   size(hist->GetNbinsX()) ) ibinx =  hist->GetNbinsX() ;

    size ibiny = hist->GetYaxis()->FindFixBin(yval);
    if(ibiny == 0) ibiny = 1;
    else if (ibiny >   size(hist->GetNbinsY()) ) ibiny =  hist->GetNbinsY() ;

    return getBinContentByBinNumber(ibinx,ibiny);
}


}
