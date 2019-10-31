# HepKDE  
## A KDE framework for High Energy Physics  

HepKDE is a tool to quickly apply Kernel Density Estimation (KDE) in either one or two dimensions. It is extremely light weight and only requires ROOT:  

https://root.cern.ch 

KDE is a powerful tool, but a feature of HEP research is the large amount of data that must be processed. For this reason, we have implemented a version of KDE especially made for HEP. It features:

1. Optimizations and approximations made for in the large data sets found in HEP.
2. No "one-size-fits-all" optimizations, instead a focus on user control. This is what you want in research.
3. We built in the features needed to estimate distributions that can rane in orders of magnitude. This is very common in HEP.  

All you need to do is to include either header file to use the code. There is one version for 1D and another for 2D. It is a bit clunky with a repeated code. This was done on purpose to make it very easy for the user to understand what is happening in either use. More information on the paramters can be found in the "KDE Implementation" section. A general overview of KDE can be found in "General Information on  KDE."


## KDE Implementation

The software implements KDE as described in the next section. The one and two dimension implementations are separate for efficiency. The input to the software is the per-event variable values, associated weights and the following parameters (one set of parameters per dimension):  

1. The binning of the pilot estimate in the case of an adaptive KDE
2. A global bandwidth scale factor $h_{\text{sf}}$
3. The bandwidth upper bound cutoff factor $h_{\text{co}}$ in the case of an adaptive KDE

The binning of the pilot estimate is necessary for efficiency. The most straight-forward method of implementing adaptive KDE is to obtain the contribution of each input event to the event density at each $X_i$, necessitating $N^{2}$ operations. This is unnecessarily precise as the adaptive estimate does not depend strongly on the precise form of the pilot estimate. Instead, the the event density is estimated only at the bin centers for the provided binning. The density is then interpolated between bins. 

The second two parameters control the bandwidth(s) of the method. The global bandwidth for either adaptive or non-adaptive KDE is the $h_\text{opt}$ defined in the previous section multiplied by $h_{\text{sf}}$. The $h_{\text{co}}$ is applied by finding the median of all $h_i$ ($h_\text{med}$) and setting all $h_i > h_{\text{co}}*h_\text{med}$ to $h_{\text{co}}*h_\text{med}$. 

The software will provide either a one or two dimensional histogram of the estimate, with or without adaptive smoothing. The user provides the binning of the output histogram, of which the bin contents are the values of the estimate at the bin center multiplied by the bin width.


## General Information on  KDE  

A KDE is a non-parametric univariate or multivariate probability density estimation constructed from an input dataset. KDE is a frequently used method, and a detailed description by Silverman (citation below). A short summary, following the convention of this reference follows. The univariate estimation $\hat{f}(x)$ of a distribution $f(x)$ is:  

\begin{equation*}
\hat{f}(x)=\frac{1}{nh}\sum_{i=1}^{n} \text{K}(\frac{x-\text{X}_{i}}{h}).  
\end{equation*}

The sum is over $n$ events in an unweighted input dataset with values $\text{X}_i$ of variable $x$. The kernel $\text{K}(t)$ describes the smearing of each event with a width controlled by the bandwidth $h$. The choice of $\text{K}(t)$ is typically not important, so a Gaussian is widely used:  

\begin{equation*}
\text{K}(t)=\frac{1}{\sqrt{2\pi}}e^{-\frac{1}{2}t^2}.  
\end{equation*}

The choice of $h$, however, is very important. A great deal of literature is dedicated to methods of automatically determining and optimizing it. We found that these methods are overly complicated or not suitable for uses in HEP, so we instead start with an "optimal" bandwidth $h_{\text{opt}}$ that is further optimized by multiplying it by a constant. $h_{\text{opt}}$ is obtained by minimizing the mean square error of $\hat{f}(x)$. In the case that $f(x)$ is normally distributed with standard deviation $\sigma$ it is:  

\begin{equation*}
h_\text{opt, norm.}=1.06\sigma n^{-1/5}.  
\end{equation*}

Silverman explains that this is not optimal for non-normally distributed distributions and instead proposes a compromise $h_{\text{opt}}$:  

\begin{equation*} \label{eq:hopt}
h_\text{opt}=0.9\text{min}(\sigma,\text{IQR}/1.34)n^{-1/5}.  
\end{equation*}

