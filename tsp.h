//
//  tsp.h
//  TSP
//
//  Created by Milad Ghaznavi on 2/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#ifndef __TSP__tsp__
#define __TSP__tsp__

#include <vector>
#include <math.h>
#include <map>
#include <ilcplex/ilocplex.h>
#ifndef __LP__datastructs__
#include "datastructs.h"
#endif

using namespace std;

#define LP_EPSILON          0.00000001

#define NOT_VALID_TOUR_LEN  -10
#define DEFAULT_START_NODE  0

class TravellingSalesperson {
private:
    Graph*                _graph;
    vector<Edge>          _edges;
    vector<vector<Edge> > _adjancyMatrix;
    IloEnv                _env;
    IloModel              _model;
    IloNumVarArray        _X;
    IloNumArray           _bestSolVals;
    
    void _printSol(IloCplex);
    
protected:
    double runTime;

    void         initLPModel                ();
    IloCplex     solveLP                    ();
    IloCplex     connect                    (IloCplex);
    double       nna                        (Identifier = DEFAULT_START_NODE);
    double       upperBound                 ();
    bool         isTour                     (IloCplex);
    void         branchAndBound             (double&, long, vector<int>&);
    Identifier   edgeToBranch               (IloCplex&, vector<int>&);
    vector<Edge> solutionEdgeSet            ();
    void         addEdgeToConstraints       (Edge&, Identifier, map<Identifier, IloExpr>&);
    void         addEdgeComponentConstraints(Edge&, DisjointSets&, map<Identifier, IloExpr>&);
    
public:
    TravellingSalesperson();
    ~TravellingSalesperson();
    
    vector<Edge> optimumTour(Graph&);
    double       runningTime();
};

#endif /* defined(__TSP__tsp__) */
