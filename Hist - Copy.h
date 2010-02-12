#pragma once
#include "Cluster.h"
#include "WCMath.h"
#include "KDTree.h"
#include "ScopedTimer.h"
#include <map>
#include <set>

//#define DO_TIMING

//binary logarithm
inline prob_t log2(prob_t d)
{
	static prob_t dl2 = log(2.0);
	return log(d) / dl2;
}

prob_t Prob(int,int);

void InitProbs(int iMaxNumElems);

inline bool GZeroMinElem(float f1,float f2)
{
	return f1 > 0.0 && (f1 < f2 || f2 <= 0.0);
}

inline float GZeroMinElem(vector<float>& v)
{
	float fm = *std::max_element(v.begin(),v.end());
	int i=0,sz=v.size();
	for(;i<sz;i++)
		if(v[i]<fm && v[i]>0.0)
			fm=v[i];
	return fm;
}

inline float MinElem(vector<float>& v,bool bAllowZero)
{
	float fm = *std::max_element(v.begin(),v.end());
	int i=0,sz=v.size();
	for(;i<sz;i++)
		if(v[i]<fm && (bAllowZero || v[i]>0.0))
			fm=v[i];
	return fm;
}

inline int MinIdx(vector<float>& v,bool bAllowZero)
{
	float fm = *std::max_element(v.begin(),v.end());
	int i=0,sz=v.size(), minID = 0;
	bool bFound = false;
	for(;i<sz;i++)
	{
		if(v[i]<fm && (bAllowZero || v[i]>0.0))
		{
			bFound = true;
			fm=v[i];
			minID=i;
		}
	}
	if(!bFound) return -1;
	return minID;
}


//kd tree from biopython used for continuous probability distribution estimates 
class KDTreeHist
{
	NSKDTree::KDTree* m_pTree;

	int m_iDims;
	int m_iNumElems;	
	prob_t m_dPiPow;
	prob_t m_dTop;
	prob_t m_dGamma;

	vector<float> m_vData;
	
	vector<float> m_vProbs;

public:
	
	KDTreeHist()
		:m_iDims(0),
		 m_iNumElems(0),
		 m_pTree(0)
	{
	}

	virtual ~KDTreeHist()
	{
		if(m_pTree) delete m_pTree;
	}

	bool SetData(vector<float>& vFloat,vector<int>& vIDs,int iNumPoints,int iDims,int iCID)
	{
		if(!vFloat.size() || iNumPoints<1) return false;

		int iV = 0 , iC = 0;

		int iTotalVs = vFloat.size() / iDims;

		m_vData = vector<float>(iNumPoints*iDims);
		int j = 0 , k = 0;
		for(iV=0;iV<vIDs.size();iV++)
		{
			if(vIDs[iV] == iCID)
			{
				int iD = 0;
				for(iD=0;iD<iDims;iD++)
				{
					m_vData[j++]=vFloat[iV*iDims+iD];
				}
			}
		}

		m_iDims = iDims;
		
		if(m_pTree) delete m_pTree;
		
		m_pTree = new NSKDTree::KDTree(m_iDims,8,false);
		m_iNumElems = iNumPoints;

		const prob_t PI=3.14159265358979323846;

		m_dPiPow = pow((prob_t)PI,(prob_t)m_iDims/two);
		m_dTop = (one/(m_iNumElems-one));
		m_dGamma = Gamma(m_iDims/two+one);
		
		m_pTree->set_data(&m_vData[0],iNumPoints);
	}

	bool SetData(int iDims,float* pData,int iNumPoints)
	{
		if(!pData || iNumPoints<1) return false;

		m_iDims = iDims;
		if(m_pTree) delete m_pTree;
		//m_pTree = new NSKDTree::KDTree(m_iDims,16,false);
		m_pTree = new NSKDTree::KDTree(m_iDims,8,false);
		m_iNumElems = iNumPoints;

		m_vData = vector<float>(iNumPoints*m_iDims);
		memcpy(&m_vData[0],pData,iNumPoints*m_iDims*sizeof(float));

		const prob_t PI=3.14159265358979323846;

		m_dPiPow = pow((prob_t)PI,(prob_t)m_iDims/two);
		m_dTop = (one/(m_iNumElems-one));
		m_dGamma = Gamma(m_iDims/two+one);
		
		m_pTree->set_data(&m_vData[0],iNumPoints);

	//	m_vProbs = vector<float>(iNumPoints);
		
		return true;
	}

