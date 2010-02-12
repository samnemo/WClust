// $Id: InfoT.cpp,v 1.12 2008/05/06 02:20:35 samn Exp $ 
#include "stdafx.h"
#include "InfoT.h"
#include "WCMath.h"
#include "combination.h"

//#define LOG_TEST 1

#ifdef DO_TIMING
 MTimer oMT("getnnrsq btest=true");
 MTimer oMF("getnnrsq btest=false");
#endif

#define SLOWER_KD 1
#define FASTER_KD 2
#define BOTH_KD 4
#define KD_MODE FASTER_KD

bool NeighborClustCount(Neighbor** vNeighbors,vector<int>& vClustIDs,vector<ClusterInfo>& vPrct,int iNNToFind,int iClusts)
{
	int iSz = vClustIDs.size();
	if(vPrct.size()!=iClusts+1)
		return false;
	int i;
	vector<int> vCounts(iClusts+1);
	for(i=1;i<=iClusts;i++)
		vCounts[i]=count(vClustIDs.begin(),vClustIDs.end(),i);
	vector< vector<prob_t> > vprct(iClusts+1);
	for(i=1;i<=iClusts;i++)
		vprct[i]=vector<prob_t>(vCounts[i],0.0);
	vector<int> vIndex(iClusts+1,0);
	for(i=0;i<iSz;i++)
	{
		int j, iCount = 0, iClust = vClustIDs[i];
		if(!iClust)continue;//skip background vectors
		int idx = vIndex[iClust]++;
		for(j=0;j<iNNToFind;j++)
		{
			if(vClustIDs[vNeighbors[i][j].m_id]==iClust)
				vprct[iClust][idx]+=1.0f;
		}
		vprct[iClust][idx] /= iNNToFind;
	}
	for(i=1;i<=iClusts;i++)
		vPrct[i].m_fPrctKNNInClust=Avg(vprct[i]);
	return true;
}

prob_t FastKLDivPQ(KDTreeHist& p,KDTreeHist& q,Neighbor** vNeighbors,int iClustIDP,int iClustIDQ,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount)
{
	int iFastP = 0, iFastQ = 0, iSlowP = 0 , iSlowQ = 0;

#ifdef LOG_TEST
	double kldiv = 1.0;
	double kldivQ=1.0,kldivP=1.0;
#else
	prob_t kldiv = 0.0;
#endif
	int isz = vClustIDs.size() , iV = 0, iT = 0, iNN = 0, iLessP = 0, iLessQ = 0;

	for(iV=0;iV<isz;iV++)
	{
		if(vClustIDs[iV]!=iClustIDP)	//skip vectors not in cluster p
			continue;

		prob_t dDistP = 0.0 , dDistQ = 0.0; // squared distances
		
		Neighbor* vnn = vNeighbors[iV]; //these are the neighbors of an element in cluster p
		int nsz = vNCount[iV]; //iNNToFind;//vnn.size();
		bool bFoundP = false, bFoundQ = false;
		for(iNN=0;iNN<nsz;iNN++)
		{	Neighbor& oN = vnn[iNN];
			if(!bFoundP && vClustIDs[oN.m_id]==iClustIDP)
			{	if(oN.m_dist>0.0)
				{	//found a different neighbor in the same cluster p
					dDistP = oN.m_dist;
					iFastP++;
					bFoundP = true;
				}
			}
			else if(!bFoundQ && vClustIDs[oN.m_id]==iClustIDQ)
			{	//found a neighbor in the other cluster q
				dDistQ = oN.m_dist;
				iFastQ++;
				bFoundQ = true;
			}
			if(bFoundP && bFoundQ) break;	//found neighbors so break
		}
		if(!bFoundP)
		{	//use slow method of searching KD-tree
#if KD_MODE == BOTH_KD // to make sure get same results
			dDistP = p.GetNearestRadiusSQ(iT,false);
			float dDistP2 = p.GetNearestRadiusSQ(iT,true);
			if(dDistP!=dDistP2)
			{
				Write2Log("not eq %.6f %.6f",dDistP,dDistP2);
			}
#elif KD_MODE == SLOWER_KD // in case have to revert for some unkown reason...
			dDistP = p.GetNearestRadiusSQ(iT,false);
#else
			dDistP = p.GetNearestRadiusSQ(iT,true);
#endif
			iSlowP++;
		}
		if(!bFoundQ)
		{	//use slow method of searching KD-tree
#if KD_MODE == BOTH_KD // to make sure get same results
			dDistQ = q.GetNearestRadiusSQ(p[iT],true,false);//true==alow zero distance,since diff cluster,can have same exact point with 0 dist
			float dDistQ2 = q.GetNearestRadiusSQ(p[iT],true,true);
			if(dDistQ2!=dDistQ)
			{
				Write2Log("not eq %.6f %.6f",dDistQ,dDistQ2);
			}
#elif KD_MODE == SLOWER_KD
			dDistQ = q.GetNearestRadiusSQ(p[iT],true,false);//true==alow zero distance,since diff cluster,can have same exact point with 0 dist
#else
			dDistQ = q.GetNearestRadiusSQ(p[iT],true,true);//true==alow zero distance,since diff cluster,can have same exact point with 0 dist
#endif
			iSlowQ++;
		}

		if(dDistP>0.0 && dDistQ>0.0)
		{	//avoid exceptions of log(0)
			//if(dDistP<dDistQ)iLessP++; else iLessQ++;
#ifdef LOG_TEST
			//kldiv *= dDistQ / dDistP;
	//		kldivQ *= dDistQ;
	//		kldivP *= dDistP;
			kldiv += log( dDistQ / dDistP );
#else
			kldiv += log2( dDistQ / dDistP ) / 2.0;
#endif
		}
		
		iT++;	//increment index into cluster p's KD-tree
	}
	//finish the calculation
#ifdef LOG_TEST
	//kldiv = log2( kldiv ) / 2.0;
	//kldiv = log2( kldivQ / kldivP ) / 2.0;
	kldiv /= ( log(2.0) * 2.0 );
#endif
	kldiv *= p.NumDims() / ((prob_t) p.NumElems() );
	kldiv += log2( (prob_t)q.NumElems() / (p.NumElems()-1.0 ) );
	//write some stats
	Write2Log("FastKLDivPQ:kldiv=%.4f iSlow=%d iSlowP=%d iSlowQ=%d iFast=%d iFastP=%d iFastQ=%d",kldiv,iSlowP+iSlowQ,iSlowP,iSlowQ,iFastP+iFastQ,iFastP,iFastQ);
	
	return kldiv;
}

