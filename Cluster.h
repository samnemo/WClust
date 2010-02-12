// $Id: Cluster.h,v 1.26 2009/06/24 18:04:16 samn Exp $ 
// Cluster.h: interface for the CCluster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLUSTER_H__41967E0D_1D55_4F3A_B881_CA39A2833FBD__INCLUDED_)
#define AFX_CLUSTER_H__41967E0D_1D55_4F3A_B881_CA39A2833FBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyObj.h"
#include "Vertex.h"
#include "UPDialog.h"
#include "FileUtils.h"

struct KLD2D
{
	int m_iD1;
	int m_iD2;
	prob_t m_kld;
	KLD2D()
	{
		m_iD1=m_iD2=-1;
		m_kld=-66666.6f;
	}
	void Init(int iD1,int iD2,prob_t kld)
	{
		m_iD1=iD1;
		m_iD2=iD2;
		m_kld=kld;
	}
};

inline bool operator<(KLD2D& k1,KLD2D& k2)
{
	return k1.m_kld<k2.m_kld;
}



////////////////////////////////////////////////////////////////////////
// CPoint2D
class CPoint2D : public CMyObject
{
public:
	float m_X,m_Y;
public:	
	CPoint2D(){};
	CPoint2D(float x,float y){ m_X=x;m_Y=y; };
	virtual ~CPoint2D(){};
	int		GetX(){ return m_X; };
	int		GetY(){ return m_Y; };
	void	GetValue(float *X, float *Y){ *X=m_X; *Y=m_Y; }; 
	int		LoadData(CFile *file);
	int		StoreData(CFile *file);
};

////////////////////////////////////////////////////////////////////////
// CVect2D
class CVect2D : public CMyObject
{
protected:
public:
	float m_X,m_Y,m_b;
public:	
	CVect2D(){};
	CVect2D(float x, float y, float b){m_X=x;m_Y=y;m_b=b;};
	virtual ~CVect2D(){};
	int		IsIn(float x,float y){ if(x*m_X+y*m_Y+m_b>=0) return 1; else return 0;};
	int		IsIn(CPoint point){ if(point.x*m_X+point.y*m_Y+m_b>=0) return 1; else return 0;};
	int		LoadData(CFile *file);
	void	Make(float x1, float y1, float x2, float y2){m_X=y1-y2;m_Y=x2-x1;m_b=x1*y2-x2*y1;};
	void	Make(CPoint2D *p1,CPoint2D *p2){m_X=p1->m_Y-p2->m_Y;m_Y=p2->m_X-p1->m_X;m_b=p1->m_X*p2->m_Y-p2->m_X*p1->m_Y;};
	int		StoreData(CFile *file);
};

////////////////////////////////////////////////////////////////////////
// CProjection
class CProjection : public CMyObject
{
public:
	int			m_AxesX,m_AxesY;
	int			m_Count;

	MY_STACK	m_Pnt2DStack;
	MY_STACK	m_Vect2DStack;
protected:
	void	ConvertAxes(char Version,int iCurDims);
public:	
	CProjection(){ m_Count=0; };
	CProjection(CPaletteStack *MyPalette){ m_Count=0;m_PalStack=MyPalette; };
	CProjection(int X,int Y){ m_AxesX=X; m_AxesY=Y;m_Count=0; };
	CProjection(int X,int Y,CPaletteStack *MyPalette){ m_AxesX=X; m_AxesY=Y;m_Count=0;m_PalStack=MyPalette; };
	virtual ~CProjection(){ SetEmpty();m_Count=0; };
	void	AddPnt2D(CMyObject *toStore) { m_Pnt2DStack.push_back(toStore); m_Count+=1; };
	void	AddPnt2Dinv(CMyObject *toStore) { m_Pnt2DStack.push_front(toStore); m_Count+=1; };
	void	AddVect2D(CMyObject *toStore) { m_Vect2DStack.push_back(toStore); };
	void	AddVect2Dinv(CMyObject *toStore) { m_Vect2DStack.push_front(toStore); };
	int		AdaptAxes(int numb);
	void	Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,int Color,int style);
	float	FindMinX();
	float	FindMaxX();
	float	FindMinY();
	float	FindMaxY();
	int		IsIn(CVertex *i_verx);
	int		LoadData(CFile *file, char Version, int iCurDims);
	void	SetAxes(int X,int Y){ if (X<Y){m_AxesX=X;m_AxesY=Y;}else{m_AxesX=Y;m_AxesY=X;};};
	void	SetEmpty();
	int		StoreData(CFile *file);
};

////////////////////////////////////////////////////////////////////////
// CBoundary
class CBoundary : public CMyObject
{
public:
	MY_STACK	m_ProjStack;
	
	CProjection	undoProj;
	int			undoPosition;

