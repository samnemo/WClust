// $Id: Vertex.h,v 1.25 2010/01/14 18:36:43 samn Exp $ 
// Vertex.h: interface for the CVertex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERTEX_H__80EE18DE_3F8F_4FBA_8BA5_36D095AD0655__INCLUDED_)
#define AFX_VERTEX_H__80EE18DE_3F8F_4FBA_8BA5_36D095AD0655__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyObj.h"
#include "SyncStack.h"
#include "EEGStack.h"
#include "A2D.h"
#include "FileUtils.h"
#include "BatchUtils.h"
#include "Log.h"
#include "pca2.hpp"
#include <deque>
#include <map>

using namespace std;

#define		FILE_BPF		1;
#define		FILE_UFF		2;

////////////////////////////////////////////////////////////////////////
// BPF struct
struct sBPF {
	int		NumberOfEegChannels;

	int		EEG_SIZE;
	int		SINGLE_SIZE;
	int		STEREO_SIZE;
	int		TETRODE_SIZE;
	int		SYNC_SIZE;
	int		ROOM_POSITION_SIZE;                      
	int		ARENA_POSITION_SIZE;
	int		KEY_EVENT_SIZE;
	int		INPUT_EVENT_SIZE;
	int		OUTPUT_EVENT_SIZE;

	// Identifiers
	char	ARENA_POSITION_TYPE;
	char	ROOM_POSITION_TYPE;
	char	EEG_TYPE;
	char	SINGLE_TYPE;
	char	STEREO_TYPE;
	char	TETRODE_TYPE;
	char	SYNC_TYPE;
	int		KEY_EVENT_TYPE;
	int		INPUT_EVENT_TYPE;
	int		OUTPUT_EVENT_TYPE;

	vector<string> vEEGNames;

	int		NumberOfTetrodeChannels;
};

////////////////////////////////////////////////////////////////////////
// Spikes parameters

#define		SPIKE_ON_DISK	0;
#define		SPIKE_IN_MEMORY	1;

struct sSpike {
	char	type;		// 0-spike is on disk, 1-spike is in memory (offset is order in stack)	
	unsigned char	cluster;
	char	origCluster;
	int		TimeStamp;
	int		offset;		// offset in file. If file is in memory than offset contains offset in m_VerxStack
	VERTEX	fileParams;	// parameters from file WCP
};

typedef deque <sSpike> MY_SPIKE_STACK;

////////////////////////////////////////////////////////////////////////
// Definition for load portrait of BPF/UFF file.
// It is for faster loading of parameters

struct sSpikeInFile {
	unsigned char	electrode;	// 1-singlode, 2-stereode, 4-tetrode
	unsigned char	channel;
	sSpikeInFile(unsigned char e,unsigned char c)
		:electrode(e),
		  channel(c)
	{}
	sSpikeInFile()
		:electrode(0),
		 channel(0)
	{}
};

typedef deque <sSpikeInFile> MY_FILE_PORTRAIT_STACK;

////////////////////////////////////////////////////////////////////////
struct sNoiseParams {
	char spikeMode; // 0-max first, 1-min first
	unsigned char timeMin;
	unsigned char timeMax;
};

////////////////////////////////////////////////////////////////////////
// sWcpFiles
struct sWcpFiles 
{
	int flag;	// 0 - bad file, 1 - OK, >127 - important
	CString fileName;
	CString paramName;	// is defined in the fileName file
};

typedef deque <sWcpFiles> MY_WCP_FILES_STACK;


////////////////////////////////////////////////////////////////////////
// CParamDim
class CParamDim : public CMyObject
{
public:
	int m_Type;
	float m_Par1, m_Par2, m_Par3, m_Par4;
	int m_iVDim;
	CParamDim(){};
	CParamDim(int type, float par1){ m_Type = type; m_Par1 = par1; };
	CParamDim(int type, float par1, float par2){ m_Type = type; m_Par1 = par1; m_Par2 = par2; };
	CParamDim(int type, float par1, float par2, float par3){ m_Type = type; m_Par1 = par1; m_Par2 = par2; m_Par3 = par3;};
	CParamDim(int type, float par1, float par2, float par3, float par4){ m_Type = type; m_Par1 = par1; m_Par2 = par2; m_Par3 = par3; m_Par4 = par4; };
	int		GetType(){ return m_Type; };
	float	GetParam(int num){ if (num==1) return m_Par1;if (num==2) return m_Par2;if (num==3) return m_Par3;if (num==4) return m_Par4;return 0;};
	void	SetParam(int index, float param);
	~CParamDim(){};
};