prob_t FastKLDivPNOTP(KDTreeHist& p,KDTreeHist& notp,Neighbor** vNeighbors,int iClustIDP,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount)
{
	int iFastP = 0, iFastNotP = 0, iSlowP = 0 , iSlowNotP = 0;

#ifdef LOG_TEST
	double kldiv = 1.0;
	double kldivP=1.0,kldivNOTP=1.0;
#else
	prob_t kldiv = 0.0;
#endif
	int isz = vClustIDs.size() , iV = 0, iT = 0, iNN = 0, iLessP = 0, iLessNotP = 0;

	for(iV=0;iV<isz;iV++)
	{
		if(vClustIDs[iV]!=iClustIDP)	//skip vectors not in cluster p
			continue;

		prob_t dDistP = 0.0 , dDistNotP = 0.0; // squared distances
		
		Neighbor* vnn = vNeighbors[iV]; //these are the neighbors of an element in cluster p
		int nsz = vNCount[iV]; //iNNToFind;//vnn.size();
		bool bFoundP = false, bFoundNotP = false;
		for(iNN=0;iNN<nsz;iNN++)
		{	Neighbor& oN = vnn[iNN];
			if(!bFoundP && vClustIDs[oN.m_id]==iClustIDP)
			{	if(oN.m_dist>0.0)
				{	//found a different neighbor in the same cluster p
					dDistP = oN.m_dist;
					iFastP++;
					bFoundP = true;
				}
			}
			else if(!bFoundNotP && vClustIDs[oN.m_id]!=iClustIDP)
			{	//found a neighbor in the other cluster q
				dDistNotP = oN.m_dist;
				iFastNotP++;
				bFoundNotP = true;
			}
			if(bFoundP && bFoundNotP) break;	//found neighbors so break
		}
		if(!bFoundP)
		{	//use slow method of searching KD-tree
#if KD_MODE == BOTH_KD // to make sure get same results
			dDistP = p.GetNearestRadiusSQ(iT,false);
			float dDistP2 = p.GetNearestRadiusSQ(iT,true);
			if(dDistP!=dDistP2)
			{
				Write2Log("not eq %.6f %.6f",dDistP,dDistP2);
			}
#elif KD_MODE == SLOWER_KD
			dDistP = p.GetNearestRadiusSQ(iT,false);
#else
			dDistP = p.GetNearestRadiusSQ(iT,true);
#endif
			iSlowP++;
		}
		if(!bFoundNotP)
		{	//use slow method of searching KD-tree
#if KD_MODE == BOTH_KD // to make sure get same results
			dDistNotP = notp.GetNearestRadiusSQ(p[iT],true,false);//true==alow zero distance,since diff cluster,can have same exact point with 0 dist
			float dDistNotP2 = notp.GetNearestRadiusSQ(p[iT],true,true);
			if(dDistNotP!=dDistNotP2)
			{
				Write2Log("not eq %.6f %.6f",dDistNotP,dDistNotP2);
			}
#elif KD_MODE == SLOWER_KD
			dDistNotP = notp.GetNearestRadiusSQ(p[iT],true,false);//true==alow zero distance,since diff cluster,can have same exact point with 0 dist
#else
			dDistNotP = notp.GetNearestRadiusSQ(p[iT],true,true);//true==alow zero distance,since diff cluster,can have same exact point with 0 dist
#endif
			iSlowNotP++;
		}

		if(dDistP>0.0 && dDistNotP>0.0)
		{	//avoid exceptions of log(0)
			//if(dDistP<dDistNotP)iLessP++; else iLessNotP++;
#ifdef LOG_TEST
			//kldiv *= dDistNotP / dDistP;
			//kldivP *= dDistP;
			//kldivNOTP *= dDistNotP;
			kldiv += log ( dDistNotP / dDistP );
#else
			kldiv += log2( dDistNotP / dDistP ) / 2.0;
#endif
		}
		
		iT++;	//increment index into cluster p's KD-tree
	}
	//finish the calculation
#ifdef LOG_TEST
	//kldiv = log2( kldiv ) / 2.0;
	//kldiv = log2( kldivNOTP / kldivP ) / 2.0;
	kldiv /= ( log(2.0) * 2.0 );
#endif
	kldiv *= p.NumDims() / ((prob_t) p.NumElems() );
	kldiv += log2( (prob_t)notp.NumElems() / (p.NumElems()-1.0 ) );
	//write some stats
	Write2Log("FastKLDivPNOTP:kldiv=%.4f iSlow=%d iSlowP=%d iSlowNotP=%d iFast=%d iFastP=%d iFastNotP=%d",kldiv,iSlowP+iSlowNotP,iSlowP,iSlowNotP,iFastP+iFastNotP,iFastP,iFastNotP);
	
	return kldiv;
}

//notp is all points not in p
//this version gets the distance from notp to p
prob_t FastKLDivNOTPP(KDTreeHist& p,KDTreeHist& notp,Neighbor** vNeighbors,int iClustIDP,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount)
{
	int iFastP = 0, iFastNP = 0, iSlowP = 0, iSlowNP = 0;

#ifdef LOG_TEST
	double kldiv = 1.0;
	double kldivNOTP=1.0,kldivP=1.0;
#else
	prob_t kldiv = 0.0;
#endif
	int isz = vClustIDs.size() , iV = 0, iT = 0, iNN = 0, iLessP = 0, iLessNotP = 0;

	for(iV=0;iV<isz;iV++)
	{
		if(vClustIDs[iV]==iClustIDP) //skip all points in p
			continue;

		prob_t dDistP = 0.0 , dDistNotP = 0.0;
		
		Neighbor* vnn = vNeighbors[iV];
		int nsz = vNCount[iV]; //iNNToFind;
		bool bFoundP = false, bFoundNotP = false;
		for(iNN=0;iNN<nsz;iNN++)
		{	Neighbor& oN = vnn[iNN];
			if(!bFoundP && vClustIDs[oN.m_id]==iClustIDP)
			{
				dDistP = oN.m_dist;
				iFastP++;
				bFoundP = true;
			}
			else if(!bFoundNotP && vClustIDs[oN.m_id]!=iClustIDP)
			{
				if(oN.m_dist>0.0)
				{
					dDistNotP = oN.m_dist;
					iFastNP++;
					bFoundNotP = true;
				}
			}
			if(bFoundP && bFoundNotP) break;
		}
		if(!bFoundP)
		{
#if KD_MODE == BOTH_KD // to make sure get same results
			dDistP = p.GetNearestRadiusSQ(notp[iT],true,false);
			float dDistP2 = p.GetNearestRadiusSQ(notp[iT],true,true);
			if(dDistP!=dDistP2)
			{
				Write2Log("not eq %.6f %.6f",dDistP,dDistP2);
			}
#elif KD_MODE == SLOWER_KD
			dDistP = p.GetNearestRadiusSQ(notp[iT],true,false);
#else
			dDistP = p.GetNearestRadiusSQ(notp[iT],true,true);
#endif
			iSlowP++;
		}
		if(!bFoundNotP)
		{
#if KD_MODE == BOTH_KD // to make sure get same results
			dDistNotP = notp.GetNearestRadiusSQ(iT,false);
			float dDistNotP2 = notp.GetNearestRadiusSQ(iT,true);
			if(dDistNotP!=dDistNotP2)
			{
				Write2Log("not eq %.6f %.6f",dDistNotP,dDistNotP2);
			}
#elif KD_MODE == SLOWER_KD
			dDistNotP = notp.GetNearestRadiusSQ(iT,false);
#else
			dDistNotP = notp.GetNearestRadiusSQ(iT,true);
#endif
			iSlowNP++;
		}

		if(dDistP>0.0 && dDistNotP>0.0)
		{
			//if(dDistNotP<dDistP)iLessNotP++; else iLessP++;
#ifdef LOG_TEST
			//kldiv *= dDistP / dDistNotP;
			//kldivP *= dDistP;
			//kldivNOTP *= dDistNotP;
			kldiv += log ( dDistP / dDistNotP );
#else
			kldiv += log2( dDistP / dDistNotP ) / 2.0;
#endif
		}
		
		iT++;
	}
#ifdef LOG_TEST
	//kldiv = log2( kldiv ) / 2.0;
	//kldiv = log2( kldivP / kldivNOTP ) / 2.0;
	kldiv /= ( log(2.0) * 2.0 );
#endif
	kldiv *= notp.NumDims() / ((prob_t) notp.NumElems() );
	kldiv += log2( (prob_t)p.NumElems() / (notp.NumElems()-1.0 ) );

	Write2Log("FastKLDivNOTPP:kldiv=%.4f iSlow=%d iSlowP=%d iSlowNP=%d iFast=%d iFastP=%d iFastNP=%d",kldiv,iSlowP+iSlowNP,iSlowP,iSlowNP,iFastP+iFastNP,iFastP,iFastNP);
	
	return kldiv;
}

