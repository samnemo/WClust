// $Id: Hist.cpp,v 1.24 2009/06/26 15:56:59 samn Exp $ 
#include "stdafx.h"
#include "Hist.h"
#include "PPM.h"

using namespace std;

Hist::Hist(void)
:m_dMin(0.0),
 m_dMax(0.0),
 m_iBins(0)
{
}

Hist::Hist(hprob dMin,hprob dMax,int iBins)
{
	Init(dMin,dMax,iBins);
}

Hist::~Hist(void)
{
}

bool Hist::Init(hprob dMin,hprob dMax,int iBins)
{
	if(iBins < 1 || dMin > dMax)
	{
		return false;
	}

	m_dMin = dMin;
	m_dMax = dMax;
	m_dRange = m_dMax - m_dMin;
	m_iBins = iBins;
	m_dNumElems = 0.0;

	m_counts = vector<int>(iBins);

	return true;
}

bool RandAssign(CVerxStack& DataStack,CCluster& MainClusters,int iClusts,int which)
{
	srand(time(0));
	MY_STACK::iterator Index;	
	for (Index=DataStack.m_VerxStack.begin();Index!=DataStack.m_VerxStack.end();Index++)
	{	
		CVertex* verx = (CVertex*)*Index;

		//skip noise
		if(verx->GetNoise()) continue;

		switch(which)
		{
		case CLUST_KM:
			verx->SetKmeansClust(1+rand()%iClusts);
			break;
		case CLUST_INFO:
			verx->SetInfoClust(1+rand()%iClusts);
			break;
		case CLUST_AP:
			verx->SetAPClust(1+rand()%iClusts);
			break;
		case CLUST_KK:
			verx->SetKKClust(1+rand()%iClusts);
			break;
		case CLUST_FL:
			verx->SetFLClust(1+rand()%iClusts);
			break;
		}
	}
	return true;
}

void FillDistribs(CVerxStack& DataStack,CCluster& MainClusters,int iBins,std::vector< std::vector<Hist> >& vDistribs,int iDistribs,int which)
{
	//distrib for each cluster + 1 for full distrib
	vDistribs = std::vector< std::vector<Hist> >(iDistribs+1);
	int iDims = DataStack.GetAutoClusteringDimension();
	
	int iD=0,iC=1; 
	for(iC=1;iC<=iDistribs;iC++)
	{
		vDistribs[iC] = std::vector<Hist>(iDims);
		for(iD=0;iD<iDims;iD++)
		{	//make sure to do +1 for dimension indices in DataStack
			vDistribs[iC][iD].Init(DataStack.GetMin(iD+1),DataStack.GetMax(iD+1),iBins);
		}
	}
	
	//go through clusters
	//each cluster checks ALL spikes for membership
	//inefficient...but is there a better way without changing the data structures?
	
	MY_STACK::iterator Index;	
	for (Index=DataStack.m_VerxStack.begin();Index!=DataStack.m_VerxStack.end();Index++)
	{	
		CVertex* verx = (CVertex*)*Index;

		//skip noise
		if(verx->GetNoise()) continue;
			
		//go through clusters filling out distrib info
		for(iC=1;iC<=iDistribs;iC++)
		{
			//either spike is in cluster or it is the FULL distribution
			//containing all spikes!!
			if(iC==iDistribs || GetVClust(verx,which)==iC)
			{
				for(iD=0;iD<iDims;iD++)
				{	//+1 since index 0 is # of clusters vertex is in
					vDistribs[iC][iD].IncBinVal(verx->GetValue(iD+1));
				}
			}
		}
	}
}

