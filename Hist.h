// $Id: Hist.h,v 1.25 2009/06/25 04:04:14 samn Exp $ 
#pragma once
#include "Cluster.h"
#include "WCMath.h"
#include "KDTree.h"
#include "ScopedTimer.h"
#include "Log.h"
#include <map>
#include <set>

//#define DO_TIMING

//binary logarithm
inline prob_t log2(prob_t d)
{
	static prob_t dl2 = log(2.0);
	return log(d) / dl2;
}

inline double log2(double d)
{
	static double dl2 = log(2.0);
	return log(d) / dl2;
}

prob_t Prob(int,int);

void InitProbs(int iMaxNumElems);

struct ProbInitFree
{
	ProbInitFree(int i);
	~ProbInitFree();
};

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

struct Neighbor
{
	prob_t m_dist;
	int m_id;
	Neighbor(int id,prob_t dist)
		:m_id(id),m_dist(dist){}
	Neighbor()
		:m_id(0),m_dist(0){}
};

inline bool operator<(Neighbor& n1,Neighbor& n2)
{
	return n1.m_dist<n2.m_dist;
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

	prob_t Top(){ return m_dTop; }
	prob_t PiPow(){ return m_dPiPow; }
	prob_t Gam(){ return m_dGamma; }

	bool SetData(vector<float>& vFloat,vector<int>& vIDs,int iNumPoints,int iDims,int iCID,bool bNot=false)
	{
		if(!vFloat.size() || iNumPoints<1) return false;

		int iV = 0 , iC = 0;

		int iTotalVs = vFloat.size() / iDims;

		m_vData = vector<float>(iNumPoints*iDims);
		int j = 0 , k = 0;
		if(bNot)
		{
			for(iV=0;iV<vIDs.size();iV++)
			{
				if(vIDs[iV] != iCID && vIDs[iV]<1000)
				{
					int iD = 0;
					for(iD=0;iD<iDims;iD++)
					{
						m_vData[j++]=vFloat[iV*iDims+iD];
					}
				}
			}
		}
		else
		{
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

		return true;
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

	float GetKNN(float* p,vector<Neighbor>& vnn,int iNNToFind,double fRadStart=8.0,double fRadFctr=2.0)
	{
		if(m_iNumElems == 1) return 0.0;
		
		int iIter = 0;
		float fRad = fRadStart;

		while(true)
		{
			m_pTree->search_center_radius_sq(p,fRad,iNNToFind);
			int iCount = m_pTree->get_count();
			if(iCount>iNNToFind)// || (iCount&&iNNToFind==1))
			{
				vector<float> vRadii(iCount);
				m_pTree->copy_radii_sq(&vRadii[0]);

				vector<long> vID(iCount);
				m_pTree->copy_indices(&vID[0]);

				vnn=vector<Neighbor>(iCount);

#ifdef _DEBUG
				Write2Log("found %d neighbors want %d",iCount,iNNToFind);
#endif

				int i = 0, j = 0;
				for(i=0;i<iCount;i++)
					if(vRadii[i]>0.0)
						vnn[j++] = Neighbor(vID[i],vRadii[i]);
				
				vnn.resize(j);
				std::sort(vnn.begin(),vnn.end());

				if(vnn.size()>iNNToFind)
				{
					vector<Neighbor> vNNTmp(iNNToFind);
					std::copy(vnn.begin(),vnn.begin()+iNNToFind,vNNTmp.begin());
					vnn=vNNTmp;
				}

#ifdef _DEBUG
				prob_t ttt = vnn[0].m_dist;
#endif
				
				return fRad;
			}
			//increase search radius
			fRad *= fRadFctr;
		}		
	}

	float GetKNN(float* p,Neighbor* vnn,int iNNToFind,double fRadStart,double fRadFctr)
	{
		if(m_iNumElems == 1) return 0.0;
		
		int iIter = 0;
		float fRad = fRadStart;

		while(true)
		{
			m_pTree->search_center_radius_sq(p,fRad,iNNToFind);
			int iCount = m_pTree->get_count();
			if(iCount>iNNToFind)// || (iCount&&iNNToFind==1))
			{
				vector<float> vRadii(iCount);
				m_pTree->copy_radii_sq(&vRadii[0]);

				vector<long> vID(iCount);
				m_pTree->copy_indices(&vID[0]);

				vector<Neighbor> vnnTMP(iCount);

				int i = 0, j = 0;
				for(i=0;i<iCount;i++)
					if(vRadii[i]>0.0)
						vnnTMP[j++] = Neighbor(vID[i],vRadii[i]);
				
				vnnTMP.resize(j);
				std::sort(vnnTMP.begin(),vnnTMP.end());

				for(i=0;i<iNNToFind;i++) vnn[i]=vnnTMP[i];

				return fRad;
			}
			//increase search radius
			fRad *= fRadFctr;
		}		
	}

	float GetAllKNN(A2D<Neighbor>& vnn,int iNNToFind,double fRadStart,double fRadFctr,vector<int>& vFound)
	{
		if(m_iNumElems == 1) return 0.0;
		
		int iIter = 0;
		float fRad = fRadStart;

		vnn.Fill(Neighbor(0,INF));
		vFound.resize(m_iNumElems);

		while(true)
		{
			m_pTree->neighbor_search_sq(fRadStart);
			int iCount = m_pTree->neighbor_get_count();
			if(iCount/2>m_iNumElems*iNNToFind)
			{
				Neighbor** pnn = vnn.GetP();

				vector<float> vRadii(iCount);
				m_pTree->neighbor_copy_radii_sq(&vRadii[0]);

				vector<long> vID(iCount*2);
				m_pTree->neighbor_copy_indices(&vID[0]);

				int i = 0 , j = 0, k = 0;
				for(i=0;i<iCount;i+=2)
				{
					int iN1 = vID[i], iN2 = vID[i+1], iPos = 0;
					
					prob_t distSQ = vRadii[i/2];
					
					if(distSQ <= 0.0f) continue;
					
					//find sorted position in iN1 neighbor array to place element in
					for(j=0;j<vFound[iN1];j++)
					{
						if(distSQ < pnn[iN1][j].m_dist)
						{
							iPos = j;
							break;
						}
					}
					//do we need to shift elements after this one?
					if(iPos < iNNToFind - 1)
					{
						for(j=vFound[iN1]-1;j>iPos;j--)
							pnn[iN1][j]=pnn[iN1][j-1];
						pnn[iN1][iPos]=Neighbor(iN2,distSQ);
						if(vFound[iN1]+1<iNNToFind)vFound[iN1]++;
					}
					else if(iPos<iNNToFind)//no shift needed, just store
					{	
						pnn[iN1][iPos]=Neighbor(iN2,distSQ);
						if(vFound[iN1]+1<iNNToFind)vFound[iN1]++;
					}

					iPos = 0;
					for(j=0;j<vFound[iN2];j++)
					{
						if(distSQ < pnn[iN2][j].m_dist)
						{
							iPos = j;
							break;
						}
					}
					if(iPos < iNNToFind - 1)
					{
						for(j=vFound[iN2]-1;j>iPos;j--)
							pnn[iN2][j]=pnn[iN2][j-1];
						pnn[iN2][iPos]=Neighbor(iN1,distSQ);
						if(vFound[iN2]+1<iNNToFind)vFound[iN2]++;
					}
					else if(iPos<iNNToFind)	
					{
						pnn[iN2][iPos]=Neighbor(iN1,distSQ);
						if(vFound[iN2]+1<iNNToFind)vFound[iN2]++;
					}
				}
				return fRad;
			}
			//increase search radius
			fRad *= fRadFctr;
		}		
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
			m_pTree->search_center_radius_sq(p,fRad,1);
			int iCount = m_pTree->get_count();
			if(iCount)
			{
				vector<float> vRadii(iCount);
				m_pTree->copy_radii_sq(&vRadii[0]);

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

	float GetNearestRadiusSQ(float* p,vector<int>& vMap,int iID)
	{
		if(m_iNumElems == 1) return 0.0;
		
		int iIter = 0;
		extern prob_t gstartrad;
		float fRad = gstartrad;

		while(true)
		{
			m_pTree->search_center_radius_sq(p,fRad,1);
			int iCount = m_pTree->get_count();
			if(iCount)
			{
				vector<float> vRadii(iCount);
				m_pTree->copy_radii_sq(&vRadii[0]);

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
			fRad *= two;
		}		
	}

	bool GetNearestNeighbor(float* p,bool bAllowZeroDist,Neighbor& n)
	{
		if(m_iNumElems == 1) return false;
		m_pTree->search_nn(p,bAllowZeroDist);
		float fm = 0.0;
		m_pTree->copy_radii_sq(&fm);
		long id;
		m_pTree->copy_indices(&id);
		n.m_dist = fm;
		n.m_id = id;
		return true;
	}

	float GetNearestRadiusSQ(float* p,bool bAllowZeroDist,bool bTest)
	{
		if(m_iNumElems == 1) return 0.0;		

		int iIter = 0;
		extern prob_t gstartrad;
		float fRad = gstartrad;

		if(bTest)
		{
#ifdef DO_TIMING
			extern MTimer oMT;	TimerInc oT(oMT);
#endif
			m_pTree->search_nn(p,bAllowZeroDist);
			float fm = 0.0;
			m_pTree->copy_radii_sq(&fm);
			return fm;
		}
		else
		{
#ifdef DO_TIMING
			extern MTimer oMF; TimerInc oT(oMF);
#endif
			while(true)
			{
				m_pTree->search_center_radius_sq(p,fRad,1);
				int iCount = m_pTree->get_count();
				if(iCount)
				{
					vector<float> vRadii(iCount);
					m_pTree->copy_radii_sq(&vRadii[0]);

					float fm = 0.0;

					if(bAllowZeroDist)
						return *std::min_element(vRadii.begin(),vRadii.end());
					else 
						fm=GZeroMinElem(vRadii);

					if(fm>0.0)
						return fm;
				}
				//increase search radius
				fRad *= two;
			}
		}
	}

	float GetNearestRadiusSQ(int i,bool bTest)
	{
		return GetNearestRadiusSQ(&m_vData[i*m_iDims],false,bTest);
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

		prob_t dRad = sqrt(GetNearestRadiusSQ(p,false,false));

		return RProb(dRad);
	}

	//returns probability of vector p
	//vector p must be in DIFFERENT distribution
	prob_t VProbOther(float* p)
	{
		if(!p || !m_pTree) return 0.0;

		if(1==m_iNumElems)return 1.0;

		prob_t dRad = sqrt(GetNearestRadiusSQ(p,true,false));

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
			prob_t dDist = GetNearestRadiusSQ(&m_vData[iOffset],false,false);
			if(dDist<=0.0)continue;
			dDist = sqrt(dDist); 
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
				prob_t dDist = GetNearestRadiusSQ(&m_vData[i*m_iDims],vIDs,iClust);
				if(dDist<=0.0)continue;
				dDist=sqrt(dDist);
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
		return GetVCount(v) / (float) m_iNumElems; // Prob(m_iNumElems,GetVCount(v)); 
	}

	inline prob_t GetITProb(typename std::map< vpwrap<T> , int>::iterator& IT)
	{
		return IT->second / (float) m_iNumElems; // Prob(m_iNumElems,IT->second);
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
				prob_t dProb = IT->second / (prob_t) m_iNumElems; // Prob(m_iNumElems,IT->second); 
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

//histogram with variable bin width
class VHist
{
	float m_fMin;
	float m_fMax;
	float m_fBinWidth;
	int   m_iBins;
	float m_fNumElems;
	vector<int> m_counts;

	bool ChangeBin(float val,int iInc)
	{
		int ibin = Bin(val);
		if(ibin<0 || ibin>=m_iBins)
			return false;
		m_counts[ibin]+=iInc;
		m_fNumElems+=iInc;
		return true;
	}

public:

	float BinWidth()
	{
		return m_fBinWidth;
	}

	float Elems()
	{
		return m_fNumElems;
	}

	int Bins()
	{
		return m_iBins;
	}

	int operator[](int i)
	{
		return m_counts[i];
	}

	int Bin(float val)
	{	//(-1.1 - -1.2) / .5 = .1 / .5 = 1/5 = 0
	    //(-0.6 - -1.2) / .5 = .6 / .5 = 6/5 = 1
		return ( val - m_fMin) / m_fBinWidth;
	}

	float Prob(float val)
	{
		try
		{
			return m_counts[Bin(val)] / m_fNumElems;
		}
		catch(...)
		{	Write2Log("VHist::Prob exception!");
			return 0.0f;
		}
	}

	float ProbSum()
	{	int i = 0;
		float s = 0.0;
		for(;i<m_iBins;i++)
			s += m_counts[i] / m_fNumElems;
		return s;
	}

	float Entropy()
	{	int i = 0;
		float e = 0.0 , p = 0.0;
		for(;i<m_iBins;i++)
		{	p = m_counts[i] / m_fNumElems;
			if(p<=0.0) continue;
			e -= p * log2(p);
		}
		return e;
	}

	bool Inc(float val)
	{
		return ChangeBin(val,1);
	}

	bool Dec(float val)
	{
		return ChangeBin(val,-1);
	}
	
	bool Init(float fMin,float fMax,float BinWidth)
	{
		m_fMin=fMin; m_fMax=fMax; m_fBinWidth=BinWidth;
		m_fNumElems=0.0f;
		m_iBins = (m_fMax-m_fMin)/m_fBinWidth;
		if(!m_iBins) return false;
		m_counts=vector<int>( m_iBins , 0);
		return true;
	}

	VHist(float fMin,float fMax,float BinWidth)
	{
		Init(fMin,fMax,BinWidth);
	}

};
#include <hash_map>
struct eqp
{
  bool operator()(const int* s1, const int* s2) const
  {
    return memcmp(s1, s2, sizeof(int)*2 );
  }
};
//histogram class, with automatic counting/scaling
//of values to be between min & max
//uses fixed bin width, so bin 0 corresponds to min value
//and last bin corresponds to max value. should only use
//to compare distributions with same min and max, otherwise
//values will be squeezed together into bins differently
typedef double hprob;
class Hist
{
protected:
	
	//counts of values
	std::vector<int> m_counts;

	//vector of probability values, has probs for (0 - m_iNumElems) / m_iNumElems
	std::vector<hprob> m_probs;
	
	//min value in histogram
	hprob m_dMin;
	
	//max value in histogram
	hprob m_dMax;

	//m_dMax - m_dMin
	hprob m_dRange;
	
	//number of bins
	int m_iBins;

	//# of elements in distribution
	hprob m_dNumElems;

	//whether need to recalc probabilities
	//(uses lazy evaluation)
	bool m_bNeedReCalc;

	hash_map< int* ,hprob, hash_compare<const int*, eqp> > m_cache;
	hash_map< int* ,hprob, hash_compare<const int*, eqp> >::iterator m_IT;

public:

	//return bin # for value
	inline int ValIndex(hprob dVal) const
	{
		if(m_dRange==0.0) return 0;

		//dVal's location in range as # btwn 0 - 1
		hprob dFctr = (dVal - m_dMin) / m_dRange;
	
		//multiply by # of bins - 1 to have it 0-based index
		//doesn't use rounding to closest bin, but probably should
		return dFctr * (m_iBins - 1);
	}
	
	inline hprob Max() const { return m_dMax; }
	inline hprob Min() const { return m_dMin; }
	inline hprob Range() const { return m_dRange; }
	inline int NumBins() const { return m_iBins; }

	//number of elements in "distribution"
	inline int NumElems() const
	{
		return (int) m_dNumElems;
	}

	//increment bin by 1
	inline bool DecBin(int idx)
	{
#ifdef _DEBUG
		if(idx < 0 || idx >= m_counts.size()){ Write2Log("out of bounds %d %d!!!!\n",idx,m_counts.size());
			return false;}
#endif
		m_dNumElems -= 1;
		//m_bNeedReCalc = true;
		m_counts[idx] -= 1;	
		return true;
	}

	//increment bin by 1
	inline bool IncBin(int idx)
	{
#ifdef _DEBUG
		if(idx < 0 || idx >= m_counts.size()){ Write2Log("out of bounds %d %d!!!!\n",idx,m_counts.size());
			return false; }
#endif
		m_dNumElems += 1;
		//m_bNeedReCalc = true;
		m_counts[idx] += 1;	
		return true;
	}

	//increment bin that dVal is in by 1
	inline bool IncBinVal(hprob dVal)
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
	inline bool DecBinVal(hprob dVal)
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
	inline bool SetBinVal(hprob dVal,int iCount)
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
	inline int GetBinVal(hprob dVal) const { return m_counts[ValIndex(dVal)]; }

	//get probability of variable being bin i
	inline hprob GetBinProb(hprob dVal) const{ return (hprob) GetBinVal(dVal) / m_dNumElems; }

	//calc probabilities of 0 - m_dNumElems to help save time
	//only recalculate when the # of elements changes
	//not used
	inline void CalcProbs()
	{
		if(m_bNeedReCalc && m_dNumElems>=1.0)
		{
			m_probs = vector<hprob>((int)m_dNumElems+1);
			hprob j;
			for(j=0.0;j<=m_dNumElems;j+=1.0) m_probs[j] = j / m_dNumElems;
			m_bNeedReCalc=false;
		}
	}

	//get probability of bin i
	inline hprob BinProb(int i)
	{ 
		return m_counts[i] / m_dNumElems; // Prob(m_dNumElems,m_counts[i]); 
	}

	inline hprob CachedEntropy()
	{
		int i = 0;
		hprob dEntropy = 0.0;
		hprob dC;
		int tmp[2] , iSZ = m_dNumElems;
		tmp[1] = iSZ;
		for(;i<m_counts.size();i++)
		{
			if(!m_counts[i]) continue;
			tmp[0] = m_counts[i];			
			if( (m_IT = m_cache.find(tmp)) == m_cache.end() )
			{	hprob dProb = m_counts[i] / m_dNumElems;
				dC = dProb * log2(dProb);								
				m_cache[tmp] = dC;
			}
			else dC = m_IT->second;
			dEntropy += dC;
		}
		return -dEntropy;
	}

	inline hprob Entropy()
	{
		int i = 0;
		hprob dEntropy = 0.0;
		for(;i<m_counts.size();i++)
		{
			if(!m_counts[i]) continue;
			hprob dProb = BinProb(i);
			dEntropy += dProb * log2(dProb);
		}
		return -dEntropy;
	}
	
	//direct access to bin
	const int& operator[] (int i) const { return m_counts[i]; }
	
	//initialize histogram
	bool Init(hprob dMin,hprob dMax,int iBins);

	Hist(void);
	Hist(hprob dMin,hprob dMax,int iBins);
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

//max entropy with option of zeroing out certain dimensions
//if vZero[i] != 0, skip that dimension
inline prob_t MaxEntropy(vector< Hist >& vHist, vector<int>& vZero)
{
	int iSz = vHist.size();
	int i;
	prob_t dMax = 0.0 , dTmp = 0.0;
	for(i=0;i<iSz;i++)
	{
		if(vZero[i]) continue;
		if( (dTmp=vHist[i].Entropy()) > dMax )
			dMax = dTmp;
	}
	return dMax;
}

//sum entropy with option of zeroing out certain dimensions
//if vZero[i] != 0, skip that dimension
inline hprob SumEntropy(vector< Hist >& vHist, vector<int>& vZero)
{
	int iSz = vHist.size();
	int i;
	hprob dSum = 0.0;
	for(i=0;i<iSz;i++)
	{
		if(vZero[i]) continue;
		dSum += vHist[i].Entropy();
	}
	return dSum;
}

inline hprob SumEntropy(vector< Hist> & vHist)
{
	int iSz = vHist.size();
	int i;
	hprob dSum = 0.0;
	for(i=0;i<iSz;i++)
		dSum += vHist[i].Entropy();
	return dSum;
}

inline hprob SumEntropy(vector< vector<Hist> >& vDistribs,vector<prob_t>& vWeights)
{
	int iSz = vDistribs.size();
	int i;
	hprob dSum = 0.0;
	for(i=0;i<iSz;i++)
		dSum += SumEntropy(vDistribs[i]);
	return dSum;
}

inline void CalcEntropy(vector< vector< Hist> >& vDistribs,vector<hprob>& vcEntropy,int iClusts,int iDim,vector<int>& vCounts,int iNumElems,vector<int>& vZeroes)
{
	int iC;
	if(vcEntropy.size() < iClusts+1) vcEntropy = vector<hprob>(iClusts+1,0.0);
	for(iC=1;iC<=iClusts;iC++)
	{	
		vcEntropy[iC] = ( (hprob) vCounts[iC] / (hprob) iNumElems ) * SumEntropy(vDistribs[iC],vZeroes);
	}
	if(vCounts[0]) vcEntropy[0] = -1.0 * ( (hprob) vCounts[0] / (hprob) iNumElems ) * SumEntropy(vDistribs[0],vZeroes); // ************* //
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


inline prob_t FullProb(Hist& h)
{
	prob_t p = 0.0;
	for(int i=0;i<h.NumBins();i++)
	{
		p += h.BinProb(i);
	}
	return p;
}

//get vector containing information gain for each cluster
//std::vector<prob_t> GetClusterInfo(CVerxStack& DataStack,CCluster& MainClusters,CPaletteStack* pMainPal,CFile* pFileBPF,int iBins,int iClusts,int which);
//get string rep of cluster info gain for writing to bpf,cl files
CString GetClusterInfoString(CVerxStack& MainDataStack,CCluster& MainClusters,HWND wnd=0,int DrawMode=CLUST_USER);

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

//this is the continuous multidimensional probability version
void FillDistribs(vector<float>& vFloat,vector<KDTreeHist>& vDistribs,vector<KDTreeHist>& vCompDistribs,int iDistribs,vector<int>& vClustIDs,vector<int>& vCounts,int iDims,A2D<int>& vBestDims,int iBestDims);
//get full 'background' distribution , containing all spikes but using dimensions specified in pBestDims
void GetFullBGDistrib(vector<float>& vFloat,KDTreeHist& oTree,int iDims,int* pBestDims,int iBestDims);

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

void FillDistribs(CVerxStack& DataStack,int** pData,CCluster& MainClusters,int iBins,std::vector< std::vector<Hist> >& vDistribs,int iDistribs,vector<int>& vClustIDs,int iMinClust=1);

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
	case CLUST_FL:
		return verx->GetFLClust();
		break;
	}
	return 0;
}
