#ifndef ANALYSISSUPPORT_UTILITIES_INTERFACE_KDEProducer2D_H_
#define ANALYSISSUPPORT_UTILITIES_INTERFACE_KDEProducer2D_H_
#include <vector>
#include <string>
#include <memory>

class TH1;
class TH2;

class KDEProducer2D {
public:
    // hSF(x,y) -> external SF to apply to the bandwidth
    // The adaptive method includes a pilot KDE to estimate event density
    // nXBins, xMin, xMax / nYBins, yMin, yMax define the binning of the pilot estimate
    // nXBins <= 0 if you want a non-adaptive KDE
    // trimFactor -> How many factors above the median do you want the individual bandwidths to go ?
    // Trim higher ones to this value...negative for no trimming
    // doSigmaScaling -> apply variance scaling to the adaptive factors (depreciated)
    KDEProducer2D(const std::vector<double> * xvals,const std::vector<double> * yvals,
            const std::vector<double> * weights, const double hxSF, const unsigned int nXBins,
            const double xMin, const double xMax,const double trimFactorX, const double hySF,
            const unsigned int nYBins, const double yMin, const double yMax,const double trimFactorY
    );
    KDEProducer2D(const std::vector<double> * xvals,const std::vector<double> * yvals,
            const std::vector<double> * weights, const double hxSF, const unsigned int nXBins,
            const double xMin, const double xMax,const double trimFactorX, const double hySF,
            const unsigned int nYBins, const double* yBins,const double trimFactorY
    );
    ~KDEProducer2D() {};
    //get non adaptive density at point y
    double getDensity(const double x,const double y, double* weight=0)const ;
    //get adaptive density at point x
    double getADensity(const double x,const double y, double* weight=0) const;
    //Get local variance in one direction
//    double getLocalVarX(const double x, const double y)const ;
//    double getLocalVarY(const double x, const double y)const ;

    //All histograms will be owned by the caller
    //Non adaptive pdf
    TH2 * getPDF(const std::string& name, const std::string& title,
            const unsigned int nXBins, const float xMin, const float xMax,
            const unsigned int nYBins, const float yMin, const float yMax) const;
    TH2 * getPDF(const std::string& name, const std::string& title,
            const unsigned int nXBins, const float xMin, const float xMax,
            const unsigned int nYBins, const double* yBins) const;
    //adaptive pdf
    TH2 * getAPDF(const std::string& name, const std::string& title,
            const unsigned int nXBins, const float xMin, const float xMax,
            const unsigned int nYBins, const float yMin, const float yMax)const;
    TH2 * getAPDF(const std::string& name, const std::string& title,
            const unsigned int nXBins, const float xMin, const float xMax,
            const unsigned int nYBins, const double* yBins)const;
    //Multiply by bin widths to get a histogram
    TH1 * convToHist(const TH1 * hist) const;
    TH2 * convToHist(const TH2 * hist) const;
    //Get pilot PDF
    TH2 * getPilotPDF() const;
    //Get the average of adaptive bandwidths for the range defined by the histogram binning

    TH2 * getABandwidths(const std::string& name, const std::string& title,
            const unsigned int nXBins, const float xMin, const float xMax,
            const unsigned int nYBins, const float yMin, const float yMax, bool doX)const;
    TH2 * getABandwidths(const std::string& name, const std::string& title,
            const unsigned int nXBins, const float xMin, const float xMax,
            const unsigned int nYBins, const double* yBins, bool doX)const;
    //Get local variance histograms
//    TH2 *  getLocalVarX(const std::string& name, const std::string& title,
//            const unsigned int nXBins, const float xMin, const float xMax,
//            const unsigned int nYBins, const float yMin, const float yMax
//            )const ;
//    TH2 *  getLocalVarY(const std::string& name, const std::string& title,
//            const unsigned int nXBins, const float xMin, const float xMax,
//            const unsigned int nYBins, const float yMin, const float yMax
//            )const ;private:
private:
    //calculates nEvt and h0
    void computeGlobalQuantities(const double hSFX,const double hSFY);
    //For the first iteration
    void buildPilotKDE(const double trimFactorX,const double trimFactorY);
    //produce a set of inv_hs
    std::vector<double>* make_invHs(const double oneOverH0,const double trimFactor) const;
    void setupPilotKDE(const unsigned int nXBins, const double xMin, const double xMax,
            const unsigned int nYBins, const double yMin, const double yMax);
    void setupPilotKDE(const unsigned int nXBins, const double xMin, const double xMax,
            const unsigned int nYBins, const double* yBins);
    // weighted geometric mean
    double geometricMean(const std::vector<double> * vals) const;
    void fillPDF(TH2 * h,const bool adaptive)const;
    //Get the average of adaptive bandwidths for the range defined by the histogram binning
    void fillABandwidths(TH2 * h,const std::vector<double>* bands)const;

private:
    //Directly from input parameters
    const std::vector<double> * xvals;
    const std::vector<double> * yvals;
    const std::vector<double> * weights;
    const unsigned int nDataPts;

    //calculated in computeGlobalQuantities
//    double sigmaX        = 1;
//    double sigmaY        = 1;
    double h0X           = 1;
    double h0Y           = 1;
    double oneOverh0X    = 1;
    double oneOverh0Y    = 1;
    double oneOverTwoNegh0XSq   = 1;
    double oneOverTwoNegh0YSq   = 1;
    double nEvt = 1; //weighted
    double oneOverNEvt = 1;

    //calculated in buildPilotKDE
    std::unique_ptr<TH2> pilotKDE;
//    std::unique_ptr<TH2> localVarX;
//    std::unique_ptr<TH2> localVarY;
    std::unique_ptr<std::vector<double>> inv_hxis;
    std::unique_ptr<std::vector<double>> inv_hyis;

    //Const
    const double oneOverTwoPi;
//    const double oneOverRTTwoPi;

};
#endif /* FRAMEWORK_PROCESSORS_INTERFACE_EVENTWEIGHTS_H_ */
