#ifndef _APHULL_H
#define _APHULL_H

#define NONE (-1)

struct APPoint {
	float x, y;
};

struct Bin {
    int    min;    // index of min point P[] in bin (>=0 or NONE)
    int    max;    // index of max point P[] in bin (>=0 or NONE)
};

int nearHull_2D( APPoint* P, int n, int k, APPoint* H );

#endif