//symmetric kldiv 
//(dpq * dqp ) / (dpq + dqp)
//dpq == distance from p to q
//dqp == distance from q to p
//both q and p are actual clusters with valid IDs!!
prob_t FastKLDivSymPQ(KDTreeHist& p,KDTreeHist& q,Neighbor** vNeighbors,int iClustIDP,int iClustIDQ,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount)
{
	prob_t dpq = FastKLDivPQ(p,q,vNeighbors,iClustIDP,iClustIDQ,vClustIDs,iNNToFind,vNCount), //dist from p to q
		   dqp = FastKLDivPQ(q,p,vNeighbors,iClustIDQ,iClustIDP,vClustIDs,iNNToFind,vNCount); //dist from q to p
	if(!dpq && !dqp) return 0.0;
	return dpq*dqp / (dpq+dqp);
}

//symmetric kldiv
//(dpnotp * dnotpp) / (dpnotp + dnotpp)
//dpnotp == distance from p to not p
//dnotpp == distance from not p to p
//p == actual cluster with valid ID
//notp == complement of p (all vectors not in p), without an actual ID 
prob_t FastKLDivSymPNOTP(KDTreeHist& p,KDTreeHist& notp,Neighbor** pNeighbors,int iClustIDP,vector<int>& vClustIDs,int iNNToFind,vector<int>& vNCount)
{
	prob_t dpq = FastKLDivPNOTP(p,notp,pNeighbors,iClustIDP,vClustIDs,iNNToFind,vNCount),  //dist from p to notp
		   dqp = FastKLDivNOTPP(p,notp,pNeighbors,iClustIDP,vClustIDs,iNNToFind,vNCount); //dist from notp to p
	if(!dpq && !dqp) return 0.0;
	return dpq*dqp / (dpq+dqp);
}

//make tree using dimension indices stored in pBestDims
//iRows is # of elements in full vFloat matrix, iCols is full # of dimensions in vFloat
void FillTree(vector<float>& vFloat,int iRows,int iCols,int iClustID,int iClustSz,vector<int>& vClustIDs,int* pBestDims,int iBestDims,KDTreeHist& oT,vector<float>& vClustData)
{
	vClustData.resize(iClustSz*iBestDims);
	int iV = 0 , idx = 0;
	for(;iV<iRows;iV++)
	{	if(vClustIDs[iV]!=iClustID) continue;
		int iD = 0;
		for(;iD<iBestDims;iD++)
			vClustData[idx++]=vFloat[iV*iCols+pBestDims[iD]];
	}
	oT.SetData(iBestDims,&vClustData[0],iClustSz);
}

//computes kldiv between each pair of actual clusters and adds the min to CCluster::m_vInfo
bool InterClustKLD(CCluster& Clusters,vector<KDTreeHist>& vDistribs,vector<int>& vClustIDs,vector<int>& vClustCounts,int iClusts,bool bFast,Neighbor** vnn,int WhichDraw,const CUPDUPDATA* pUp,int iNNToFind,vector<int>& vNCount,vector<float>& vFloat,int iRows,int iCols)
{	//kldiv cluster to other clusters
	int iC1 = 0, iC2 = 0, iTot = ((iClusts-1)*(iClusts-1)+iClusts-1)/2, iCurr = 0;
	try
	{	CString msg;
		vector< vector<prob_t> > vcInfInter(iClusts+1, vector<prob_t>(iClusts+1));		
		Write2Log("Calculating inter-cluster KLDiv");
		prob_t kldiv = 0.0f;
		if(Clusters.m_bFindBestDims)	//compute inter-clust kldiv using best dimensions
		{	iTot = iClusts*iClusts;		//distances are not symmetrical since use different dimensions
			for(iC1=1;iC1<=iClusts && !pUp->ShouldTerminate();iC1++)
			{	for(iC2=1;iC2<=iClusts && !pUp->ShouldTerminate();iC2++,iCurr++)
				{	if(iC2==iC1) continue;
					msg.Format("Calculating kldiv btwn clust %d and %d",iC1,iC2);
					pUp->SetProgress(msg,100*(iCurr/static_cast<double>(iTot)));
					Write2Log(msg);
					KDTreeHist oT; vector<float> vTmpData;	//make temporary tree
					FillTree(vFloat,iRows,iCols,iC2,vClustCounts[iC2],vClustIDs,Clusters.m_vBestDims[iC1],Clusters.m_iBestDims,oT,vTmpData);
					vcInfInter[iC1][iC2]=KLDivSym(vDistribs[iC1],oT);
					Write2Log("sym. kldiv from %d to %d = %.4f",iC1,iC2,vcInfInter[iC1][iC2]);
				}
			}
		}
		else
		{	for(iC1=1;iC1<=iClusts && !pUp->ShouldTerminate();iC1++)
			{	for(iC2=iC1+1;iC2<=iClusts && !pUp->ShouldTerminate();iC2++,iCurr++)
				{	msg.Format("Calculating kldiv btwn clust %d and %d",iC1,iC2);
					pUp->SetProgress(msg,100*(iCurr/static_cast<double>(iTot)));
					Write2Log(msg);
					if(bFast)
						vcInfInter[iC1][iC2]=vcInfInter[iC2][iC1]=FastKLDivSymPQ(vDistribs[iC1],vDistribs[iC2],vnn,iC1,iC2,vClustIDs,iNNToFind,vNCount);
					else
						vcInfInter[iC1][iC2]=vcInfInter[iC2][iC1]=KLDivSym(vDistribs[iC1],vDistribs[iC2]);
					Write2Log("sym. kldiv from %d to %d = %.4f",iC1,iC2,vcInfInter[iC1][iC2]);
				}
			}
		}
		if(!pUp->ShouldTerminate()){ for(iC1=1;iC1<=iClusts;iC1++)
		{	prob_t min_int = iClusts>1 ? INF : 0.0; //*INF;
			int min_ind = 0;
			if(iClusts>1) for(iC2=1;iC2<=iClusts;iC2++)
			{	if(iC1==iC2 || vClustCounts[iC2]<2)continue;
				prob_t tmpK = vcInfInter[iC1][iC2];
				if(tmpK<min_int)
				{	min_int=tmpK; 
					min_ind=iC2; 
				}
			}
			Clusters.m_vInfo[WhichDraw][iC1].m_fInterClustGain = min_int;
			Clusters.m_vInfo[WhichDraw][iC1].m_iClosestID = min_ind;
			Write2Log("Nearest kldiv from clust %d to %d is %.6f",iC1,min_ind,min_int);
		}
		CString strTab("\ninter clust kldiv table\n") , strTmp;//write inter-cluster kldiv table to log for inspection...
		for(iC1=1;iC1<=iClusts;iC1++)
		{	for(iC2=1;iC2<=iClusts;iC2++)
			{	strTmp.Format("%.6f\t",vcInfInter[iC1][iC2]);
				strTab += strTmp;
			}
			strTab += "\n";			
		}
		Write2Log(strTab);
		}
	} 
	catch(...)
	{	Write2Log("Exception in InterClustKLD!!! iC1=%d iC2=%d iClusts=%d",iC1,iC2,iClusts);
		return false;
	}
	return true;
}

