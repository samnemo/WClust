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

Hist::Hist(double dMin,double dMax,int iBins)
{
	Init(dMin,dMax,iBins);
}

Hist::~Hist(void)
{
}

bool Hist::Init(double dMin,double dMax,int iBins)
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

void FillDistribs(CVerxStack& DataStack,int** pBinData,CCluster& MainClusters,int iBins,std::vector< std::vector<Hist> >& vDistribs,int iDistribs,vector<int>& vClustIDs)
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
	
	int iV = 0;
	for(iV=0;iV<vClustIDs.size();iV++)
	{	
		for(iD=0;iD<iDims;iD++)
		{
			//cluster spike belongs to
			vDistribs[vClustIDs[iV]][iD].IncBin(pBinData[iV][iD]);

			//FULL distribution containing all spikes!!
			vDistribs[iDistribs][iD].IncBin(pBinData[iV][iD]);
		}
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

	double dScale = (iH - 1) * (1.0 / iMaxCount);

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

CString GetClusterInfoString(CVerxStack& MainDataStack,CCluster& MainClusters)
{
	CString strInfo,strTmp;

	//get cluster I,E info gain for each of # of bins and clusters
	MainClusters.CalcClusterInfo(MainDataStack,true);

	char cTmp[1024]={0};

	//create info gain string
	strInfo += "%%BEGIN CLUSTER_INFORMATION_GAIN\n";

	int iNumBins = 10, iBinInc = 20 , iMaxBins = 210, idx = 0, iOff = 0;
	strInfo.Format("// %%InformationGain.0 ( ClusterId ");

	CString strTmpI,strTmpIN,strTmpUD,strTmpUDN,strTmpEX,strTmpEXN,strTmpEntropy,strTmpEntropyN;
	for(iNumBins=10;iNumBins<=iMaxBins;iNumBins+=iBinInc)
	{
		strTmp.Format("%dbinInfoI ",iNumBins);
		strTmpI += strTmp;

		strTmp.Format("%dbinInfoIN ",iNumBins);
		strTmpIN += strTmp;

		strTmp.Format("%dbinInfoUD ",iNumBins);
		strTmpUD += strTmp;

		strTmp.Format("%dbinInfoUDN ",iNumBins);
		strTmpUDN += strTmp;

		strTmp.Format("%dbinInfoEX ",iNumBins);
		strTmpEX += strTmp;

		strTmp.Format("%dbinInfoEXN ",iNumBins);
		strTmpEXN += strTmp;

		strTmp.Format("%dbinEntropy ",iNumBins);
		strTmpEntropy += strTmp;

		strTmp.Format("%dbinEntropyN ",iNumBins);
		strTmpEntropy += strTmp;
	}

	strInfo += strTmpI;
	strInfo += strTmpIN;
	strInfo += strTmpUD;
	strInfo += strTmpUDN;
	strInfo += strTmpEntropy;
	strInfo += strTmpEntropyN;
	strInfo += strTmpEX;
	strInfo += strTmpEXN;

	strInfo += ")\n";

	unsigned int iC=0;
	const int which = CLUST_USER;
	//go through all clusters (last one is FULL distribution so doesn't need to be saved to string)
	for(iC=1;iC<MainClusters.GetCount() && iC<MainClusters.m_vInfo[which].size();iC++)
	{
		strTmp.Format("%%InformationGain.0 ( %d ",iC);
		strInfo += strTmp;

		idx = iOff = 0;

		for(idx=0;idx<8;idx++)
		{
			iOff = idx;
			for(iNumBins=10;iNumBins<=iMaxBins;iNumBins+=iBinInc,iOff+=8)
			{
				strTmp.Format("%.2f ",MainClusters.m_vInfo[which][iC][iOff]);
				strInfo += strTmp;
			}
		}
		strInfo += " )\n";
	}
	strInfo += "%%END CLUSTER_INFORMATION_GAIN\n\n";
	
	return strInfo;
}

static vector< vector<double> > gvprobs;
void InitProbs(int iMaxNumElems)
{
	gvprobs = vector< vector<double> >(iMaxNumElems+1);
	int i,j;
	gvprobs[0] = vector<double>(1);
	gvprobs[0][0] = 0.0;
	for(i=1;i<=iMaxNumElems;i++)
	{
		gvprobs[i] = vector<double>(i+1);
		for(j=0;j<=i;j++)
		{
			gvprobs[i][j] = (double) j / (double) i;
		}
	}
}
double Prob(int iElems,int i)
{
#ifdef _DEBUG
	if(iElems >= gvprobs.size() || i >= gvprobs[iElems].size())
	{
		char msg[1024];
		sprintf(msg,"index out of bounds in Prob iElems=%d gvprobs.size()=%d i=%d gvprobs[iElems].size()=%d",iElems,gvprobs.size(),i,gvprobs[iElems].size());
		MessageBox(0,msg,msg,0);
	}
#endif
#ifdef _DEBUG
	if(iElems < gvprobs.size() && i < gvprobs[iElems].size())
#endif
		return gvprobs[iElems][i];
#ifdef _DEBUG
	return (double) i / (double) iElems;
#endif
}

