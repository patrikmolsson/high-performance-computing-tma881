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

To represent the graph we use a 2D array, nachbar_nodes. For every row in the array, the connected vertices to the base vertex (the vertex with the corresponding ID to the array row index) are represented, it's nachbar's. The connected vertices holds the information about the distance from the vertex to the base vertex, as well as their ID's.

Another array used is the one that holds the result from the dijkstra algorithm itself, dijkstra_data. The array holds, for each vertex, the information if it has been or shall be visited by the dijkstra algorithm, the tentative distance to that vertex, as well as from which vertex the shortest tentative distance to that vertex has been calculated so far.

## Dijkstras algorithm

The algorithm starts at the source vertex and calculates the distance to its neighbouring vertices. It then chooses the vertex with the shortest tentative distance, that has not yet been visited, and calculates the tentative distance to that vertex neighbours, and sets these distances only if the new distance is shorter than the previously set tentative distances on the neighbouring vertices. The algorithm then repeats until all nodes have been visited. The shortest path can then be accessed by accessing the tentative distance at the target vertex. To illustrate the path, one can traverse backwards from the target until reaching the source vertex.


## Parallelisation with Open MPI

The algorithm, when used to find the distance of some specific pair of vertices, is not intuitively parallelisable. There is one source and one target and we only care about the shortest distance between them. Our take on it was to assign an equally divided subset of the nodes to each process and have them explore paths of their own. However we still want to change the tentative distances w.r.t. the source. 











