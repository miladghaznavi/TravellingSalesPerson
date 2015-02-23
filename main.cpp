//
//  main.cpp
//  TSP
//
//  Created by Milad Ghaznavi on 2/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include <iostream>
#include <map>
#include <string>
#include <getopt.h>
#include <ilcplex/ilocplex.h>
#include "util.h"
#include "tsp.h"
using namespace std;

#define POSSIBLE_OPTIONS      "b:gk:s:"
#define SEED_OPTION           's'
#define GEOMETRIC_DATA_OPTION 'g'
#define NCOUNT_RAND_OPTION    'k'
#define GRID_SIZE_RAND_OPTION 'b'
#define PROB_FILE_OPTION      ''

#define SEED_DEFAULT           (int) Utility::realZeit()
#define GEOMETRIC_DATA_DEFAULT false
#define NCOUNT_RAND_DEFAULT    0
#define GRID_SIZE_RAND_DEFAULT 100

#define PRINT_PRECISION        2

void usage(string);
bool extractArgs(int, char*[], int&, int&, int&, bool&, string&);

int main(int argc, char * argv[]) {
    int    seed          = SEED_DEFAULT;
    bool   geometricData = GEOMETRIC_DATA_DEFAULT;
    int    ncountRand    = NCOUNT_RAND_DEFAULT;
    int    gridSize      = GRID_SIZE_RAND_DEFAULT;
    string path          = "";

    if (!extractArgs(argc, argv, seed, ncountRand, gridSize, geometricData, path)) {
        usage(string(argv[0]));
        exit(1);
    }//if

    if (path  == "" && !ncountRand) {
        cout << "Must specify a problem file or use -k for random prob\n";
        usage(string(argv[0]));
        exit(1);
    }//if

    cout << "Seed = " << seed << endl;
    srandom (seed);

    if (path != "") {
        cout << "Problem name: " << path << endl;
        if (geometricData)
            cout << "Geometric data" << endl;
    }//if

    Graph g(geometricData);
    if (path != "") {
        cout << "Reading problem file..." << endl;
        g.readGraphFile(path);
    }//if
    else {
        cout << "Building random problem..." << endl;
        vector<double> xlist(ncountRand), ylist(ncountRand);
        Utility::buildXY(ncountRand, gridSize, xlist, ylist);
        g = Graph::makeGraph(xlist, ylist, ncountRand);
    }//if

    cout << "Start to process!" << endl;
    TravellingSalesperson tsp;
    vector<Edge> result = tsp.optimumTour(g);
    cout << "Finish!" << endl;

    double tourLength   = Edge::sumWeights(result);
    double runningTime  = tsp.runningTime();

    cout.setf(ios::fixed, ios::floatfield);
    cout.setf(ios::showpoint);
    cout.precision(PRINT_PRECISION);
    cout << "Running Time: " << runningTime << " sec" << endl;
    cout << "Optimal Tour: " << tourLength  << endl;

    return 0;
}

void usage (string p) {
    cerr << "Usage: " << p << " [-see below-] [prob_file]\n";
    cerr << "   -b d  gridsize d for random problems\n";
    cerr << "   -g    prob_file has x-y coordinates\n" ;
    cerr << "   -k d  generate problem with d cities\n";
    cerr << "   -s d  random seed\n";
}

bool extractArgs(int argc,
                 char* argv[],
                 int& seed,
                 int& ncountRand,
                 int& gridSize,
                 bool& geometricData,
                 string& path) {
    bool re = true;
    char ch;
    while ((ch = getopt(argc, argv, POSSIBLE_OPTIONS)) != EOF) {
        switch (ch) {
            case SEED_OPTION:
                seed = atoi(optarg);
                break;
            case NCOUNT_RAND_OPTION:
                ncountRand = atoi(optarg);
                break;
            case GRID_SIZE_RAND_OPTION:
                gridSize = atoi(optarg);
                break;
            case GEOMETRIC_DATA_OPTION:
                geometricData = true;
                break;
            case '?':
            default:
                re = false;
                break;
        }//switch
    }//while
    if (optind < argc)
        path = argv[optind++];

    if (optind != argc )
        re = false;

    return re;
}
