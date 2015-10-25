# TravellingSalesPerson
This project implements an exact solution for Traveling Sales Person (TSP) problem. The solution is an efficient implementation of "Solution of a large scale traveling salesman problem" [1] by G.B.Dantzig, D.R.Fulkerson, and S.M.Johnson. To run this program, the cplex library for solving the linear program is required.

## Files
This project contains three header files:
- datastructs.h
- util.h
- tsp.h

## Data structure
Header "datastructs.h" contain required data structure for undirected graph and disjoint-set. 
- Graph data structures: Node, Edge, Graph
- DisjointSets

## Utility
Header "util.h" contains couple of functions for measuring running time, and generating random input for the TSP problem.

## TSP
Header "tsp.h" constains 

[1] G.B.Dantzig, D.R.Fulkerson, and S.M.Johnson, Solution of a large scale traveling salesman problem, Technical Report P-510, RAND Corporation, Santa Monica, California, USA, 1954.