////////////////////////////////////////////////////////////////////////
// CVertex
class CVertex : public CMyObject  
{
	friend class CVerxStack;
protected:
	VERTEX	m_Vertex;		// Main values of vector
	int		m_Flag;			// bit: 0-draw it, 1-YValues and d2Y is allocated, 2-draw spike, 7-noise
	int		m_TimeStamp;
	vector<int>		m_MyClusts;	// Stack of clusters in which this vector is
	vector<float>   m_YValues;     //waveform stored here (there are NUM_CHANNELS of these stored in order, for
								   //a total of NUM_CHANNELS * NUM_SAMPLES values
	vector<float>	m_d2YValues;
	char	m_OrigClust;//original cluster
	char    m_KmeansClust;//k-means cluster
	char    m_InfoClust;//info gain cluster
	char    m_APClust;	//affinity propagation cluster
	char    m_KKClust;	//klustakwik cluster
	char    m_FLClust; //F.L.A.M.E. cluster
	unsigned char	timeMin;	// resolution 10 us, i.e. max 2.55 ms 
	unsigned char	timeMax;	// dtto
	static sNoiseParams noiseParams;	
public:
	int VXSize(){ return m_Vertex.size(); }
	int VYSize(){ return m_YValues.size(); }
	CVertex(CPaletteStack *MyPalette){m_PalStack=MyPalette; m_Flag=1; m_KmeansClust=m_InfoClust=m_OrigClust=m_APClust=m_KKClust=m_FLClust=0;};
	virtual ~CVertex(){ EmptyClust(); };
	void	AddClust(int toStore);
	void	AddPnt(float toStore);
	void	CalcAvgSpike(float *xa, int Freq, int Samples, CRect DrawWin, int whichChanel, int whichClust, float *Average,int *Numb, int whichDraw);
	void	CalcParam(CParamDim *param,float *xa,int Freq,int Samples, sSpike *spike);
	bool	CheckNoise(int iChannels=0,int iSamples=0,double dLowSat=-9.5,double dHighSat=9.5,double dFracSat=(1.0/3.0),double dMinSpikeAmp=-20.0);
	void	ClearHighDim(int Dim);
	void    DoDraw(CDC* pDC,CRect DrawWin, CFloatRect* whichValues,CPoint whichAxes,int PntSize,unsigned long color);
	void	Draw(CDC *pDC, CRect DrawWin, CFloatRect *whichValues, CPoint whichAxes, unsigned char *Clust, int PntSize);
	void	DrawOrig(CDC *pDC, CRect DrawWin, CFloatRect *whichValues, CPoint whichAxes, unsigned char *Clust, int PntSize);
	void	DrawAutoC(CDC *pDC, CRect DrawWin, CFloatRect *whichValues, CPoint whichAxes, unsigned char *Clust, int PntSize,char whichAuto);
	void	DrawTimeSpikes(CDC *pDC, float *xa, int freq, int samples, CRect spikeWin, CRect drawWin, int whichChanel, int whichSpDraw);
	int		DrawSpikes(CDC *pDC, float *xa, int Freq, int Samples, CRect DrawWin, int whichChanel, int whichClust, int whichDrawMode);
	int		DrawSpikes(CDC *pDC, float *xa, int Freq, int Samples, CRect DrawWin, int whichChanel);
	void	EmptyClust(){ if (*m_Vertex.begin()!=0) { m_MyClusts.clear(); SetValue(0,0);}; };
	void	ExportValues( FILE *file, int* pOverRideClust=0 );
	int		GetClust(int mIndex){ return m_MyClusts[mIndex]; };
	int		GetClust();
	int		GetFlag() { return m_Flag; };
	char	GetNoise() { if ((m_Flag & 0x80 ) || m_OrigClust<0) return 1; else return 0; }; // 0-signal, 1-noise
	char	GetOrigClust() { return m_OrigClust; };
	char    GetKmeansClust() { return m_KmeansClust; };
	void    SetKmeansClust(char c) { m_KmeansClust = c; }
	char    GetInfoClust() { return m_InfoClust; };
	void    SetInfoClust(char c) { m_InfoClust = c; };
	char    GetAPClust(){ return m_APClust; }
	void    SetAPClust(char c) { m_APClust = c; }
	char    GetKKClust(){ return m_KKClust; }
	void    SetKKClust(char c) { m_KKClust = c; }
	char    GetFLClust(){ return m_FLClust; }
	void    SetFLClust(char c) { m_FLClust = c; }
	int		GetTimeStamp() { return m_TimeStamp; };
	//gets number of dims in this vector, remember that dim 0 is the # of clusters this vector belongs to
	int		GetNumDims() { return m_Vertex.size(); };
	float	GetValue(int mIndex,MY_STACK::iterator whichVerx);
	float	GetValue(int mIndex){ return *(m_Vertex.begin()+mIndex);};
	float	GetYValues(int mIndex);
	prob_t  GetEnergy(int iSamples,int iChannel);
	int		IsInCluster(int num);
	void	RemHighDim(int num);
	void	SelectCluster(int modeCl,vector<bool>& vNoise);
	void	SetFlag(int Value) { m_Flag = Value; };
	void	SetDraw(int Num) { if (Num) SetFlag(GetFlag()|0x0001);else SetFlag(GetFlag()&0xFFFE); };
	void	SetDrawSpike(int Num) { if (Num) SetFlag(GetFlag()|0x0004);else SetFlag(GetFlag()&0xFFFB); };
	void	SetTimeMin(char timeMin_10us) { timeMin = timeMin_10us; };
	int     GetTimeMin() const { return timeMin; }
	void	SetTimeMax(char timeMax_10us) { timeMax = timeMax_10us; };
	int	    GetTimeMax() const { return timeMax; }
	void	SetNoise(char on_off);// { if (on_off != 0) m_Flag |= 0x80; else m_Flag &= 0xFFFFFF7F; };
	void	SetOrigCl(char OCl){ m_OrigClust = OCl;};
	void	SetTimeStamp(int TS) { m_TimeStamp = TS; };
	void	SetYValues(vector<float>& mNew, vector<float>& x, vector<float>& u, int channels, int samples);
	void	SetYValues(vector<short>& vBuffer, vector<float>& x, vector<float>& u, int channels, int samples);
	void	SetValue(int mIndex,float Value){ m_Vertex[mIndex]=Value; };
	void	Spline(float* x,float* y,int n,float* d2y,float* u);
	void	Spline(float* x,int n,float* u);
	float	Splope(float* xa,float* ya,float* d2y,float x,int lo_pt,int hi_pt);
	float	Splope(float* xa,float x,int lo_pt,int hi_pt);
	float	Splope(float* xa,float x,int lo_pt,int hi_pt,int channel,int Samples);
	float	Splint(float* xa,float* ya,float* d2y,float x,int lo_pt,int hi_pt);
	float	Splint(float* xa,float x,int lo_pt,int hi_pt);
	float	Splint(float* xa,float x,int lo_pt,int hi_pt,int channel,int Samples);
	void	Swap(int channels, int samples); // Swap +/- polarity of spikes

