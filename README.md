# HepKDE  
## A KDE framework for High Energy Physics  

HepKDE is a tool to quickly apply Kernel Density Estimation (KDE) in either one or two dimensions. It is extremely light weight and only requires ROOT:  

https://root.cern.ch 

KDE is a powerful tool, but a feature of HEP research is the large amount of data that must be processed. For this reason, we have implemented a version of KDE especially made for HEP. It features:

1. Optimizations and approximations made for in the large data sets found in HEP.
2. No "one-size-fits-all" optimizations, instead a focus on user control. This is what you want in research.
3. We built in the features needed to estimate distributions that can rane in orders of magnitude. This is very common in HEP.  

All you need to do is to include either header file to use the code. There is one version for 1D and another for 2D. It is a bit clunky with a repeated code. This was done on purpose to make it very easy for the user to understand what is happening in either use. More information on the paramters can be found in the "KDE Implementation" section. A general overview of KDE can be found in "General Information on  KDE."

If you want to try it out, first compile the library: 

```Shell
cd HepKDE
make
```

Then run a test:

```Shell
cd test
root
root [0] .L ../HepKDE.so
root [1] .x testMacro.C+
```

Here is the test output:

<img src="/test/testOutput.png">

With a dataset of 500 events, we were able to resonably estimate the true distribution down to 3 standard deviations. Not too shabby!



## KDE Implementation

The software implements KDE as described in the next section. The one and two dimension implementations are separate for efficiency. The input to the software is the per-event variable values, associated weights and the following parameters (one set of parameters per dimension):  

1. The binning of the pilot estimate in the case of an adaptive KDE
2. A global bandwidth scale factor <img src="/tex/94132bf9890691f4014b7993f16f2c05.svg?invert_in_darkmode&sanitize=true" align=middle width=19.673256899999988pt height=22.831056599999986pt/>
3. The bandwidth upper bound cutoff factor <img src="/tex/abb14cc3bfe27185df72f81288a37a2e.svg?invert_in_darkmode&sanitize=true" align=middle width=21.86844329999999pt height=22.831056599999986pt/> in the case of an adaptive KDE

The binning of the pilot estimate is necessary for efficiency. The most straight-forward method of implementing adaptive KDE is to obtain the contribution of each input event to the event density at each <img src="/tex/1338d1e5163ba5bc872f1411dd30b36a.svg?invert_in_darkmode&sanitize=true" align=middle width=18.269651399999987pt height=22.465723500000017pt/>, necessitating <img src="/tex/085dd6b72790ca14d8b2a56401086d6c.svg?invert_in_darkmode&sanitize=true" align=middle width=21.552516149999988pt height=26.76175259999998pt/> operations. This is unnecessarily precise as the adaptive estimate does not depend strongly on the precise form of the pilot estimate. Instead, the the event density is estimated only at the bin centers for the provided binning. The density is then interpolated between bins. 

The second two parameters control the bandwidth(s) of the method. The global bandwidth for either adaptive or non-adaptive KDE is the <img src="/tex/c50dd765889002ae04ee7c72bd5277b3.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/> defined in the previous section multiplied by <img src="/tex/94132bf9890691f4014b7993f16f2c05.svg?invert_in_darkmode&sanitize=true" align=middle width=19.673256899999988pt height=22.831056599999986pt/>. The <img src="/tex/abb14cc3bfe27185df72f81288a37a2e.svg?invert_in_darkmode&sanitize=true" align=middle width=21.86844329999999pt height=22.831056599999986pt/> is applied by finding the median of all <img src="/tex/ddd3bc35b936d6a00e6a81cab0061f32.svg?invert_in_darkmode&sanitize=true" align=middle width=14.12201339999999pt height=22.831056599999986pt/> (<img src="/tex/628ec1af4d42ef67c37dd38c0947ace8.svg?invert_in_darkmode&sanitize=true" align=middle width=33.375346949999994pt height=22.831056599999986pt/>) and setting all <img src="/tex/2a841dee4f4b5e36f7f5a391416855e3.svg?invert_in_darkmode&sanitize=true" align=middle width=108.45220814999999pt height=22.831056599999986pt/> to <img src="/tex/8385291a045ae36e173f19111c292f78.svg?invert_in_darkmode&sanitize=true" align=middle width=71.5906653pt height=22.831056599999986pt/>. 

The software will provide either a one or two dimensional histogram of the estimate, with or without adaptive smoothing. The user provides the binning of the output histogram, of which the bin contents are the values of the estimate at the bin center multiplied by the bin width.


## General Information on  KDE  

A KDE is a non-parametric univariate or multivariate probability density estimation constructed from an input dataset. KDE is a frequently used method, and a detailed description by Silverman (citation below). A short summary, following the convention of this reference follows. The univariate estimation <img src="/tex/e83f9bd3b587873c61cc583202b96fdb.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=31.50689519999998pt/> of a distribution <img src="/tex/7997339883ac20f551e7f35efff0a2b9.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=24.65753399999998pt/> is:  