//gets KLDiv using approximate probabilities
//P(j) = (nj+0.5)/(N+0.5jmax)
//n1+n2+...+nmax=N
//the p distribution's counts are first subtracted
//from the q distribution so that the function will
//calculate the "exclusive" information gain
//p is "partial" distribution
//q is "full" distribution
prob_t KLDivApproxExclusiveProb(Hist& p,Hist& q)
{
	if(p.NumBins() != q.NumBins()) return -1.0;

	prob_t d = 0.0;

	int i = 0, iBins = p.NumBins();

	prob_t eps = 1e-100;

	prob_t pelems = p.NumElems();
	prob_t qelems = q.NumElems();

	for(i=0;i<iBins;i++)
	{
		prob_t pcount = p[i];
		prob_t qcount = q[i];

		//approximate probability of bin i in distribution p
		prob_t pp = (pcount+0.5) / ( pelems + 0.5*iBins);

		//approximate probability of bin i in distribution q
		//must subtract pcount elements from numerator & denominator of probability
		//approximation so as to maintain "exclusivity" of p distribution
//		prob_t qp = (qcount - pcount + 0.5) / ( qelems - pcount + 0.5*iBins);
		prob_t qp = (qcount - pcount + 0.5) / ( qelems - pelems + 0.5*iBins);

		if(pp<=eps) continue;

		if(qp<=eps) continue;

		d += pp * ( log2(pp) - log2(qp) );
	}

	return d;
}

prob_t KLDiv(KDTreeHist& p,KDTreeHist& q)
{	
	int i=0;
#ifdef DO_TIMING
	char sMsg[1024];
	sprintf(sMsg,"KLDiv szp=%d, szq=%d, dims=%d",p.NumElems(),q.NumElems(),p.NumDims());
	ScopedTimer S(sMsg);
#endif
	
	try	//just in case something goes wrong from invalid type of data passed in
	{
		//make sure we can do calculation with div by zero and need same # of dimensions 
		//in each distribution
		if(p.NumElems() < 2 || q.NumElems()<1 || p.NumDims()!=q.NumDims()) 
			return 0.0;

		int iLessP = 0, iLessQ = 0;

		prob_t dpq = 0.0;

		const prob_t eps = 0.0;

		int isz = p.NumElems();

		const bool bWrite = p.NumDims()==2;
		int iWrite = 0;

		i = 0;
		for(i=0;i<isz;i++)
		{
			//prob_t distp2 = p.GetNearestRadiusSQ(i,false);
//#ifdef _DEBUG
			//prob_t distp = p.GetNearestRadiusSQ(i,true);
			Neighbor np;
			p.GetNearestNeighbor(p[i],false,np);
			prob_t distp = np.m_dist;
			int checkpid = i;
			float checkpd = 0.0;
			if(bWrite && iWrite<10)
			{
				int j=0,k=0;
				float mind = 1e6;
				int minid = i;
				for(;j<isz;j++)
				{
					if(j==i) continue;
					float* p1 = p[i] , *p2 = p[j];
					float tmpd = 0.0;
					for(k=0;k<2;k++)
						tmpd += (p1[k]-p2[k])*(p1[k]-p2[k]);
					if(tmpd < mind)
					{ mind = tmpd; minid = checkpid = j; checkpd = tmpd;}
				}
			}
			//if(distp!=distp2)
			{
			//	Write2Log("not eq %.6f %.6f",distp,distp2);
			}
//#endif
			
			if(distp<=eps){
				Write2Log("continuep i=%d",i); continue;}

			//prob_t distq2 = q.GetNearestRadiusSQ(p[i],true,false);
//#ifdef _DEBUG
			//prob_t distq = q.GetNearestRadiusSQ(p[i],true,true);
			Neighbor nq;
			q.GetNearestNeighbor(p[i],true,nq);
			prob_t distq = nq.m_dist;
			//if(distq != distq2)
			{
			//	Write2Log("not eq %.6f %.6f",distq,distq2);
			}
//#endif
			
			if(distq<=eps){
				Write2Log("continueq i=%d",i); continue;}

			dpq += log2(distq / distp) / 2.0;

			if(bWrite && iWrite++<10)
			{	Write2Log("i%d chkp%d chkpd=%g p%d q%d dp=%g dq=%g dq/dp=%g log2(dq/dp)=%g dpq=%g",
							i,checkpid,checkpd,np.m_id,nq.m_id,distp,distq,distq/distp,log2(distq/distp),dpq);
				Write2Log(" p[%d][0]=%g p[%d][1]=%g ",i,p[i][0],i,p[i][1]);
				Write2Log(" p[%d][0]=%g p[%d][1]=%g ",np.m_id,p[np.m_id][0],np.m_id,p[np.m_id][1]);
			}
		}

		dpq *= ((prob_t)p.NumDims()/p.NumElems());

		dpq += log2( (prob_t)q.NumElems() / (p.NumElems()-1.0 ) );

	#ifdef DO_TIMING
		sprintf(sMsg,"iLessP=%d iLessQ=%d",iLessP,iLessQ);
		MessageBox(0,sMsg,"WClust",MB_ICONINFORMATION);
	#endif
		
		return dpq;
	}
	catch(...)
	{
		char sMsg[1024];
		sprintf(sMsg,"KLDiv caught exception!! i=%d",i);
		MessageBox(0,sMsg,"WClust",MB_ICONERROR);
		Write2Log(sMsg);
		return -666.0;
	}
}

//resistor avg.
prob_t KLDivSym(KDTreeHist& p,KDTreeHist& q)
{
	prob_t dpq = KLDiv(p,q), dqp = KLDiv(q,p);
	if(!dpq && !dqp) return 0.0;
	return dpq*dqp / (dpq+dqp);
}


