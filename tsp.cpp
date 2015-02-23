//
//  tsp.cpp
//  TSP
//
//  Created by Milad Ghaznavi on 2/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "tsp.h"
#include <sstream>
#include <map>
#include <stack>
#include <memory>

#define NO_CONSTRAINT           -1
#define SELECT_EDGE_CONSTRAINT  1
#define IGNORE_EDGE_CONSTRAINT  0

#define SELECT_EDGE             1.0
#define IGNORE_EDGE             0.0
#define SELECT_EDGE_LB          (SELECT_EDGE - LP_EPSILON)
#define IGNORE_EDGE_UB          (IGNORE_EDGE + LP_EPSILON)

TravellingSalesperson::TravellingSalesperson() : _env(), _model(_env), _X(_env), _bestSolVals(_env) {
    
}

TravellingSalesperson::~TravellingSalesperson() {
    
}

void TravellingSalesperson::initLPModel() {
    IloExpr z(_env);
    
    // Create a variable for each edge and creating the objective function
    size_t count = 0;
    for (Edge e : _edges) {
        stringstream ss;
        ss << "x" << count;
        _X.add(IloNumVar(_env, IGNORE_EDGE, SELECT_EDGE, ILOFLOAT, ss.str().c_str()));
        z += e.weight() * _X[count++];
    }//for
    
    // Adding initialize constraints:
    //  - For each node, we force that at least two edges should be chosen
    _adjancyMatrix = _graph->sortedAdjancyMatrix();
    for (auto elist: _adjancyMatrix) {
        IloExpr nodeConstraint(_env);
        
        for (Edge e: elist)
            nodeConstraint += _X[e.identifier()];
        _model.add((nodeConstraint == 2));
    }//for
    
    _model.add(IloMinimize(_env, z));
}

IloCplex TravellingSalesperson::solveLP() {
    IloCplex cplex(_model);
    cplex.setOut(_env.getNullStream());
    cplex.solve();
    return cplex;
}

IloCplex TravellingSalesperson::connect(IloCplex solution) {
    while(solution.getStatus() != IloAlgorithm::Infeasible) {
        DisjointSets islands(_graph->nodesCount());
        IloNumArray vals(_env);
        solution.getValues(vals, _X);

        // Find islands
        size_t edgesCount = _graph->edgesCount();
        vector<Edge> notSelectedEdges;
        notSelectedEdges.reserve(edgesCount);
        for (size_t i = 0; i < edgesCount; i++) {
            if (vals[i] > LP_EPSILON)
                islands.merge(_edges[i].source(), _edges[i].destination());
            else
                notSelectedEdges.push_back(_edges[i]);
        }//for
        
        if (islands.count() == 1) break;
        
        //Construct constraints for not connected islands
        map<Identifier, IloExpr> mapComp;
        map<Identifier, IloExpr> map;
        for (Edge e : notSelectedEdges) {
            Identifier srcIslandId = islands.find(e.source());
            Identifier dstIslandId = islands.find(e.destination());
            
            if (srcIslandId != dstIslandId) {
                addEdgeToConstraints(e, srcIslandId, map);
                addEdgeToConstraints(e, dstIslandId, map);
            }//if
//            else {
//                addEdgeComponentConstraints(e, islands, mapComp);
//            }//else
        }//for
        
        // Add constraints to model
        // At least there should be two edges between a pair of islands
        for (std::map<Identifier, IloExpr>::iterator it = map.begin(); it != map.end(); it++) {
            _model.add(it->second >= 2);
        }//for
        // The number of edges cannot be more than (#nodes - 1) in each island
//        for (std::map<Identifier, IloExpr>::iterator it = mapComp.begin(); it != mapComp.end(); it++) {
//            _model.add(it->second <= ((int)islands.set(it->first).size() - 1));
//        }//for
        
        solution = solveLP();
    }//while
    
    return solution;
}

double TravellingSalesperson::nna(Identifier startNode) {
    size_t ncount = _graph->nodesCount();
    vector<bool> nselected(ncount, false);
    
    double cost = 0;
    
    Identifier nextNode = startNode;
    nselected[nextNode] = true;
    for (size_t i = 0; i < ncount; i++) {
        size_t j = 0;
        for (; j < _adjancyMatrix[nextNode].size(); j++) {
            Identifier nodeId = _adjancyMatrix[nextNode][j].otherEndpoint(nextNode);
            if (!nselected[nodeId]) {
                //select this node
                cost += _adjancyMatrix[nextNode][j].weight();
                nextNode = nodeId;
                nselected[nextNode] = true;
                break;
            }//if
        }//for
        if (j == _adjancyMatrix[nextNode].size()) break;
    }//for
    
    //Check Cycle
    bool foundLastEdge = false;
    for (Edge e: _adjancyMatrix[nextNode]) {
        if (e.otherEndpoint(nextNode) == startNode) {
            cost += e.weight();
            foundLastEdge = true;
            break;
        }//if
    }//for
    
    //Check that it is tour
    double re = (foundLastEdge) ? cost : NOT_VALID_TOUR_LEN;
    for (size_t i = 0; i < ncount && foundLastEdge; i++) {
        if(!nselected[i]) {
            re = NOT_VALID_TOUR_LEN;
            break;
        }//if
    }//for
    
    return re;
}

