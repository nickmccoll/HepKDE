#HepKDE  
##A KDE framework for High Energy Physics  

HepKDE is a tool to quickly apply Kernel Density Estimation (KDE) in either one or two dimensions. It is extremely light weight and only requires ROOT:  

https://root.cern.ch 

##Information on  KDE  

A KDE is a non-parametric univariate or multivariate probability density estimation constructed from an input dataset. KDE is a frequently used method, and a detailed description by Silverman (citation below). A short summary, following the convention of this reference follows. The univariate estimation $\hat{f}(x)$ of a distribution $f(x)$ is:  

\begin{equation}
\hat{f}(x)=\frac{1}{nh}\sum_{i=1}^{n} \text{K}(\frac{x-\text{X}_{i}}{h}).  
\end{equation}

The sum is over $n$ events in an unweighted input dataset with values $\text{X}_i$ of variable $x$. The kernel $\text{K}(t)$ describes the smearing of each event with a width controlled by the bandwidth $h$. The choice of $\text{K}(t)$ is typically not important, so a Gaussian is widely used:  

\begin{equation}
\text{K}(t)=\frac{1}{\sqrt{2\pi}}e^{-\frac{1}{2}t^2}.  
\end{equation}

The choice of $h$, however, is very important. A great deal of literature is dedicated to methods of automatically determining and optimizing it. We found that these methods are overly complicated or not suitable for uses in HEP, so we instead start with an "optimal" bandwidth $h_{\text{opt}}$ that is further optimized by multiplying it by a constant. $h_{\text{opt}}$ is obtained by minimizing the mean square error of $\hat{f}(x)$. In the case that $f(x)$ is normally distributed with standard deviation $\sigma$ it is:  

\begin{equation}
h_\text{opt, norm.}=1.06\sigma n^{-1/5}.  
\end{equation}

Silverman explains that this is not optimal for non-normally distributed distributions and instead proposes a compromise $h_{\text{opt}}$:  

\begin{equation} \label{eq:hopt}
h_\text{opt}=0.9\text{min}(\sigma,\text{IQR}/1.34)n^{-1/5}.  
\end{equation}

The interquartile range IQR is included because it is a better estimate of width in the case of bi-modal distributions. The $\sigma$ and IQR are both estimated with the input dataset. In all cases, we verify and obtain systematic uncertainties for our templates by comparing them to data, so while the the choice of $h$ is important in determining the KDE, its choice is not taken as an additional uncertainty.  

This method can also be used with weighted input datasets. In that case the KDE is:  

\begin{equation}
\hat{f}(x)=\frac{1}{\tilde{n}h}\sum_{i=1}^{n} w_{i} \text{K}(\frac{x-\text{X}_{i}}{h})  
\end{equation}

for events with weights $w_i$ and a a weighted event yield $\tilde{n}=\sum_{i=1}^n w_{i}$. $h_{\text{opt}}$ can also be extended for weighted event samples by changing $n$ in eq.~\ref{eq:hopt}:  