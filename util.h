//
//  util.h
//  TSP
//
//  Created by Milad Ghaznavi on 2/7/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#ifndef __TSP__util__
#define __TSP__util__

#include <string>
#include <math.h>
#include <vector>
#include <map>
using namespace std;

static class Utility {
public:
    static bool   OUTPUT;
    static double zeit     ();
    static double realZeit ();
    static bool   buildXY  (int, int, vector<double>&, vector<double>&);
};

#endif /* defined(__TSP__util__) */
