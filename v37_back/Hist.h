#pragma once
#include "Cluster.h"

#include <math.h>

//binary logarithm
inline double log2(double d)
{
	static double dl2 = log(2.0);
	return log(d) / dl2;
}

//#include <vector>

//static vector< vector<double> > gvprobs;

//inline void InitProbs(ve

double Prob(int,int);

void InitProbs(int iMaxNumElems);

//histogram class, with automatic counting/scaling
//of values to be between min & max
class Hist //: public CMyObject
{
protected:
	
	//counts of values
	std::vector<int> m_counts;

	//vector of probability values, has probs for (0 - m_iNumElems) / m_iNumElems
	std::vector<double> m_probs;
	
	//min value in histogram
	double m_dMin;
	
	//max value in histogram
	double m_dMax;

	//m_dMax - m_dMin
	double m_dRange;
	
	//number of bins
	int m_iBins;

	//# of elements in distribution
	double m_dNumElems;

	//whether need to recalc probabilities
	//(uses lazy evaluation)
	bool m_bNeedReCalc;

public:

	//return bin # for value
	inline int ValIndex(double dVal) const
	{
		if(m_dRange==0.0) return 0;

		//dVal's location in range as # btwn 0 - 1
		double dFctr = (dVal - m_dMin) / m_dRange;
	
		//multiply by # of bins - 1 to have it 0-based index
		return dFctr * (m_iBins - 1);
	}
	
	inline double Max() const { return m_dMax; }
	inline double Min() const { return m_dMin; }
	inline double Range() const { return m_dRange; }
	inline int NumBins() const { return m_iBins; }

	//number of elements in "distribution"
	inline int NumElems() const
	{
		return (int) m_dNumElems;
	}

	//increment bin by 1
	inline bool DecBin(int idx)
	{
		if(idx < 0 || idx >= m_counts.size())
		{
			return false;
		}
		m_dNumElems -= 1;
		m_bNeedReCalc = true;
		m_counts[idx] -= 1;	
		return true;
	}

	//increment bin by 1
	inline bool IncBin(int idx)
	{
		if(idx < 0 || idx >= m_counts.size())
		{
			return false;
		}
		m_dNumElems += 1;
		m_bNeedReCalc = true;
		m_counts[idx] += 1;	
		return true;
	}

	//increment bin that dVal is in by 1
	inline bool IncBinVal(double dVal)
	{
		if(dVal < m_dMin || dVal > m_dMax)
		{
			return false;
		}
		m_dNumElems += 1;
		m_bNeedReCalc = true;
		m_counts[ValIndex(dVal)] += 1;	
		return true;
	}

	//decrement bin that dVal is in by 1
	inline bool DecBinVal(double dVal)
	{
		if(dVal < m_dMin || dVal > m_dMax)
		{
			return false;
		}	
		m_dNumElems -= 1;
		m_bNeedReCalc=true;
		m_counts[ValIndex(dVal)] -= 1;	
		return true;
	}
	
	//set bin that dVal is in to iCount
	inline bool SetBinVal(double dVal,int iCount)
	{
		if(dVal < m_dMin || dVal > m_dMax)
		{
			return false;
		}
		int iDx = ValIndex(dVal);
		int iTmp = m_counts[iDx];
		m_dNumElems -= iTmp;
		m_dNumElems += iCount;
		m_counts[iDx] = iCount;
		m_bNeedReCalc = iCount != iTmp;
		return true;
	}
	
	//get count of bin that dVal is in 
	inline int GetBinVal(double dVal) const { return m_counts[ValIndex(dVal)]; }

	//get probability of variable being bin i
	inline double GetBinProb(double dVal) const{ return (double) GetBinVal(dVal) / m_dNumElems; }

	//calc probabilities of 0 - m_dNumElems to help save time
	//only recalculate when the # of elements changes
	//not used
	inline void CalcProbs()
	{
		if(m_bNeedReCalc && m_dNumElems>=1.0)
		{
			m_probs = vector<double>((int)m_dNumElems+1);
			double j;
			for(j=0.0;j<=m_dNumElems;j+=1.0) m_probs[j] = j / m_dNumElems;
			m_bNeedReCalc=false;
		}
	}

	//get probability of bin i
	inline double BinProb(int i)
	{ 
		//CalcProbs();
		//return m_probs[m_counts[i]];
		//return m_counts[i] / m_dNumElems; 
		return Prob(m_dNumElems,m_counts[i]); 
	}

	inline double Entropy()
	{
		int i = 0;
		double dEntropy = 0.0;
		for(;i<m_counts.size();i++)
		{
			if(!m_counts[i]) continue;
			double dProb = BinProb(i);
			dEntropy += dProb * log2(dProb);
		}
		return -dEntropy;
	}
	
	//direct access to bin
	const int& operator[] (int i) const { return m_counts[i]; }
	
	//initialize histogram
	bool Init(double dMin,double dMax,int iBins);

	Hist(void);
	Hist(double dMin,double dMax,int iBins);
	~Hist(void);

