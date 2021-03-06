---
title: Assignment 1
author:
 - Michaela Fritiofsson
 - Jakob Lindqvist
 - Per Nilsson Lundberg
 - Patrik Olsson
date: April 6th 2017
---

# Time
The  subtask is made up of a program that calculates the sum of the first billion integers, by looping over all the integers. At first the sum was calculated with a more efficient algorithm: `sum = n*(n+1)/2`, although by using this type of algorithm we did not get much difference in timing results depending on the optimization flags, therefore we stuck to the more naive implementation.

To  the functions we used bash built-in time function, and ran the program 10 times for each optimization flag. The results are presented in the table below.

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

# Inlining

Running 30000 runs(on ozzy) and using CLOCK\_MONOTONIC\_RAW from clock\_gettime without
optimization flags yields following averaged time:

| Method | Time |
| ------ | ---- |
| Time for mul\_cpx: | 0.00024145336 |
| Time for mul\_cpx\_sep: | 0.000238245753 |
| Time for mul\_cpx\_inline: | 0.000136710719 |

The version included in a separate file had approximately the same running time as the
function in the main file. We found this a little surprising, one thought was that
there would be some overhead when reading from a seperate file.

The function inlined by hand was the fastest, this is because
the program skips the function overhead(i.e. no need for call or return sequences).

When running with optimization flag -O3 this yields averaged running times

| Method | Time |
| ------ | ---- |
| Time for mul\_cpx: | 7.02799487e-05 |
| Time for mul\_cpx\_sep: | 9.43747255e-05 |
| Time for mul\_cpx\_inline: | 7.02518996e-05 |

Here the function in the main file and the one inlined by hand performs equally well.
We are guessing that the compiler inlines the function from the main file, thus
it would become equal to the one inlined by hand.

The one in the seperate file suffers worse performance, it seems as if the compiler
is not able to inline this one in the same way(although it still approx 3 times faster
when comparing to running without optimization flags).

# Locality
The locality subtask is using three different algorithms to compute sums. One algorithm to calculate row sums (`row_sums`) and two algorithms to calculate column sums (`col_sums` and `col_sums2`).

By running the clock function in C, we get the following results, without using optimization flags:

| Method | Time (s) |
| ------- | ------- |
|col_sums | 0.005989 |
|col_sums2 | 0.003598 |
|row_sums | 0.002862 |

When using full optimization (`-O3`) we get these results:

|Method | Time (s) |
| ------- | ------- |
|col_sums | 0.002001 |
|col_sums2 | 0.000574 |
|row_sums | 0.001479 |

When not optimizing, the `row_sums` is fastest, and `col_sums` slowest. This could be explained by that the row elements in a matrix are stored next to each other in the memory, therefore the algorithm does not need to "jump" as much when accessing the elements to calculate the sum, as in the `col_sums` algorithms.

The improved `col_sums2` algorithm performs better than the `col_sums`, since we do not calculate the column sum before storing it in the results vector, but instead sum directly in the vector, saving some memory and avoiding some statements.

When using full optimization the `col_sums2` is instead the fastest algorithm by far, and the `col_sums` and `row_sums` do not differ as much as before.

# Indirect addressing
We choose to implement the latter version of the index vector meaning that the only difference between the algorithms is that the index to sum is obtained from a separate vector rather than the actual loop iterator.
Timing the summation of two vectors of length N = 1000000 with indirect/direct addressing, compiled with none/full optimisation, yielded the results in the table below (average over 1000 runs):

| Flag	| Indirect		| Direct 		|
| ----- | ---------------------	| ---------------------	|
| -O0   | 0.003591462984 	| 0.002395621980	|
| -O3   | 0.001502420311 	| 0.000980806824	|

The table shows that the full optimisation can obviously infer some speed-ups to the execution, however the relative difference between the two implementation is roughly the same for the two flags. This indicates that the optimised compiler cannot resolve the issues of the slower implementation. It's reasonable that the indirect scheme is slower since it has to access an extra piece of information from the memory/register, namely the external index vector. This in itself is of course slower but it also introduces an element of uncertainty to the compiler since it cannot know which index that will be read the next time as opposed to having a predictable loop iterator give the index.

# Valgrind
Running valgrind for the executable leak produces the following output:

==10061== HEAP SUMMARY:
==10061==     in use at exit: 4,000 bytes in 1 blocks
==10061==   total heap usage: 1 allocs, 0 frees, 4,000 bytes allocated
==10061==
==10061== LEAK SUMMARY:
==10061==    definitely lost: 4,000 bytes in 1 blocks
==10061==    indirectly lost: 0 bytes in 0 blocks
==10061==      possibly lost: 0 bytes in 0 blocks
==10061==    still reachable: 0 bytes in 0 blocks
==10061==         suppressed: 0 bytes in 0 blocks
==10061== Rerun with --leak-check=full to see details of leaked memory

Without freeing the allocated memory some of it is still in use att exit. The size of it confirms that the allocated ints are the cause of this; 4000 bytes in 1 block corresponds to the 1000 ints allocated since each integer requires 4 bytes. Altering the program so that it frees the memory makes this memory leak go away.

# Profiling
The profiling subtask is using three different algorithms implemented earlier in locality. One algorithm to calculate row sums (`row_sums`) and two algorithms to calculate column sums (`col_sums` and `col_sums2`).

By running the clock function in C, we get the following results:

|Method | Time (s) |
| ------- | ------- |
|col_sums | 0.005989 |
|col_sums2 | 0.003598 |
|row_sums | 0.002862 |

When running `gprof` we unfortunately got results that did not make sense, they differ every execution. This was since the execution time for the methods were so small, and that `gprof` only measures in centi-seconds. This was solved by running the algorithms 1000 times each, and timing that. The results are presented in the table below. The results are very similar to the times received by the clock function.


|Method | Time with gprof (s) |
| ------- | ------- |
|col_sums | 6.49 |
|col_sums2 | 3.00 |
|row_sums | 2.82 |

When running `gcov` we see that we have the most operations on `col_sums` and `row_sums`, although `row_sums` is the fastest algorithm. This is explained by the reason mentioned earlier, that row elements are stored next to each other in memory, compared to column elements.

The `col_sums2` performs better than the `col_sums` as explained previously.