	void    ClearWaveformData();

	friend class CCluster;

	static int FREQ;
	static int SAMPLES;
	static int RESOLUTION;
	static int CHANNELS;
	static vector<float> VX;
};

double* getrank (int n, double data[]);

////////////////////////////////////////////////////////////////////////
// CVerxStack
class CVerxStack : public CMyObject
{
public:
	// stacks
	MY_STACK		m_VerxStack;	// main vectors of spikes
	MY_SPIKE_STACK	m_SpikesStack;	// data of used channel (extern. params, file pos. etc.)
	MY_FILE_PORTRAIT_STACK filePortraitStack;	// image of file
	MY_STR_STACK	m_AxesStack;	// axes' names
	CSyncStack		m_MainSyncStack;// sync signal
	CEEGStack		m_MainEEGStack;	// eeg signal
	MY_WCP_FILES_STACK wcpFilesStack;	// found external parameters files
	
	// constants
	int				SAMPLE_FREQ;		// Frequency of electrode signal
	int				EEG_SAMPLE_FREQ;	// Frequency of eeg signal
	int				NUM_CHANNELS;
	int				NUM_SAMPLES;
	char			BASE_DIMENSION;
	int				RESOLUTION;
	
	int				AreData;
	int				Dimension;
	int				whichDraw;			//original clustering = 1 or new clustering = 0
	char			InBpfExist;			//0 - nothing, 1 - arena, 2 - room, 3 - both
	char			LoadedAdd;			//0 - nothing, 1 - arena, 2 - room, 3 - both
	char			FileType;			//0 - none, 1 - BPF, 2 -UFF
	int				AmountDataFromFile;	

	vector<float>	m_x;		
	CString         m_FileName;
	CString         m_TmpName;
	CString			m_FileNameWithoutExt;	//this is a CString, you can add ext
	int				m_NumVerx;		// in memory
	int				m_NumVerxAll;	// in file (actual channel)
	int				m_NumOriginalCl;
	char			m_ElectrodeChannel;
	char			m_ElectrodeType;	//1 - single, 2 - stereo, 3 - tetrode
	VERTEX			m_MainMin,m_MainMax; // without noise
	VERTEX			m_MainNoisedMin,m_MainNoisedMax;
	VERTEX			m_MainRange,m_MainStdev,m_MainMean,m_MainEntropy;

	int				m_iPosCollapse;

	//////////////////////////////////////////////////////////////
	//      pca related params
	bool			m_bUsePCA; //use pca
	bool            m_bCombinePCA; //combine waveforms or not
	int             m_iNumPCsToStore; //# of pcs to keep
	int				m_iPCAStartDim; //starting pca dimension
	int				m_iPCAEndDim; //ending pca dimension
	//////////////////////////////////////////////////////////////
	int GetForStoreBPFSpikeIndex(){ return forStoreBPF_SpikeIndex; }

	bool            m_bUseEEG; //whether to load EEG data from BPF file -- default == true

	vector<string> GetEEGNames(){ return bpf.vEEGNames; }

	bool			m_bNormFloatV;//whether to normalize dimensions to be between 0-1 when computing KLD

	int				NumTetrodeChans(){ return bpf.NumberOfTetrodeChannels; }

protected:
	vector<short>	forStoreBPF_buffer;
	vector<float>	forStoreBPF_u;
	CFile			forStoreBPF_file;
	long			forStoreBPF_IndexFile;
	MY_STACK::iterator forStoreBPF_IndexVx;
	int				forStoreBPF_ArenaIndex;
	int				forStoreBPF_RoomIndex;
	int				forStoreBPF_SpikeIndex;
	
	CString			wcpFileNameMask;
	CString			wcpPath;
	sBPF			bpf;

	char			dFileOK;		// is <>0 if BPF is OK
	char			dLastType;		// last type of record in BPF
	long			dLastPos;		// position of last record;
	long			dFileLength;	// length of BPF;
	