	//add h to this
	void Add(Hist& h)
	{
		int i;
		for(i=0;i<m_iBins;i++)
		{
			m_counts[i] += h.m_counts[i];
			m_dNumElems += h.m_counts[i];
		}
	}

	inline void Print(FILE* fp)
	{
		int i;
		for(i=0;i<m_iBins;i++)
		{
			fprintf(fp,"B%d : ",i);
			int j;
			fprintf(fp,"%d p=%0.2f ",m_counts[i],BinProb(i));
			for(j=0;j<m_counts[i];j++)
			{
				fprintf(fp,"*");
			}
			fprintf(fp,"\n");
		}
		fprintf(fp,"numelems=%d\n\n",NumElems());
	}
};

//sum entropy with option of zeroing out certain dimensions
//if vZero[i] != 0, skip that dimension
inline double SumEntropy(vector< Hist >& vHist, vector<int>& vZero)
{
	int iSz = vHist.size();
	int i;
	double dSum = 0.0;
	for(i=0;i<iSz;i++)
	{
		if(vZero[i]) continue;
		dSum += vHist[i].Entropy();
	}
	return dSum;
}

inline double SumEntropy(vector< Hist> & vHist)
{
	int iSz = vHist.size();
	int i;
	double dSum = 0.0;
	for(i=0;i<iSz;i++)
		dSum += vHist[i].Entropy();
	return dSum;
}

inline double SumEntropy(vector< vector<Hist> >& vDistribs,vector<double>& vWeights)
{
	int iSz = vDistribs.size();
	int i;
	double dSum = 0.0;
	for(i=0;i<iSz;i++)
		dSum += vWeights[i] * SumEntropy(vDistribs[i]);
	return dSum;
}

inline void CalcEntropy(vector< vector< Hist> >& vDistribs,vector<double>& vcEntropy,int iClusts,int iDim,vector<int>& vCounts,int iNumElems,vector<int>& vZeroes,bool bWeight)
{
	int iC;
	if(vcEntropy.size() < iClusts+1) vcEntropy = vector<double>(iClusts+1);
	if(bWeight)
	{
		for(iC=1;iC<=iClusts;iC++)
		{
			vcEntropy[iC] = ( (double) vCounts[iC] / (double) iNumElems ) * SumEntropy(vDistribs[iC],vZeroes);
		}
	}
	else
	{
		for(iC=1;iC<=iClusts;iC++)
		{
			vcEntropy[iC] = SumEntropy(vDistribs[iC],vZeroes);
		}
	}
}

//gets KLDiv using approximate probabilities
//P(j) = (nj+0.5)/(N+0.5jmax)
//n1+n2+...+nmax=N
//the p distribution's counts are first subtracted
//from the q distribution so that the function will
//calculate the "exclusive" information gain
//p is "partial" distribution
//q is "full" distribution
inline double KLDivApproxExclusiveProb(Hist& p,Hist& q)
{
	if(p.NumBins() != q.NumBins()) return -1.0;

	double d = 0.0;

	int i = 0, iBins = p.NumBins();

	double eps = 10e-12;

	double pelems = p.NumElems();
	double qelems = q.NumElems();

	for(i=0;i<iBins;i++)
	{
		double pcount = p[i];
		double qcount = q[i];

		//approximate probability of bin i in distribution p
		double pp = (pcount+0.5) / ( pelems + 0.5*iBins);

		//approximate probability of bin i in distribution q
		//must subtract pcount elements from numerator & denominator of probability
		//approximation so as to maintain "exclusivity" of p distribution
//		double qp = (qcount - pcount + 0.5) / ( qelems - pcount + 0.5*iBins);
		double qp = (qcount - pcount + 0.5) / ( qelems - pelems + 0.5*iBins);

		if(pp<=eps) continue;

		if(qp<=eps) continue;

		d += pp * ( log2(pp) - log2(qp) );
	}

	return d;
}