<p align="center"><img src="/tex/c9fe42ce35db914ca5c52f563df1a473.svg?invert_in_darkmode&sanitize=true" align=middle width=187.792308pt height=44.89738935pt/></p>

The sum is over <img src="/tex/55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode&sanitize=true" align=middle width=9.86687624999999pt height=14.15524440000002pt/> events in an unweighted input dataset with values <img src="/tex/8f6f3a4b5e21eb8c4a4c3af95fed3a15.svg?invert_in_darkmode&sanitize=true" align=middle width=16.979691299999992pt height=22.465723500000017pt/> of variable <img src="/tex/332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode&sanitize=true" align=middle width=9.39498779999999pt height=14.15524440000002pt/>. The kernel <img src="/tex/ecc2b8179cb4e68201a28cca24ee1948.svg?invert_in_darkmode&sanitize=true" align=middle width=31.50695789999999pt height=24.65753399999998pt/> describes the smearing of each event with a width controlled by the bandwidth <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/>. The choice of <img src="/tex/ecc2b8179cb4e68201a28cca24ee1948.svg?invert_in_darkmode&sanitize=true" align=middle width=31.50695789999999pt height=24.65753399999998pt/> is typically not important, so a Gaussian is widely used:  

<p align="center"><img src="/tex/0ee22a26b3b677b321ea5122e7831bba.svg?invert_in_darkmode&sanitize=true" align=middle width=133.4842971pt height=37.0017615pt/></p>

The choice of <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/>, however, is very important. A great deal of literature is dedicated to methods of automatically determining and optimizing it. We found that these methods are overly complicated or not suitable for uses in HEP, so we instead start with an "optimal" bandwidth <img src="/tex/3cb8f632046f54d8cb1fe943dcd14eaa.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/> that is further optimized by multiplying it by a constant. <img src="/tex/3cb8f632046f54d8cb1fe943dcd14eaa.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/> is obtained by minimizing the mean square error of <img src="/tex/e83f9bd3b587873c61cc583202b96fdb.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=31.50689519999998pt/>. In the case that <img src="/tex/7997339883ac20f551e7f35efff0a2b9.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=24.65753399999998pt/> is normally distributed with standard deviation <img src="/tex/8cda31ed38c6d59d14ebefa440099572.svg?invert_in_darkmode&sanitize=true" align=middle width=9.98290094999999pt height=14.15524440000002pt/> it is:  

<p align="center"><img src="/tex/99a993ce43a4e95aff0a41877c7726a3.svg?invert_in_darkmode&sanitize=true" align=middle width=177.3577311pt height=20.1205785pt/></p>

Silverman explains that this is not optimal for non-normally distributed distributions and instead proposes a compromise <img src="/tex/3cb8f632046f54d8cb1fe943dcd14eaa.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/>:  

<p align="center"><img src="/tex/905c711f93a7476848b1e795c8120f73.svg?invert_in_darkmode&sanitize=true" align=middle width=242.3577651pt height=20.1205785pt/></p>

The interquartile range IQR is included because it is a better estimate of width in the case of bi-modal distributions. The <img src="/tex/8cda31ed38c6d59d14ebefa440099572.svg?invert_in_darkmode&sanitize=true" align=middle width=9.98290094999999pt height=14.15524440000002pt/> and IQR are both estimated with the input dataset. In all cases, we verify and obtain systematic uncertainties for our templates by comparing them to data, so while the the choice of <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/> is important in determining the KDE, its choice is not taken as an additional uncertainty.  

This method can also be used with weighted input datasets. In that case the KDE is:  

<p align="center"><img src="/tex/f8096cfb44ed6814e80629e3c71260a5.svg?invert_in_darkmode&sanitize=true" align=middle width=200.46738854999998pt height=44.89738935pt/></p>

for events with weights <img src="/tex/c2a29561d89e139b3c7bffe51570c3ce.svg?invert_in_darkmode&sanitize=true" align=middle width=16.41940739999999pt height=14.15524440000002pt/> and a a weighted event yield <img src="/tex/70a8c57f1d90b3fa484ba01a24b9626f.svg?invert_in_darkmode&sanitize=true" align=middle width=90.41193314999998pt height=26.438629799999987pt/>. <img src="/tex/3cb8f632046f54d8cb1fe943dcd14eaa.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/> can also be extended for weighted event samples by changing <img src="/tex/55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode&sanitize=true" align=middle width=9.86687624999999pt height=14.15524440000002pt/> in the equation for <img src="/tex/c50dd765889002ae04ee7c72bd5277b3.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/>.  

<p align="center"><img src="/tex/7fbfaf7acad88ca330f6cc8b0c3731f2.svg?invert_in_darkmode&sanitize=true" align=middle width=105.74220029999998pt height=40.9042854pt/></p>

With this form, <img src="/tex/c50dd765889002ae04ee7c72bd5277b3.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/> minimizes the mean square error of <img src="/tex/e83f9bd3b587873c61cc583202b96fdb.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=31.50689519999998pt/> in the case of normally distributed distributions for both weighted or unweighted input datasets.  

