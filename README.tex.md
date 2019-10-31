#HepKDE 
##A KDE framework for High Energy Physics 

HepKDE is a tool to quickly apply Kernel Density Estimation (KDE) in either one or two dimensions. It is extremely light weight and only requires ROOT: 

https://root.cern.ch 

##Information on  KDE

A KDE is a non-parametric univariate or multivariate probability density estimation constructed from an input dataset. KDE is a frequently used method, and a detailed description by Silverman (citation below). A short summary, following the convention of this reference follows. The univariate estimation $\hat{f}(x)$ of a distribution $f(x)$ is:

\begin{equation}
\hat{f}(x)=\frac{1}{nh}\sum_{i=1}^{n} \text{K}(\frac{x-\text{X}_{i}}{h}).
\end{equation}