	int NumElems()
	{
		return m_iNumElems;
	}

	int NumDims()
	{
		return m_iDims;
	}

	float* operator[](int i)
	{
		if(i >= m_iNumElems) return 0;
		
		return &m_vData[i*m_iDims];
	}

	//get nearest neighbor as float*
	float* GetNearestNeighbor(float* p,bool bAllowZeroDist)
	{
		if(m_iNumElems == 1) return 0;
		
		const int iNumRads = 7;
		float pRads[7] = {3.0f,30.0f,150.0f,300.0f,600.0f,900.0f,1000.0f};

		int iIter = 0;
		float fRad = pRads[0];

		while(true)
		{
			m_pTree->search_center_radius(p,fRad);
			int iCount = m_pTree->get_count();
			if(iCount)
			{
				vector<float> vRadii(iCount);
				m_pTree->copy_radii(&vRadii[0]);

				int id = MinIdx(vRadii,bAllowZeroDist);
				if(id != -1)
				{
					vector<long> vID(iCount);
					m_pTree->copy_indices(&vID[0]);
					return &m_vData[vID[id]*m_iDims];
				}
			}
			//increase search radius
			if(iIter+1 >= iNumRads)
				fRad *= two;
			else
				fRad = pRads[++iIter];
		}		
	}

	float GetNearestRadius(float* p,vector<int>& vMap,int iID)
	{
		if(m_iNumElems == 1) return 0.0;
		
		//const int iNumRads = 7;
		//float pRads[iNumRads] = {3.0f,30.0f,150.0f,300.0f,600.0f,900.0f,1000.0f};

		int iIter = 0;
		float fRad = 15.0f ;//pRads[0];

		while(true)
		{
			m_pTree->search_center_radius(p,fRad);
			int iCount = m_pTree->get_count();
			if(iCount)
			{
				vector<float> vRadii(iCount);
				m_pTree->copy_radii(&vRadii[0]);

				float fm = FLT_MAX; 

				vector<long> vIndices(iCount);
				m_pTree->copy_indices(&vIndices[0]);

				bool bFound = false;

				int i = 0;
				for(i=0;i<iCount;i++)
				{
					if(vMap[vIndices[i]]==iID && vRadii[i]<= fm && vRadii[i]>0.0)
					{	bFound = true;
						fm = vRadii[i];
					}
				}

				if(bFound)
					return fm;
			}
			//increase search radius
			//if(iIter+1 >= iNumRads)
				fRad *= two;
			//else
			//	fRad = pRads[++iIter];
		}		
	}

	float GetNearestRadius(float* p,bool bAllowZeroDist)
	{
		if(m_iNumElems == 1) return 0.0;
		
		const int iNumRads = 7;
		float pRads[7] = {3.0f,30.0f,150.0f,300.0f,600.0f,900.0f,1000.0f};

		int iIter = 0;
		float fRad = pRads[0];

		while(true)
		{
			m_pTree->search_center_radius(p,fRad);
			int iCount = m_pTree->get_count();
			if(iCount)
			{
				vector<float> vRadii(iCount);
				m_pTree->copy_radii(&vRadii[0]);

				float fm = 0.0;

				if(bAllowZeroDist)
					return *std::min_element(vRadii.begin(),vRadii.end());
				else 
					fm=GZeroMinElem(vRadii);

				if(fm>0.0)
					return fm;
			}
			//increase search radius
			if(iIter+1 >= iNumRads)
				fRad *= two;
			else
				fRad = pRads[++iIter];
		}		
	}

	float GetNearestRadius(int i)
	{
		return GetNearestRadius(&m_vData[i*m_iDims],false);
	}

	//returns probability based on distance
	//of an arbitrary element in THIS distribution
	//to it's nearest neighbor in THIS distribution
	prob_t RProb(prob_t dRad)
	{
		return m_dTop / (m_dPiPow*dRad*m_dGamma);
	}

	//returns probability based on distance
	//of an arbitrary element in a DIFFERENT distribution
	//to it's nearest neighbor in THIS distribution
	prob_t RProbOther(prob_t dRad)
	{
		return (one/m_iNumElems) / (m_dPiPow*dRad*m_dGamma);
	}

	//returns probability of element i
	prob_t IProb(int i)
	{
		if(!m_pTree || i<0 || i>=m_iNumElems) return 0.0;

		if(1==m_iNumElems)return 1.0;

		return VProb(&m_vData[i*m_iDims]);
	}