//returns -1 iff num bins not equal
//otherwise returns KL divergence of histogram/distributions
//this returns "divergence" of distribution q from p
//so q should be a cluster's distribution and p should be full distribution
//make sure when passing in...
//the order matters because it is not symmetric
//however...
//In Bayesian statistics the KL divergence can be used as a measure of the information
//gain in moving from a prior distribution to a posterior distribution. If some new fact
//Y=y is discovered, it can be used to update the probability distribution for X from p(x|I)
//to a new posterior probability distribution p(x|y) using Bayes' theorem
// DKL(p(x|y)||p(x|I)) = sum( p(x|y) * log( p(x|y) / p(x|I) )
//so p can be cluster distribution and q can be full distribution
prob_t KLDiv(Hist& p,Hist& q)
{
	if(p.NumBins() != q.NumBins())
	{
		return -1.0;
	}

	prob_t d = 0.0;

	int i = 0 , iBins = p.NumBins();

	prob_t eps = 1e-100;

	for(i=0;i<iBins;i++)
	{
		prob_t pp = p.BinProb(i); //probability of bin i in distribution p
		
		if(pp<=eps) 
			continue;
		
		prob_t qp = q.BinProb(i); //probability of bin i in distribution q
		
		if(qp<=eps) 
			continue;
		
		d += pp * ( log2(pp) - log2(qp) );
	}

/*#ifdef _DEBUG
	if(d<0.0)
	{
		prob_t pf = FullProb(p);
		prob_t qf = FullProb(q);
		FILE* fp = fopen("kl_prob.txt","w");
		fprintf(fp,"p:\n");
		p.Print(fp);
		fprintf(fp,"q:\n");
		q.Print(fp);
		fclose(fp);
		int moo=0;
	}
#endif*/

	return d;
}

prob_t ResistorAvg(prob_t& p,prob_t& q)
{	if(!p && !q) return 0.0f;
	return p*q/(p+q);
}

//symmetrized form of KL-divergence
prob_t ResistorAvg(Hist& p,Hist& q)
{
	prob_t d12 = KLDiv(p,q), d21 = KLDiv(q,p);
	return (d12 * d21) / (d12 + d21);
}

//vZeroes = vector of dimensions that shouldn't contribute to calculations
//vZeroes[i] == 1 , iff you want to exclude dimension i
void CalcUDDist(vector< vector<Hist> >& vDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter,vector<int>& vCounts,int iElems,vector<int>& vZeroes,bool bUseCounts)
{
	if(vDistribs.size() != iClusts + 2) return;

	vcInf = vector<prob_t>(iClusts+1);

	vcInfInter = vector< vector<prob_t> >(iClusts+1);
	int iC=1;
	for(iC=1;iC<=iClusts;iC++) vcInfInter[iC] = vector<prob_t>(iClusts+1);

	int iDims = vDistribs[1].size() , iD = 0;

	//uniqueness from full distribution for each cluster
	int iC1=1,iC2=1;
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		prob_t kldiv=0.0;
		for(iD=0;iD<iDims;iD++)
		{
			if(vZeroes[iD]) continue;

			kldiv += KLDiv(vDistribs[iC1][iD],vDistribs[iClusts+1][iD]);
		}
		vcInf[iC1] = kldiv;
	}

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
				for(iD=0;iD<iDims;iD++)
				{
					if(vZeroes[iD]) continue;

					Hist oTmp = vDistribs[iC1][iD];
					oTmp.Add(vDistribs[iC2][iD]);
					vcInfInter[iC1][iC2] += KLDiv(vDistribs[iC1][iD],oTmp);
				}
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

void CalcUDDist(vector< KDTreeHist >& vDistribs,vector< KDTreeHist >& vCompDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter,vector<int>& vCounts)
{
	if(vDistribs.size() != iClusts + 2 || vDistribs.size()!=vCompDistribs.size()) return;

	vcInf = vector<prob_t>(iClusts+1);

	vcInfInter = vector< vector<prob_t> >(iClusts+1);
	int iC=1;
	for(iC=1;iC<=iClusts;iC++) vcInfInter[iC] = vector<prob_t>(iClusts+1);

	//uniqueness from full distribution for each cluster
	int iC1=1,iC2=1;
	for(iC1=1;iC1<=iClusts;iC1++)
		vcInf[iC1] = KLDiv(vDistribs[iC1],vCompDistribs[iC1]);//KLDivSym(vDistribs[iC1],vCompDistribs[iC1]);

	//inter-cluster distinction measures, KL divergence between
	//a cluster and cluster+other_cluster
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		//for(iC2=iC1+1;iC2<=iClusts;iC2++)
		for(iC1=1;iC2<=iClusts;iC2++)
		{
			if(iC1==iC2)
				vcInfInter[iC1][iC2]=0.0;
			else
				//vcInfInter[iC1][iC2] =  vcInfInter[iC2][iC1] = KLDivSym(vDistribs[iC1],vDistribs[iC2]);
				vcInfInter[iC1][iC2] =  KLDiv(vDistribs[iC1],vDistribs[iC2]);
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
}

//calculates information gain as resistor average of all clusters against full distribution
//plus smallest resistor average between cluster and all other clusters
//vDistribs must have size of iClusts + 1
void CalcRDist(vector< vector<Hist> >& vDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter)
{
	if(vDistribs.size() != iClusts + 2) return;

	vcInf = vector<prob_t>(iClusts+1);

	vcInfInter = vector< vector<prob_t> >(iClusts+1);
	int iC=1;
	for(iC=0;iC<=iClusts;iC++) vcInfInter[iC] = vector<prob_t>(iClusts+1);

	int iDims = vDistribs[1].size() , iD = 0;

	//only need this for first time, since its recalculated for distributions who's
	//points change, when they change
	int iC1=1,iC2=1;
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		prob_t kldiv=0.0;
		for(iD=0;iD<iDims;iD++)
		{
			kldiv += ResistorAvg(vDistribs[iC1][iD],vDistribs[iClusts+1][iD]);
		}
		vcInf[iC1]=kldiv;
	}

	//fill inter-cluster resistor averages
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		for(iC2=1;iC2<=iC1;iC2++)
		{
			if(iC1==iC2)
				vcInfInter[iC1][iC2]=0.0;
			else
			{
				for(iD=0;iD<iDims;iD++)
				{
					vcInfInter[iC1][iC2]+=ResistorAvg(vDistribs[iC1][iD],vDistribs[iC2][iD]);
				}
				vcInfInter[iC2][iC1]=vcInfInter[iC1][iC2];
			}				
		}
	}
	//add largest inter-cluster resistor average
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		prob_t dMinInter = 9e10;
		bool bFound = false;
		for(iC2=1;iC2<=iClusts;iC2++)
		{
			if(iC1 != iC2 && vcInfInter[iC1][iC2] < dMinInter)
			{
				dMinInter = vcInfInter[iC1][iC2];
				bFound = true;
			}
		}
		if(bFound)
			vcInf[iC1] += dMinInter;
	}
}

