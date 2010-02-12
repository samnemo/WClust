// $Id: IsolationDist.h,v 1.3 2008/08/11 20:50:17 samn Exp $ 
#ifndef ISOLATION_DIST
#define ISOLATION_DIST

#include <vector>

using namespace std;

//compute Isolation Distance
float IsolationDist(vector<int>& vClustIDs,int iClustID,vector<double>& vFloat,int iRows,int iCols);

//compute L-Ratio of a cluster
float LRatio(vector<int>& vClustIDs,int iClustID,vector<double>& vFloat,int iRows,int iCols);

#endif
