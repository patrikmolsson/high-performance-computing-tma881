---
title: Assignment 4
author:
 - Jakob Lindqvist
 - Per Nilsson Lundberg
 - Patrik Olsson
date: May 19th 2017
---

The heat data matrix is padded with zeroes and represented in a vector, containing two instances of the matrix. Depending on the index of the iteration the parts are for writing or reading, respectively, and alternating every other iteration.

OpenCL is then used to make the calculations for the heat diffusion. The local group size is calculated as the highest possible integer, that also can be expressed as two to the power of some integer, and also should be able do evenly divide global group size. Why the local group size should be an integer that could also be expressed as two to the power of some integer is so that the graph reduction technique can be used.

The heat diffusion kernel calculates the new temperature by reading the temperatures of the adjacent points, and in the case it is close to the edge, the edge temperature will be 0. These calculation are repeated as many times as the input argument states.

A new kernel is then used to calculate the average of the heat matrix. This kernel utilizes the graph reduction technique, calculating the average for each reduction. A similar kernel is used to calculate the standard deviation.