inline double FullProb(Hist& h)
{
	double p = 0.0;
	for(int i=0;i<h.NumBins();i++)
	{
		p += h.BinProb(i);
	}
	return p;
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
inline double KLDiv(Hist& p,Hist& q)
{
	if(p.NumBins() != q.NumBins())
	{
		return -1.0;
	}

	double d = 0.0;

	int i = 0 , iBins = p.NumBins();

	double eps = 10e-12;

	for(i=0;i<iBins;i++)
	{
		double pp = p.BinProb(i); //probability of bin i in distribution p
		
		if(pp<=eps) 
			continue;
		
		double qp = q.BinProb(i); //probability of bin i in distribution q
		
		if(qp<=eps) 
			continue;
		
		d += pp * ( log2(pp) - log2(qp) );
	}

/*#ifdef _DEBUG
	if(d<0.0)
	{
		double pf = FullProb(p);
		double qf = FullProb(q);
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

//symmetrized form of KL-divergence
inline double ResistorAvg(Hist& p,Hist& q)
{
	double d12 = KLDiv(p,q), d21 = KLDiv(q,p);
	return (d12 * d21) / (d12 + d21);
}

//p is cluster distrib, q is full distrib
inline double Uniqueness(Hist& p,Hist& q)
{
	return KLDiv(p,q);
}

//vZeroes = vector of dimensions that shouldn't contribute to calculations
//vZeroes[i] == 1 , iff you want to exclude dimension i
inline void CalcUDDist(vector< vector<Hist> >& vDistribs,int iClusts,vector<double>& vcInf,vector< vector<double> >& vcInfInter,vector<int>& vCounts,int iElems,vector<int>& vZeroes,bool bUseCounts)
{
	if(vDistribs.size() != iClusts + 2) return;

	vcInf = vector<double>(iClusts+1);

	vcInfInter = vector< vector<double> >(iClusts+1);
	int iC=1;
	for(iC=1;iC<=iClusts;iC++) vcInfInter[iC] = vector<double>(iClusts+1);

	int iDims = vDistribs[1].size() , iD = 0;

	//uniqueness from full distribution for each cluster
	int iC1=1,iC2=1;
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		double kldiv=0.0;
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
		double dMinInter = 9e10;
		if(vCounts[iC1])
		for(iC2=1;iC2<=iClusts;iC2++)
		{
			if(iC1 != iC2 && vCounts[iC2] && vcInfInter[iC1][iC2] < dMinInter)
			{
				dMinInter = vcInfInter[iC1][iC2];
			}
		}
		vcInf[iC1] += dMinInter;
	}

	if(bUseCounts)
	{
		for(iC1=1;iC1<=iClusts;iC1++)
		{
			vcInf[iC1] = ( (double)vCounts[iC1] / iElems) * vcInf[iC1];
		}
	}
}

//calculates information gain as resistor average of all clusters against full distribution
//plus smallest resistor average between cluster and all other clusters
//vDistribs must have size of iClusts + 1
inline void CalcRDist(vector< vector<Hist> >& vDistribs,int iClusts,vector<double>& vcInf,vector< vector<double> >& vcInfInter)
{
	if(vDistribs.size() != iClusts + 2) return;

	vcInf = vector<double>(iClusts+1);

	vcInfInter = vector< vector<double> >(iClusts+1);
	int iC=1;
	for(iC=0;iC<=iClusts;iC++) vcInfInter[iC] = vector<double>(iClusts+1);

	int iDims = vDistribs[1].size() , iD = 0;

	//only need this for first time, since its recalculated for distributions who's
	//points change, when they change
	int iC1=1,iC2=1;
	for(iC1=1;iC1<=iClusts;iC1++)
	{
		double kldiv=0.0;
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
		double dMinInter = 9e10;
		for(iC2=1;iC2<=iClusts;iC2++)
		{
			if(iC1 != iC2 && vcInfInter[iC1][iC2] < dMinInter)
			{
				dMinInter = vcInfInter[iC1][iC2];
			}
		}
		vcInf[iC1] += dMinInter;
	}
}

template< class T > 
T Sum(vector<T>& v)
{
	int i;
	T val = T(0);
	for(i=0;i<v.size();i++) val += v[i];
	return val;
}

template< class T > 
T Avg(vector<T>& v)
{
	if(!v.size()) return T(0);
	int i;
	T val = T(0);
	for(i=0;i<v.size();i++) val += v[i];
	return val / (T) v.size();
}

//get vector containing information gain for each cluster
//std::vector<double> GetClusterInfo(CVerxStack& DataStack,CCluster& MainClusters,CPaletteStack* pMainPal,CFile* pFileBPF,int iBins,int iClusts,int which);
//get string rep of cluster info gain for writing to bpf,cl files
CString GetClusterInfoString(CVerxStack& MainDataStack,CCluster& MainClusters);

bool PrintDistribs(vector<vector< Hist> >& vDistribs,char* fname);

bool Distribs2Images(vector< vector< Hist > >& vDistribs, char* fname_base);

bool Distribs2Matlab(vector< vector< Hist > >& vDistribs, char* fname_base);

void FillDistribs(CVerxStack& DataStack,CCluster& MainClusters,int iBins,std::vector< std::vector<Hist> >& vDistribs,int iClusts,int which);

void FillDistribs(CVerxStack& DataStack,int** pData,CCluster& MainClusters,int iBins,std::vector< std::vector<Hist> >& vDistribs,int iDistribs,vector<int>& vClustIDs);

bool RandAssign(CVerxStack& DataStack,CCluster& MainClusters,int iClusts,int which);

inline char GetVClust(CVertex* verx,int which)
{
	switch(which)
	{
	case CLUST_USER:
		return verx->GetClust();
		break;
	case CLUST_ORIG:
		return verx->GetOrigClust();
		break;
	case CLUST_KM:
		return verx->GetKmeansClust();
		break;
	case CLUST_INFO:
		return verx->GetInfoClust();
		break;
	case CLUST_AP:
		return verx->GetAPClust();
		break;
	case CLUST_KK:
		return verx->GetKKClust();
		break;
	}
	return 0;
}