	//returns probability of vector p
	//vector p must be in THIS distribution
	prob_t VProb(float* p)
	{
		if(!p || !m_pTree) return 0.0;

		if(1==m_iNumElems)return 1.0;

		prob_t dRad = GetNearestRadius(p,false);

		return RProb(dRad);
	}

	//returns probability of vector p
	//vector p must be in DIFFERENT distribution
	prob_t VProbOther(float* p)
	{
		if(!p || !m_pTree) return 0.0;

		if(1==m_iNumElems)return 1.0;

		prob_t dRad = GetNearestRadius(p,true);

		if(dRad == 0.0) return 0.0;

		return RProbOther(dRad);
	}

	char m_strMsg[1024];

	//entropy of distribution
	prob_t Entropy()
	{	//sprintf(m_strMsg,"Entropy sz=%d",m_iNumElems);
		//ScopedTimer S(m_strMsg);
		if(m_iNumElems<2) return 0.0;
		
		prob_t dEntrop = 0.0;
		prob_t dPiPowGamma = m_dPiPow*m_dGamma;

		int isz = m_iNumElems , i=0, iOffset = 0;
		
		for(i=0;i<isz;i++)
		{
			prob_t dDist = GetNearestRadius(&m_vData[iOffset],false);
			if(dDist<=0.0)continue;
			prob_t dProb = m_dTop / (dDist*dPiPowGamma);
			if(dProb<=0.0)continue;
			dEntrop += dProb * log2(dProb);
			iOffset += m_iDims;
		}
		return -dEntrop;
	}

	//vIDs specifies which cluster each element
	//belongs to. iClust specifies which cluster
	//to get entropy for
	prob_t Entropy(vector<int>& vIDs,int iClust)
	{
		prob_t dEntrop = 0.0;
		int isz = m_iNumElems , i=0;
		for(i=0;i<isz;i++)
		{
			if(vIDs[i]==iClust)
			{
				prob_t dProb = IProb(i);
				if(dProb==0.0) continue;
				dEntrop += dProb * log2(dProb);
			}
		}
		return -dEntrop;
	}

	//vIDs specifies which cluster each element
	//belongs to. iClust specifies which cluster
	//to get entropy for
	prob_t Entropy(vector<int>& vIDs,int iClust,int iNumElems)
	{	sprintf(m_strMsg,"Entropy c%d sz=%d totsz=%d",iClust,iNumElems,m_iNumElems);
		ScopedTimer S(m_strMsg);
		if(iNumElems<2) return 0.0;

		const prob_t PI=3.14159265358979323846;
		prob_t dPiPow = pow((prob_t)PI,(prob_t)m_iDims/two);
		prob_t dTop = (one/(iNumElems-one));
		prob_t dGamma = Gamma(m_iDims/two+one);
		prob_t dPiPowGamma = dPiPow*dGamma;

		prob_t dEntrop = 0.0;
		int isz = m_iNumElems , i=0;
		for(i=0;i<isz;i++)
		{
			if(vIDs[i]==iClust)
			{
				prob_t dDist = GetNearestRadius(&m_vData[i*m_iDims],vIDs,iClust);
				if(dDist<=0.0)continue;
				prob_t dProb = dTop / (dDist*dPiPowGamma);
				if(dProb<=0.0)continue;
				dEntrop += dProb * log2(dProb);
			}
		}
		return -dEntrop;
	}
};

template< class T >
struct vpwrap
{
	vector<T>* p_;
	int iMax_, iMin_;
	
	vpwrap(vector<T>* p,int iMin=-1,int iMax=-1)
		:p_(p),
	     iMax_(iMax),
		 iMin_(iMin){}

	//global operator== is used, not this one
    //this one doesn't even get compiled
	bool operator==(const vpwrap<T>& vr) const
	{
		if(iMax_ == -1 || iMin_ == -1) 
			return *p_ == *vr.p_;
		
		dont
		
		vector<T>& v1 = *p_;
		vector<T>& v2 = *vr.p_;
		
		int i = iMin_;
		for(;i<iMax_;i++)
			if(v1[i] != v2[i]) 
				return false;
		
		return true;
	}

	bool operator<(const vpwrap<T>& vr) const
	{
		if(iMax_ == -1 || iMin_ == -1) 
			return *p_ < *vr.p_;

		vector<T>& v1 = *p_;
		vector<T>& v2 = *vr.p_;

		return lexicographical_compare(&v1[iMin_],&v1[iMax_],&v2[iMin_],&v2[iMax_]);
	}