void FillDistribs(CVerxStack& DataStack,int** pBinData,CCluster& MainClusters,int iBins,std::vector< std::vector<Hist> >& vDistribs,int iDistribs,vector<int>& vClustIDs,int iMinClust)
{
	//distrib for each cluster + 1 for full distrib
	vDistribs = std::vector< std::vector<Hist> >(iDistribs+1);
	int iDims = DataStack.GetAutoClusteringDimension();
	
	int iD=0,iC=1; 
	//for(iC=1;iC<=iDistribs;iC++)
	for(iC=iMinClust;iC<=iDistribs;iC++) // **************** //
	{
		vDistribs[iC] = std::vector<Hist>(iDims);
		for(iD=0;iD<iDims;iD++)
		{	//make sure to do +1 for dimension indices in DataStack
			vDistribs[iC][iD].Init(DataStack.GetMin(iD+1),DataStack.GetMax(iD+1),iBins);
		}
	}
	
	int iV = 0;
	for(iV=0;iV<vClustIDs.size();iV++)
	{	
		if(vClustIDs[iV]==0)continue;
		for(iD=0;iD<iDims;iD++)
		{
			//cluster spike belongs to
			vDistribs[vClustIDs[iV]][iD].IncBin(pBinData[iV][iD]);

			//FULL distribution containing all spikes!!
			vDistribs[iDistribs][iD].IncBin(pBinData[iV][iD]);
		}
	}
}

void GetFullBGDistrib(vector<float>& vFloat,KDTreeHist& oTree,int iDims,int* pBestDims,int iBestDims)
{
	int i = 0, j = 0 , iV = 0 , iTotalVs = vFloat.size() / iDims;
	vector<float> vFullData(iTotalVs*iBestDims);
	for(iV=0;iV<iTotalVs;iV++)
	{	for(i=0;i<iBestDims;i++)
			vFullData[j++]=vFloat[iV*iDims+pBestDims[i]];
	}
	oTree.SetData(iBestDims,&vFullData[0],iTotalVs);
}

//this is the continuous multidimensional probability version
void FillDistribs(vector<float>& vFloat,vector<KDTreeHist>& vDistribs,vector<KDTreeHist>& vCompDistribs,int iDistribs,vector<int>& vClustIDs,vector<int>& vCounts,int iDims,A2D<int>& vBestDims,int iBestDims)
{
	vDistribs = vector< KDTreeHist >(iDistribs+1);
	vCompDistribs = vector< KDTreeHist >(iDistribs+1);

	int iTotalVs = vFloat.size() / iDims, iC = 0;

	//full distribution not really used so no need to initialize!!!!!

	for(iC=1;iC<iDistribs;iC++)
	{	int iCompSize = iTotalVs - vCounts[iC];
		vector<float> vClustData(vCounts[iC]*iBestDims), vCompData(iCompSize*iBestDims);
		int i = 0, j = 0 , k = 0, iV = 0;
		for(iV=0;iV<vClustIDs.size();iV++)
		{	if(vClustIDs[iV] == iC)
			{	for(i=0;i<iBestDims;i++)
					vClustData[j++]=vFloat[iV*iDims+vBestDims[iC][i]];
			}
			else 
			{	for(i=0;i<iBestDims;i++)
					vCompData[k++]=vFloat[iV*iDims+vBestDims[iC][i]];
			}
		}
		vDistribs[iC].SetData(iBestDims,&vClustData[0],vCounts[iC]);
		vCompDistribs[iC].SetData(iBestDims,&vCompData[0],iCompSize);
	}
}

bool Distribs2Matlab(vector< vector< Hist > >& vDistribs, char* fname_base)
{
	int iClusts = vDistribs.size();

	if(iClusts < 1) return false;

	int iDims = vDistribs[1].size();

	int iC=1,iD=0,iB=0;

	int iBins = vDistribs[iC][iD].NumBins();

	for(iC=1;iC<iClusts;iC++)
	{
		char fname[1024];
		sprintf(fname,"%d_bins__clust%d__%s___.txt",iBins,iC,fname_base);
		FILE* fp = fopen(fname,"w");
		if(!fp) return false;
		int xpos = 0,ypos = 0;
		for(iD=0;iD<iDims;iD++)
		{
			for(iB=0;iB<iBins;iB++)
				fprintf(fp,"%lf " ,vDistribs[iC][iD].BinProb(iB));
			fprintf(fp,"\n");
		}
		fclose(fp);
	}

	return true;
}

