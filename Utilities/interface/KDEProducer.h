#ifndef ANALYSISSUPPORT_UTILITIES_INTERFACE_KDEProducer_H_
#define ANALYSISSUPPORT_UTILITIES_INTERFACE_KDEProducer_H_
#include <vector>
#include <string>
#include <memory>

class TH1;

class KDEProducer {
public:
    // hSF -> external SF to apply to the bandwidth
    // The adaptive method includes a pilot KDE to estimate event density
    // nXBins, xMin, xMax define the binning of the pilot estimate
    // nXBins <= 0 if you want a non-adaptive KDE
    // trimFactor -> How many factors above the median do you want the individual bandwidths to go ?
    // Trim higher ones to this value...negative for no trimming
    // doSigmaScaling -> apply variance scaling to the adaptive factors (depreciated)
    KDEProducer(const std::vector<double> * xvals, const std::vector<double> * weights,
            const double hSF, const unsigned int nXBins, const double xMin, const double xMax,
            const double trimFactor
    );
    KDEProducer(const std::vector<double> * xvals, const std::vector<double> * weights,
            const double hSF, const unsigned int nXBins, const double* xBins,
            const double trimFactor
    );

    double getH0() const {return h0;}

    //get non adaptive density at point x
    double getDensity(const double x, double* weight=0)const ;
    //get adaptive density at point x
    double getADensity(const double x, double* weight=0) const;
    //get local variance
//    double getLocalVariance(const double x)const ;

    //All histograms will be owned by the caller
    //Non adaptive pdf
    TH1 * getPDF(const std::string& name, const std::string& title, const unsigned int nBins,
            const float xMin, const float xMax) const;
    TH1 * getPDF(const std::string& name, const std::string& title, const unsigned int nBins,
            const double* bins) const;
    //adaptive pdf
    TH1 * getAPDF(const std::string& name, const std::string& title, const unsigned int nBins,
            const float xMin, const float xMax) const;
    TH1 * getAPDF(const std::string& name, const std::string& title, const unsigned int nBins,
            const double* bins) const;
    //Multiply by bin widths to get a histogram
    TH1 * convToHist(const TH1 * hist) const;
    //Get pilot PDF
    TH1 * getPilotPDF() const;
    //Get the average of adaptive bandwidths for the range defined by the histogram binning
    TH1 * getABandwidths(const std::string& name, const std::string& title,
            const unsigned int nBins, const float xMin, const float xMax) const;
    TH1 * getABandwidths(const std::string& name, const std::string& title,
            const unsigned int nBins, const double* bins) const;
    //Get local variance histogram
//    TH1 * getLocalVariance(const std::string& name, const std::string& title,
//    const unsigned int nBins, const float xMin, const float xMax) const;
private:
    //calculates nEvt and h0
    void computeGlobalQuantities(const double hSF);
    //For the first iteration
    void buildPilotKDE(const double trimFactor);
    void setupPilotKDE(const unsigned int nXBins, const double xMin, const double xMax);
    void setupPilotKDE(const unsigned int nXBins, const double* xbins);
    // weighted geometric mean
    double geometricMean() const;
    //Helper for functions
    void fillPDF(TH1* newHist, const bool adaptive) const;
    void fillABandwidths(TH1* newHist) const;
private:
    //Directly from input parameters
    const std::vector<double> * xvals;
    const std::vector<double> * weights;
    const unsigned int nDataPts;

    //calculated in computeGlobalQuantities
    double h0            = 1;
    double oneOverh0     = 1;
    double oneOverTwoNegh0Sq   = 1;
    double nEvt = 1; //weighted
    double oneOverNEvt = 1;

    //calculated in buildPilotKDE
    std::unique_ptr<TH1> pilotKDE;
//    std::unique_ptr<TH1> localVar;
    std::unique_ptr<std::vector<double>> inv_his;

    //Const
    const double oneOverRootTwoPi;

};
#endif /* FRAMEWORK_PROCESSORS_INTERFACE_EVENTWEIGHTS_H_ */
