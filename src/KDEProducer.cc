
#include "../interface/KDEProducer.h"
#include <cmath>
#include <iostream>
#include "TMath.h"
#include "TH1F.h"
//--------------------------------------------------------------------------------------------------
KDEProducer::KDEProducer(const std::vector<double> * xvals,
        const std::vector<double> * weights, const double hSF,
        const unsigned int nXBins, const double xMin, const double xMax,
        const double trimFactor)
        : xvals(xvals),weights(weights),nDataPts(xvals->size()),
          oneOverRootTwoPi(std::sqrt(1./TMath::TwoPi()))
{
    computeGlobalQuantities(hSF);
    if(nXBins > 0){
        setupPilotKDE(nXBins,xMin,xMax);
        buildPilotKDE(trimFactor);
    }
}
//--------------------------------------------------------------------------------------------------
KDEProducer::KDEProducer(const std::vector<double> * xvals,
        const std::vector<double> * weights, const double hSF,
        const unsigned int nXBins, const double* xBins,const double trimFactor)
        : xvals(xvals),weights(weights),nDataPts(xvals->size()),
          oneOverRootTwoPi(std::sqrt(1./TMath::TwoPi()))
{
    computeGlobalQuantities(hSF);
    if(nXBins > 0){
        setupPilotKDE(nXBins,xBins);
        buildPilotKDE(trimFactor);
    }
}
//--------------------------------------------------------------------------------------------------
void KDEProducer::computeGlobalQuantities(const double hSF) {
    double sumW   = 0;
    double sumW2  = 0;
    double sumX   = 0;
    double sumX2  = 0;

    //For IQR
    std::vector< std::pair<float,float> > orderedData; orderedData.reserve(nDataPts);

    for(unsigned int iX = 0; iX < nDataPts; ++iX){
        sumW  += (*weights)[iX];
        sumW2 += (*weights)[iX]*(*weights)[iX];
        sumX  += (*xvals)[iX]*(*weights)[iX];
        sumX2 += (*xvals)[iX]*(*xvals)[iX]*(*weights)[iX];
        orderedData.emplace_back((*xvals)[iX],  (*weights)[iX]);
    }

    const double sigma = std::sqrt( sumX2/sumW - (sumX*sumX)/(sumW*sumW)  );

    //compute IQR
    std::sort(orderedData.begin(),orderedData.end(),
            [](const std::pair<float,float> a, std::pair<float,float> b ) {return a.first < b.first;}
    );
    bool foundQ1 = false;
    double q1  = 0;
    double q3  = 0;
    double runningSum = 0;
    for(unsigned int iX = 0; iX < nDataPts; ++iX){
        runningSum += orderedData[iX].second;

        if(!foundQ1 && runningSum >= sumW/4. ){
            q1 = orderedData[iX].first;
            foundQ1 = true;
        }
        if(runningSum >= sumW*3/4. ){
            q3 =  orderedData[iX].first;
            break;
        }
    }
    if(q3 <= q1) q3 = orderedData.back().first;
    if(q3 <= q1) q1 = orderedData.front().first;

    const double IQR = q3 - q1;

    //Following silverman (1986): Density estimations for statistics and data analysis
    //For optimal bandwidth by using both the std dev and the IQR
    // pg 48
    double A = std::min(sigma, IQR/1.34);
    if(A <= 0){
        std::cout << "Could not find a good global spread estimate!!!"<<std::endl;
        A = 1;
    }

    //correction because we are using weighted events
    const double effNevnts = sumW*sumW/sumW2;

    h0 = hSF*0.9*A*std::pow(1./effNevnts, 0.2);
    oneOverh0 = 1./h0;
    oneOverTwoNegh0Sq = -0.5/(h0*h0);
    nEvt = sumW;
    oneOverNEvt = 1./nEvt;

    //Debugging output:
//    std::cout <<"Nvt: "<< nDataPts << " SumW: "<< nEvt <<" stdDev: " << sigma <<" (q1,q3,IQR): ("<< q1<<","<<q3<<","<<IQR
//            <<") A: "<< A << " nEff: "<< effNevnts <<" h0: "<<h0<<std::endl;
}
//--------------------------------------------------------------------------------------------------
void KDEProducer::setupPilotKDE(const unsigned int nXBins, const double xMin, const double xMax) {
    //For speed, the pilot density will be computed
    //in a grid, set by the histogram bin centers
    pilotKDE.reset(getPDF("pilotKDE","; pilot KDE",nXBins,xMin,xMax));
}
//--------------------------------------------------------------------------------------------------
void KDEProducer::setupPilotKDE(const unsigned int nXBins, const double* xBins) {
    //For speed, the pilot density will be computed
    //in a grid, set by the histogram bin centers
    pilotKDE.reset(getPDF("pilotKDE","; pilot KDE",nXBins,xBins));
}
//--------------------------------------------------------------------------------------------------
void KDEProducer::buildPilotKDE(const double trimFactor) {
//    localVar.reset(getLocalVariance("localSigma",";local sigma",nXBins,xMin,xMax));
    //store the inverse of the bandwidths for spead
    inv_his.reset(new std::vector<double> );
    inv_his->reserve(nDataPts);

    //for calculating median h
    std::vector< std::pair<float,float> > orderedData; orderedData.reserve(nDataPts);

    for(unsigned int iX = 0; iX < nDataPts; ++iX){
        double dens = pilotKDE->Interpolate((*xvals)[iX]);
//        double sigma = localVar->Interpolate((*xvals)[iX]);
//        if(doSigmaScaling && sigma) dens *=  (sigma);
        inv_his->emplace_back(  dens );
        orderedData.emplace_back(dens,  (*weights)[iX]);
    }

    //trim the densities to put a max on the bandwidths
    if(trimFactor > 0){
        //first calculate the median
        std::sort(orderedData.begin(),orderedData.end(),
                [](const std::pair<float,float> a, std::pair<float,float> b ) {return a.first < b.first;}
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
    const double geoMean = geometricMean();
    //get the final bandwidths by multiplying by h0*g...do the inversion now
    const double inv_bandNorm = oneOverh0*std::sqrt(1./geoMean);


//    for(unsigned int iX = 0; iX < nDataPts; ++iX){
//        std::cout <<"("<< (*xvals)[iX]<<","<<  (*inv_his)[iX] <<","<<inv_bandNorm*std::sqrt((*inv_his)[iX])<<","<< 1/(inv_bandNorm*std::sqrt((*inv_his)[iX]))<<") ";
//    }
    for(auto& d : *inv_his) {
        d = inv_bandNorm*std::sqrt(d);
    }

    //Debugging output:
//    std::cout <<"geoMean: "<<geoMean<< nDataPts << " NormFactor: "<< 1./inv_bandNorm <<std::endl;


}
//--------------------------------------------------------------------------------------------------
double KDEProducer::getADensity(const double x, double* weight) const {
    double dens = 0;
    double sumW2 =0;
    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
        const double delt = (x - (*xvals)[iX]);
        const double wt = (*weights)[iX]*(*inv_his)[iX]*std::exp(-0.5*delt*delt*(*inv_his)[iX]*(*inv_his)[iX]);
        dens += wt;
        sumW2 += wt*wt;
    }
    if(weight) (*weight) = oneOverRootTwoPi*oneOverNEvt*std::sqrt(sumW2);
    return dens*oneOverRootTwoPi*oneOverNEvt;
}
//--------------------------------------------------------------------------------------------------
// Only to be used when *inv_hist* is f(x)
double KDEProducer::geometricMean() const
{
    double sumTerms = 0;
    for(unsigned int iX = 0; iX < nDataPts; ++iX) sumTerms += (*weights)[iX]*std::log((*inv_his)[iX]);
    return std::exp(sumTerms*oneOverNEvt);
}
//--------------------------------------------------------------------------------------------------
double KDEProducer::getDensity(const double x, double* weight) const {
    double dens = 0;
    double sumW2 =0;
    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
        const double delt = (x - (*xvals)[iX]);
        const double wt = (*weights)[iX]*std::exp(oneOverTwoNegh0Sq*delt*delt);
        dens += wt;
        sumW2 += wt*wt;
    }
    if(weight) (*weight) = oneOverRootTwoPi*oneOverh0*oneOverNEvt*std::sqrt(sumW2);
    return dens*oneOverRootTwoPi*oneOverh0*oneOverNEvt;
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer::getPDF(const std::string& name, const std::string& title,
        const int unsigned nBins, const float xMin, const float xMax) const {
    TH1 * h = new TH1F(name.c_str(), title.c_str(), nBins,xMin,xMax);
    fillPDF(h,false);
    return h;
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer::getPDF(const std::string& name, const std::string& title,
        const int unsigned nBins, const double* bins) const {
    TH1 * h = new TH1F(name.c_str(), title.c_str(), nBins,bins);
    fillPDF(h,false);
    return h;
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer::getAPDF(const std::string& name, const std::string& title,
        const unsigned int nBins, const float xMin, const float xMax) const {
    TH1 * h = new TH1F(name.c_str(), title.c_str(), nBins,xMin,xMax);
    fillPDF(h,true);
    return h;
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer::getAPDF(const std::string& name, const std::string& title,
        const unsigned int nBins, const double* bins) const {
    TH1 * h = new TH1F(name.c_str(), title.c_str(), nBins,bins);
    fillPDF(h,true);
    return h;
}
//--------------------------------------------------------------------------------------------------
void KDEProducer::fillPDF(TH1* newHist,  const bool adaptive) const {
    double weight;
    for(int iB = 1; iB <= newHist->GetNbinsX(); ++iB){
        newHist->SetBinContent(iB,adaptive ? getADensity(newHist->GetBinCenter(iB),&weight)
                :getDensity(newHist->GetBinCenter(iB),&weight));
        newHist->SetBinError(iB,weight);
    }
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer::convToHist(const TH1 * hist) const {
    TH1* oH = (TH1*)hist->Clone();
    for(int iB = 0; iB <= hist->GetNbinsX(); ++iB)
        oH->SetBinContent(iB,hist->GetBinWidth(iB)*hist->GetBinContent(iB));
    return oH;
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer::getPilotPDF() const {
    TH1* oH = (TH1*)pilotKDE->Clone();
    return oH;
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer::getABandwidths(const std::string& name, const std::string& title,
        const unsigned int nBins, const float xMin, const float xMax) const {
    TH1 * h = new TH1F(name.c_str(), title.c_str(), nBins,xMin,xMax);
    fillABandwidths(h);
    return h;
}
//--------------------------------------------------------------------------------------------------
TH1* KDEProducer::getABandwidths(const std::string& name, const std::string& title,
        const unsigned int nBins, const double* bins) const {
    TH1 * h = new TH1F(name.c_str(), title.c_str(), nBins,bins);
    fillABandwidths(h);
    return h;
}
//--------------------------------------------------------------------------------------------------
void KDEProducer::fillABandwidths(TH1* newHist) const {
    const int nBins = newHist->GetNbinsX();
    std::vector<float> sumWs (nBins+1,0);
    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
        const int bin = newHist->FindFixBin((*xvals)[iX]);
        if(bin < 1 || bin > nBins) continue;
        newHist->SetBinContent(bin, newHist->GetBinContent(bin) + (*weights)[iX]/(*inv_his)[iX]);
        sumWs[bin] += (*weights)[iX] ;
    }
    for(int iB = 1; iB <= newHist->GetNbinsX(); ++iB)
        if(sumWs[iB])
            newHist->SetBinContent(iB, newHist->GetBinContent(iB)/sumWs[iB] );
}
////--------------------------------------------------------------------------------------------------
//double KDEProducer::getLocalVariance(const double x) const {
//    double sumW = 0;
//    double sumX = 0;
//    double sumX2 = 0;
//    int nEvt = 0;
//    for(unsigned int iX = 0; iX < nDataPts; ++iX ){
//        if(std::fabs(x - (*xvals)[iX]) > h0   ) continue;
//        nEvt++;
//        sumX += (*weights)[iX]*(*xvals)[iX];
//        sumX2 += (*weights)[iX]*(*xvals)[iX]*(*xvals)[iX];
//        sumW  += (*weights)[iX];
//    }
//    const double var = sumX2/sumW - (sumX*sumX)/(sumW*sumW);
//    return nEvt > 100 && sumW > 0 && var > 0 ? var : 0;
//
//}
////--------------------------------------------------------------------------------------------------
//TH1 *  KDEProducer::getLocalVariance(const std::string& name, const std::string& title,
//        const unsigned int nBins, const float xMin, const float xMax) const {
//    TH1 * h = new TH1F(name.c_str(), title.c_str(), nBins,xMin,xMax);
//    for(unsigned int iB = 1; iB <= nBins; ++iB)
//        h->SetBinContent(iB,getLocalVariance(h->GetBinCenter(iB)));
//    for(unsigned int iB = 2; iB <= nBins; ++iB)
//        if(h->GetBinContent(iB) == 0) h->SetBinContent(iB,h->GetBinContent(iB-1));
//    for(unsigned int iB = nBins -1; iB > 0; --iB)
//        if(h->GetBinContent(iB) == 0) h->SetBinContent(iB,h->GetBinContent(iB+1));
//    return h;
//}
