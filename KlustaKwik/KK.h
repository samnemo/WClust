// $Id: KK.h,v 1.2 2009/06/24 17:59:11 samn Exp $ 
//
// KK.h
//
// KlustaKwik clustering engine
//
#pragma once
#include "Array.h"
#include "../A2D.h"

class KK {
public:
	// FUNCTIONS
	void AllocateArrays();
	void AlocateCholeskyVecs();	// allocate space to save best Cholesky 
					// matrices
	void SaveBestMeans();		// save cluster centers for later output
	bool LoadData();
	bool LoadData(A2D<float>& Dat);
	float Penalty(int n);
	float ComputeScore();
	void MStep();
	void EStep();
	void CStep();
	void ConsiderDeletion();
    bool LoadClu(char *StartCluFile);
    int TrySplits();
	float CEM(char *CluFile = (char*)0L, int recurse = 1);
    void Reindex();
public:
	// VARIABLES
	int nDims,  // nDims is the number of features, as specified by the 
		    // '1' entries in UseFeatures
	    nDims2; // nDims2 is nDims squared
	int nStartingClusters; // total # starting clusters, including clu 0, the noise cluster.
	int nClustersAlive;	// nClustersAlive is total number with points in, 
    				// excluding noise cluster
	int nPoints;
	int NoisePoint;	// number of fake points always in noise cluster to ensure 
    			// noise weight>0.  Default is 1.
	int FullStep; // Indicates that the next E-step should be a full step (no time saving)
	float penaltyMix;		// amount of BIC to use for penalty, must be between 0 and 1
	FArray Data; // Data[p*nDims + d] = Input data for poitn p, dimension d
	FArray Weight; // Weight[c] = Class weight for class c
	FArray Mean; // Mean[c*nDims + d] = cluster mean for cluster c in dimension d
	FArray Cov; // Cov[c*nDims*nDims + i*nDims + j] = Covariance for cluster C, entry i,j
					// NB covariances are stored in upper triangle (j>=i)
	FArray LogP; // LogP[p*MaxClusters + c] = minus log likelihood for point p in cluster c
	IArray Class; // Class[p] = best cluster for point p
	IArray OldClass; // Class[p] = previous cluster for point p
	IArray Class2; // Class[p] = second best cluster for point p
	IArray BestClass; // BestClass = best classification yet achieved
	IArray ClassAlive; // contains 1 if the class is still alive - otherwise 0
    IArray AliveIndex; // a list of the alive classes to iterate over

};
