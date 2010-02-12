#pragma once

#include "UPDialog.h"

#include <vector>

using namespace std;

#define APNUM double

//void apcluster_main(int argc, char** argv,vector<int>& vIDs,const CUPDUPDATA* pUp);
bool apcluster_main(vector<APNUM>& s,vector<unsigned long>& i,vector<unsigned long>& k,int maxits,int convits,APNUM lam,int n,vector<int>& vIDs,const CUPDUPDATA* pUp);