prob_t Entropy(KDTreeHist& oTree,vector<Neighbor>& vNeighbors,int iClustID,vector<int>& vClustIDs)
{
	//ScopedTimer S("Entropy...");

	int iFast = 0,iSlow = 0;

	prob_t dEntrop = 0.0;
	int isz = vClustIDs.size() , iV = 0, iT = 0, iNN = 0;

	for(iV=0;iV<isz;iV++)
	{
		if(vClustIDs[iV]!=iClustID)
			continue;

		prob_t dDist = 0.0;
		
		Neighbor& oN = vNeighbors[iV];
		if(vClustIDs[oN.m_id]==iClustID && oN.m_dist>0.0)
		{
			dDist = oN.m_dist;
			iFast++;
		}

		if(dDist <= 0.0)
		{
			dDist = oTree.GetNearestRadiusSQ(iT,true);
			iSlow++;
		}

		if(dDist >= 0.0)
		{
			prob_t dProb = oTree.RProb(MySqrt(dDist));

			if(dProb >= 0.0)
			{
				dEntrop += dProb * log2(dProb);
			}
		}
		
		iT++;
	}
	return -dEntrop;
}

void CalcEXInfo(vector< vector< Hist> >& vDistribs,vector<prob_t>& vcInf,int iClusts,int iDim,vector<int>& vCounts,int iElems)
{
	const bool bUseCounts = false;
	int iC;
	if(!vcInf.size()) vcInf = vector<prob_t>(iClusts+1);
	for(iC=1;iC<=iClusts;iC++)
	{
		prob_t kldiv=0.0;
		int iD=0;
		for(iD=0;iD<iDim;iD++)
		{
			kldiv += KLDivApproxExclusiveProb(vDistribs[iC][iD],vDistribs[iClusts+1][iD]);
		}
		if(bUseCounts)
			vcInf[iC] = ( (prob_t)vCounts[iC] / (prob_t)iElems ) * kldiv;
		else
			vcInf[iC] = kldiv;
	}
}

//which 2D slices do we skip?
//slices that are X vs X or
//T1-V(peak) and T1-Peak are highly correlated so skip them too
inline bool SkipPair(int idx1,int idx2)
{
	if(idx1==idx2) // same dimensions or...
		return true;

	// idx 0 thru 3 == T1,2,3,4-Peak
	// idx 4 thru 7 == T1,2,3,4-V(peak)
	// idx 8 thru 11 == T1,2,3,4-Valley
	// idx 12 thru 15 == T1,2,3,4-V-(valley)

	return (idx1==0 && idx2==4)||   // 0,4 4,0 T1-Peak and T1-V(peak)
		   (idx1==4 && idx2==0)||
		   (idx1==1 && idx2==5)||   // 1,5 5,1 T2-Peak and T2-V(peak)
		   (idx1==5 && idx2==1)||
		   (idx1==2 && idx2==6)||   // 2,6 6,2 T3-Peak and T3-V(peak)
		   (idx1==6 && idx2==2)||		   
		   (idx1==3 && idx2==7)||   // 3,7 7,3 T4-Peak and T4-V(peak)
		   (idx1==7 && idx2==3)||
		   (idx1==8 && idx2==12)||  // 8,12 12,8 T1-Valley and T1-V(valley)
		   (idx1==12 && idx2==8)||
		   (idx1==9 && idx2==13)||  // 9,13 13,9 T2-Valley and T2-V(valley)
		   (idx1==13 && idx2==9)||
		   (idx1==10 && idx2==14)|| // 10,14 14,10 T3-Valley and T3-V(valley)
		   (idx1==14 && idx2==10)||
		   (idx1==11 && idx2==15)|| // 11,15 15,11 T4-Valley and T4-V(valley)
		   (idx1==15 && idx2==11);	
}

bool HasSkipPair(set<int>& s,int i)
{
	set<int>::iterator IT = s.begin();
	for(;IT!=s.end();IT++)
		if(SkipPair(*IT,i))
			return true;
	return false;
}

float KLDivSym1D(vector<float>& vFloat,vector<int>& vClustIDs,vector<int>& vCounts,int iCols,int iRows,int iClust,int iDim)
{
	vector<float> v1DFloatClust(vCounts[iClust]),v1DFloatComp(iRows-vCounts[iClust]);
	int idxClust = 0, idxComp = 0;
	KDTreeHist o1DTClust,o1DTComp;
	int iV = 0;
	for(iV=0;iV<vClustIDs.size();iV++)
	{	if(vClustIDs[iV]==iClust)
			v1DFloatClust[idxClust++]=vFloat[iV*iCols+iDim];
		else
			v1DFloatComp[idxComp++]=vFloat[iV*iCols+iDim];
	}
	o1DTClust.SetData(1,&v1DFloatClust[0],vCounts[iClust]);
	o1DTComp.SetData(1,&v1DFloatComp[0],iRows-vCounts[iClust]);
	return KLDivSym(o1DTClust,o1DTComp);
}

