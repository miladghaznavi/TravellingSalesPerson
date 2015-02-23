//
//  datastructs.cpp
//  TSP
//
//  Created by Milad Ghaznavi on 2/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "datastructs.h"

#include <fstream>
#include <sstream>
#include <math.h>

#define BYTE 8

void Node::init(Identifier id) {
    this->_id = id;
    this->_parentId = INVALID_ID;
}

Identifier Node::identifier() {
    return _id;
}

Identifier Node::parent() {
    return _parentId;
}

void Node::parent(Identifier id) {
    _parentId = id;
}

void Edge::identifier(Identifier id) {
    _id = id;
}

Identifier Edge::identifier() {
    return _id;
}

Edge::Edge(Identifier source, Identifier destination, Weight weight) {
    _src = source;
    _dst = destination;
    _w   = weight;
}

Identifier Edge::source() {
    return _src;
}

Identifier Edge::destination() {
    return _dst;
}

Identifier Edge::otherEndpoint(Identifier id) {
    Identifier re = INVALID_ID;
    if (id == _dst){
        re = _src;
    }//if
    else {
        re = _dst;
    }//else if
    
    return re;
}

double Edge::weight() {
    return _w;
}

void Edge::sort(vector<Edge>& edges) {
    std::sort(edges.begin(), edges.end(), compareEdges());
}

double Edge::sumWeights(vector<Edge> edges) {
    double re = 0.0;
    for (Edge e: edges)
        re += e.weight();
    return re;
}


int Edge::euclideEdgeLen(double x1, double x2, double y1, double y2) {
    double xDist = x1 - x2;
    double yDist = y1 - y2;
    return (int) (sqrt (xDist * xDist + yDist * yDist) + 0.5);
}

int Edge::euclideEdgeLen (pair<double, double> a, pair<double, double> b) {
    return euclideEdgeLen(a.first, b.first, a.second, b.second);
}

bool compareEdges::operator() (Edge e1, Edge e2) const {
    return e1.weight() < e2.weight();
}


Graph::Graph(bool isGeo) {
    _currEdge = 0;
    _isGeo = isGeo;
}

Graph::~Graph() {
    _nodes.clear();
    _edges.clear();
}

void Graph::_initByXY(vector<double>& xlist, vector<double>& ylist, size_t ncount) {
    // Initialize nodes
    for (size_t i = 0; i < ncount; i++) {
        Node n;
        n.init(i);
        n.x = xlist[i]; n.y = ylist[i];
        n.parent(n.identifier());
        _nodes.push_back(n);
    }//for
    
    // Initialize edges
    size_t ecount = 0;
    for (size_t i = 0; i < ncount; i++) {
        for (size_t j = i + 1; j < ncount; j++) {
            Edge e(_nodes[i].identifier(), _nodes[j].identifier(),
                   Edge::euclideEdgeLen(xlist[i], xlist[j], ylist[i], ylist[j]));
            e.identifier(ecount++);
            _edges.push_back(e);
        }//for
    }//for
}

void Graph::_readGraphFile   (string path) {
    ifstream inputFile(path.c_str());
    
    if (!inputFile)
        throw "Error in reading file!";
    
    // Read Header
    int nodesCount, edgesCount;
    inputFile >> nodesCount >> edgesCount;
    _edges.reserve(edgesCount);
    
    // Read edges and their weights
    int    src, dst;
    Weight w;
    
    //Init Nodes
    _nodes.reserve(nodesCount);
    for (int i = 0; i < nodesCount; i++) {
        Node v;
        v.init(i);
        v.parent(v.identifier());
        _nodes.push_back(v);
    }//for
    
    for (int i = 0; i < edgesCount; i++) {
        inputFile >> src >> dst >> w;
        addEdge(src, dst, w);
    }//for
}

void Graph::_readGeoGraphFile(string path) {
    
    ifstream inputFile(path.c_str());
    
    if (!inputFile)
        throw "Error in reading file!";
    
    // Read Header
    int nodesCount;
    inputFile >> nodesCount;
    _nodes.reserve(nodesCount);
    _edges.reserve((nodesCount * (nodesCount - 1)) / 2);
    
    vector<double> xlist(nodesCount), ylist(nodesCount);
    
    for (int i = 0; i < nodesCount; i++) {
        inputFile >> xlist[i] >> ylist[i];
    }//for
    
    _initByXY(xlist, ylist, xlist.size());
    
    xlist.clear();
    ylist.clear();
}

void Graph::readGraphFile(string path) {
    if (_isGeo)
        _readGeoGraphFile(path);
    else
        _readGraphFile(path);
}