bool Distribs2Images(vector< vector< Hist > >& vDistribs, char* fname_base)
{
	int iClusts = vDistribs.size();

	if(iClusts < 1) return false;

	int iDims = vDistribs[1].size();

	int iC=1,iD=0,iB=0;

	int iBins = vDistribs[iC][iD].NumBins();

	int iMaxCount = 0;
	for(iC=1;iC<iClusts;iC++)
	{
		for(iD=0;iD<iDims;iD++)
		{
			for(iB=0;iB<vDistribs[iC][iD].NumBins();iB++)
			{
				if(vDistribs[iC][iD][iB] > iMaxCount)
				{
					iMaxCount = vDistribs[iC][iD][iB];
				}
			}
		}
	}

	int iInterDim = 20 , iBinWidth = 10;
	int iH = 600;
	int iW = iBinWidth * iBins * iDims + iInterDim * iDims;

	prob_t dScale = (iH - 1) * (1.0 / iMaxCount);

	color red(255,0,0) , green(0,255,0), blue(0,0,255), black(0,0,0), white(255,255,255);

	for(iC=1;iC<iClusts;iC++)
	{
		ppmImage oImg( iW, iH);
		oImg.setWhite();
		char fname[1024];
		sprintf(fname,"%s_%d_.ppm",fname_base,iC);
		int xpos = 0,ypos = 0;
		for(iD=0;iD<iDims;iD++)
		{
			for(iB=0;iB<iBins;iB++)
			{
				int xmin = xpos;
				int xmax = xpos + iBinWidth;
				int ymin = iH - 1;
				int ymax = (iH - 1) - ( vDistribs[iC][iD][iB] / iMaxCount) * (iH - 1);

				int x,y;
				for(y=ymax;y<=ymin;y++)
					for(x=xmin;x<xmax;x++)
						oImg[y][x] = red;

				xpos += iBinWidth;
			}
			xpos += iInterDim;
		}
		oImg >> fname;
	}

	return true;
}

bool PrintDistribs2(vector< vector< Hist> >& vDistribs, char* fname)
{
	int iClusts = vDistribs.size();

	if(iClusts < 1) return false;

	FILE* fp = fopen(fname,"w");

	if(!fp) return false;

	int iDims = vDistribs[0].size();

	int iC=0,iD=0,iB=0;

	int iBins = vDistribs[iC][iD].NumBins();

	int iMaxCount = 0;
	for(iC=0;iC<iClusts;iC++)
	{
		for(iD=0;iD<iDims;iD++)
		{
			for(iB=0;iB<vDistribs[iC][iD].NumBins();iB++)
			{
				if(vDistribs[iC][iD][iB] > iMaxCount)
				{
					iMaxCount = vDistribs[iC][iD][iB];
				}
			}
		}
	}

	vector< vector<char> > varr((iDims+1) * iBins);

	for(iC=0;iC<varr.size();iC++)
	{
		varr[iC].resize(iMaxCount);
		for(iD=0;iD<iMaxCount;iD++)
		{
			varr[iC][iD]='_';
		}
	}

	for(iC=0;iC<iClusts;iC++)
	{
		for(iD=0;iD<iDims;iD++)
		{
			for(iB=0;iB<iBins;iB++)
			{
				int k;
				for(k=0;k<vDistribs[iC][iD][iB];k++)
				{
					if(varr[iD][iB]=='_')
					{
						varr[iD][iB]=iC;
					}
				}
			}
		}
	}

	for(iD=0;iD<iDims;iD++)
	{
		fprintf(fp,"D%d\n",iD);
		for(iB=0;iB<iBins;iB++)
		{
			int k;
			for(k=0;k<iMaxCount;k++)
			{
				fprintf(fp,"%c",varr[iD][iB]);
			}			
		}
		//fprint
	}

	fclose(fp);

	return true;
}

bool PrintDistribs(vector<vector< Hist> >& vDistribs,char* fname)
{
	int iClusts = vDistribs.size();

	if(iClusts < 1) return false;

	FILE* fp = fopen(fname,"w");
	
	if(!fp) return false;

	int iDims = vDistribs[0].size();

	int iC = 0, iD = 0;

	for(iC=0;iC<iClusts;iC++)
	{
		fprintf(fp,"C%d\n",iC);
		for(iD=0;iD<iDims;iD++)
		{
			fprintf(fp,"D%d\n",iD);
			vDistribs[iC][iD].Print(fp);
		}
		fprintf(fp,"\n");
	}

	fclose(fp);

	return true;
}