bool FindBest2DDims(vector<float>& vFloat,vector<float>& vRange,int iClusts,int iCols,int iBestDims,vector<int>& vCounts,vector<int>& vClustIDs,A2D<int>& vBestDims,A2D<KLD2D>& vKLDivs,MY_STR_STACK& vAxes,const CUPDUPDATA* pUp)
{	vBestDims.Init(iClusts+1,iBestDims);//each cluster will get iBestDims to perform multidimensional kldiv on later
	bool bInit = false;
	//vKLDivs.Init(iClusts+1,iBestDims);
	int iC = 1 , iRows = vClustIDs.size() , iTot = iClusts*IntegerSum(iCols-1);
	double dJnk = 0.0;
	const float fMinRange = 0.009; //min range for a dimension to be usable
	CString msg;
	for(iC=1;iC<=iClusts;iC++)
	{	
		vector<KLD2D> vKLDivTmp(IntegerSum(iCols-1));
		int iD1,iD2, iK = 0;
		for(iD1=0;iD1<iCols;iD1++)
		{	
			for(iD2=iD1+1;iD2<iCols;iD2++,dJnk++)
			{
				if(SkipPair(iD1,iD2)) continue;

				//exclude 2D slice consisting of empty signal -- occurs when a wire is grounded
				//note index+1 -- this is because of CVertex using index 0 as # of clusters vertex belongs to
				//so all dimensions are offset by 1 -- see CVerxStack::GetFloatV for more details, that's
				//where vRange is initialized
				if(vRange[iD1+1]<fMinRange || vRange[iD2+1]<fMinRange)
				{	Write2Log("Skipping slice %s %s with ranges %.12f %.12f",*vAxes[iD1],*vAxes[iD2],vRange[iD1+1],vRange[iD2+1]);
					continue;
				}

#ifdef _DEBUG
				if(iD1==2 && iC==4)// && iD2==17)
				{
					int moo=0;
				}
#endif
				msg.Format("Finding best %d dimensions for cluster %d of %d : Dim1=%s Dim2=%s",iBestDims,iC,iClusts,*vAxes[iD1],*vAxes[iD2]);
				pUp->SetProgress(msg,100.0*dJnk/iTot);
				vector<float> v2DFloatClust(2*vCounts[iC]),v2DFloatComp(2*(iRows-vCounts[iC]));
				int idxClust = 0, idxComp = 0;
				KDTreeHist o2DTClust,o2DTComp;
				int iV = 0;
				for(iV=0;iV<vClustIDs.size();iV++)
				{	//initialize the trees
					if(vClustIDs[iV]==iC)
					{	v2DFloatClust[idxClust++]=vFloat[iV*iCols+iD1];
						v2DFloatClust[idxClust++]=vFloat[iV*iCols+iD2];
					}
					else
					{	v2DFloatComp[idxComp++]=vFloat[iV*iCols+iD1];
						v2DFloatComp[idxComp++]=vFloat[iV*iCols+iD2];
					}
				}
				o2DTClust.SetData(2,&v2DFloatClust[0],vCounts[iC]);
				o2DTComp.SetData(2,&v2DFloatComp[0],iRows-vCounts[iC]);
				prob_t kld = KLDivSym(o2DTClust,o2DTComp);
				Write2Log("2D kld C%d dims:%s %s = %.4f",iC,*vAxes[iD1],*vAxes[iD2],kld);
				vKLDivTmp[iK++].Init(iD1,iD2,kld);//compute & store kldiv
			}
		}
		vKLDivTmp.resize(iK);
		//sort results by kldiv values
		sort(vKLDivTmp.begin(),vKLDivTmp.end());

		if(!bInit)
		{
			vKLDivs.Init(iClusts+1,iK); // init here
			bInit=true;
		}

		copy(vKLDivTmp.begin(),vKLDivTmp.end(),&vKLDivs[iC][0]); // copy for later use

		//go through results, picking out top 8 dimensions, make sure not to pick
		//the same dimension twice
		int iFound = 0;
		set<int> sDims; // stores dimensions already picked
		int idx = iK-1, jdx; // start at best kldiv pair
		set< pair<float,int> > vKD1D;
		for(;idx>=0 && iFound<iBestDims;idx--)
		{	
			bool bHas1=sDims.find(vKLDivTmp[idx].m_iD1)!=sDims.end() || HasSkipPair(sDims,vKLDivTmp[idx].m_iD1),
				bHas2=sDims.find(vKLDivTmp[idx].m_iD2)!=sDims.end() || HasSkipPair(sDims,vKLDivTmp[idx].m_iD2);
			if(iFound+2<=iBestDims)
			{	//can add both
				if(!bHas1)
				{	sDims.insert(vKLDivTmp[idx].m_iD1);	//store which dims we already have
					vBestDims[iC][iBestDims-iFound-1]=vKLDivTmp[idx].m_iD1;
					iFound++;
				}
				if(!bHas2)
				{	sDims.insert(vKLDivTmp[idx].m_iD2);
					vBestDims[iC][iBestDims-iFound-1]=vKLDivTmp[idx].m_iD2;
					iFound++;
				}
			}
			else if(iFound+1<=iBestDims)
			{	//can add only 1
				if(bHas1 && !bHas2)
				{	sDims.insert(vKLDivTmp[idx].m_iD2);
					vBestDims[iC][iBestDims-iFound-1]=vKLDivTmp[idx].m_iD2;
					iFound++;
				}
				else if(bHas2 && !bHas1)
				{	sDims.insert(vKLDivTmp[idx].m_iD1);	//store which dims we already have
					vBestDims[iC][iBestDims-iFound-1]=vKLDivTmp[idx].m_iD1;
					iFound++;
				}
				else if(!bHas1)
				{	sDims.insert(vKLDivTmp[idx].m_iD1);	//store which dims we already have
					vBestDims[iC][iBestDims-iFound-1]=vKLDivTmp[idx].m_iD1;
					iFound++;
				}
				else if(!bHas2) // can't ever go here
				{	sDims.insert(vKLDivTmp[idx].m_iD2);
					vBestDims[iC][iBestDims-iFound-1]=vKLDivTmp[idx].m_iD2;
					iFound++;
				}
				/*               fun with truth tables
						has1   has2  !has1  !has2   has1 && !has2   has2 && !has1
						true   true  false  false       false          false
						true  false  false  true        true           false
						false  true  true   false       false          true
						false false  true   true        false          false
				*/
			}
		}
#if 1
		Write2Log("\nClust%d 2D kldiv pairs(best 16) info follows:\n",iC);
		LogF F;
		FILE* fp = F.Open();
		int y=iK-16>=0?iK-16:0;
		for(;y<iK;y++)
		{	fprintf(fp,"pair%d D1=%s D2=%s kld=%.4f\n",
				y,*vAxes[vKLDivTmp[y].m_iD1],*vAxes[vKLDivTmp[y].m_iD2],vKLDivTmp[y].m_kld);
		} fprintf(fp,"\n\n");
#endif
	}
	return true;
}

prob_t MaxAbsCorr(vector<vector<prob_t> >& vCorrelMat,int* vBestDims,int iDim,int iBestDims)
{
	int i;
	prob_t bestCor = 0.0, tmpCor = 0.0;
	for(i=iBestDims-1;i>=0;i--)
		if(vBestDims[i]!=iDim && (tmpCor=fabs(vCorrelMat[vBestDims[i]][iDim]))>bestCor)
			bestCor=tmpCor;
	return bestCor;
}

prob_t MaxAbsCorr(vector<vector<prob_t> >& vCorrelMat,vector< pair<float,int> >& vKLDDim,int iDim)
{
	int iSz = vKLDDim.size(), i = 0;
	prob_t maxScore = 0.0 , tmpScore = 0.0;
	for(i=0;i<iSz;i++)
	{	if( vKLDDim[i].second == iDim ) continue;
		if( (tmpScore = fabs(vCorrelMat[vKLDDim[i].second][iDim]))>maxScore)
			maxScore = tmpScore;
	}
	return maxScore;
}

prob_t KLDCorVal(vector<vector<prob_t> >& vCorrelMat,vector< pair<float,int> >& vKLDDim)
{
	int iSz = vKLDDim.size() , i = 0;
	prob_t score = 0.0;
	for(i=0;i<iSz;i++)
		score += vKLDDim[i].first * (1.0 - MaxAbsCorr(vCorrelMat,vKLDDim,vKLDDim[i].second));
	return score;
}

int GetReplaceIndex(vector<vector<prob_t> >& vCorrelMat,vector< pair<float,int> >& vOrig,pair<float,int>& oP)
{
	int iSz = vOrig.size() , iBestIDX = -1 , i = 0;
	prob_t maxVal = KLDCorVal(vCorrelMat,vOrig);
	prob_t curVal = maxVal;
	for(i=0;i<iSz;i++)
	{
		vector< pair<float,int> > vTmp(vOrig);
		vTmp[i]=oP;
		if( (curVal=KLDCorVal(vCorrelMat,vTmp)) > maxVal )
		{
			maxVal = curVal;
			iBestIDX = i;
		}
	}
	return iBestIDX;
}