	int			m_Numb;
	int			m_Count;
	bool        m_bNoise;
	bool        m_bNoiseCollapsed;
public:
	CBoundary(){ m_Count=0;m_Numb=0;m_bNoise=false;m_bNoiseCollapsed=false; };
	CBoundary(CPaletteStack *MyPalette){ m_Count=0;m_Numb=0;m_PalStack=MyPalette;m_bNoise=false;m_bNoiseCollapsed=false; };
	virtual ~CBoundary(){ SetEmpty(); };
	void	AdaptProj(int numb);
	void	AddProj(CMyObject *toStore) { m_ProjStack.push_back(toStore); m_Count+=1; };
//	void	Clustering(CVerxStack *i_VerxStack);
	void	Clustering(CVertex *Pnt,bool NoiseOn=false);
	bool    IsIn(CVertex* Pnt);
	void	Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int style);
	int		GetNumb(){return m_Numb;}
	int		FindMinXY(int which, float *Value);
	int		FindMaxXY(int which, float *Value);
	int		LoadData(CFile *file, char Version,int iCurDims);
	void	RemLastProj() { m_ProjStack.pop_back(); m_Count--; }
	void	SetEmpty();
	int		StoreData(CFile *file);
};

struct ClusterInfo
{
	prob_t m_fBGInfoGain;
	prob_t m_fInterClustGain;
	int m_iMyID;
	int m_iClosestID;
	prob_t m_fPrctKNNInClust;
	prob_t m_fSilhouetteWidth;
	int m_iSz;	//# of spikes loaded in
	prob_t m_dIsolationDist;
	prob_t m_dLRatio;
	
	ClusterInfo()
		:m_fBGInfoGain(0.0),
		 m_fInterClustGain(0.0),
		 m_iMyID(0),
		 m_iClosestID(0),
		 m_fPrctKNNInClust(0.0),
		 m_fSilhouetteWidth(-2.0), //starts as invalid value -- should be btwn -1,1
		 m_iSz(0),
		 m_dIsolationDist(-1.0), // starts invalid
		 m_dLRatio(-1.0) // starts invalid
	{}
	
	ClusterInfo(prob_t fBGInfoGain,prob_t fInterClustGain,int iID,int iClosestID)
		:m_fBGInfoGain(fBGInfoGain),
		 m_fInterClustGain(fInterClustGain),
		 m_iMyID(iID),
		 m_iClosestID(iClosestID),
		 m_fPrctKNNInClust(0.0),
		 m_fSilhouetteWidth(-2.0), //starts as invalid value -- should be btwn -1,1
		 m_iSz(0),
		 m_dIsolationDist(-1.0), // starts invalid
		 m_dLRatio(-1.0) // starts invalid
	{}
};

//struct storing cluster quality options
struct CQOpts
{
	int			m_iNNToFind; //how many nearest neighbors to search for initially (when computing KLDIV)
	bool		m_bFastKLD;  //whether to use fast KLDIV (uses more mem.)
	bool		m_bInterKLD; //whether to compute inter-cluster kldiv
	bool		m_bDoKLDiv;  //whether to compute kldiv at all
	bool		m_bIsolationDist; //whether to compute isolation distance
	bool		m_bLRatio;		//whether to compute L-Ratio
	int			m_iBestDims;	//how many dimensions to use for kldiv
	bool		m_bFindBestDims; //whether to find best dimensions before computing KLDIV
	bool        m_b1DBest;		//iff==true use 1D KLDivs, otherwise use 2D KLDivs for finding best
	bool		m_bSilhouette;	//whether to compute silhouette width for each cluster -- only works in find best dims mode!!
	bool		m_bUseDefDims;  //whether to use default 8 dimensions for KLDIV (T1-4Peak,T1-4Energy)

	void Default()
	{
		m_iNNToFind = 1;
		m_bFastKLD = false;
		m_bInterKLD = true;
		m_bDoKLDiv = true;
		m_bIsolationDist = false;
		m_iBestDims = 8;
		m_bFindBestDims = true;
		m_b1DBest = false; //use 2D by default
		m_bSilhouette = false;
		m_bLRatio = false;
		m_bUseDefDims = false;
	}
	CQOpts(){ Default(); }
};

////////////////////////////////////////////////////////////////////////
// CCluster
class CCluster : public CMyObject  
{
public:	
	MY_STACK	m_BndrStack;
	MY_STACK	m_ParamDimStack;
	MY_WCP_FILES_STACK usedExternWCP;
	CBoundary	*undoBndry;
	char		undoType;
	int			m_Count;	//# of clusters?
	char		m_ClMode;  // 0 - random, 1 - exclude, 2 - weighted random
	char		m_Swap;		// 0 - normal, 1 - swapped
	char		m_newChanges;	//  1 - yes, it needs to be saved, 2 - yes, autoSave, 3-both

	//////////////////////////////////////////////////////////////
	//      pca related params
	bool			m_bUsePCA; //use pca
	bool            m_bCombinePCA; //combine waveforms or not
	int             m_iNumPCsToStore; //# of pcs to keep
	int				m_iPCAStartDim; //starting pca dimension
	int				m_iPCAEndDim; //ending pca dimension
	//////////////////////////////////////////////////////////////

	CQOpts m_oCQO;
	A2D<int>	m_vBestDims;	//best dimensions for each cluster
	A2D<prob_t>	m_v1DKLDivs;	//1D kldivs (clust vs background) of best dimensions for each cluster
	A2D<KLD2D>  m_v2DKLDivs;    //2D kldivs (clust vs background) of all pairs of dimensions

