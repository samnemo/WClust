// $Id: KlustaSave.h,v 1.2 2009/06/24 17:58:28 samn Exp $ 
// ************************************************
// KlustaSave.h
//
// for saving models
// ************************************************
#pragma once

#include <vector>
#include <string>
#include "Array.h"

using std::vector;
using std::string;

class KlustaSave {

public:
	// To save the best Cholesky matrices, one per cluster.  Will
	// keep two copies, working copy and best copy, and alternate
	// pointers whenever a new best has been identified.
	// These are global, as we don't want separate copies for each KK 
	// class.

vector < FArray >
	*pChol; 	// pChol[c] is the nDim squared Cholesky matrix 
			// for cluster c
vector< FArray >*pBestChol;	// Best array of matrices for output

int nDims;			// saved number of dimensions
string FileBase;		// string name of files
vector < float > dataMin;	// track minimum value for input
vector < float > dataMax;	// track maximum value for input
vector < int > BestAliveIndex;	// Cholesky matrices can have gaps, 
				// so need to save relocation array
int nDimsBest;			// number of inputs 
int nBestClustersAlive;		// nClustersAlive for best clustering
FArray BestWeight; 	// Weights for best solution (contiguous; 
				// no skips)
FArray BestMean; 		// Means for best solution (contiguous; 
				// no skips)
float BestScoreSave;		// the best score, corresponding to save of 
				// model
int cEStepCallsLast;		// cEStepCall id number of which call preceeded
				// the saving of class ID's
int cEStepCallsSave;		// cEStepCall id number of which call preceeded
				// the saving of means and Cholesky mat's.

};	// class KlustaSave 

