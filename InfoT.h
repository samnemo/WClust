// $Id: InfoT.h,v 1.10 2008/08/11 20:46:58 samn Exp $ 
#ifndef INFOT_H
#define INFOT_H

#include "Hist.h"
#include "HashHist.h"

//calculates kldiv from p to q using previously computed nearest neighbors
//this version assumes both p and q have ids of iClustIDP and iClustIDQ
//this means q is not the 'background' cluster but an actual cluster!!
prob_t FastKLDivPQ(KDTreeHist& p,KDTreeHist& q,Neighbor** vNeighbors,int iClustIDP,int iClustIDQ,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount);

//calculates kldiv from p to notp using previously computed nearest neighbors
//this version assumes p is an actual cluster and notp is
//the complement of p == 'background' cluster -- doesn't have it's own ID!!
prob_t FastKLDivPNOTP(KDTreeHist& p,KDTreeHist& notp,Neighbor** vNeighbors,int iClustIDP,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount);

//calculates kldiv from notp to p using previously computed nearest neighbors
//notp is the complement of p -- or the background cluster relative to p -- doesn't have it's own ID!
prob_t FastKLDivNOTPP(KDTreeHist& p,KDTreeHist& notp,Neighbor** vNeighbors,int iClustIDP,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount);

//calculates (symmetric) resistor average of kldiv. (kldiv(p,q)*kldiv(q,p))/(kldiv(p,q)+kldiv(q,p))
//here both p and q are actual clusters with proper IDs.
//neither p nor q is a 'background' cluster relative to the other
prob_t FastKLDivSymPQ(KDTreeHist& p,KDTreeHist& q,Neighbor** vNeighbors,int iClustIDP,int iClustIDQ,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount);

//calculates (symmetric) resistor average of kldiv between p and not p
//here p is an actual cluster with proper ID and not p is the background cluster relative to p
prob_t FastKLDivSymPNOTP(KDTreeHist& p,KDTreeHist& notp,Neighbor** pNeighbors,int iClustIDP,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount);

//thread to calculate minimum inter-cluster kldiv for each cluster and add it to Clusters.m_vInfo
bool InterClustKLD(CCluster& Clusters,vector<KDTreeHist>& vDistribs,vector<int>& vClustIDs,vector<int>& vClustCounts,int iClusts,bool bFast,Neighbor** vnn,int WhichDraw,const CUPDUPDATA* pUp,int iNNToFind,vector<int>& vNCount,vector<float>& vFloat,int iRows,int iCols);

//get prct of iNNToFind neighbors that have same cluster for each cluster
bool NeighborClustCount(Neighbor** vNeighbors,vector<int>& vClustIDs,vector<ClusterInfo>& vPrct,int iNNToFind,int iClusts);

//use probability approximations to calculate kldiv of each dimension independently (returns sum over dimensions)
prob_t KLDivApproxExclusiveProb(Hist& p,Hist& q);

//kldiv
//bAllowZeroQDist is used for when you don't want to allow any zero distances for
//probability calculations of elements in distribution q. this is not generally useful
//for distributions containing different elements, but if q is the FULL background distribution
//bAllowZeroQDist should be set to false. default == true and is whats used for standard KLDiv
//calculations of cluster quality
prob_t KLDiv(KDTreeHist& p,KDTreeHist& q,bool bAllowZeroQDist=true);
//symmetric kldiv
prob_t KLDivSym(KDTreeHist& p,KDTreeHist& q);
prob_t KLDivSym(HashHist& P,HashHist& Q);
//kldiv
prob_t KLDiv(Hist& p,Hist& q);
//resistor average
prob_t ResistorAvg(prob_t& p,prob_t& q);
//symmetrized form of KL-divergence
prob_t ResistorAvg(Hist& p,Hist& q);

//vZeroes = vector of dimensions that shouldn't contribute to calculations
//vZeroes[i] == 1 , iff you want to exclude dimension i
void CalcUDDist(vector< vector<Hist> >& vDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter,vector<int>& vCounts,int iElems,vector<int>& vZeroes,bool bUseCounts);

void CalcRDist(vector< vector<Hist> >& vDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter);

void CalcUDDist(vector< KDTreeHist >& vDistribs,vector< KDTreeHist >& vCompDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter,vector<int>& vCounts);

//entropy using previously computed nearest neighbors
prob_t Entropy(KDTreeHist& oTree,vector< Neighbor >& vNeighbors,int iClustID,vector<int>& vClustIDs);