	bool			m_FakeSpikes;	// true, if fake spikes are used

	bool            m_bShowXYSpikes; // true, if user wants to see x,y of spikes in
	                                 // views to help validate clusters
	bool            m_bLoadedXYSpikes;

	bool			m_bAutoCheckNoise; //whether to check vertices for noise automatically

public:
	CVertex			*forStoreBPF_NonClVx;	// vertex which isn't in memory
	sSpike			*forStoreBPF_Spike;

	// true, if user wants to see x,y of spikes in views to help validate clusters
	bool GetShowXYSpikes(){ return m_bShowXYSpikes; }
	void SetShowXYSpikes(bool b){ m_bShowXYSpikes=b; }
	void SetAutoCheckNoise(bool b){ m_bAutoCheckNoise=b; }
	bool GetAutoCheckNoise(){ return m_bAutoCheckNoise; }

	void ClearVertexWaveData();

	//adjust time stamps of spikes and vertices to be at time of max spike's peak. ALL 100% of the vertices must be loaded!!
	bool AdjustTimeStamps();

	//defaults to draw mode for lookup, otherwise
	//may be specified by user
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

	//get number of non-noise vertices in each cluster
	//uses whichDraw to determine which mode to get counts for
	void GetCounts(vector<int>& vCounts,int iClusts,int which)
	{
		vCounts = vector<int>(iClusts+1);
		MY_STACK::iterator IT = m_VerxStack.begin();
		for(;IT!=m_VerxStack.end();IT++)
		{
			CVertex* verx = (CVertex*)*IT;
			if(verx->GetNoise()) continue;
			vCounts[GetVClust(verx,which)]++;
		}
	}

	//get clust IDs, but skip noise vertices!!
	void GetClustIDs(vector<int>& vIDs,int which)
	{
		vIDs = vector<int>(NumNonNoiseVertices());
		int iV = 0;
		MY_STACK::iterator IT = m_VerxStack.begin();
		for(;IT!=m_VerxStack.end();IT++)
		{
			CVertex* verx = (CVertex*)*IT;
			if(verx->GetNoise()) continue;
			vIDs[iV++] = GetVClust(verx,which);
		}
	}

	inline int NumNoiseVertices()
	{
		MY_STACK::iterator Index;
		int iSz = 0;
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
		{
			CVertex* verx = (CVertex*) *Index;
			if(verx->GetNoise()) iSz++;
		}
		return iSz;
	}

	inline int NumNonNoiseVertices()
	{
		MY_STACK::iterator Index;
		int iSz = 0;
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
		{
			CVertex* verx = (CVertex*) *Index;
			if(!verx->GetNoise()) iSz++;
		}
		return iSz;
	}

	double** GetWaveForms(int& iRows,int& iCols)
	{
		iRows = m_VerxStack.size(); iCols = NUM_CHANNELS*NUM_SAMPLES;
		double** p = Allocate2DArray<double>(iRows,iCols);

		MY_STACK::iterator Index; int i = 0 , j = 0;
		for(Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++ , i++)
		{
			CVertex* Verx = (CVertex*) *Index;
			for(j=0;j<iCols;j++) p[i][j] = Verx->GetYValues(j);
		}
		return p;
	}