#define giBins 4
int pBins[giBins] = {10,20,30,40};//,50,60,70,80,90,100};

CString GetClusterInfoString(CVerxStack& MainDataStack,CCluster& MainClusters,HWND wnd,int DrawMode)
{
	//this func NOT ONLY called on user data when saving .cl file
	MainDataStack.whichDraw = DrawMode;

	CString strInfo,strTmp;

	//calculate cluster info gain for each cluster
	if(MainClusters.CalcClusterInfo(MainDataStack,true,wnd))
	{	//create info gain string for
		//multidimensional continuous probability distribution information gain
		//using resistor average of KL-divergence between points in a cluster and
		//all other points as background distribution -- 
		//also resistor avg. of inter-cluster KL-divergence for each cluster and corresponding closets
		//cluster ID, iff not using inter clust kldiv, these values will be 0
		CString strInfo( "%%BEGIN CLUSTER_INFORMATION_GAIN\n// %%InformationGain.0 ( ClusterId TotalKLDiv BGKLDiv InterKLDiv ClosestClusterID ");
		CString strTmp;
		strTmp.Format("%dNNCC SilhouetteWidth NumLoadedSpikes )\n",MainClusters.m_oCQO.m_iNNToFind);
		strInfo += strTmp;
		unsigned int iC=0;
		const int which = DrawMode;
		//go through all clusters (last one is FULL distribution so doesn't need to be saved to string)
		for(iC=1;iC<MainClusters.GetCount() && iC<MainClusters.m_vInfo[which].size();iC++)
		{	CString strTmp;
			strTmp.Format("%%InformationGain.0 ( %d ",iC);
			strInfo += strTmp;
			strTmp.Format("%.2f %.2f %.2f %d %.6f %.4f %d",
				MainClusters.m_vInfo[which][iC].m_fBGInfoGain+MainClusters.m_vInfo[which][iC].m_fInterClustGain,
				MainClusters.m_vInfo[which][iC].m_fBGInfoGain,
				MainClusters.m_vInfo[which][iC].m_fInterClustGain,
				MainClusters.m_vInfo[which][iC].m_iClosestID,
				MainClusters.m_vInfo[which][iC].m_fPrctKNNInClust,
				MainClusters.m_vInfo[which][iC].m_fSilhouetteWidth,
				MainClusters.m_vInfo[which][iC].m_iSz);
			strInfo += strTmp;
			strInfo += " )\n";
		}
		strInfo += "%%END CLUSTER_INFORMATION_GAIN\n\n";
		{	//store dimension info
			if(!MainClusters.m_oCQO.m_bFindBestDims) MainDataStack.CalcDimStats();
			strInfo += 	"\n%%BEGIN KLDIV_DIMENSION_INFO\n// %%KLDivDimInfo.0 ( ClusterId DimRank(1=best) DimID DimEntropy DimMean DimStdev DimMin DimMax DimRange DimName )\n";
			A2D<int>& vBestDims = MainClusters.m_vBestDims;
			int iD = 0, iC , iBestDims = MainClusters.m_oCQO.m_iBestDims;
			MY_STR_STACK& vAxes = MainDataStack.m_AxesStack;
			vector<float>& Entropy = MainDataStack.m_MainEntropy;
			vector<float>& Mean = MainDataStack.m_MainMean;
			vector<float>& Stdev = MainDataStack.m_MainStdev;
			vector<float>& VMin = MainDataStack.m_MainMin;
			vector<float>& VMax = MainDataStack.m_MainMax;
			vector<float>& VRange = MainDataStack.m_MainRange;

			//ClusterId DimRank(1=best) DimID DimEntropy DimMean DimStdev DimMin DimMax DimRange DimName
			if(MainClusters.m_oCQO.m_bFindBestDims) for(iC=1;iC<vBestDims.Rows();iC++)
			{	for(iD=0;iD<vBestDims.Cols();iD++)
				{	strTmp.Format("%%KLDivDimInfo.0 ( %d %d %d %.2f %.2f %.2f %.2f %.2f %.2f %s )\n",
						iC,iBestDims-iD,vBestDims[iC][iD],Entropy[vBestDims[iC][iD]+1],
						Mean[vBestDims[iC][iD]+1],Stdev[vBestDims[iC][iD]+1],VMin[vBestDims[iC][iD]+1],
						VMax[vBestDims[iC][iD]+1],VRange[vBestDims[iC][iD]+1],*vAxes[vBestDims[iC][iD]]);
					strInfo += strTmp;
				}
			}
			for(iD=1;iD<=MainDataStack.GetAutoClusteringDimension();iD++)//store all dim info
			{	strTmp.Format("%%KLDivDimInfo.0 ( 0 -1 %d %.2f %.2f %.2f %.2f %.2f %.2f %s )\n",
					iD-1,Entropy[iD],Mean[iD],Stdev[iD],VMin[iD],VMax[iD],VRange[iD],*vAxes[iD-1]);
				strInfo += strTmp;
			}
			strInfo+="%%END KLDIV_DIMENSION_INFO\n\n";
		}
	}
	if(MainClusters.m_oCQO.m_bIsolationDist)
	{	strInfo += "%%BEGIN CLUSTER_ISOLATION_DISTANCE\n// %%IsolationDistance.0 ( ClusterId IsolationDist )\n";
		unsigned int iC=0;
		const int which = CLUST_USER;
		//go through all clusters (last one is FULL distribution so doesn't need to be saved to string)
		for(iC=1;iC<MainClusters.GetCount() && iC<MainClusters.m_vInfo[which].size();iC++)
		{	CString strTmp;
			strTmp.Format("%%IsolationDistance.0 ( %d ",iC);
			strInfo += strTmp;
			strTmp.Format("%.2f ",MainClusters.m_vInfo[which][iC].m_dIsolationDist); //isolation distance
			strInfo += strTmp;
			strInfo += " )\n";
		}
		strInfo += "%%END CLUSTER_ISOLATION_DISTANCE\n\n";
	}

	if(MainClusters.m_oCQO.m_bLRatio)
	{	strInfo += "%%BEGIN CLUSTER_L_RATIO\n// %%LRatio.0 ( ClusterId LRatio )\n";
		unsigned int iC=0;
		const int which = CLUST_USER;
		//go through all clusters (last one is FULL distribution so doesn't need to be saved to string)
		for(iC=1;iC<MainClusters.GetCount() && iC<MainClusters.m_vInfo[which].size();iC++)
		{	CString strTmp;
			strTmp.Format("%%LRatio.0 ( %d ",iC);
			strInfo += strTmp;
			strTmp.Format("%g ",MainClusters.m_vInfo[which][iC].m_dLRatio); //L-Ratio
			strInfo += strTmp;
			strInfo += " )\n";
		}
		strInfo += "%%END CLUSTER_L_RATIO\n\n";
	}
	
	return strInfo;
}

static vector< vector<float> > gvprobs;
void InitProbs(int iMaxNumElems)
{
	if(gvprobs.size()>=iMaxNumElems+1)return;
	gvprobs = vector< vector<float> >(iMaxNumElems+1);
	int i,j;
	gvprobs[0] = vector<prob_t>(1);
	gvprobs[0][0] = 0.0;
	for(i=1;i<=iMaxNumElems;i++)
	{
		gvprobs[i] = vector<prob_t>(i+1);
		for(j=0;j<=i;j++)
		{
			gvprobs[i][j] = (prob_t) j / (prob_t) i;
		}
	}
}

float Prob(int iElems,int i)
{
	if(iElems >= gvprobs.size() || i >= gvprobs[iElems].size())
		return (float) i / (float) iElems;
	return gvprobs[iElems][i];
}

ProbInitFree::ProbInitFree(int i)
{
	InitProbs(i);
}

ProbInitFree::~ProbInitFree()
{
	gvprobs.clear();
}
