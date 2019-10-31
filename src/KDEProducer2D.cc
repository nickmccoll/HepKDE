
#include "../interface/KDEProducer2D.h"
#include <cmath>
#include <iostream>
#include "TMath.h"
#include "TH2F.h"
#include "TH1F.h"
//--------------------------------------------------------------------------------------------------
KDEProducer2D::KDEProducer2D(const std::vector<double> * xvals,const std::vector<double> * yvals,
        const std::vector<double> * weights, const double hxSF,  const unsigned int nXBins,
        const double xMin, const double xMax, const double trimFactorX, const double hySF,
        const unsigned int nYBins, const double yMin, const double yMax, const double trimFactorY)
        : xvals(xvals),yvals(yvals),weights(weights),nDataPts(xvals->size()),
          oneOverTwoPi(1./TMath::TwoPi())
{
    computeGlobalQuantities(hxSF,hySF);
    if(nXBins > 0){
        setupPilotKDE(nXBins,xMin,xMax,nYBins,yMin,yMax);
        buildPilotKDE(trimFactorX,trimFactorY);
    }
}
//--------------------------------------------------------------------------------------------------
KDEProducer2D::KDEProducer2D(const std::vector<double> * xvals,const std::vector<double> * yvals,
        const std::vector<double> * weights, const double hxSF,  const unsigned int nXBins,
        const double xMin, const double xMax, const double trimFactorX, const double hySF,
        const unsigned int nYBins, const double* yBins, const double trimFactorY)
        : xvals(xvals),yvals(yvals),weights(weights),nDataPts(xvals->size()),
          oneOverTwoPi(1./TMath::TwoPi())
{
    computeGlobalQuantities(hxSF,hySF);
    if(nXBins > 0){
        setupPilotKDE(nXBins,xMin,xMax,nYBins,yBins);
        buildPilotKDE(trimFactorX,trimFactorY);
    }
}
//--------------------------------------------------------------------------------------------------
void KDEProducer2D::computeGlobalQuantities(const double hSFX,const double hSFY) {
    double sumW   = 0;
    double sumW2  = 0;
    double sumX   = 0;
    double sumX2  = 0;
    double sumY   = 0;
    double sumY2  = 0;


    for(unsigned int iX = 0; iX < nDataPts; ++iX){
        sumW  += (*weights)[iX];
        sumW2 += (*weights)[iX]*(*weights)[iX];
        sumX  += (*xvals)[iX]*(*weights)[iX];
        sumX2 += (*xvals)[iX]*(*xvals)[iX]*(*weights)[iX];
        sumY  += (*yvals)[iX]*(*weights)[iX];
        sumY2 += (*yvals)[iX]*(*yvals)[iX]*(*weights)[iX];
    }

    const double sigmaX = std::sqrt( sumX2/sumW - (sumX*sumX)/(sumW*sumW)  );
    const double sigmaY = std::sqrt( sumY2/sumW - (sumY*sumY)/(sumW*sumW)  );

    //correction because we are using weighted events
    const double effNevnts = sumW*sumW/sumW2;

    h0X = hSFX*sigmaX*std::pow(1./effNevnts, 1./6.);
    h0Y = hSFY*sigmaY*std::pow(1./effNevnts, 1./6.);
    oneOverh0X = 1./h0X;
    oneOverh0Y = 1./h0Y;
    oneOverTwoNegh0XSq = -0.5*oneOverh0X*oneOverh0X;
    oneOverTwoNegh0YSq = -0.5*oneOverh0Y*oneOverh0Y;
    nEvt = sumW;
    oneOverNEvt = 1./nEvt;

    //Debugging output:
//    std::cout <<"Nvt: "<< nDataPts << " SumW: "<< nEvt
//    <<" stdDev: (" << sigmaX<<","<<sigmaY <<") h0: ("<<
//            h0X<<","<<h0Y<<") nEff: "<<effNevnts <<std::endl;
}
//--------------------------------------------------------------------------------------------------
void KDEProducer2D::setupPilotKDE(const unsigned int nXBins, const double xMin,
        const double xMax,const unsigned int nYBins, const double yMin, const double yMax) {
    //For speed, the pilot density will be computed
    //in a grid, set by the histogram bin centers
    pilotKDE.reset(getPDF("pilotKDE","; pilot KDE",nXBins,xMin,xMax,nYBins,yMin,yMax));
}
//--------------------------------------------------------------------------------------------------
void KDEProducer2D::setupPilotKDE(const unsigned int nXBins, const double xMin,
        const double xMax,const unsigned int nYBins, const double* yBins) {
    //For speed, the pilot density will be computed
    //in a grid, set by the histogram bin centers
    pilotKDE.reset(getPDF("pilotKDE","; pilot KDE",nXBins,xMin,xMax,nYBins,yBins));
}
//--------------------------------------------------------------------------------------------------
void KDEProducer2D::buildPilotKDE(const double trimFactorX,const double trimFactorY) {
    //store the inverse of the bandwidths for spead
    inv_hxis.reset(make_invHs(oneOverh0X,trimFactorX) );
    inv_hyis.reset(make_invHs(oneOverh0Y,trimFactorY));
    //    localVarX.reset(getLocalVarX("localVarX",";local var",nXBins,xMin,xMax,nYBins,yMin,yMax));
    //    localVarY.reset(getLocalVarY("localVarY",";local var",nXBins,xMin,xMax,nYBins,yMin,yMax));
//    inv_hxis.reset(make_invHs(oneOverh0X,localVarX.get(),trimFactorX,doSigmaScaling) );
//    inv_hyis.reset(make_invHs(oneOverh0Y,localVarY.get(),trimFactorY,doSigmaScaling));
}
//--------------------------------------------------------------------------------------------------
std::vector<double>* KDEProducer2D::make_invHs(const double oneOverH0,const double trimFactor)const{
    auto * inv_his = new std::vector<double>;
    inv_his->reserve(nDataPts);
    std::vector< std::pair<float,float> > orderedData; orderedData.reserve(nDataPts);

    const double minIntX = pilotKDE->GetXaxis()->GetBinCenter(1);
    const double maxIntX = pilotKDE->GetXaxis()->GetBinCenter(pilotKDE->GetNbinsX());
    const double minIntY = pilotKDE->GetYaxis()->GetBinCenter(1);
    const double maxIntY = pilotKDE->GetYaxis()->GetBinCenter(pilotKDE->GetNbinsY());

    for(unsigned int iX = 0; iX < nDataPts; ++iX){
        const float xv = std::max(minIntX,std::min((*xvals)[iX],maxIntX));
        const float yv = std::max(minIntY,std::min((*yvals)[iX],maxIntY));
        double dens = pilotKDE->Interpolate(xv,yv);
//        const double var = localVar->Interpolate(xv,yv);
//        std::cout <<"("<< (*xvals)[iX]<<","<<(*yvals)[iX]<<","<<dens<<"," << sigmax<<","<<sigmay;
//        if(doSigmaScaling && var) dens *= var;
//        std::cout <<","<<dens <<") ";
        inv_his->emplace_back(  dens );
        orderedData.emplace_back(dens,  (*weights)[iX]);
    }

    //trim the densities to put a max on the bandwidths
    if(trimFactor > 0){
        //first calculate the median
        std::sort(orderedData.begin(),orderedData.end(),
                [](const std::pair<float,float> a, std::pair<float,float> b )
                    {return a.first < b.first;}
        );
        double runningSum = 0;
        double median = 0;
        for(unsigned int iX = 0; iX < nDataPts; ++iX){
            runningSum += orderedData[iX].second;
            if(runningSum < nEvt/2.) continue;
            median = orderedData[iX].first;
            break;
        }
        // Want to trim based on the median bandwidth...which is
        // 1/sqrt(f)...convert from median and inv_his, which is
        // in f
        const double minF = median/(trimFactor*trimFactor);
        for(auto& d : *inv_his) if(d < minF){
            d = minF;
        }

        //Debugging output:
//        std::cout <<"Median h: "<< median <<std::endl;
    }

    const double geoMean = geometricMean(inv_his);
    //get the final bandwidths by multiplying by h0*g...do the inversion now
    const double inv_bandNorm = oneOverH0*std::sqrt(1./geoMean);

    for(auto& d : *inv_his) {
        d = inv_bandNorm*std::sqrt(d);
    }
    return inv_his;
}
//--------------------------------------------------------------------------------------------------
double KDEProducer2D::getADensity(const double x,const double y, double* weight) const {
    double dens = 0;
    double sumW2 = 0;
    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
        const double deltX = (x - (*xvals)[iX]);
        const double deltY = (y - (*yvals)[iX]);
        const double wt = (*weights)[iX]*(*inv_hxis)[iX]*(*inv_hyis)[iX]*
                std::exp(-0.5*deltX*deltX*(*inv_hxis)[iX]*(*inv_hxis)[iX] +
                        -0.5*deltY*deltY*(*inv_hyis)[iX]*(*inv_hyis)[iX]);
        dens += wt;
        sumW2 += wt*wt;
    }
    if(weight) *weight = oneOverTwoPi*oneOverNEvt*std::sqrt(sumW2);
    return dens*oneOverTwoPi*oneOverNEvt;
}
//--------------------------------------------------------------------------------------------------
// Only to be used when *inv_hist* is f(x)
double KDEProducer2D::geometricMean(const std::vector<double> * vals) const
{
    double sumTerms = 0;
    for(unsigned int iX = 0; iX < nDataPts; ++iX) {
        sumTerms += (*weights)[iX]*std::log((*vals)[iX]);
//        std::cout <<"("<<(*weights)[iX]<<","<< (*inv_his)[iX]<<","<<sumTerms<<") ";
    }
    return std::exp(sumTerms*oneOverNEvt);
}
//--------------------------------------------------------------------------------------------------
double KDEProducer2D::getDensity(const double x,const double y, double* weight) const {
    double dens = 0;
    double sumW2 = 0;
    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
        const double deltX = (x - (*xvals)[iX]);
        const double deltY = (y - (*yvals)[iX]);
        const double wt = (*weights)[iX]
                           *std::exp(oneOverTwoNegh0XSq*deltX*deltX+oneOverTwoNegh0YSq*deltY*deltY);
        dens += wt;
        sumW2 += wt*wt;
    }
    if(weight) *weight = oneOverTwoPi*oneOverh0X*oneOverh0Y*oneOverNEvt*std::sqrt(sumW2);
    return dens*oneOverTwoPi*oneOverh0X*oneOverh0Y*oneOverNEvt;
}
//--------------------------------------------------------------------------------------------------
TH2* KDEProducer2D::getPDF(const std::string& name, const std::string& title,
        const int unsigned nXBins, const float xMin, const float xMax,
        const int unsigned nYBins, const float yMin, const float yMax
        ) const {
    TH2 * h = new TH2F(name.c_str(), title.c_str(), nXBins,xMin,xMax,nYBins,yMin,yMax);
    fillPDF(h,false);
    return h;
}
//--------------------------------------------------------------------------------------------------
TH2* KDEProducer2D::getPDF(const std::string& name, const std::string& title,
        const int unsigned nXBins, const float xMin, const float xMax,
        const int unsigned nYBins, const double* yBins) const {
    TH2 * h = new TH2F(name.c_str(), title.c_str(), nXBins,xMin,xMax,nYBins,yBins);
    fillPDF(h,false);
    return h;
}
//--------------------------------------------------------------------------------------------------
TH2* KDEProducer2D::getAPDF(const std::string& name, const std::string& title,
        const int unsigned nXBins, const float xMin, const float xMax,
        const int unsigned nYBins, const float yMin, const float yMax
        ) const {
    TH2 * h = new TH2F(name.c_str(), title.c_str(), nXBins,xMin,xMax,nYBins,yMin,yMax);
    fillPDF(h,true);
    return h;
}
//--------------------------------------------------------------------------------------------------
TH2* KDEProducer2D::getAPDF(const std::string& name, const std::string& title,
        const int unsigned nXBins, const float xMin, const float xMax,
        const int unsigned nYBins, const double* yBins) const {
    TH2 * h = new TH2F(name.c_str(), title.c_str(), nXBins,xMin,xMax,nYBins,yBins);
    fillPDF(h,true);
    return h;
}
//--------------------------------------------------------------------------------------------------
void KDEProducer2D::fillPDF(TH2 * h,const bool adaptive) const {
    double weight;
    const unsigned int nBinsX = h->GetNbinsX();
    const int unsigned nBinsY = h->GetNbinsY();
    const TAxis * xAxis = h->GetXaxis();
    const TAxis * yAxis = h->GetYaxis();
    for(unsigned int iX = 1; iX <= nBinsX; ++iX)
        for(unsigned int iY = 1; iY <= nBinsY; ++iY){
            h->SetBinContent(iX,iY, adaptive ?
                    getADensity(xAxis->GetBinCenter(iX),yAxis->GetBinCenter(iY),&weight)
                    : getDensity(xAxis->GetBinCenter(iX),yAxis->GetBinCenter(iY),&weight)
                );
            h->SetBinError(iX,iY,weight);
        }
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer2D::convToHist(const TH1 * hist) const {
    TH1* oH = (TH1*)hist->Clone();
    for(int iB = 1; iB <= hist->GetNbinsX(); ++iB)
        oH->SetBinContent(iB,hist->GetBinWidth(iB)*hist->GetBinContent(iB));
    return oH;
}
//--------------------------------------------------------------------------------------------------
TH2* KDEProducer2D::convToHist(const TH2 * hist) const {
    TH2* oH = (TH2*)hist->Clone();
    for(int iX = 1; iX <= hist->GetNbinsX(); ++iX)
        for(int iY = 1; iY <= hist->GetNbinsY(); ++iY)
        oH->SetBinContent(iX,iY,hist->GetXaxis()->GetBinWidth(iX)*
                hist->GetYaxis()->GetBinWidth(iY)*hist->GetBinContent(iX,iY));
    return oH;
}
//--------------------------------------------------------------------------------------------------
TH2* KDEProducer2D::getPilotPDF() const {
    TH2* oH = (TH2*)pilotKDE->Clone();
    return oH;
}
//--------------------------------------------------------------------------------------------------
void KDEProducer2D::fillABandwidths(TH2 * h, const std::vector<double>* bands)const{
    const unsigned int nXBins = h->GetNbinsX();
    const unsigned int nYBins = h->GetNbinsY();
    std::vector<std::vector<float>> sumWs (nXBins+1,std::vector<float>(nYBins+1,0));
    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
        const unsigned int binx = h->GetXaxis()->FindFixBin((*xvals)[iX]);
        const unsigned int biny = h->GetYaxis()->FindFixBin((*yvals)[iX]);
        if(binx < 1 || binx > nXBins) continue;
        if(biny < 1 || biny > nYBins) continue;
        h->SetBinContent(binx,biny, h->GetBinContent(binx,biny) + (*weights)[iX]/(*bands)[iX]);
        sumWs[binx][biny] += (*weights)[iX] ;
    }
    for(int iX = 1; iX <= h->GetNbinsX(); ++iX)
        for(int iY = 1; iY <= h->GetNbinsY(); ++iY)
            if(sumWs[iX][iY] )
                h->SetBinContent(iX,iY, h->GetBinContent(iX,iY)/sumWs[iX][iY] );
}
//--------------------------------------------------------------------------------------------------
TH2* KDEProducer2D::getABandwidths(const std::string& name, const std::string& title,
        const unsigned int nXBins, const float xMin, const float xMax,
        const unsigned int nYBins, const float yMin, const float yMax, bool doX)const{
    TH2 * h = new TH2F(name.c_str(), title.c_str(), nXBins,xMin,xMax,nYBins,yMin,yMax);
    fillABandwidths(h,doX ? inv_hxis.get() :inv_hyis.get() );
    return h;
}
//--------------------------------------------------------------------------------------------------
TH2* KDEProducer2D::getABandwidths(const std::string& name, const std::string& title,
        const unsigned int nXBins, const float xMin, const float xMax,
        const unsigned int nYBins, const double* yBins, bool doX)const{
    TH2 * h = new TH2F(name.c_str(), title.c_str(), nXBins,xMin,xMax,nYBins,yBins);
    fillABandwidths(h,doX ? inv_hxis.get() :inv_hyis.get() );
    return h;
}
////--------------------------------------------------------------------------------------------------
//double KDEProducer2D::getLocalVarX(const double x, const double y) const {
//    double sumW = 0;
//    double sumX = 0;
//    double sumX2 = 0;
//    int nEvt = 0;
//    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
//        if(std::fabs(x - (*xvals)[iX]) > h0X   ) continue;
//        nEvt++;
//        sumX +=  (*weights)[iX]*(*xvals)[iX];
//        sumX2 += (*weights)[iX]*(*xvals)[iX]*(*xvals)[iX];
//        sumW  += (*weights)[iX];
//    }
//    const double var = sumX2/sumW - (sumX*sumX)/(sumW*sumW);
//    return nEvt > 100 && sumW > 0 && var > 0 ? var : 0;
//    double y2 = y; y2++;//unrun garbage to remove warning
//}
////--------------------------------------------------------------------------------------------------
//double KDEProducer2D::getLocalVarY(const double x, const double y) const {
//    double sumW = 0;
//    double sumX = 0;
//    double sumX2 = 0;
//    int nEvt = 0;
//    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
//        if(std::fabs(y - (*yvals)[iX]) > h0Y   ) continue;
//        nEvt++;
//        sumX +=  (*weights)[iX]*(*yvals)[iX];
//        sumX2 += (*weights)[iX]*(*yvals)[iX]*(*yvals)[iX];
//        sumW  += (*weights)[iX];
//    }
//    const double var = sumX2/sumW - (sumX*sumX)/(sumW*sumW);
//    return nEvt > 100 && sumW > 0 && var > 0 ? var : 0;
//    double x2 = x; x2++;//unrun garbage to remove warning
//}
////--------------------------------------------------------------------------------------------------
//TH2 *  KDEProducer2D::getLocalVarX(const std::string& name, const std::string& title,
//        const unsigned int nXBins, const float xMin, const float xMax,
//        const unsigned int nYBins, const float yMin, const float yMax)const {
//    TH2 * h = new TH2F(name.c_str(), title.c_str(), nXBins,xMin,xMax,nYBins,yMin,yMax);
//    for(unsigned int iY = 1; iY <= nYBins; ++iY){
//        for(unsigned int iX = 1; iX <= nXBins; ++iX){
//            const double locVar = iY > 1 ? h->GetBinContent(iX, 1)
//                    : getLocalVarX(h->GetXaxis()->GetBinCenter(iX),h->GetYaxis()->GetBinCenter(iY));
//            if(locVar)
//                h->SetBinContent(iX,iY,locVar );
//            else
//                h->SetBinContent(iX,iY,h->GetBinContent(iX -1, iY) );
//        }
//        for(unsigned int iX = nXBins -1; iX > 0; --iX)
//            if(h->GetBinContent(iX,iY) == 0) h->SetBinContent(iX,iY,h->GetBinContent(iX+1,iY));
//    }
//    return h;
//}
////--------------------------------------------------------------------------------------------------
//TH2 *  KDEProducer2D::getLocalVarY(const std::string& name, const std::string& title,
//        const unsigned int nXBins, const float xMin, const float xMax,
//        const unsigned int nYBins, const float yMin, const float yMax)const {
//    TH2 * h = new TH2F(name.c_str(), title.c_str(), nXBins,xMin,xMax,nYBins,yMin,yMax);
//    for(unsigned int iX = 1; iX <= nXBins; ++iX){
//    for(unsigned int iY = 1; iY <= nYBins; ++iY){
//        const double locVar = iX > 1 ? h->GetBinContent(1, iY)
//                : getLocalVarY(h->GetXaxis()->GetBinCenter(iX),h->GetYaxis()->GetBinCenter(iY));
//            if(locVar)
//                h->SetBinContent(iX,iY,locVar );
//            else
//                h->SetBinContent(iX,iY,h->GetBinContent(iX, iY-1) );
//        }
//        for(unsigned int iY = nYBins -1; iY > 0; --iY)
//            if(h->GetBinContent(iX,iY) == 0) h->SetBinContent(iX,iY,h->GetBinContent(iX,iY+1));
//    }
//    return h;
//}
