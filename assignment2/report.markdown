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

Being the driving algorithm in the program. A newton iterate is called the mean number of iterations times  $l^2$ every execution meaning that small improvements yield large time saves. We do not involve any C libraries that handle complex numbers but instead treat the complex calculations in house, as it were. To meet the performance demands we hard coded degrees 1 and 2 as these simplify significantly for a non general $d$. $d=1$ renders a trivial iteration which converges to the only root regardless of starting point. The second degree polynomial results in an independent scaling of the real and imaginary parts. For larger $d$ we used a general iteration method combining polar and rectangular representation in an effort to reduce operations.

We actually tried to split the calculation of the magnitude in two lines as to assure an integer exponent in the `pow` function. This was however slower, probably because the compiler could not infer any integer assumptions. Because of this we implemented our own power algorithm which given $d$ performs repeated self multiplication until the correct exponent is reached; a boolean is introduced to include or omit an extra square root multiplication if $d$ is even or odd. As expected, this was a lot faster.

When determining whether an iteration has converged we demand it be closer than 0.001 of the true root. This is possible since the true root is easy to find for this polynomial; since the coefficients are both one, the roots are evenly spread around the unit circle in the complex plane. Therefore, to reduce costly comparisons we first check if the current iterate is on the unit circle, if not we know that an additional iterate is required without further comparison.

## Writing files

Since we are limited to $d = 7$ we have seven roots which means we only need (at most) eight colours (one for each root and one for non-convergent starting points). To achieve this quick and simple we create an array of strings containing the binary representation of zero to $d$ as strings and use these as colour encodings. When the newton method is finished (converged or aborted) we do not store it but rather write the colour directly to a string array that later will be written to file. The same goes for the number of iterations which is directly written to a string. The string array is three dimensional $[2,$t$,6$l$]$: One thread writes to its own string, so $6l$ is an example of the attractor string because we encode one colour with 6 chars. When one chunk is finished another commences and the first index is switched, so that a writing thread can access the previous.

These strings are then written to the respective file. We implemented this with the `fprintf` before we new that this was notoriously slow. However, due to parallelisation (discussed below) this is not very significant to the performance.

## Parallelisation

We introduce a hyper parameter that is the maximal size of easily accessible memory that Ozzy can handle. If we consider this fixed we split the domain $[-2,2][-2i,2i]$ horizontally in chunks of this size. The rows in each chunk is then calculated sequentially by further dividing the rows in the chunk evenly among the number of threads. The threads calculate their part and as soon as the first chunck is ready the writing thread starts writing the strings to file in correct order. The writing of a chunk is much faster than calculating a chunk which means that our slow writing function still has time to finish until it is called again.

Late in the task we realised that we could allocate a structure with $l$ empty strings and an $l$ long integer array containing a status. A thread could allocate one string per row in the string array, calculate this, then mutex mark it as ready to write in the status array and start working on another row. A write thread could lie idle probing for a ready to write call for the first row in order. When one call comes, it writes the row to file, frees the string and begins probing for the next row. However, we were already getting below the benchmarks and were quite busy with the upcoming assignments.
 



