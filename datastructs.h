//
//  datastructs.h
//  TSP
//
//  Created by Milad Ghaznavi on 2/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#ifndef __TSP__datastructs__
#define __TSP__datastructs__

#include <iostream>
#include <vector>
using namespace std;

#define INVALID_ID -1

typedef unsigned long long Weight;
typedef long long          Identifier;
typedef double             Information;

class Node {
private:
    Identifier _id;
    Identifier _parentId;
    
public:
    Information info;
    double x;
    double y;
    
    void       init      (Identifier);
    Identifier identifier();
    Identifier parent    ();
    void       parent    (Identifier);
};

class Edge {
private:
    Weight     _w;
    Identifier _src;
    Identifier _dst;
    Identifier _id;
    
public:
    Edge       (Identifier, Identifier, Weight = 0.0);
    void       identifier   (Identifier);
    Identifier identifier   ();
    Identifier source       ();
    Identifier destination  ();
    Identifier otherEndpoint(Identifier);
    double     weight       ();
    
    static void   sort          (vector<Edge>&);
    static double sumWeights    (vector<Edge>);
    static int    euclideEdgeLen(double, double, double, double);
    static int    euclideEdgeLen(pair<double, double>, pair<double, double>);
};

struct compareEdges
{
    bool operator() (Edge, Edge) const;
};

class Graph {
private:
    static const bool   _defIsGeo    = false;
    
    vector<Node>   _nodes;
    vector<Edge>   _edges;
    int            _currEdge;
    bool           _isGeo;
    
    void _initByXY(vector<double>&, vector<double>&, size_t);
    void _readGraphFile   (string);
    void _readGeoGraphFile(string);
    
public:
    Graph(bool = _defIsGeo);
    ~Graph();
    
    void                  readGraphFile      (string);
    void                  addEdge            (Identifier, Identifier, Weight);
    size_t                edgesCount         ();
    size_t                nodesCount         ();
    Node                  node               (Identifier);
    bool                  isGeo              ();
    string                str                ();
    vector<Node>          nodes              ();
    vector<Identifier>    nodesIds           ();
    vector<Edge>          edges              ();
    void                  nodeParent         (Identifier, Identifier);
    void                  nodeInfo           (Identifier, Information);
    Information           nodeInfo           (Identifier);
    vector<vector<Edge> > adjancyMatrix      ();
    vector<vector<Edge> > sortedAdjancyMatrix();
    
    static Graph  makeGraph  (vector<Node>&);
    static Graph  makeGraph  (vector<double>&, vector<double>&, size_t);
    static Weight graphWeight(Graph&);
};

class DisjointSets {
private:
    Identifier* _ids;
    size_t* _ranks;
    size_t _count;
    size_t _elemsCount;
public:
    DisjointSets (size_t);
    ~DisjointSets();
    
    Identifier find     (Identifier);
    void       merge    (Identifier, Identifier);
    bool       connected(Identifier, Identifier);
    size_t     count    ();
    
    size_t             setSize(Identifier);
    vector<Identifier> set    (Identifier);
    
};

#endif /* defined(__TSP__datastructs__) */
