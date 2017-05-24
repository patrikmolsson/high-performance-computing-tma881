---
title: Assignment 5
author:
 - Jakob Lindqvist
 - Per Nilsson Lundberg
 - Patrik Olsson
date: May 19th 2017
---
## Definitions
- _vertex_: Point or node in the graph
- _arc_ : Connection between two vertices in the graph. Has a weight associated with it, corresponding to a distance between the nodes. All arcs are symmetric.
- _source_: Starting vertex
- _target_: Final vertex
- _tentative distance_: shortest current distance from source to a vertex. Initially every tentative distance except the sources' is set to INFINITY = 1e9
- _unvisited set_: Set of vertices that have not been the starting point for a Dijkstra iteration.
- $d$: Degree of the graph, i.e. maximal number of arcs connected to any vertex. Assumes from now on that all nodes are fully connected
- $V$: Number of vertices in the graph
- $P$: Number of MPI processes.

## Representing the graph

To represent the graph we use a 2D array, nachbar_nodes. For every row in the array, the connected vertices to the base vertex (the vertex with the corresponding ID to the array row index) are represented, it's nachbar's. The connected vertices holds the information about the distance from the vertex to the base vertex, as well as their ID's.

Another array used is the one that holds the result from the dijkstra algorithm itself, dijkstra_data. The array holds, for each vertex, the information if it has been or shall be visited by the dijkstra algorithm, the tentative distance to that vertex, as well as from which vertex the shortest tentative distance to that vertex has been calculated so far.

## Dijkstras algorithm

The algorithm starts at the source vertex and calculates the distance to its neighbouring vertices. It then chooses the vertex with the shortest tentative distance, that has not yet been visited, and calculates the tentative distance to that vertex neighbours, and sets these distances only if the new distance is shorter than the previously set tentative distances on the neighbouring vertices. The algorithm then repeats until all nodes have been visited. The shortest path can then be accessed by accessing the tentative distance at the target vertex. To illustrate the path, one can traverse backwards from the target until reaching the source vertex.


## Parallelisation with Open MPI

The algorithm, when used to find the distance of some specific pair of vertices, is not intuitively parallelisable. There is one source and one target and we only care about the shortest distance between them. Our take on it was to divide the set of vertices into equal subset and assign them to each process. We then have them explore paths of their own in order to speed up the process. However we still want to change the tentative distances w.r.t. the source so initially only the process assigned the subset including the source is active. The other processes just goes through their unvisited vertices at every iteration, looking for one to choose. But since they all have infinite tentative distances they can't be visited. The processes cannot visit each others vertices but they can still neighbour external vertices and can thus update those tentative distances. This means that the source process ``unlocks'' the other processes so that they can get to work.

It is crucial that all processes share information about the state of the graph since we would get conflicting updates otherwise. Because of this we synchronise this with the ```MPIAllreduce``` function with a custom made routine to use for the reduction. In the reduction we make sure that the smallest possible tentative distances as well as a correct unvisited set is broadcast to all processes in the beginning of each iteration.

NB that the notion of an unvisited set is not so definitive since another node can at any iteration change a tentative distance which forces the process owning that vertex to revisit it and explore the new paths. Only when no tentative distances are lowered should the algorithm terminate. We kept the unvisited set even for $P>1$ but everytime a tentative distance of a visited vertex was updated we reassigned it to the unvisited set and the process owning it had to check it again.