	//get 2D vector of vertex values NOT normalized between 0 - 1
	//if bExcludeNoise, no noise vertices will be stored and user
	//should know the indices will be offset accordingly
	template < class T > 
	T** GetPV( bool bExcludeNoise,int& iRows,int& iCols, bool SkipBadCols=false)
	{
		MY_STACK::iterator Index;
		int iSz = m_VerxStack.size();
		if(bExcludeNoise)
		{
			iSz = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise()) continue;
				iSz++;
			}
		}

		int iDims = GetAutoClusteringDimension();
		if(SkipBadCols)
			iDims -= 12; //skip Peak-V,Valley-V,Valley

		T** p = Allocate2DArray<T>(iSz,iDims);
		iRows = iSz; iCols = iDims;

		int iV=0;
		if(SkipBadCols)
		{	int iC = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(bExcludeNoise && verx->GetNoise()) continue;
				iC = 0;
				int iD = 0;
				for(iD=0;iD<=3;iD++,iC++)
				{	//+1 since index 0 is # of clusters vertex is in
					p[iV][iC] = verx->GetValue(iD+1);
				}
				for(iD=16;iD<=19;iD++,iC++)
				{	//+1 since index 0 is # of clusters vertex is in
					p[iV][iC] = verx->GetValue(iD+1);
				}
				for(;iD<iDims;iD++,iC++)
				{	//+1 since index 0 is # of clusters vertex is in
					p[iV][iC] = verx->GetValue(iD+1);
				}
				iV++;
			}
		}
		else
		{
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(bExcludeNoise && verx->GetNoise()) continue;
				int iD = 0;
				for(iD=0;iD<iDims;iD++)
				{	//+1 since index 0 is # of clusters vertex is in
					p[iV][iD] = verx->GetValue(iD+1);
				}
				iV++;
			}
		}

		return p;
	}

	//get 2D vector of vertex values as indices to bins in a distrib
	bool GetVertexFloatps( bool bExcludeNoise,int& iRows,int& iCols,int iBins, vector< vector<float>* >& vFloatps)
	{
		MY_STACK::iterator Index;
		int iSz = m_VerxStack.size();
		
		int iDims = GetAutoClusteringDimension();

		if(bExcludeNoise)
		{	vFloatps = vector< vector<float>* >(iSz);
			int iV = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{
				CVertex* verx = (CVertex*) *Index;
				vFloatps[iV] = &verx->m_Vertex;
			}
		}

		iRows = iSz; iCols = iDims;
		return true;
	}

	bool GetFloatV(int& iRows,int& iCols,vector<float>& vFloat, vector<float>& vRange, bool bAutoClust)
	{	
		MY_STACK::iterator Index;
		int iSz = NumNonNoiseVertices();
		
		//just gets # of dimensions without x,y,time locations
		int iDims = GetAutoClusteringDimension();

		if(bAutoClust)
		{	CalcMinMax();
			iCols = iDims - 12; //leave out valleys if auto-clustering
			iRows = iSz;       //since gives bad results
		}
		else
		{	CalcDimStats();
			iRows = iSz;
			iCols = iDims;
		}

		vFloat = vector<float>(iSz*iCols);

		Write2Log("m_bNormFloatV = %s",m_bNormFloatV?"true":"false");

		int i = 0;
		vRange = vector<float>(iDims+1);
		for(i=1;i<=iDims;i++) vRange[i]=GetMax(i)-GetMin(i);

		if(bAutoClust)
		{	int j = 0, iD = 0;
			if(m_bNormFloatV)
			{	for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
				{	CVertex* verx = (CVertex*) *Index;
					if(verx->GetNoise()) continue;
					for(iD=0;iD<iDims;iD++)
					{	//+1 since index 0 is # of clusters vertex is in					
						//skip Peak-V,Valley-V,Valley for auto-clustering
						if(iD>=4 && iD<=15) continue;
						vFloat[j++]=(verx->GetValue(iD+1) - GetMin(iD+1)) / vRange[iD+1];
					}
				}
			} 
			else 
			{	for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
				{	CVertex* verx = (CVertex*) *Index;
					if(verx->GetNoise()) continue;
					for(iD=0;iD<iDims;iD++)
					{	//+1 since index 0 is # of clusters vertex is in					
						//skip Peak-V,Valley-V,Valley for auto-clustering
						if(iD>=4 && iD<=15) continue;
						vFloat[j++]=verx->GetValue(iD+1);
					}
				}
			}
		}
		else if(m_bNormFloatV)	//do normalization of data
		{	int iV=0, j = 0, iD = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{	CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise())continue;
				for(iD=0;iD<iDims;iD++)					
					vFloat[j++]=(verx->GetValue(iD+1) - GetMin(iD+1)) / vRange[iD+1];//+1 since index 0 is # of clusters vertex is in
			}
		}
		else	//don't do normalization
		{	int iV=0, j = 0, iD = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{	CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise())continue;
				for(iD=0;iD<iDims;iD++)				
					vFloat[j++]=verx->GetValue(iD+1);//+1 since index 0 is # of clusters vertex is in
			}
		}
		return true;
	}

	bool GetFloat2D(int& iRows,int& iCols,A2D<float>& vFloat, vector<float>& vRange, bool bAutoClust)
	{	
		MY_STACK::iterator Index;
		int iSz = NumNonNoiseVertices();
		
		//just gets # of dimensions without x,y,time locations
		int iDims = GetAutoClusteringDimension();

		if(bAutoClust)
		{	CalcMinMax();
			iCols = iDims - 12; //leave out Valley-V, Valley, & Peak-V if auto-clustering
			iRows = iSz;       //since gives bad results
		}
		else
		{	CalcDimStats();
			iRows = iSz;
			iCols = iDims;
		}

		vFloat.Init(iSz,iCols);

		Write2Log("m_bNormFloatV = %s",m_bNormFloatV?"true":"false");

		int i = 0, Y = 0;
		vRange = vector<float>(iDims+1);
		for(i=1;i<=iDims;i++) vRange[i]=GetMax(i)-GetMin(i);

		if(bAutoClust)
		{	int j = 0, iD = 0;
			if(m_bNormFloatV)
			{	for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,Y++)
				{	CVertex* verx = (CVertex*) *Index;
					if(verx->GetNoise()) continue;
					j=0;
					for(iD=0;iD<iDims;iD++)
					{	//+1 since index 0 is # of clusters vertex is in					
						//skip Peak-V,Valley-V,Valley for auto-clustering
						if(iD>=4 && iD<=15) continue;
						vFloat[Y][j++]=(verx->GetValue(iD+1) - GetMin(iD+1)) / vRange[iD+1];
					}
				}
			} 
			else 
			{	for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,Y++)
				{	CVertex* verx = (CVertex*) *Index;
					if(verx->GetNoise()) continue;
					int j=0;
					for(iD=0;iD<iDims;iD++)
					{	//+1 since index 0 is # of clusters vertex is in					
						//skip Peak-V,Valley-V,Valley for auto-clustering
						if(iD>=4 && iD<=15) continue;
						vFloat[Y][j++]=verx->GetValue(iD+1);
					}
				}
			}
		}
		else if(m_bNormFloatV)	//do normalization of data
		{	int iV=0, j = 0, iD = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{	CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise())continue;
				j=0;
				for(iD=0;iD<iDims;iD++)					
					vFloat[iV][j++]=(verx->GetValue(iD+1) - GetMin(iD+1)) / vRange[iD+1];//+1 since index 0 is # of clusters vertex is in
			}
		}
		else	//don't do normalization
		{	int iV=0, j = 0, iD = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{	CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise())continue;
				j=0;
				for(iD=0;iD<iDims;iD++)				
					vFloat[Y][j++]=verx->GetValue(iD+1);//+1 since index 0 is # of clusters vertex is in
			}
		}
		return true;
	}


	//get 2D vector of vertex values as indices to bins in a distrib
	bool GetVBinIDs( bool bExcludeNoise,int& iRows,int& iCols,int iBins, vector< vector<int> >& vBinIDs)
	{
		MY_STACK::iterator Index;
		int iSz = m_VerxStack.size();
		
		int iDims = GetAutoClusteringDimension();

		if(bExcludeNoise)
		{	vBinIDs = vector< vector<int> >(iSz);
			int iV = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise()) continue;
				vBinIDs[iV] =  vector<int>(iDims);
			}
		}
		else
		{
			vBinIDs = vector< vector<int> >(iSz, vector<int>(iDims));
		}

		iRows = iSz; iCols = iDims;

		int iV=0, iB=0;
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
		{
			CVertex* verx = (CVertex*) *Index;
			if(bExcludeNoise && verx->GetNoise()) continue;
			int iD = 0;
			for(iD=0;iD<iDims;iD++)
			{	//+1 since index 0 is # of clusters vertex is in
				iB = (iBins)*(verx->GetValue(iD+1) - GetMin(iD+1)) / (GetMax(iD+1) - GetMin(iD+1));
				if(iB >= iBins) iB = iBins - 1;
				vBinIDs[iV][iD] = iB;
			}
		}
		return true;
	}

	//get 2D vector of vertex values as indices to bins in a distrib
	//if bExcludeNoise, no noise vertices will be stored and user
	//should know the indices will be offset accordingly
	template < class T > 
	T** GetVBinIDs( bool bExcludeNoise,int& iRows,int& iCols,int iBins,bool bRank=false)
	{
		MY_STACK::iterator Index;
		int iSz = m_VerxStack.size();
		if(bExcludeNoise)
		{
			iSz = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise()) continue;
				iSz++;
			}
		}

		int iDims = GetAutoClusteringDimension();

		T** p = Allocate2DArray<T>(iSz,iDims);
		iRows = iSz; iCols = iDims;

		int iV=0,iB=0,iNNSz=0;
		if(bRank)
		{
			iNNSz = NumNonNoiseVertices();
			double* ptmp = (double*) malloc(sizeof(double)*iNNSz);
			int iD;
			for(iD=0;iD<iDims;iD++)
			{	iV=0;
				for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
				{
					CVertex* verx = (CVertex*) *Index;
					if(verx->GetNoise()) continue;
					ptmp[iV] = verx->GetValue(iD+1);
					iV++;
				}
				double* prank = getrank(iNNSz,ptmp);
				for(iV=0;iV<iNNSz;iV++) 
				{	iB = iBins * prank[iV] / (iNNSz-1);
					if(iB >= iBins) iB = iBins - 1;
					p[iV][iD] = iB;
				}
				free(prank);
			}
			free(ptmp);
		}
		else
		{
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(bExcludeNoise && verx->GetNoise()) continue;
				int iD = 0;
				for(iD=0;iD<iDims;iD++)
				{	//+1 since index 0 is # of clusters vertex is in
					iB = (iBins)*(verx->GetValue(iD+1) - GetMin(iD+1)) / (GetMax(iD+1) - GetMin(iD+1));
					if(iB >= iBins) iB = iBins - 1;
					p[iV][iD] = iB;
				}
				iV++;
			}
		}

		return p;
	}


	//get 2D vector of vertex values NOT normalized between 0 - 1
	//if bExcludeNoise, no noise vertices will be stored and user
	//should know the indices will be offset accordingly
	template < class T > 
	void GetV( std::vector<std::vector<T> >& V,bool bExcludeNoise)
	{
		MY_STACK::iterator Index;
		int iSz = m_VerxStack.size();
		if(bExcludeNoise)
		{
			iSz = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise()) continue;
				iSz++;
			}
		}
		
		int iDims = GetAutoClusteringDimension();
		V = vector< vector<T> >(iSz, vector<T>(iDims));

		int iV=0;
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
		{
			CVertex* verx = (CVertex*) *Index;
			if(bExcludeNoise && verx->GetNoise()) continue;
			int iD = 0;
			for(iD=0;iD<iDims;iD++)
			{	//+1 since index 0 is # of clusters vertex is in
				V[iV][iD] = verx->GetValue(iD+1);
			}
			iV++;
		}
	}

	//get 2D vector of vertex values normalized between 0 - 1
	//if bExcludeNoise, no noise vertices will be stored and user
	//should know the indices will be offset accordingly
	template < class T >
	T** NormalizedPV(bool bExcludeNoise,int& iRows,int& iCols)
	{
		MY_STACK::iterator Index;
		int iSz = m_VerxStack.size();
		if(bExcludeNoise)
		{
			iSz = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise()) continue;
				iSz++;
			}
		}

		int iDims = GetAutoClusteringDimension();

		T** p = Allocate2DArray<T>(iSz,iDims);
		iRows = iSz; iCols = iDims;

		vector<T> vRange(iDims);
		int i;
		for(i=0;i<iDims;i++) vRange[i] = GetMax(i+1) - GetMin(i+1);
		int iV=0;
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
		{
			CVertex* verx = (CVertex*) *Index;
			if(bExcludeNoise && verx->GetNoise()) continue;
			int iD = 0;
			for(iD=0;iD<iDims;iD++)
			{	//+1 since index 0 is # of clusters vertex is in
				p[iV][iD] = (verx->GetValue(iD+1) - GetMin(iD+1)) /  vRange[iD];;
			}
			iV++;
		}
		return p;
	}

	//get 2D vector of vertex values normalized between 0 - 1
	//if bExcludeNoise, no noise vertices will be stored and user
	//should know the indices will be offset accordingly
	template < class T >
	void NormalizedV(std::vector< std::vector<T> >& vN, bool bExcludeNoise)
	{
		MY_STACK::iterator Index;
		int iSz = m_VerxStack.size();
		if(bExcludeNoise)
		{
			iSz = 0;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
			{
				CVertex* verx = (CVertex*) *Index;
				if(verx->GetNoise()) continue;
				iSz++;
			}
		}

		int iDims = GetAutoClusteringDimension();
		vN = vector< vector<T> >(iSz, vector<T>(iDims));
		int i;

		vector<T> vRange(iDims);
		for(i=0;i<iDims;i++) vRange[i] = GetMax(i+1) - GetMin(i+1);
		int iV=0;
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
		{
			CVertex* verx = (CVertex*) *Index;
			if(bExcludeNoise && verx->GetNoise()) continue;
			int iD = 0;
			for(iD=0;iD<iDims;iD++)
			{	//+1 since index 0 is # of clusters vertex is in
				vN[iV][iD] = (verx->GetValue(iD+1) - GetMin(iD+1)) /  vRange[iD];;
			}
			iV++;
		}
	}

	//idx is index, look up vertex cluster, assuming it is in memory
	inline char LookupVClust(int idx,int which=-1)
	{
		int iV = 0;
		MY_SPIKE_STACK::iterator indexSp = m_SpikesStack.begin() + idx;
		if(indexSp->type == 1)
		{
			CVertex* verx = (CVertex*)*(m_VerxStack.begin() + indexSp->offset);
			return GetVClust(verx,which);
		}
		return indexSp->cluster;
	}