KDE can also be used for weighted data with <img src="/tex/2103f85b8b1477f430fc407cad462224.svg?invert_in_darkmode&sanitize=true" align=middle width=8.55596444999999pt height=22.831056599999986pt/> dimensions:  

<p align="center"><img src="/tex/2a450ee91d16414f263b639790e1444a.svg?invert_in_darkmode&sanitize=true" align=middle width=260.3601363pt height=50.1713685pt/></p>

<img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/> is much harder to optimize in multiple dimensions than in a single dimension, so we once again take a reasonable, initial value that is then multiplied by a constant for optimization. We simplify the approach proposed by Silverman, using an independent <img src="/tex/679d4610c033345713a1fcdfb7669b7b.svg?invert_in_darkmode&sanitize=true" align=middle width=15.57562379999999pt height=22.831056599999986pt/> in each dimension, but retaining his formula for its dependence on the event count after extending it for use with weighted input datasets:  

<p align="center"><img src="/tex/3577aa561229d39b7a799690c15591ba.svg?invert_in_darkmode&sanitize=true" align=middle width=218.72342744999997pt height=40.9042854pt/></p>

While this formulation is simple, it is not ideal for estimating distributions that span many orders of magnitude in event density. In such cases, a more optimal procedure would involve a smaller <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/> at high event density, where we care more about the fine structure of the distribution, and a smaller <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/> at low event density, where the statistical uncertainty of the input dataset is high. This is precisely the problem that adaptive KDE attempts to solve. For simplicity, we will describe univariate, unweighted KDE, but it is easily extended to other cases following the above arguments.  

As is described in Silverman, the global <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/> is replaced with a per event <img src="/tex/ddd3bc35b936d6a00e6a81cab0061f32.svg?invert_in_darkmode&sanitize=true" align=middle width=14.12201339999999pt height=22.831056599999986pt/>:  

<p align="center"><img src="/tex/c89ab909491401b9c363470a9228d80c.svg?invert_in_darkmode&sanitize=true" align=middle width=197.21030009999998pt height=44.89738935pt/></p>

with <img src="/tex/ddd3bc35b936d6a00e6a81cab0061f32.svg?invert_in_darkmode&sanitize=true" align=middle width=14.12201339999999pt height=22.831056599999986pt/>:  

<p align="center"><img src="/tex/abb4d4a2e9e9dfdbe493ff4f00e4cdbf.svg?invert_in_darkmode&sanitize=true" align=middle width=125.76228554999999pt height=35.8159494pt/></p> 


The local bandwidth has two parameters: a global bandwidth <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/>, which controls the overall smearing of the estimate, and a pilot <img src="/tex/1be415bedf77d1dc80452b7e1a1146b7.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=30.632847300000012pt/>, which provides an estimate of the sample event density at points <img src="/tex/9fc20fb1d3825674c6a279cb0d5ca636.svg?invert_in_darkmode&sanitize=true" align=middle width=14.045887349999989pt height=14.15524440000002pt/>. <img src="/tex/3cf4fbd05970446973fc3d9fa3fe3c41.svg?invert_in_darkmode&sanitize=true" align=middle width=8.430376349999989pt height=14.15524440000002pt/> is the geometric mean of the <img src="/tex/86ce7c0c40214ab21dedd7acc7bf83e6.svg?invert_in_darkmode&sanitize=true" align=middle width=37.47063044999999pt height=30.632847300000012pt/>. It is included so that the scale of the bandwidth is controlled by <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/>. The value of <img src="/tex/d5d5564ce0bb9999695f32da6ba7af42.svg?invert_in_darkmode&sanitize=true" align=middle width=24.657628049999992pt height=24.65753399999998pt/> in the exponent is included to reduce the bias in <img src="/tex/e83f9bd3b587873c61cc583202b96fdb.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=31.50689519999998pt/> (see the text for more information). <img src="/tex/e83f9bd3b587873c61cc583202b96fdb.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=31.50689519999998pt/> does not depend strongly on the accuracy of <img src="/tex/86ce7c0c40214ab21dedd7acc7bf83e6.svg?invert_in_darkmode&sanitize=true" align=middle width=37.47063044999999pt height=30.632847300000012pt/>, so we simply use the the non-adaptive estimate with bandwidth <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/> for it.  

A useful overview of adaptive KDE by Scott (reference below). He describes a possible pitfall in the method: it can over-smooth distribution tails. A solution that he proposes is to set an upper-bound on the <img src="/tex/ddd3bc35b936d6a00e6a81cab0061f32.svg?invert_in_darkmode&sanitize=true" align=middle width=14.12201339999999pt height=22.831056599999986pt/>.  


## Citations

Most of this code uses ROOT, a tool used widely in HEP. More information:

https://root.cern.ch 

Silverman is great textbook on KDE. Most of the treatment discussed on this page can be found in much more detail here:

Silverman, Bernard W., "Density estimation for statistics and data analysis." Chapman and Hall (1986)

More information about the pitfalls of adaptive KDE can be found in Scott: 

Scott, David W., "Multivariate density estimation: theory, practice, and visualization."  John Wiley and Sons (1992)  