void Graph::addEdge(Identifier src, Identifier dst, Weight w) {
    if (_isGeo)
        throw "Graph is geo graph!\nEdge cannot be added!";
    
    Edge e(src, dst, w);
    e.identifier(_edges.size());
    _edges.push_back(e);
}

size_t Graph::edgesCount() {
    return _edges.size();
}

size_t Graph::nodesCount() {
    return _nodes.size();
}

Node Graph::node(Identifier id) {
    return _nodes[id];
}

bool Graph::isGeo() {
    return _isGeo;
}

string Graph::str() {
    stringstream ss;
    
    if (_isGeo) {
        for (Node n : _nodes) {
            ss << n.x << "\t" << n.y << endl;
        }//for
    }//if
    else {
        for (Edge e: _edges)
            ss << e.source() << "\t" << e.destination() << "\t" << e.weight() << endl;
    }//else
    
    return ss.str();
}

vector<Node> Graph::nodes() {
    return _nodes;
}

vector<Identifier> Graph::nodesIds() {
    vector<Identifier> re;
    for(Node v : _nodes)
        re.push_back(v.identifier());
    
    return re;
}

vector<Edge> Graph::edges() {
    return _edges;
}

void Graph::nodeParent(Identifier nodeId, Identifier parentId) {
    _nodes[nodeId].parent(parentId);
}

void Graph::nodeInfo(Identifier nodeId, Information info) {
    if (_isGeo)
        throw "Adding info to geo graph node is not supported!";
    _nodes[nodeId].info = info;
}

Information Graph::nodeInfo(Identifier nodeId) {
    if (_isGeo)
        throw "Retreiving info of a geo graph node is not supported!";
    return _nodes[nodeId].info;
}

vector<vector<Edge> > Graph::adjancyMatrix() {
    vector<vector<Edge> > re(this->nodesCount());
    
    for (Edge e : _edges) {
        re[e.source()     ].push_back(e);
        re[e.destination()].push_back(e);
    }//for
    
    return re;
}

vector<vector<Edge> > Graph::sortedAdjancyMatrix() {
    vector<vector<Edge> > re = adjancyMatrix();
    
    for (size_t i = 0; i < re.size(); i++)
        Edge::sort(re[i]);
    
    return re;
}


Graph Graph::makeGraph(vector<Node>& nodes) {
    Graph g(false);
    
    for (Node v : nodes)
        g._nodes.push_back(v);
    
    return g;
}

Graph Graph::makeGraph(vector<double>& xlist, vector<double>& ylist, size_t ncount) {
    Graph g(true);
    if (xlist.size() != ylist.size())
        throw "List of x coordinate and y coordinate must be same!";
    
    g._initByXY(xlist, ylist, ncount);
    
    return g;
}

Weight Graph::graphWeight(Graph& g) {
    Weight re = 0;
    for (Edge e : g.edges())
        re += e.weight();
    return re;
}

DisjointSets::DisjointSets(size_t count)   {
    _elemsCount = count;
    _ids = new Identifier[_elemsCount];
    _ranks = new size_t[_elemsCount];
    for(size_t i = 0; i < _elemsCount; i++)	{
        _ids[i] = i;
        _ranks[i] = 1;
    }//for
    _count = _elemsCount;
}
 
DisjointSets::~DisjointSets() {
    delete _ranks;
    delete _ids;
}

Identifier DisjointSets::find(Identifier p)	{
    Identifier root = p;
    while (root != _ids[root])
        root = _ids[root];
    
    while (p != root) {
        Identifier newp = _ids[p];
        _ids[p] = root;
        p = newp;
    }//while
    return root;
}

void DisjointSets::merge(Identifier x, Identifier y)	{
    Identifier i = find(x);
    Identifier j = find(y);
    if(i == j) return;
    
    // make smaller root point to larger one
    if(_ranks[i] < _ranks[j]) {
        _ids[i] = j;
        _ranks[j] += _ranks[i];
    }//if
    else {
        _ids[j] = i;
        _ranks[i] += _ranks[j];
    }//else
    _count--;
}

bool DisjointSets::connected(Identifier x, Identifier y)    {
    return find(x) == find(y);
}

size_t DisjointSets::count() {
    return _count;
}

size_t DisjointSets::setSize(Identifier id) {
    size_t re = 0;
    Identifier refRootId = find(id);
    for (size_t i = 0; i < _elemsCount; i++)
        if (find(_ids[i]) == refRootId)
            re ++;
    return re;
}

vector<Identifier> DisjointSets::set(Identifier id) {
    vector<Identifier> re;
    re.reserve(_elemsCount);
    
    Identifier refRootId = find(id);
    for (size_t i = 0; i < _elemsCount; i++)
        if (find(_ids[i]) == refRootId)
            re.push_back(i);
    
    return re;
}

