---
title: Assignment 2
author:
 - Jakob Lindqvist
 - Per Nilsson Lundberg
 - Patrik Olsson
 - Hampus Toren
date: May 12th 2017
---

# Program layout

## Definitions

Variables:

 - Point with index $i$: $x_i \in R^3$

 - Number of points: $n$

## Calculating distances

The points are read from file and stored in a 2D array with single floating point precision. The pair wise distances are obviously symmetric so we need only calculate the distance in one direction. A points distance to itself is of course trivial so the number of distances to be calculated is

$n(n-1)/2$.

This formula is found by thinking of the distances as being in an $n \times n$ matrix. Where we disregard the lower triangular part, including the diagonal. Counting the elements from the lower right corner this equates summing the first $(n-1)$ integers, $(n-1)$ being the number of elements in the top row. 

The actual distance calculation is straight forward, but we specify the ``sqrtf`` instead of the ordinary square root, sacrificing accuracy in favour of speed. We multiply the distance with a factor 100 to move the decimal point and round it to an integer for storing reasons discussed below.

## Storing distances

The precision and geometry determine the size of the memory needed. We assume that the coordinates of each point is in the interval $[-10,10]$, meaning that the largest possible distance is

$\max_{i,j} |x_i - x_j| \leq \sqrt{ 20^2 + 20^2 + 20^2 } = 34.64$.

With a precision to one centesimal there are only $34.64 * 100 = 3464$ possible distances (3466 to account for the inclusion of the actual minimum and maximum) thus there is no need for any live update of occuring distances but we can rather preallocate an array ``count_array`` of size 3466 since we know no more unique distances may appear.

Furthermore there is no need to scan for the actual distance in the array since the index of the array implicitly gives the distance. A distance with two decimal accuracy defines a unique index if we multiply the result by a factor 100 and cast it as an integer. In practice this means that we can achieve the task essentially by running the line

``count_array[(int) dist * 100]++; ``

for every pair wise distance we encounter.

## Parallelisation 

We utilise three OpenMP statements. In the first we create a parallel scope, here a private count array is initialised. The idea is that each calculation is completely independent as long as the threads can keep the counts separated. At first we had a reduction statement but this was not (initially) supported on the Ozzy system so we achieved the reduction by hand by with the private array for storing distances. Then an OMP for loop iterates through every point. Finally a critical statement add the independent counts together, giving the final result. We are a bit unsure how this self made implementation behaves with regards to memory efficiency compared to the reduction version. If each thread requires a count array of its own this would cause memory usage to grow with each thread. The alternative is that there is some cunning locking mechanism in place, preventing conflicts. 