	T Dist(const vpwrap<T>& vr, T tMinSoFar) const
	{
		vector<T>& v1 = *p_;
		vector<T>& v2 = *vr.p_;

		int iMinT = iMin_ == -1 ? 0 : iMin_,
			iMaxT = iMax_ == -1 ? v1.size() : iMax_;

		T tDist(0);
		
		int i = iMinT;

		for(;i<iMaxT;i++)
		{
			T val = v1[i] - v2[i];
			val *= val;
			tDist += val;
			if(tDist > tMinSoFar) return tDist;
		}
		
		return tDist;
	}

	void Print(FILE* fp=stdout,bool bNewLine=false) const
	{
		vector<T>& v = *p_;		
		
		int iMinT = iMin_ == -1 ? 0 : iMin_;
		int iMaxT = iMax_ == -1 ? v.size() : iMax_;
		int i = iMinT;
		
		if(sizeof(T)==sizeof(int))
		{
			for(;i<iMaxT;i++)
			{
				fprintf(fp," %d ",v[i]);
			}
		}
		else// if(sizeof(T)==sizeof(float))
		{
			for(;i<iMaxT;i++)
			{
				fprintf(fp," %f ",v[i]);
			}
		}
		if(bNewLine) fprintf(fp,"\n");
	}
};

inline bool operator==(const vpwrap<float>& vl,const vpwrap<float>& vr)
{
	if(vl.iMax_ == -1 || vl.iMin_ == -1) 
		return *vl.p_ == *vr.p_;
	
	vector<float>& v1 = *vl.p_;
	vector<float>& v2 = *vr.p_;
	
	int i = vl.iMin_;
	for(;i<vl.iMax_;i++)
		if(v1[i] != v2[i]) 
			return false;
	
	return true;
}

inline bool operator==(const vpwrap<int>& vl,const vpwrap<int>& vr)
{
	if(vl.iMax_ == -1 || vl.iMin_ == -1) 
		return *vl.p_ == *vr.p_;
	
	vector<int>& v1 = *vl.p_;
	vector<int>& v2 = *vr.p_;
	
	int i = vl.iMin_;
	for(;i<vl.iMax_;i++)
		if(v1[i] != v2[i]) 
			return false;
	
	return true;
}

typedef std::map< vpwrap<int> , int> THMapI;
typedef THMapI::iterator THMapITI;

typedef std::map< vpwrap<float> , int> THMapF;
typedef THMapF::iterator THMapITF;

template< class T >
class TreeHist
{
	typename std::map< vpwrap<T> , int> m_hist;
	int m_iNumDims;
	int m_iNumElems;
	bool m_bBinless;
	int m_iMinD;
	int m_iMaxD;

public:

	void SetDRange(int iMinD,int iMaxD){ m_iMinD=iMinD; m_iMaxD=iMaxD; }

	bool GetBinless(){ return m_bBinless; }
	void SetBinless(bool b){ m_bBinless=b; }
	
	typename std::map< vpwrap<T> , int>::iterator Begin(){ return m_hist.begin(); }
	typename std::map< vpwrap<T> , int>::iterator End(){ return m_hist.end(); }
	int Size(){ return m_hist.size(); }

	void Print(FILE* fp=stdout)
	{
		int i = 0;
		std::map< vpwrap<T> , int>::iterator IT = m_hist.begin();
		for(;IT!=m_hist.end();IT++)
		{
			fprintf(fp,"vec%d  ",i++);
			IT->first.Print(fp,true);
			fprintf(fp,"count = %d, probability = %.4f\n\n",IT->second,GetITProb(IT));
		}
	}

	prob_t SumProb()
	{
		if(m_iNumElems == 0) return 0.0;

		prob_t dSum = 0.0;
		std::map< vpwrap<T> , int>::iterator IT = Begin();
		for(;IT!=End();IT++)
		{
			dSum += IT->second;
		}
		dSum /= (prob_t) m_iNumElems;
		return dSum;
	}

	bool UpdateProb( vector<T>* v,int iCount)
	{
		vpwrap<T> vr(v,m_iMinD,m_iMaxD);
		std::map< vpwrap<T> , int>::iterator IT = m_hist.find(vr);
		if(IT==m_hist.end())
		{
			m_hist.insert( make_pair(vr,iCount) );
			m_iNumElems += iCount;
		}
		else
		{
#ifdef _DEBUG
			vector<T>& v1 = *v;
			vector<T>& v2 = *IT->first.p_;
			int moo=0;
#endif
			IT->second+=iCount;
			m_iNumElems += iCount;

			if(IT->second == 0)
				m_hist.erase(IT);
		}
		return true;
	}

