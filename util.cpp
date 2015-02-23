//
//  util.cpp
//  TSP
//
//  Created by Milad Ghaznavi on 2/7/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "util.h"
#include <string>
#include <iostream>
using namespace std;

#define SWAP(x, y, temp) {temp = x; x = y; y = temp;}

bool Utility::OUTPUT = true;

double Utility::zeit ()
{
    struct rusage ru;
    getrusage (RUSAGE_SELF, &ru);
    
    return ((double) ru.ru_utime.tv_sec) +
                ((double) ru.ru_utime.tv_usec)/1000000.0;
}

double Utility::realZeit() {
    return (double) time (0);
}

/* function for creating a random set of points in unit square */
bool Utility::buildXY (int ncount, int gridsize, vector<double>& xlist, vector<double>& ylist) {
    bool rval = true;
    int i, j, winner, x, y;
    int **hit = (int **) NULL, *hitcount = (int *) NULL;
    
    if (OUTPUT) {
        printf ("Random %d point set, gridsize = %d\n", ncount, gridsize);
        fflush (stdout);
    }//if
    hit =  (int **) malloc (gridsize * sizeof (int *));
    if (!hit) {
        fprintf (stderr, "out of memory for hit\n");
        rval = false; goto CLEANUP;
    }
    for (i = 0; i < gridsize; i++) hit[i] = (int *) NULL;
    
    hitcount = (int *) malloc (gridsize * sizeof (int));
    if (!hitcount) {
        fprintf (stderr, "out of memory for hitcount\n");
        rval = false; goto CLEANUP;
    }
    for (i = 0; i < gridsize; i++) hitcount[i] = 0;
    
    for (i = 0; i < ncount; i++) {
        winner = 0;
        do {
            x = (int) (random () % gridsize);
            y = (int) (random () % gridsize);
            
            /* check to see if (x,y) is a duplicate point */
            
            for (j = 0; j < hitcount[x]; j++) {
                if (hit[x][j] == y) break;
            }
            if (j == hitcount[x]) {
                void *tmp_ptr = (void *) hit[x];
                tmp_ptr = realloc (tmp_ptr, (hitcount[x]+1)*sizeof (int));
                if (!tmp_ptr) {
                    fprintf (stderr, "out of member in realloc of hit\n");
                    rval = false; goto CLEANUP;
                }
                hit[x] = (int *) tmp_ptr;
                hit[x][hitcount[x]] = y;
                hitcount[x]++;
                winner = 1;
            }
            if (!winner) {
                printf ("X"); fflush (stdout);
            }
        } while (!winner);
        xlist[i] = (double) x;
        ylist[i] = (double) y;
    }
    
CLEANUP:
    
    printf ("\n");
    
    if (hit) {
        for (i = 0; i < gridsize; i++) {
            if (hit[i]) free (hit[i]);
        }
        free (hit);
    }
    if (hitcount) free (hitcount);
    return rval;
}
