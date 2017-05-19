---
title: Assignment 4
author:
 - Jakob Lindqvist
 - Per Nilsson Lundberg
 - Patrik Olsson
date: May 19th 2017
---

The heat data matrix is padded with zeroes and represented in a vector, containing two instances of the matrix. Depending on the index of the iteration the parts are for writing or reading, respectively, and alternating every iteration.

OpenCL is then used to make the calculations for the heat diffusion. The local group size is calculated as the highest possible integer that can be expressed as two to the power of some integer while also being an integer divisor to the global group size. The reason for this is to ensure that the graph reduction technique can be used.

The heat diffusion kernel calculates the new temperature by reading the temperatures of the adjacent points, and in the case it is close to the edge, the edge temperature will be 0. These calculations are repeated as many times as the input argument states.

A new kernel is then used to calculate the average of the heat matrix. This kernel utilizes the graph reduction technique. We increment with the average for each reduction as opposed to summing everything and the averaging. This is to guard against overflow since the large numbers can cause the summation to rise quite fast. Given the average a similar kernel can then calculate the standard deviation.