	//get count of bin that dVal is in 
	inline int GetVCount( vector<int>* v)  
	{ 
		typename std::map< vpwrap<T> , int>::iterator IT = m_hist.find(vpwrap<T>(v,m_iMinD,m_iMaxD));
		if(IT==m_hist.end())
			return 0;
		return IT->second;
	}

	//get probability of variable being bin i
	inline prob_t GetVProb(vector<int>* v) 
	{
		if(m_iNumElems == 0) return 0.0;
		return Prob(m_iNumElems,GetVCount(v)); 
	}

	inline prob_t GetITProb(typename std::map< vpwrap<T> , int>::iterator& IT)
	{
		return Prob(m_iNumElems,IT->second);
	}

	inline prob_t Entropy()
	{
		int i = 0;
		typename std::map< vpwrap<T> , int>::iterator IT = m_hist.begin();
		prob_t dEntropy = 0.0;

		if(m_bBinless)
		{
			if(1==m_hist.size())return 0.0;
			prob_t dSz = m_hist.size();
			prob_t dDim = Begin()->first.iMax_ - Begin()->first.iMin_ + 1.0;
			prob_t dPiPow = pow(PI,(prob_t)dDim/2.0);
			prob_t dTop = (1.0/(dSz-1.0));
			prob_t dGamma = Gamma(dDim/2.0+1.0);
			for(;IT!=m_hist.end();IT++)
			{
				T tDistC = sqrt( (prob_t) ClosestDist(IT));
				if(tDistC == 0.0) continue;
				prob_t dProb = (IT->second*dTop) / (dPiPow*tDistC*dGamma);
				dEntropy += dProb * log2(dProb);
			}
		}
		else
		{
			for(;IT!=m_hist.end();IT++)
			{
				prob_t dProb = Prob(m_iNumElems,IT->second); //IT->second / (prob_t) m_iNumElems;
				if(dProb == 0.0) continue;
				dEntropy += dProb * log2(dProb);
			}
		}
		return -dEntropy;
	}
	
	TreeHist(){ m_iNumElems = 0; m_bBinless = false; m_iMinD=-1;m_iMaxD=-1;};
	~TreeHist(){};

	//add h to this
	void Add(TreeHist& h)
	{
		typename std::map< vpwrap<T> , int>::iterator IT = h.m_hist.begin();
		for(;IT!=h.m_hist.end();IT++)
		{
			UpdateProb(IT->first.p_,IT->second);
		}
	}

	T ClosestDist(typename const std::map< vpwrap<T>, int>::iterator ITOther)
	{
		T tMinSoFar = 9e10;
		
		typename std::map< vpwrap<T>, int>::iterator IT = Begin();
		
		for(;IT!=End();IT++)
		{
			if(IT == ITOther) continue;

			T tDistTmp = IT->first.Dist(ITOther->first,tMinSoFar);
			
			if(tDistTmp < tMinSoFar)
				tMinSoFar = tDistTmp;
		}
		return tMinSoFar;
	}
};

//histogram class, with automatic counting/scaling
//of values to be between min & max
class Hist
{
protected:
	
	//counts of values
	std::vector<int> m_counts;

	//vector of probability values, has probs for (0 - m_iNumElems) / m_iNumElems
	std::vector<prob_t> m_probs;
	
	//min value in histogram
	prob_t m_dMin;
	
	//max value in histogram
	prob_t m_dMax;

	//m_dMax - m_dMin
	prob_t m_dRange;
	
	//number of bins
	int m_iBins;

	//# of elements in distribution
	prob_t m_dNumElems;

	//whether need to recalc probabilities
	//(uses lazy evaluation)
	bool m_bNeedReCalc;

public:

	//return bin # for value
	inline int ValIndex(prob_t dVal) const
	{
		if(m_dRange==0.0) return 0;

		//dVal's location in range as # btwn 0 - 1
		prob_t dFctr = (dVal - m_dMin) / m_dRange;
	
		//multiply by # of bins - 1 to have it 0-based index
		//doesn't use rounding to closest bin, but probably should
		return dFctr * (m_iBins - 1);
	}
	
	inline prob_t Max() const { return m_dMax; }
	inline prob_t Min() const { return m_dMin; }
	inline prob_t Range() const { return m_dRange; }
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
	inline bool IncBinVal(prob_t dVal)
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
	inline bool DecBinVal(prob_t dVal)
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
	inline bool SetBinVal(prob_t dVal,int iCount)
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
	inline int GetBinVal(prob_t dVal) const { return m_counts[ValIndex(dVal)]; }

