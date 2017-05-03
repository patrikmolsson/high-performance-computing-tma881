---
Title: Assignment 2
Authors:
 - Jakob Lindqvist
 - Per Nilsson Lundberg
 - Patrik Olsson
date: May 5th 2017
---

# Program layout

## Definitions
Variables:
 - Number of lines: $l$
 - Polynomial degree: $d$
 -  Number of threads: $t$

## Newton method

Being the driving algorithm in the program. A newton iterate is called the mean number of iterations times $ l^2 $ every execution meaning that small improvements yield large time saves. We do not involve anyC libraries that handle complex numbers but instead treat the complex calculations in house, as it were. To meet the performance demands we hard coded degrees 1 and 2 as these simplify significantly for a non general $d$. $d=1$ renders a trivial iteration which converges to the only root regardless of starting point. The second degree polynomial results in an independent scaling of the real and imaginary parts. For larger $d$ we used a general iteration method combining polar and rectangular representation in an effort to reduce operations.

We actually tried to split the calculation of the magnitude in two lines as to assure an integer exponent in the `pow()` functions.  
## Writing files