double TravellingSalesperson::upperBound() {
    Identifier randNode = random() % _graph->nodesCount();
    double re = nna(_graph->node(randNode).identifier());
    if(re == NOT_VALID_TOUR_LEN) re = INFINITY;
    return re;
}

// We assume that the if degree of each node is 2, and the graph is connected
bool TravellingSalesperson::isTour(IloCplex solution) {
    bool re = true;
    
    size_t ncount = _graph->nodesCount();
    size_t ecount = _graph->edgesCount();
    vector<int> edgesCount(ncount, 0);
    IloNumArray vals(_env);
    solution.getValues(vals, _X);
    for (size_t i = 0; i < ecount; i++) {
        if (vals[i] >= SELECT_EDGE_LB) {
//            cout << _edges[i].source() << "-" << _edges[i].destination() << ": " << _edges[i].weight() << endl;
            edgesCount[_edges[i].source()] ++;
            edgesCount[_edges[i].destination()] ++;
        }//if
    }//for
    
    for (size_t i = 0; i < ncount; i++) {
        if (edgesCount[i]!= 2) {
            re = false;
            break;
        }//if
    }//for
    
    return re;
}

Identifier TravellingSalesperson::edgeToBranch(IloCplex& lp, vector<int>& constraints) {
    size_t ecount = _graph->edgesCount();
    IloNumArray vals(_env);
    lp.getValues(vals, _X);
    
    Identifier _check = INVALID_ID;
    for (size_t i = 0; i < ecount; i++) {
        if (vals[i] < SELECT_EDGE_LB && vals[i] > IGNORE_EDGE_UB && constraints[i] == NO_CONSTRAINT) {
            _check = i;
            break;
        }//if
    }//for
    
    return _check;
}

void TravellingSalesperson::branchAndBound(double& tval, long depth, vector<int>& constraints) {
    depth++;
//    cout << "Depth: " << depth << endl << endl;
    IloCplex tmp = solveLP();
    IloCplex lp  = connect(tmp);
    if (lp.getStatus() == IloAlgorithm::Infeasible || depth > _edges.size()) {
        return;
    }//if
    
    double lpval = lp.getObjValue();
    // Cut the branch
    if (lpval > tval) {
        return;
    }//if
    Identifier branchIt = edgeToBranch(lp, constraints);
    // Reached the leaf
    if (branchIt == INVALID_ID) {
        if (isTour(lp)) {
            lp.getValues(_bestSolVals, _X);
            tval = lpval;
        }//if
        return;
    }//if
    
    // Branch left child
    auto a = (_X[branchIt] >= (SELECT_EDGE_LB));
    _model.add(a);
    constraints[branchIt] = SELECT_EDGE_CONSTRAINT;
    branchAndBound(tval, depth, constraints);
    _model.remove(a);
    constraints[branchIt] = NO_CONSTRAINT;
    
    // Branch right child
    auto b = (_X[branchIt] <= (LP_EPSILON));
    _model.add(b);
    constraints[branchIt] = IGNORE_EDGE_CONSTRAINT;
    branchAndBound(tval, depth, constraints);
    _model.remove(b);
    constraints[branchIt] = NO_CONSTRAINT;
}

void TravellingSalesperson::addEdgeToConstraints(Edge& e, Identifier key, map<Identifier, IloExpr>& map) {
    std::map<Identifier, IloExpr>::iterator it = map.find(key);
    if (it == map.end()) {
        IloExpr newConstraint(_env);
        newConstraint += _X[e.identifier()];
        map.insert(make_pair(key, newConstraint));
    }//if
    else {
        it->second += _X[e.identifier()];
    }//else
}

void TravellingSalesperson::addEdgeComponentConstraints(Edge& e, DisjointSets& islands, map<Identifier, IloExpr>& map) {
    Identifier rootId = islands.find(e.source());
    std::map<Identifier, IloExpr>::iterator it = map.find(rootId);
    if (it == map.end()) {
        IloExpr newConstraint(_env);
        newConstraint += _X[e.identifier()];
        map.insert(make_pair(rootId, newConstraint));
    }//if
    else {
        it->second += _X[e.identifier()];
    }//else
}

vector<Edge> TravellingSalesperson::solutionEdgeSet() {
    vector<Edge> re;
    for (size_t i = 0; i < _edges.size(); i++)
        if (_bestSolVals[i] >= SELECT_EDGE_LB)
            re.push_back(_edges[i]);
    
    return re;
}

vector<Edge> TravellingSalesperson::optimumTour(Graph& graph) {
    clock_t time = clock();
    _graph = &graph;
    _edges = _graph->edges();
    
    // Initialize the model
    initLPModel();

    // Branch and bound
    double tval = upperBound();
    vector<int> constraints(_edges.size(), NO_CONSTRAINT);
    branchAndBound(tval, 0, constraints);
    
    // Extract the set of edges
    vector<Edge> re = solutionEdgeSet();

    _env.end();
    runTime = clock() - time;
    return re;
}

double TravellingSalesperson::runningTime(){
    return runTime / CLOCKS_PER_SEC;
}

void TravellingSalesperson::_printSol(IloCplex sol){
    IloNumArray tmpvals (_env);
    sol.getValues(tmpvals, _X);
    
    for (size_t i = 0; i < _graph->edgesCount(); i++) {
        if (tmpvals[i] >= LP_EPSILON)
            cout << _edges[i].source() << "-" << _edges[i].destination() << ":" << tmpvals[i] << endl;
    }//for
}