The interquartile range IQR is included because it is a better estimate of width in the case of bi-modal distributions. The $\sigma$ and IQR are both estimated with the input dataset. In all cases, we verify and obtain systematic uncertainties for our templates by comparing them to data, so while the the choice of $h$ is important in determining the KDE, its choice is not taken as an additional uncertainty.  

This method can also be used with weighted input datasets. In that case the KDE is:  

\begin{equation*}
\hat{f}(x)=\frac{1}{\tilde{n}h}\sum_{i=1}^{n} w_{i} \text{K}(\frac{x-\text{X}_{i}}{h})  
\end{equation*}

for events with weights $w_i$ and a a weighted event yield $\tilde{n}=\sum_{i=1}^n w_{i}$. $h_{\text{opt}}$ can also be extended for weighted event samples by changing $n$ in the equation for $h_\text{opt}$.  

\begin{equation*}
n \rightarrow \frac{\sum_{i=1}^n w_{i}}{\sum_{i=1}^n w_{i}^2}.  
\end{equation*}

With this form, $h_\text{opt}$ minimizes the mean square error of $\hat{f}(x)$ in the case of normally distributed distributions for both weighted or unweighted input datasets.  

KDE can also be used for weighted data with $d$ dimensions:  

\begin{equation*}
\hat{f}(\vec{x})= \frac{1}{\tilde{n}} \sum_{i=1}^{n} w_{i} \prod_{j=1}^d \frac{1}{h_{j}}\text{K}(\frac{x_{j}-\text{X}_{j,i}}{h_{j}}).  
\end{equation*}

$h$ is much harder to optimize in multiple dimensions than in a single dimension, so we once again take a reasonable, initial value that is then multiplied by a constant for optimization. We simplify the approach proposed by Silverman, using an independent $h_{j}$ in each dimension, but retaining his formula for its dependence on the event count after extending it for use with weighted input datasets:  

\begin{equation*}
h_{\text{opt},j}=\sigma_j(\frac{\sum_{i=1}^n w_{i}}{\sum_{i=1}^n w_{i}^2})^{-1/(d+4)}.  
\end{equation*}

While this formulation is simple, it is not ideal for estimating distributions that span many orders of magnitude in event density. In such cases, a more optimal procedure would involve a smaller $h$ at high event density, where we care more about the fine structure of the distribution, and a smaller $h$ at low event density, where the statistical uncertainty of the input dataset is high. This is precisely the problem that adaptive KDE attempts to solve. For simplicity, we will describe univariate, unweighted KDE, but it is easily extended to other cases following the above arguments.  

As is described in Silverman, the global $h$ is replaced with a per event $h_i$:  

\begin{equation*}
\hat{f}(x)=\frac{1}{n}\sum_{i=1}^{n} \frac{1}{h_i}\text{K}(\frac{x-\text{X}_{i}}{h_i}),  
\end{equation*}

with $h_i$:  

\begin{equation*} 
h_i=h(\frac{g}{\tilde{f}(x_i)})^{1/2}.  
\end{equation*} 


The local bandwidth has two parameters: a global bandwidth $h$, which controls the overall smearing of the estimate, and a pilot $\tilde{f}(x)$, which provides an estimate of the sample event density at points $x_i$. $g$ is the geometric mean of the $\tilde{f}(x_i)$. It is included so that the scale of the bandwidth is controlled by $h$. The value of $1/2$ in the exponent is included to reduce the bias in $\hat{f}(x)$ (see the text for more information). $\hat{f}(x)$ does not depend strongly on the accuracy of $\tilde{f}(x_i)$, so we simply use the the non-adaptive estimate with bandwidth $h$ for it.  

A useful overview of adaptive KDE by Scott (reference below). He describes a possible pitfall in the method: it can over-smooth distribution tails. A solution that he proposes is to set an upper-bound on the $h_i$.  


## Citations

Most of this code uses ROOT, a tool used widely in HEP. More information:

https://root.cern.ch 

Silverman is great textbook on KDE. Most of the treatment discussed on this page can be found in much more detail here:

Silverman, Bernard W., "Density estimation for statistics and data analysis." Chapman and Hall (1986)

More information about the pitfalls of adaptive KDE can be found in Scott: 

Scott, David W., "Multivariate density estimation: theory, practice, and visualization."  John Wiley and Sons (1992)  