	//get probability of variable being bin i
	inline prob_t GetBinProb(prob_t dVal) const{ return (prob_t) GetBinVal(dVal) / m_dNumElems; }

	//calc probabilities of 0 - m_dNumElems to help save time
	//only recalculate when the # of elements changes
	//not used
	inline void CalcProbs()
	{
		if(m_bNeedReCalc && m_dNumElems>=1.0)
		{
			m_probs = vector<prob_t>((int)m_dNumElems+1);
			prob_t j;
			for(j=0.0;j<=m_dNumElems;j+=1.0) m_probs[j] = j / m_dNumElems;
			m_bNeedReCalc=false;
		}
	}

	//get probability of bin i
	inline prob_t BinProb(int i)
	{ 
		//CalcProbs();
		//return m_probs[m_counts[i]];
		//return m_counts[i] / m_dNumElems; 
		return Prob(m_dNumElems,m_counts[i]); 
	}

	inline prob_t Entropy()
	{
		int i = 0;
		prob_t dEntropy = 0.0;
		for(;i<m_counts.size();i++)
		{
			if(!m_counts[i]) continue;
			prob_t dProb = BinProb(i);
			dEntropy += dProb * log2(dProb);
		}
		return -dEntropy;
	}
	
	//direct access to bin
	const int& operator[] (int i) const { return m_counts[i]; }
	
	//initialize histogram
	bool Init(prob_t dMin,prob_t dMax,int iBins);

