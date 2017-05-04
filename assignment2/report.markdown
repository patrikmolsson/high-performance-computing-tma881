---
title: Assignment 2
author:
 - Jakob Lindqvist
 - Per Nilsson Lundberg
 - Patrik Olsson
 - Hampus Toren
date: May 5th 2017
---

# Program layout

## Definitions
Variables:
 - Number of lines: $l$
 - Polynomial degree: $d$
 -  Number of threads: $t$

## Newton method

Being the driving algorithm in the program. A newton iterate is called the mean number of iterations times $l^2$ every execution meaning that small improvements yield large time saves. We do not involve anyC libraries that handle complex numbers but instead treat the complex calculations in house, as it were. To meet the performance demands we hard coded degrees 1 and 2 as these simplify significantly for a non general $d$. $d$ = 1 renders a trivial iteration which converges to the only root regardless of starting point. The second degree polynomial results in an independent scaling of the real and imaginary parts. For larger $d$ we used a general iteration method combining polar and rectangular representation in an effort to reduce operations.

We actually tried to split the calculation of the magnitude in two lines as to assure an integer exponent in the `pow` function. This was however slower, probably because the compiler could not infer any integer assumptions. 

## Writing files

Since we are limited to $d$ $<$ 7 we have seven roots which means we only need eight colours (one for each root and one for non-convergent starting points). To achieve this quick and simple we create an array of strings containing the binary representation of zero to $d$ as strings and use these as colour encodings. When the newton method is finished (converged or aborted) we do not store it but rather write the colour directly to a string that will be written to file. The same goes for the number of iterations which is directly written to a string.

These strings are then written to the respective file. We implemented this with the `fprintf` before we new that this was notoriously slow. However, due to parallelisation (discussed below) this is not very significant to the performance.

## Parallelisation