	vector< ClusterInfo > m_vInfo[6];   //cluster info. for each clustering mode and for each cluster within that mode. 
	int m_pNumClusts[6];

	bool CalcClusterInfo(CVerxStack& DataStack,bool bSymmetric,HWND wnd=0);
	void GetClusterInfo();
	//entropy reduction info clustering
	vector<double> EntropyInfoClust(CVerxStack& DataStack,int iClust,int iIters,const CUPDUPDATA* pCUPDUPData,double dErrProb);
	vector<prob_t> EntropyInfoClustMD(CVerxStack& DataStack,int iClust,int iIters,const CUPDUPDATA* pCUPDUPData);
	vector<double> EntropyInfoClustKDTree(CVerxStack& DataStack,int iClust,int iIters,const CUPDUPDATA* pCUPDUPData);
	vector<double> EntropyInfoClustKDTree2(CVerxStack& DataStack,int iClust,int iIters,const CUPDUPDATA* pCUPDUPData);
	//resistor info clustering
	void ResistorInfoClust(CVerxStack& DataStack,int iClusts,const CUPDUPDATA* pUp);
	//k-means
	void KmeansClust(CVerxStack& DataStack,int iMinClust,int iMaxClust,int iIter,int iDBIter,bool bNormalize,bool bUseMedians,const CUPDUPDATA* pUp);
	//self-organizing map clustering
	void SOMClust(CVerxStack& DataStack,int iIter,bool bNormalize,int iSXGrid,int iSYGrid,double dTau,const CUPDUPDATA* pUp);
	//affinity propagation clustering
	void APClust(CVerxStack& DataStack,int iIter,int iConvIter,prob_t dDamp,prob_t dPref,const CUPDUPDATA* pUp);
	//KlustaKwik clustering
	bool KlustaKwikClust(CVerxStack& DataStack,int iMinClusts,int iMaxClusts,int iStartIters,int iMaxPossClusts,const CUPDUPDATA* pUp);
	//Flame clustering
	bool FLAMEClust(CVerxStack& DataStack,int DistT,int Steps,float Eps,float KNNPrct,float OutTH,const CUPDUPDATA* pUp);

protected:
	long		dataStartPtr;
public:
	CCluster();
	CCluster(CPaletteStack *MyPalette);
	virtual ~CCluster(){ SetEmpty();m_Count=0; };
	void	AddAxes(CVerxStack *Pnts,CParamDim *param);
	void	AddBndry(CMyObject *toStore) { m_BndrStack.push_back(toStore); m_Count+=1; /*m_newChanges = 3;*/ };
	void	AddParam(CMyObject *toStore, CVerxStack *Pnts, bool bUserAdd);
	void	AutoSave(char *fileAS,CString& strInfo);
	void	CalcParam(CVerxStack *Pnts);
	void	CalcParamLast(CVerxStack *Pnts);
	void	CalcParamOneSpike(CVerxStack *Pnts, CVertex *Pnt, sSpike *spike);
	int		ChooseFileAndStore(CString& strInfo);
	int     Store(CString& strPath,CString& strInfo);
	void	Clear();
	void	ClearSwap() { m_Swap = 0; };
	void	ClearUNDO();
	void    SetNoise(CVerxStack *Pnts);
	void	Clustering(CVerxStack *Pnts,bool NoiseOn=false);
	void	Clustering(CVertex *Pnt);
	void	CreateHeader(FILE *file,CString& strInfo);
	void	Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int style);  // 0-bottom is down, 1 bottom is up
	int		FindDataStartPtr( CFile *file );
	void	FindNoise( CVerxStack *pnts );
	void	FindNoise( CVertex *pnt );
	int		GetCount(){ return m_Count; };
	void	GetDataStart();
	char	GetSwap() { return m_Swap; };
	char	GetWantStore() { return m_newChanges; };
	char	IsUNDO(); 
	int		LoadData(CFile *file, CVerxStack *Pnts);
	void	MakeUNDO(int cluster, char type); // type: 0-none, 1-change, 2-insert (after removing)
	int		OpenFile(CFile *file, CVerxStack *Pnts, BatchRec* pQB);

	///****************************
	// is this a memory leak???
	void	RemLastBndry() 
	{	
		delete *(m_BndrStack.end()-1);
		m_BndrStack.pop_back(); 
		m_Count--; 
	};
	///****************************
	
	void	RemParam(CVerxStack*Pnts, int iNum);
	void	RemParamZeros(CVerxStack *Pnts);
	void	SetEmpty();
	void	StoreData(CFile *file);//,CVerxStack* Pnts);
	void	SwapPolarity(CVerxStack *Pnts);
	void	UNDO();
	void	WantStore() { m_newChanges = 3; };
	int		GetNumClusts(CVerxStack& DataStack);

	friend bool QBThread(const CUPDUPDATA* pUp);
};

#endif // !defined(AFX_CLUSTER_H__41967E0D_1D55_4F3A_B881_CA39A2833FBD__INCLUDED_)