	Hist(void);
	Hist(prob_t dMin,prob_t dMax,int iBins);
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
inline prob_t SumEntropy(vector< Hist >& vHist, vector<int>& vZero)
{
	int iSz = vHist.size();
	int i;
	prob_t dSum = 0.0;
	for(i=0;i<iSz;i++)
	{
		if(vZero[i]) continue;
		dSum += vHist[i].Entropy();
	}
	return dSum;
}

inline prob_t SumEntropy(vector< Hist> & vHist)
{
	int iSz = vHist.size();
	int i;
	prob_t dSum = 0.0;
	for(i=0;i<iSz;i++)
		dSum += vHist[i].Entropy();
	return dSum;
}

inline prob_t SumEntropy(vector< vector<Hist> >& vDistribs,vector<prob_t>& vWeights)
{
	int iSz = vDistribs.size();
	int i;
	prob_t dSum = 0.0;
	for(i=0;i<iSz;i++)
		dSum += vWeights[i] * SumEntropy(vDistribs[i]);
	return dSum;
}

inline void CalcEntropy(vector< vector< Hist> >& vDistribs,vector<prob_t>& vcEntropy,int iClusts,int iDim,vector<int>& vCounts,int iNumElems,vector<int>& vZeroes,bool bWeight)
{
	int iC;
	if(vcEntropy.size() < iClusts+1) vcEntropy = vector<prob_t>(iClusts+1);
	if(bWeight)
	{
		for(iC=1;iC<=iClusts;iC++)
		{
			vcEntropy[iC] = ( (prob_t) vCounts[iC] / (prob_t) iNumElems ) * SumEntropy(vDistribs[iC],vZeroes);
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

template< class T >
inline void CalcEntropy(vector< TreeHist<T> >& vDistribs,vector<prob_t>& vcEntropy,int iClusts,vector<int>& vCounts,int iNumElems,bool bWeight)
{
	int iC;
	if(vcEntropy.size() < iClusts+1) vcEntropy = vector<prob_t>(iClusts+1);
	if(bWeight)
	{
		for(iC=1;iC<=iClusts;iC++)
		{
			vcEntropy[iC] = ( (prob_t) vCounts[iC] / (prob_t) iNumElems ) * vDistribs[iC].Entropy();
		}
	}
	else
	{
		for(iC=1;iC<=iClusts;iC++)
		{
			vcEntropy[iC] = vDistribs[iC].Entropy();
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
inline prob_t KLDivApproxExclusiveProb(Hist& p,Hist& q)
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

inline prob_t FullProb(Hist& h)
{
	prob_t p = 0.0;
	for(int i=0;i<h.NumBins();i++)
	{
		p += h.BinProb(i);
	}
	return p;
}


inline prob_t KLDiv(KDTreeHist& p,KDTreeHist& q)
{	
	int i=0;
#ifdef DO_TIMING
	char sMsg[1024];
	sprintf(sMsg,"KLDiv szp=%d, szq=%d, dims=%d",p.NumElems(),q.NumElems(),p.NumDims());
	ScopedTimer S(sMsg);
#endif
	
	//make sure we can do calculation with div by zero and need same # of dimensions 
	//in each distribution
	if(p.NumElems() < 2 || q.NumElems()<1 || p.NumDims()!=q.NumDims()) return 0.0;

	try	//just in case something goes wrong from invalid type of data passed in
	{
		int iLessP = 0, iLessQ = 0;

		prob_t dpq = 0.0;

		const prob_t eps = 0.0;

		int isz = p.NumElems();

		i = 0;
		for(i=0;i<isz;i++)
		{
			prob_t distp = p.GetNearestRadius(i);
			
			if(distp<=eps)
				continue;

			prob_t distq = q.GetNearestRadius(p[i],true);
			
			if(distq<=eps) 
				continue;

			//if(distq<distp)
			//	iLessQ++;
			//else
			//	iLessP++;

			dpq += log2(distq / distp);
		}

		dpq *= ((prob_t)p.NumDims()/p.NumElems());

		dpq += log2( (prob_t)q.NumElems() / (p.NumElems()-1.0 ) );

	#ifdef DO_TIMING
		sprintf(sMsg,"iLessP=%d iLessQ=%d",iLessP,iLessQ);
		MessageBox(0,sMsg,"WClust",MB_ICONINFORMATION);
	#endif
		//if(dpq<0.0)
		//{	char sMsg[1024];
		//	sprintf(sMsg,"iLessP=%d iLessQ=%d",iLessP,iLessQ);
		//	MessageBox(0,sMsg,"WClust",MB_ICONERROR);
		//}
		
		return dpq;
	}
	catch(...)
	{
		char sMsg[1024];
		sprintf(sMsg,"KLDiv caught exception!! i=%d",i);
		MessageBox(0,sMsg,"WClust",MB_ICONERROR);
		return -666.0;
	}
}

//resistor avg.
inline prob_t KLDivSym(KDTreeHist& p,KDTreeHist& q)
{
	prob_t dpq = KLDiv(p,q), dqp = KLDiv(q,p);
	if(!dpq && !dqp) return 0.0;
	return dpq*dqp / (dpq+dqp);
}

template< class T >
inline prob_t KLDiv(TreeHist<T>& p,TreeHist<T>& q)
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
inline prob_t KLDiv(Hist& p,Hist& q)
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

//symmetrized form of KL-divergence
inline prob_t ResistorAvg(Hist& p,Hist& q)
{
	prob_t d12 = KLDiv(p,q), d21 = KLDiv(q,p);
	return (d12 * d21) / (d12 + d21);
}

//p is cluster distrib, q is full distrib
inline prob_t Uniqueness(Hist& p,Hist& q)
{
	return KLDiv(p,q);
}

//vZeroes = vector of dimensions that shouldn't contribute to calculations
//vZeroes[i] == 1 , iff you want to exclude dimension i
inline void CalcUDDist(vector< vector<Hist> >& vDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter,vector<int>& vCounts,int iElems,vector<int>& vZeroes,bool bUseCounts)
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

//vZeroes = vector of dimensions that shouldn't contribute to calculations
//vZeroes[i] == 1 , iff you want to exclude dimension i
template< class T >
inline void CalcUDDist(vector< TreeHist<T> >& vDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter,vector<int>& vCounts,int iElems,vector<int>& vZeroes,bool bUseCounts)
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

inline void CalcUDDist(vector< KDTreeHist >& vDistribs,vector< KDTreeHist >& vCompDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter,vector<int>& vCounts)
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
inline void CalcRDist(vector< vector<Hist> >& vDistribs,int iClusts,vector<prob_t>& vcInf,vector< vector<prob_t> >& vcInfInter)
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
//std::vector<prob_t> GetClusterInfo(CVerxStack& DataStack,CCluster& MainClusters,CPaletteStack* pMainPal,CFile* pFileBPF,int iBins,int iClusts,int which);
//get string rep of cluster info gain for writing to bpf,cl files
CString GetClusterInfoString(CVerxStack& MainDataStack,CCluster& MainClusters,HWND wnd=0);

bool PrintDistribs(vector<vector< Hist> >& vDistribs,char* fname);

bool Distribs2Images(vector< vector< Hist > >& vDistribs, char* fname_base);

bool Distribs2Matlab(vector< vector< Hist > >& vDistribs, char* fname_base);

void FillDistribs(CVerxStack& DataStack,CCluster& MainClusters,int iBins,std::vector< std::vector<Hist> >& vDistribs,int iClusts,int which);

//this is the continuous multidimensional probability version
inline void FillDistribs(vector<float>& vFloat,vector<KDTreeHist>& vDistribs,vector<KDTreeHist>& vCompDistribs,int iDistribs,vector<int>& vClustIDs,vector<int>& vCounts,int iDims,bool bGetComplements)
{
	vDistribs = vector< KDTreeHist >(iDistribs+1);

	if(bGetComplements)
		vCompDistribs = vector< KDTreeHist >(iDistribs+1);

	int iV = 0 , iC = 0;

	//full distribution
	vDistribs[iDistribs].SetData(iDims,&vFloat[0],vFloat.size()/iDims);

	int iTotalVs = vFloat.size() / iDims;

	for(iC=1;iC<iDistribs;iC++)
	{
		vector<float> vClustData(vCounts[iC]*iDims), vCompData;
		int iCompSize = iTotalVs - vCounts[iC];
		if(bGetComplements) vCompData = vector<float>(iCompSize*iDims);
		int j = 0 , k = 0;
		for(iV=0;iV<vClustIDs.size();iV++)
		{
			if(vClustIDs[iV] == iC)
			{
				int iD = 0;
				for(iD=0;iD<iDims;iD++)
				{
					vClustData[j++]=vFloat[iV*iDims+iD];
				}
			}
			else if(bGetComplements)
			{
				int iD = 0;
				for(iD=0;iD<iDims;iD++)
				{
					vCompData[k++]=vFloat[iV*iDims+iD];
				}
			}
		}
		vDistribs[iC].SetData(iDims,&vClustData[0],vCounts[iC]);
		if(bGetComplements)
			vCompDistribs[iC].SetData(iDims,&vCompData[0],iCompSize);
	}
}


template< class T >
void FillDistribs(CVerxStack& DataStack,vector< vector<int> >& vBinData,CCluster& MainClusters,int iBins,vector< TreeHist<T> >& vDistribs,int iDistribs,vector<int>& vClustIDs)
{
	//distrib for each cluster + 1 for full distrib
	vDistribs = std::vector< TreeHist<T> >(iDistribs+1);
		
	int iV = 0;
	for(iV=0;iV<vClustIDs.size();iV++)
	{		
		//cluster spike belongs to
		vDistribs[vClustIDs[iV]].UpdateProb(&vBinData[iV],1);

		//FULL distribution containing all spikes!!
		vDistribs[iDistribs].UpdateProb(&vBinData[iV],1);
	}
}

inline void FillDistribs(CVerxStack& DataStack,vector< vector<float>* >& vFloatps,CCluster& MainClusters,int iBins,vector< TreeHist<float> >& vDistribs,int iDistribs,vector<int>& vClustIDs,int iMinD,int iMaxD)
{
	//distrib for each cluster + 1 for full distrib
	vDistribs = std::vector< TreeHist<float> >(iDistribs+1);

	int iC=1;
	for(iC=1;iC<=iDistribs;iC++)
	{
		vDistribs[iC].SetBinless(true);
		vDistribs[iC].SetDRange(iMinD,iMaxD);
	}
		
	int iV = 0;
	for(iV=0;iV<vClustIDs.size();iV++)
	{		
		//cluster spike belongs to
		vDistribs[vClustIDs[iV]].UpdateProb(vFloatps[iV],1);

		//FULL distribution containing all spikes!!
		vDistribs[iDistribs].UpdateProb(vFloatps[iV],1);
	}
}


template< class T >
void FillDistribs(CVerxStack& DataStack,CCluster& MainClusters,int iBins,vector< TreeHist<T> >& vDistribs,int iDistribs,int which,vector<vector<int> >& vBinIDs)
{
	//distrib for each cluster + 1 for full distrib
	vDistribs = vector< TreeHist<T> >(iDistribs+1);
	int iDims = DataStack.GetAutoClusteringDimension();
	
	int iV=0,iC=1; 
	
	//go through vertices
	MY_STACK::iterator Index;
	for (Index=DataStack.m_VerxStack.begin();Index!=DataStack.m_VerxStack.end();Index++,iV++)
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
				vDistribs[iC].UpdateProb(&vBinIDs[iV],1);
			}
		}
	}
}

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