// Methods
protected:
	void	CalcAfterLoad();
	long	GetBPFDataPtr(CFile *file);
	int		GetBPFHeader(char *buffer, int nRead, char *errChar);
	int		GetBPFHeader_SetupInformation(char *buffer, int nRead, char *errChar);
	void    SetupEEGChanNames(char* formatInf);
	int		LoadBPF(CFile *file, char *buffer, int nRead, char *errChar, long *errOffset, BatchRec* pQB = 0);
	int		LoadUFF(CFile *file, char *buffer, int nRead);
	int		StoreUFF(CFile *file);
	
	void	AddFakeSpike( int TimeStamp );

	//for popping/pushing vertices elements
	vector< vector<float> > m_vPopped;
	//for popping/pushing dim string names
	vector<CString*> m_AxPopped;

	int NumTopDims();

	//pca related data structures
	PCA m_vPCA[4]; // shouldn't be hard-coded in case # of channels increases later...will have to fix later if that happens...
	vector<double> m_vIsolationDistData;//data vector used for standard harris version of isolation dist & L-Ratio

	int PCASampleSize();

public:	
	vector<double>& ICED(){ return m_vIsolationDistData; }
	void ClearICED(){ m_vIsolationDistData.clear(); }
	CVerxStack();
	CVerxStack(CPaletteStack *MyPalette){ m_PalStack=MyPalette; RESOLUTION = 16; whichDraw = 1;};
	~CVerxStack(){ SetEmpty(); };
	void	AddVrx(CMyObject *toStore);
	void	AddAxes(CString *toStore){ m_AxesStack.push_back(toStore); };
	void	CalcDimStats();	//mean, stdev, min, max, range, entropy
	void	CalcMinMax();
	void	CalcMinMaxLast();
	void	CalcOneSpike(CVertex *verx);
	void    AddVertexTimeStamps();
	int		MaxIndex(CVertex* verx,int iChannel,int iINC);
	void    SplineUpSample(CVertex* verx,float* vout,int iChannel,int iINC);

	//pop dims off vertices onto temp stack
	void    PopDims(int iNumDims);
	//push dims off temp stack back onto vertices
	void    PushDims(int iNumDims);
	
	//# of pca dimensions - valid after loading
	int    NumPCADims();
	//does pca of waveforms for each vertex and stores results
	void    DoVertexPCA(bool bForIsoDLRat=false); 
	//add axes for display
	void    AddPCAAxes();
	//removes pca results from vertices in memory
	void    RemoveVertexPCA();
	//remove axes
	void    RemovePCAAxes();
	//get single vertex data vec used for computing pca projections
	void    GetPCADataVec(CVertex* p, vector<double>& v);
	//get single vertex data vec used for computing pca projections
	void    GetPCADataVec(CVertex* p, vector<double>& v, int iChannel);
	//appends pca projections for single vertex (used for vertices that
	//were not in memory when original pca was computed)
	void    AppendPCAProjections(CVertex* p);
	//remove a dimension from all vertices and it's associated text axis
	//this should ONLY be done when corresponding axes are removed from boundary
	//projections -- only done as a result of user request in EditSpikes.cpp
	void    RemoveADim(int iDim);

	//adds x,y location of vertices to vertex point list
	bool    AddVertexLocations();
	void	CalcViewVx(int FirstEv,int LastEv,int PercEv);
	void	CalcViewSpike(int FirstEv,int LastEv,int PercEv);
	void	CalcViewSpike(int FirstEv,int LastEv,int PercEv,int Clust);
	void	CalcViewSpike(int FirstEv,int LastEv,int PercEv, unsigned char *clView, char numb);
	int		CheckNoise(double dLowSat=-9.5,double dHighSat=9.5,double dFracSat=(1.0/3.0),double dMinSpikeAmp=-20.0);
	void	CheckWcpFiles();
	void	ClearExternalParamData();
	void	ClearHighDim();
	void	CreateHeaderForExport(FILE *file,CString& oStrExtra);//oStrExtra has optional extra header information
	void	Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust); 
	void	Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize); 
	void	DrawOrig(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust); 
	void	DrawOrig(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize); 
	void	DrawAutoC(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,char whichAutoC); 
	void	DrawAutoC(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize,char whichAutoC); 
	void	DrawTimeSpikes(CDC *pDC, CRect drawWin, int startTime, int timeWindow, int TSFirst, char params, unsigned char *clView, CFile *file);
	void	DrawTimeSpikesAutoC(CDC *pDC, CRect drawWin, int startTime, int timeWindow, int TSFirst, char params, unsigned char *clView, CFile *file);
	void	DrawSpikes(CDC *pDC,CRect DrawWin,int whichChanel,int whichClust);
	void	DrawSpikes(CDC *pDC,CRect DrawWin,int whichChanel,int Start,int Stop);
	int		DrawSpikes(CDC *pDC,CRect DrawWin,int whichChanel,int Start,int HowMany,int *Last);
	void	FinishStoreBPF();
	int		GetBaseDimension(){ return BASE_DIMENSION; };
	int		GetClust(int NoOfVerx);
	char	GetDFileOK() { return dFileOK; };		// for repair BPF
	char	GedDLastType() { return dLastType; };	// for repair BPF
	long	GetDLastPos() { return dLastPos; };		// for repair BPF
	long	GetDFileLength() { return dFileLength; };// for repair BPF
	int		GetDimension(){ return Dimension; };
	//# of dimensions used for clustering, doesn't use x,y position for auto-clustering
	//always at least -1 because time is last dimension, which isn't used for clustering
	int     GetAutoClusteringDimension(){ return m_bLoadedXYSpikes ? Dimension-2-1 : Dimension-1; }
	bool	GetFakeSpikes() { return m_FakeSpikes; };
	char	GetFileType(){ return FileType; };
	int		GetNumVerx(){ return m_NumVerx; };
	float	GetMin(int Index);
	float	GetMax(int Index);
	CString* GetAxesName(int mIndex){ if (mIndex>0 && mIndex<=Dimension) return *(m_AxesStack.begin()+mIndex-1);else return *(m_AxesStack.begin()); };
	int		IsEmpty(){ if (AreData) return 0; else return 1; };
	int		LoadBPFElde(CVertex *pnt, CFile *file, int offset);
	int		LoadVerx( char *errChar, long *errOffset, BatchRec* pQB);
	bool	ImportSpikes(char* path);
	void	PasteFileParamAsZeros( );
	void	PasteFileParameter( sWcpFiles *paramToAdd );
	int		PrepareStoreBPF();
	void	RemoveExternalParam(int iNum);
	int		StoreBPFElde();
	int		StoreBPF(char *State,bool bTSAdjust=false);
	int		StoreBPFAddition();
	int		StoreClMarks();
	void	SetEmpty();
	void	SetPalette(CPaletteStack *pal);
	void	SwapPolarity(); // Swap +/- polarity of spikes
	bool	ExportSpikes(char* fname,bool bUpsample);
	bool	ExportHQPC1Energy(char* fname);

	friend class CCluster;
	friend class TopDimPopPush;
};

class TopDimPopPush
{
	CVerxStack* p_;
public:
	TopDimPopPush(CVerxStack* pData)
		:p_(pData)
	{
		p_->PopDims(p_->NumTopDims());
	}
	~TopDimPopPush()
	{
		p_->PushDims(p_->NumTopDims());
	}
};


#endif // !defined(AFX_VERTEX_H__80EE18DE_3F8F_4FBA_8BA5_36D095AD0655__INCLUDED_)
