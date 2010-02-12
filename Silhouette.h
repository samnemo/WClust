// $Id: Silhouette.h,v 1.1 2008/04/24 04:54:24 samn Exp $ 
#ifndef SILHOUETTE_H
#define SILHOUETTE_H

#include <vector>
#include "A2D.h"
#include "Cluster.h"

using namespace std;

bool Silhouette(vector<float>& vFloat,int iCols,int iRows,vector<int>& vClustIDs,vector<int>& vCounts,int iClusts,vector<ClusterInfo>& vCI,A2D<int>& vBestDims,int iBestDims,const CUPDUPDATA* pUp);

#endif