void CalcEXInfo(vector< vector< Hist> >& vDistribs,vector<prob_t>& vcInf,int iClusts,int iDim,vector<int>& vCounts,int iElems);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                FindBestDims                                                          //
//                 find best dimensions for each cluster using KLDIV of 1 or 2 dimensional slices and ranking           //
//                                                                                                                      //
//  vFloat is full data matrix                                                                                          //
//  iClusts == # of clusters                                                                                            //
//  iCols == full # of dimensions                                                                                       //
//  iBestDims == desired # of dimensions after selection                                                                //
//  vCounts == vector of # of vectors in a cluster                                                                      //
//  vClustIDs == cluster IDs for each vector                                                                            //
//  vBestDims == stores best dimensions for all clusters                                                                //
//  vKLDivs == stores kldiv values when using 1D slices                                                                 //
//  b1D == true iff user wants 1D slices to estimate best dimensions                                                    //
//  vAxes == names of dimensions                                                                                        //
//  pUp == GUI progress control                                                                                         //
//                                                                                                                      //
bool FindBestDims(vector<float>& vFloat,int iClusts,int iCols,int iBestDims,vector<int>& vCounts,                       //
				  vector<int>& vClustIDs,A2D<int>& vBestDims,A2D<prob_t>& vKLDivs,bool b1D,MY_STR_STACK& vAxes,         //
				  const CUPDUPDATA* pUp);                                                                               //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int FindBest2DDims(vector<float>& vFloat,vector<float>& vRange,int iClusts,int iCols,int iBestDims,vector<int>& vCounts,
					vector<int>& vClustIDs,A2D<int>& vBestDims,A2D<KLD2D>& vKLDivs,MY_STR_STACK& vAxes,
					const CUPDUPDATA* pUp);
bool FindBest1DDims(vector<float>& vFloat,int iClusts,int iCols,int iBestDims,vector<int>& vCounts,
					vector<int>& vClustIDs,A2D<int>& vBestDims,A2D<prob_t>& vKLDivs,MY_STR_STACK& vAxes,
					const CUPDUPDATA* pUp);

//vZeroes = vector of dimensions that shouldn't contribute to calculations
//vZeroes[i] == 1 , iff you want to exclude dimension i
template< class T >
void CalcUDDist(vector< TreeHist<T> >& vDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter,vector<int>& vCounts,int iElems,vector<int>& vZeroes,bool bUseCounts)
{
	if(vDistribs.size() != iClusts + 2) return;

	vcInf = vector<prob_t>(iClusts+1);

	vcInfInter = vector< vector<prob_t> >(iClusts+1);
	int iC=1;
	for(iC=1;iC<=iClusts;iC++) vcInfInter[iC] = vector<prob_t>(iClusts+1);

	//uniqueness from full distribution for each cluster
	int iC1=1,iC2=1;
	for(iC1=1;iC1<=iClusts;iC1++)
		vcInf[iC1] = KLDiv(vDistribs[iC1],vDistribs[iClusts+1]);

	//inter-cluster distinction measures, KL divergence between
	//a cluster and cluster+other_cluster
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		for(iC2=1;iC2<=iClusts;iC2++)
		{
			if(iC1==iC2)
				vcInfInter[iC1][iC2]=0.0;
			else
			{
				TreeHist<T> oTmp = vDistribs[iC1];
				oTmp.Add(vDistribs[iC2]);
				vcInfInter[iC1][iC2] += KLDiv(vDistribs[iC1],oTmp);
			}				
		}
	}
	//add smallest inter-cluster KL-div
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		prob_t dMinInter = 9e10;
		bool bFound = false;
		if(vCounts[iC1])
		for(iC2=1;iC2<=iClusts;iC2++)
		{
			if(iC1 != iC2 && vCounts[iC2] && vcInfInter[iC1][iC2] < dMinInter)
			{
				dMinInter = vcInfInter[iC1][iC2];
				bFound = true;
			}
		}
		if(bFound)
			vcInf[iC1] += dMinInter;
	}

	if(bUseCounts)
	{
		for(iC1=1;iC1<=iClusts;iC1++)
		{
			vcInf[iC1] = ( (prob_t)vCounts[iC1] / iElems) * vcInf[iC1];
		}
	}
}

template< class T >
prob_t KLDiv(TreeHist<T>& p,TreeHist<T>& q)
{
	prob_t d = 0.0;

	int i = 0;

	prob_t eps = 1e-40;

	typename std::map< vpwrap<T>, int>::iterator PIT = p.Begin();
	for(;PIT!=p.End();PIT++)
	{
		prob_t pp = p.GetITProb(PIT);
		if(pp<=eps) 
			continue;
		
		prob_t qp = q.GetVProb(PIT->first.p_);
		
		if(qp<=eps) 
			continue;
		
		d += pp * ( log2(pp) - log2(qp) );
	}
	return d;
}


#endif
