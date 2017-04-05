# Assignment 1

## Time
The time subtask is made up of a program that calculates the sum of the first billion integers, by looping over all the integers. At first the sum was calculated with a more efficient algorithm: `sum = n*(n+1)/2`, although by using this type of algorithm we did not get much difference in timing results depending on the optimization flags, therefore we stuck to the more naive implementation.

To time the functions we used bash built-in time function, and ran the program 10 times for each optimization flag. The results are presented in the table below.

| Flag    | Time    |
| ------- | ------- |
| -O0     | 27.775s |
| -O1     | 3.381s  |
| -O2     | 0.005s  |
| -O3     | 0.005s  |
| -Os     | 0.005s  |
| -Og     | 3.038s  |

We see that with optimization flag `-O0` we got the worst results, and with optimization flags `-O2`, `-O3` and `-Os` the by far best results. 

By comparing the assembly code for the optimization flags `-O0`, `-O1`, and `-O2`, we see that in `-O0` the sum is not calculated by the compiler, and the for-loop is still present. With the optimization flag `-O1` we find the calculated sum in the assembly code, but the upper limit of the loop is still present, which hints that the loop is still running. In the `-O2` assembly code we do not find any trails of the loop, and only the final sum. This indicates that there is no loop present at all, hence the good results.

## Inlining

## Locality

## Indirect addressing

## Valgrind

## Profiling