bool FindBest1DDims(vector<float>& vFloat,int iClusts,int iCols,int iBestDims,vector<int>& vCounts,vector<int>& vClustIDs,A2D<int>& vBestDims,A2D<prob_t>& vKLDivs,MY_STR_STACK& vAxes,const CUPDUPDATA* pUp)
{	vBestDims.Init(iClusts+1,iBestDims);
	vKLDivs.Init(iClusts+1,iBestDims);
	vKLDivs.Fill(-99999.9f);
	int iC = 1 , iRows = vClustIDs.size();
	double dJnk = 0.0;
	CString msg;
	vector< vector<float> > vCorrelMat;
	vector<float> vMean;
	msg.Format("Computing %d X %d correlation matrix",iCols,iCols);
	pUp->SetProgress(msg);
	CovarMat(vFloat,vClustIDs.size(),iCols,vCorrelMat,vMean,true);
	for(iC=1;iC<=iClusts;iC++)
	{	int iD;
		vector< pair<float, int> > vKLDDims(iCols);
		for(iD=0;iD<iCols;iD++,dJnk++)
		{	msg.Format("Finding best %d dimensions for cluster %d of %d : Dim=%s",iBestDims,iC,iClusts,*vAxes[iD]);
			pUp->SetProgress(msg,100.0*dJnk/(iClusts*iCols));
			vector<float> v1DFloatClust(vCounts[iC]),v1DFloatComp(iRows-vCounts[iC]);
			int idxClust = 0, idxComp = 0;
			KDTreeHist o1DTClust,o1DTComp;
			int iV = 0;
			for(iV=0;iV<vClustIDs.size();iV++)
			{	if(vClustIDs[iV]==iC)
					v1DFloatClust[idxClust++]=vFloat[iV*iCols+iD];
				else
					v1DFloatComp[idxComp++]=vFloat[iV*iCols+iD];
			}
			o1DTClust.SetData(1,&v1DFloatClust[0],vCounts[iC]);
			o1DTComp.SetData(1,&v1DFloatComp[0],iRows-vCounts[iC]);
			prob_t kld = KLDivSym(o1DTClust,o1DTComp);
			
			vKLDDims[iD]=pair<float,int>(kld,iD);
			/*pair<float,int> oP(kld,iD);
			if(vKLDDims.size()<iBestDims)
				vKLDDims.push_back(oP);
			else
			{	int idx = GetReplaceIndex(vCorrelMat,vKLDDims,oP);
				if(idx != -1)
					vKLDDims[idx]=oP;
			}*/
		}
		/*sort(vKLDTmp.begin(),vKLDTmp.end());
		int iJ , idx = iBestDims-1, iFound=0;
		for(iJ=iCols-1;iJ>=0;iJ--)
		{
			int iK;
			bool bRedund = false;
			for(iK=iBestDims-1;iK>=idx;iK--)
			{
				if(SkipPair(vBestDims[iC][iK],vKLDTmp[iJ].second))
				{
					bRedund = true;
					break;
				}
			}
			if(!bRedund)
			{
				vBestDims[iC][idx]=vKLDTmp[iJ].second;
				vKLDivs[iC][idx]=vKLDTmp[iJ].first;
				idx--;
				iFound++;
			}
			if(iFound==iBestDims)
				break;
		}*/
		vector<int> dimIDs(iCols) , bestdimIDs(iCols);
		int iJ = 0;
		for(iJ=0;iJ<iCols;iJ++) dimIDs[iJ]=iJ;
		//LogF F; FILE* fp=F.Open();
		float maxScore = -10000.0 , tmpScore = 0.0;
		btb::combination_init(&dimIDs[0],&dimIDs[iBestDims],&dimIDs[dimIDs.size()]);
		do
		{	vector< pair<float,int> > vKLDTmp(iBestDims);
			int iK = 0;
			for(;iK<iBestDims;iK++)
			{
				vKLDTmp[iK]=vKLDDims[dimIDs[iK]];
				//fprintf(fp,"%d\t",dimIDs[iK]);
			} //fprintf(fp,"\n");
			if((tmpScore=KLDCorVal(vCorrelMat,vKLDTmp))>maxScore)
			{   maxScore = tmpScore;
				bestdimIDs=dimIDs;
				Write2Log("maxScore=%.2f",maxScore);
				WriteVec2Log(bestdimIDs);
			}

		}while(btb::next_combination(&dimIDs[0], &dimIDs[iBestDims], &dimIDs[dimIDs.size()]));
		//F.Close();
		vector< pair<float,int> > vKLDTmp(iBestDims);
		for(iJ=0;iJ<iBestDims;iJ++) vKLDTmp[iJ] = vKLDDims[bestdimIDs[iJ]];
		vKLDDims=vKLDTmp;
		sort(vKLDDims.begin(),vKLDDims.end());
		for(iJ=0;iJ<vKLDDims.size();iJ++)
		{	vBestDims[iC][iJ]=vKLDDims[iJ].second;
			vKLDivs[iC][iJ]=vKLDDims[iJ].first;
		}
	}
	return true;
}

#ifdef _OLDER_V
bool FindBest1DDims(vector<float>& vFloat,int iClusts,int iCols,int iBestDims,vector<int>& vCounts,vector<int>& vClustIDs,A2D<int>& vBestDims,A2D<prob_t>& vKLDivs,MY_STR_STACK& vAxes,const CUPDUPDATA* pUp)
{	vBestDims.Init(iClusts+1,iBestDims);
	vKLDivs.Init(iClusts+1,iBestDims);
	vKLDivs.Fill(-99999.9f);
	int iC = 1 , iRows = vClustIDs.size();
	double dJnk = 0.0;
	CString msg;
	for(iC=1;iC<=iClusts;iC++)
	{	int iD;
		for(iD=0;iD<iCols;iD++,dJnk++)
		{	msg.Format("Finding best %d dimensions for cluster %d of %d : Dim=%s",iBestDims,iC,iClusts,*vAxes[iD]);
			pUp->SetProgress(msg,100.0*dJnk/(iClusts*iCols));
			vector<float> v1DFloatClust(vCounts[iC]),v1DFloatComp(iRows-vCounts[iC]);
			int idxClust = 0, idxComp = 0;
			KDTreeHist o1DTClust,o1DTComp;
			int iV = 0;
			for(iV=0;iV<vClustIDs.size();iV++)
			{	if(vClustIDs[iV]==iC)
					v1DFloatClust[idxClust++]=vFloat[iV*iCols+iD];
				else
					v1DFloatComp[idxComp++]=vFloat[iV*iCols+iD];
			}
			o1DTClust.SetData(1,&v1DFloatClust[0],vCounts[iC]);
			o1DTComp.SetData(1,&v1DFloatComp[0],iRows-vCounts[iC]);
			prob_t kld = KLDivSym(o1DTClust,o1DTComp);
			int iJ;
			for(iJ=iBestDims-1;iJ>=0;iJ--)
			{	if(kld>vKLDivs[iC][iJ])
				{	int iJ2;
					for(iJ2=0;iJ2<iJ;iJ2++)
					{	vKLDivs[iC][iJ2]=vKLDivs[iC][iJ2+1];
						vBestDims[iC][iJ2]=vBestDims[iC][iJ2+1];
					}
					vKLDivs[iC][iJ]=kld;
					vBestDims[iC][iJ]=iD;
					break;
				}
			}
		}
	}
	return true;
}
#endif
