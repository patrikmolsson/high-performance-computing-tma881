---
title: Assignment 5
author:
 - Jakob Lindqvist
 - Per Nilsson Lundberg
 - Patrik Olsson
date: May 19th 2017
---
## Definitions
- _vertix_: Point or node in the graph
- _arc_ : Connection between two vertices in the graph. Has a weight associated with it, corresponding to a distance between the nodes. All arcs are symmetric.
- _source_: Starting vertix
- _target_: Final vertix
- _tentative distance_: shortest current distance from source to a vertix. Initially every tentative distance except the sources' is set to INFINITY = 1e9 
- $d$: Degree of the graph, i.e. maximal number of arcs connected to any vertix. Assumes from now on that all nodes are fully connected
- $V$: Number of vertices in the graph
- $P$: Number of MPI processes.

## Representing the graph

## Dijkstras algorithm


## Parallelisation with Open MPI

The algorithm, when used to find the distance of some specific pair of vertices, is not intuitively parallelisable. There is one source and one target and we only care about the shortest distance between them. Our take on it was to assign an equally divided subset of the nodes to each process and have them explore paths of their own. However we still want to change the tentative distances w.r.t. the source. 











