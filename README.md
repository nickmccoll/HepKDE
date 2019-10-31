#HepKDE  
##A KDE framework for High Energy Physics  

HepKDE is a tool to quickly apply Kernel Density Estimation (KDE) in either one or two dimensions. It is extremely light weight and only requires ROOT:  

https://root.cern.ch 

##Information on  KDE  

A KDE is a non-parametric univariate or multivariate probability density estimation constructed from an input dataset. KDE is a frequently used method, and a detailed description by Silverman (citation below). A short summary, following the convention of this reference follows. The univariate estimation <img src="/tex/e83f9bd3b587873c61cc583202b96fdb.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=31.50689519999998pt/> of a distribution <img src="/tex/7997339883ac20f551e7f35efff0a2b9.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=24.65753399999998pt/> is:  

<p align="center"><img src="/tex/4058310a27077ed377de644dd5d4f319.svg?invert_in_darkmode&sanitize=true" align=middle width=444.03313185pt height=44.89738935pt/></p>

The sum is over <img src="/tex/55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode&sanitize=true" align=middle width=9.86687624999999pt height=14.15524440000002pt/> events in an unweighted input dataset with values <img src="/tex/8f6f3a4b5e21eb8c4a4c3af95fed3a15.svg?invert_in_darkmode&sanitize=true" align=middle width=16.979691299999992pt height=22.465723500000017pt/> of variable <img src="/tex/332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode&sanitize=true" align=middle width=9.39498779999999pt height=14.15524440000002pt/>. The kernel <img src="/tex/ecc2b8179cb4e68201a28cca24ee1948.svg?invert_in_darkmode&sanitize=true" align=middle width=31.50695789999999pt height=24.65753399999998pt/> describes the smearing of each event with a width controlled by the bandwidth <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/>. The choice of <img src="/tex/ecc2b8179cb4e68201a28cca24ee1948.svg?invert_in_darkmode&sanitize=true" align=middle width=31.50695789999999pt height=24.65753399999998pt/> is typically not important, so a Gaussian is widely used:  

<p align="center"><img src="/tex/8602372ff08a49a2905e2d205c988886.svg?invert_in_darkmode&sanitize=true" align=middle width=416.87911814999995pt height=37.0017615pt/></p>

The choice of <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/>, however, is very important. A great deal of literature is dedicated to methods of automatically determining and optimizing it. We found that these methods are overly complicated or not suitable for uses in HEP, so we instead start with an "optimal" bandwidth <img src="/tex/3cb8f632046f54d8cb1fe943dcd14eaa.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/> that is further optimized by multiplying it by a constant. <img src="/tex/3cb8f632046f54d8cb1fe943dcd14eaa.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/> is obtained by minimizing the mean square error of <img src="/tex/e83f9bd3b587873c61cc583202b96fdb.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=31.50689519999998pt/>. In the case that <img src="/tex/7997339883ac20f551e7f35efff0a2b9.svg?invert_in_darkmode&sanitize=true" align=middle width=31.99783454999999pt height=24.65753399999998pt/> is normally distributed with standard deviation <img src="/tex/8cda31ed38c6d59d14ebefa440099572.svg?invert_in_darkmode&sanitize=true" align=middle width=9.98290094999999pt height=14.15524440000002pt/> it is:  

<p align="center"><img src="/tex/e436dd36c1bae8fe813964c411576959.svg?invert_in_darkmode&sanitize=true" align=middle width=438.81583019999994pt height=20.1205785pt/></p>

Silverman explains that this is not optimal for non-normally distributed distributions and instead proposes a compromise <img src="/tex/3cb8f632046f54d8cb1fe943dcd14eaa.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/>:  

<p align="center"><img src="/tex/91fcbf74b86cad0628e3d3628acd6724.svg?invert_in_darkmode&sanitize=true" align=middle width=471.31582245pt height=20.1205785pt/></p>

The interquartile range IQR is included because it is a better estimate of width in the case of bi-modal distributions. The <img src="/tex/8cda31ed38c6d59d14ebefa440099572.svg?invert_in_darkmode&sanitize=true" align=middle width=9.98290094999999pt height=14.15524440000002pt/> and IQR are both estimated with the input dataset. In all cases, we verify and obtain systematic uncertainties for our templates by comparing them to data, so while the the choice of <img src="/tex/2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode&sanitize=true" align=middle width=9.47111549999999pt height=22.831056599999986pt/> is important in determining the KDE, its choice is not taken as an additional uncertainty.  

This method can also be used with weighted input datasets. In that case the KDE is:  

<p align="center"><img src="/tex/7639b2e2169a07961b3bed146993554f.svg?invert_in_darkmode&sanitize=true" align=middle width=450.37066304999996pt height=44.89738935pt/></p>

for events with weights <img src="/tex/c2a29561d89e139b3c7bffe51570c3ce.svg?invert_in_darkmode&sanitize=true" align=middle width=16.41940739999999pt height=14.15524440000002pt/> and a a weighted event yield <img src="/tex/70a8c57f1d90b3fa484ba01a24b9626f.svg?invert_in_darkmode&sanitize=true" align=middle width=90.41193314999998pt height=26.438629799999987pt/>. <img src="/tex/3cb8f632046f54d8cb1fe943dcd14eaa.svg?invert_in_darkmode&sanitize=true" align=middle width=28.42098929999999pt height=22.831056599999986pt/> can also be extended for weighted event samples by changing <img src="/tex/55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode&sanitize=true" align=middle width=9.86687624999999pt height=14.15524440000002pt/> in eq.~\ref{eq:hopt}:  