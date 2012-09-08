// $Id: Vertex.cpp,v 1.28 2010/01/14 18:36:17 samn Exp $ 
// Vertex.cpp: implementation of the CVertex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wclust.h"
#include "Vertex.h"
#include "SyncStack.h"
#include "SelectTT.h"
#include "uff.h"
#include "pca.h"
#include "FileUtils.h"
#include "MyVers.h"
#include "simple_haar.h"
#include "Log.h"
#include "pca2.hpp"
#include "WCMath.h"
#include "ScopedTimer.h"
#include "Hist.h"
#include "StringUtils.h"
#include <algorithm>
#include <math.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <errno.h> 
#include <fstream>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
void CParamDim::SetParam(int index, float param)
{
	switch (index)
	{
	case 1:
		m_Par1 = param;
		break;
	case 2:
		m_Par2 = param;
		break;
	case 3:
		m_Par3 = param;
		break;
	case 4:
		m_Par4 = param;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
int CVertex::FREQ = 32000;
int CVertex::SAMPLES = 64;
int CVertex::RESOLUTION = 16;
int CVertex::CHANNELS = 4;
vector<float> CVertex::VX;
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
void CVertex::ExportValues(FILE *file,int* pOverRideCluster)
{
	if(pOverRideCluster)
		fprintf(file,"%d\t%d\t", GetTimeStamp(), pOverRideCluster[0] );
	else
		fprintf(file,"%d\t%d\t", GetTimeStamp(), GetNoise() ? 0 : GetClust() );
	
	//starts at m_Vertex.begin() + 1 because the first value is the # of clusters
	//that this CVertex belongs to (see IsInCluster)
	VERTEX::iterator index;
	for (index = m_Vertex.begin() + 1; index != m_Vertex.end(); index++)
	{
		fprintf(file,"%f\t",*index);
	}
	fprintf(file,"\n");
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::AddPnt(float toStore)
{
	m_Vertex.push_back(toStore);
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::ClearWaveformData()
{
	m_d2YValues.clear();
	m_YValues.clear();
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::AddClust(int toStore)
{
	if(false)
	{	
		m_MyClusts.push_back(toStore);	
		SetValue(0,GetValue(0)+1);
	}
	else 
	{ 	
		int HowMany = GetValue(0);
	
		vector<int> newMyClusts(HowMany+1);

		for (int i=0; i<HowMany; i++)
			newMyClusts[i] = m_MyClusts[i];
		newMyClusts[HowMany] = toStore;

		m_MyClusts = newMyClusts;

		SetValue(0,HowMany+1);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::SelectCluster(int modeCl,vector<bool>& vNoise)
{
	int NumbCl = GetValue(0);	
	
	if (modeCl == 0) // assignment random
	{
		if ( NumbCl == 2 )
		{
			m_MyClusts[1] &= 0xFFFF;
		}
		else if ( NumbCl > 2 )
		{
			int rnd = (NumbCl-1) * rand()/SHRT_MAX;
			if (rnd == NumbCl)
				rnd -=1;
			int newCl = GetClust(rnd+1) & 0xFFFF;
			EmptyClust();
			AddClust(0);
			AddClust(newCl);
			
			SetNoise(vNoise[newCl]);
		}
	}
	else if (modeCl == 1) // exclude
	{
		if (NumbCl == 2)
		{
			m_MyClusts[1] &= 0xFFFF;
		}
		else if (NumbCl > 2)
		{
			EmptyClust();
			AddClust(0);
			SetNoise(0);
		}
	}
	else if (modeCl == 2) // weighted random
	{
		if (NumbCl == 2)
		{
			m_MyClusts[1] &= 0xFFFF;
		}
		else if ( NumbCl > 2 )
		{
			int weight = 0;
			for (int i=1; i<NumbCl; i++)
			{
				weight += (GetClust(i) & 0xFFFF0000) / 0xFFFF;
			}
			int rnd = (weight+1) * rand()/SHRT_MAX;
			
			int state=0;
			weight=0;
			for (i=1; i<NumbCl && state==0; i++)
			{
				weight += (GetClust(i) & 0xFFFF0000) / 0xFFFF;
				if (weight >= rnd)
					state=i;
			}
				
			int newCl = GetClust(state) & 0xFFFF;
			
			EmptyClust();
			AddClust(0);
			AddClust(newCl);

			SetNoise(vNoise[newCl]);
		}
	}
}

void CVertex::SetNoise(char TurnOn)
{
	if (TurnOn)
	{
		m_Flag |= 0x80;
		m_KmeansClust = m_InfoClust = m_APClust = m_KKClust = m_FLClust = -1;
	}
	else
	{
		m_Flag &= 0xFFFFFF7F; 
		if( m_KmeansClust < 0 ) m_KmeansClust = 0;
		if( m_InfoClust < 0 ) m_InfoClust = 0;
		if( m_APClust < 0 ) m_APClust = 0;
		if( m_KKClust < 0 ) m_KKClust = 0;
		if( m_FLClust < 0 ) m_FLClust = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::SetYValues(vector<short>& vBuffer, vector<float>& x, vector<float>& u, int channels, int samples)
{
	int iSz = vBuffer.size();

	//ensure proper size for buffers
	m_YValues.resize(iSz);
	m_d2YValues.resize(iSz);
	
	// Convert to real value
	for (int m_l=0;m_l<iSz;m_l++)
		m_YValues[m_l] = (float) (10 * vBuffer[m_l])/SHRT_MAX;

	//do interpolation
	for (int i=0; i<channels; i++)
		Spline(&x[0],&m_YValues[samples*i],samples,&m_d2YValues[samples*i],&u[0]);

	m_Flag |= 0x0002;
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::SetYValues(vector<float>& mNew, vector<float>& x,vector<float>& u, int channels, int samples)
{
	m_YValues.resize(mNew.size());
	copy(mNew.begin(),mNew.end(),m_YValues.begin());
	m_d2YValues.resize(channels*samples);
	for (int i=0; i<channels; i++)
		Spline(&x[0],&m_YValues[samples*i],samples,&m_d2YValues[samples*i],&u[0]);

	m_Flag |= 0x0002;
}

/////////////////////////////////////////////////////////////////////////////
float CVertex::GetYValues(int Index)
{ 
	if ( Index>=0 && Index<m_YValues.size() && m_Flag&0x0002 ) 
		return m_YValues[Index]; 
	else 
		return 0; 
}

/////////////////////////////////////////////////////////////////////////////
int CVertex::IsInCluster(int num)
{
	int i=0;
	while ( i < *m_Vertex.begin() )
	{
		if ( m_MyClusts[i] == num )
			return 1;
		i++;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CVertex::GetClust()
{
//	int Index;
//	Index = *m_Vertex.begin();
//	Index--;
//	return *(m_MyClusts +  Index); 
	return GetClust(GetValue(0)-1);
};

void CVertex::DoDraw(CDC* pDC,CRect DrawWin, CFloatRect* whichValues,CPoint whichAxes,int PntSize,unsigned long color)
{
	float fx = m_Vertex[whichAxes.x];
	float fy = m_Vertex[whichAxes.y];
	if (whichValues->IsInRect(fx,fy))
	{
		int x=DrawWin.left+(fx-whichValues->GetLeft())*(DrawWin.right-DrawWin.left)/whichValues->GetSizeX();
		int y=DrawWin.bottom+(fy-whichValues->GetBottom())*(DrawWin.top-DrawWin.bottom)/whichValues->GetSizeY();

#ifdef _DEBUG
		if(whichValues->GetSizeX()==0.0 || whichValues->GetSizeY()==0.0)
		{
			int moo=0;
		}
#endif

		if (PntSize==1)
		{
			pDC->SetPixelV(x,y,color); 
		}			
		else if (PntSize==2)
		{
			pDC->SetPixelV(x,y,color);
			pDC->SetPixelV(x+1,y+1,color); 
			pDC->SetPixelV(x+1,y,color); 
			pDC->SetPixelV(x,y+1,color); 
		}
		else if (PntSize==3)
		{
			CPen newPen(PS_SOLID,1,color);
			CPen *pOldPen=pDC->SelectObject(&newPen);
			CBrush newBrush(color);
			CBrush *pOldBrush=pDC->SelectObject(&newBrush);
			pDC->Rectangle(x-1,y-1,x+2,y+2);
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
		}
		else if (PntSize==4)
		{
			CPen newPen(PS_SOLID,1,color);
			CPen *pOldPen=pDC->SelectObject(&newPen);
			CBrush newBrush(color);
			CBrush *pOldBrush=pDC->SelectObject(&newBrush);
			pDC->Ellipse(x-2,y-2,x+2,y+2);
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
		}
		else if (PntSize==5)
		{
			CPen newPen(PS_SOLID,1,color);
			CPen *pOldPen=pDC->SelectObject(&newPen);
			CBrush newBrush(color);
			CBrush *pOldBrush=pDC->SelectObject(&newBrush);
			pDC->Ellipse(x-2,y-2,x+3,y+3);
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
		}
	} 
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize)
{
	//if(GetNoise()) MessageBox(0,"draw noise","draw noise",0);
	if ( m_Flag & 0x0001 ) 
	{
		if(GetNoise() && !(Clust[255] & 1)) return;

		if(GetNoise() && *m_Vertex.begin()==0)
		{	//not hiding given cluster
			if( (*(Clust+255)) & 1)
			{
				unsigned long color=RGB(204,204,204);
				DoDraw(pDC,DrawWin,whichValues,whichAxes,PntSize,color);
			}
		}
		else
		{	//not hiding given cluster
			int Index = *m_Vertex.begin();
			Index--;
			if ( (*(Clust+(int) m_MyClusts[Index])) & 1 )
			{
				unsigned long color=m_PalStack->GetSColor( m_MyClusts[Index] );
				if(GetNoise()) color = RGB(204,204,204);
				DoDraw(pDC,DrawWin,whichValues,whichAxes,PntSize,color);
			}
		}
	}	
}


/////////////////////////////////////////////////////////////////////////////
void CVertex::DrawOrig(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize)
{
	unsigned long color;

	int ShowIt=0;

	if(GetNoise()) //the cluster ID will be -1
	{
		if(!(Clust[255] & 1)) //added
			return;
		ShowIt=1;
	}

#ifdef _DEBUG
	if(m_OrigClust < 0)
		int moo = 9;
#endif

	if ( m_Flag & 0x0001 ) 
	{
		if(m_OrigClust >= 0 && m_OrigClust < 255 && Clust[m_OrigClust] & 1)
		{
			ShowIt=1;
			color=m_PalStack->GetSColor( (int) (m_OrigClust) );
			if ((int)m_OrigClust == 255)
				MessageBeep(0);
		}
	}

	if (m_OrigClust < 0 || GetNoise())
		color = RGB(204,204,204);

	if ( ShowIt )
	{
		DoDraw(pDC,DrawWin,whichValues,whichAxes,PntSize,color);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::DrawAutoC(CDC* pDC,CRect DrawWin,CFloatRect* whichValues,CPoint whichAxes,unsigned char* Clust,int PntSize,char whichAuto)
{
	if ( m_Flag & 0x0001 ) 
	{
		if(GetNoise())
		{
			if( (*(Clust+255)) & 1)
			{
				unsigned long color = RGB(204,204,204);
				DoDraw(pDC,DrawWin,whichValues,whichAxes,PntSize,color);
			}
		}
		else
		{
			char c;
			if(whichAuto == CLUST_KM)
				c = m_KmeansClust;
			else if(whichAuto == CLUST_INFO)
				c = m_InfoClust;
			else if(whichAuto == CLUST_AP)
				c = m_APClust;
			else if(whichAuto == CLUST_KK)
				c = m_KKClust;
			else
				c = m_FLClust;
			
			if ( (*(Clust+(int) c)) & 1 )
			{
				unsigned long color=m_PalStack->GetSColor( (int) c );
				DoDraw(pDC,DrawWin,whichValues,whichAxes,PntSize,color);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::CalcAvgSpike(float *xa,int Freq,int Samples,CRect DrawWin,int whichChanel,int whichClust,float *Average,int *Numb,int whichDraw)
{
	int CountIt=0;

	if(whichDraw!=CLUST_USER && whichDraw!=CLUST_ORIG && GetNoise())
	{
		if(whichClust>=0) return;
		else CountIt=1;
	}

	int Index = 0;

	if(!CountIt) switch(whichDraw)
	{
	case CLUST_USER:
		Index = *(m_Vertex.begin())-1;
		if ( m_MyClusts[Index]==whichClust ) CountIt=1;
		break;
	case CLUST_ORIG:
		if(m_OrigClust==whichClust) CountIt=1;
		break;
	case CLUST_KM:
		if(whichClust==0 || m_KmeansClust==whichClust) CountIt=1;
		break;
	case CLUST_INFO:
		if(whichClust==0 || m_InfoClust==whichClust) CountIt=1;
		break;
	case CLUST_AP:
		if(whichClust==0 || m_APClust==whichClust) CountIt=1;
		break;
	case CLUST_KK:
		if(whichClust==0 || m_KKClust==whichClust) CountIt=1;
		break;
	case CLUST_FL:
		if(whichClust==0 || m_FLClust==whichClust) CountIt=1;
		break;
	}


	if ( CountIt )
	{
		* Numb += 1;
		float fy;
		float t=0;
		float tStep=(float) (Samples-1)/DrawWin.Width();


		for (int i=0; i<DrawWin.Width(); i++)
		{
			fy = Splint(xa, t/(Freq) ,(int)floor(t),(int)floor(t)+1,whichChanel,Samples);
			*(Average+i) += fy;
			t += tStep;
		}
	}
	

}

/////////////////////////////////////////////////////////////////////////////
void CVertex::DrawTimeSpikes(CDC *pDC, float *xa, int freq, int samples, CRect spikeWin, CRect drawWin, int whichChanel, int whichSpDraw)
{
	unsigned long color = 0;
	int Index = *m_Vertex.begin();
	
	if(GetNoise())
	{
		color = RGB(204,204,204);
	}
	else switch(whichSpDraw)
	{
	case CLUST_ORIG:
	case CLUST_USER:
		color = ( whichSpDraw == 0 ) ? m_PalStack->GetSColor( (int) m_MyClusts[Index-1] ) : m_PalStack->GetSColor( GetOrigClust() );
		break;
	case CLUST_AP:
		color = m_PalStack->GetSColor( m_APClust );
		break;
	case CLUST_KM:
		color = m_PalStack->GetSColor( m_KmeansClust );
		break;
	case CLUST_INFO:
		color = m_PalStack->GetSColor( m_InfoClust );
		break;
	case CLUST_KK:
		color = m_PalStack->GetSColor( m_KKClust );
		break;
	case CLUST_FL:
		color = m_PalStack->GetSColor( m_FLClust );
		break;
	}

	CPen newPen(PS_SOLID,1,color);
	CPen *pOldPen=pDC->SelectObject(&newPen);

	int y, yPrev;
	float fy;
	float t=0;
	float tStep=(float) (samples - 1) / spikeWin.Width();

	fy = Splint(xa, t/(freq), (int)floor(t), (int)floor(t)+1, whichChanel, samples);
	yPrev = spikeWin.bottom - fy*spikeWin.Height()/20 - spikeWin.Height()/2;
	if ( spikeWin.right > drawWin.right )
	{
		spikeWin.right = drawWin.right;
	}
//	if ( spikeWin.left < drawWin.left )
//	{
//		t = tStep * ( drawWin.left - spikeWin.left );		
//		spikeWin.left = drawWin.left;
//	}

	for (int i = spikeWin.left + 1; i <= spikeWin.right; i++)
	{
		fy = Splint(xa, t/(freq), (int)floor(t), (int)floor(t)+1, whichChanel, samples);
		y = spikeWin.bottom - fy*spikeWin.Height()/20 - spikeWin.Height()/2;
		pDC->MoveTo(i-1,yPrev);
		pDC->LineTo(i,y);
		yPrev=y;

		t += tStep;
	}
	pDC->SelectObject(pOldPen);
};

/////////////////////////////////////////////////////////////////////////////
void CVertex::DrawSpikes(CDC *pDC, float *xa, int Freq, int Samples, CRect DrawWin,int whichChanel,int whichClust,int whichDrawMode)
{
	int ShowIt=0;
	int Index = *m_Vertex.begin();
	unsigned long color;

	if(GetNoise())// && whichDrawMode!=CLUST_USER && whichDrawMode!=CLUST_ORIG)
	{
		if(whichClust>=0) return;
		else{ ShowIt=1; color = RGB(204,204,204); }
	}

	if(!ShowIt) switch(whichDrawMode)
	{
	case CLUST_USER:
		if ( true || (m_Flag & 0x0004) ) 
		{
			Index--;
			if ( Index>=0 && m_MyClusts[Index]==whichClust )
			{
				ShowIt=1;
				color=m_PalStack->GetSColor( (int) m_MyClusts[Index] );
			}
			if(GetNoise()) 
				color = RGB(204,204,204);
		}
		break;
	case CLUST_ORIG:
		if(whichClust == m_OrigClust)
		{
			ShowIt=1;
			color=m_PalStack->GetSColor( (int) whichClust );
			if(GetNoise()) color = RGB(204,204,204);
		}
		break;
	case CLUST_KM:
		if(whichClust==m_KmeansClust)
		{
			ShowIt=1;
			color=m_PalStack->GetSColor( (int) whichClust );
		}
		break;
	case CLUST_INFO:
		if(whichClust==m_InfoClust)
		{
			ShowIt=1;
			color=m_PalStack->GetSColor( (int) whichClust );
		}
		break;
	case CLUST_AP:
		if(whichClust==m_APClust)
		{
			ShowIt=1;
			color=m_PalStack->GetSColor( (int) whichClust );
		}
		break;
	case CLUST_KK:
		if(whichClust==m_KKClust)
		{
			ShowIt=1;
			color=m_PalStack->GetSColor( (int) whichClust );
		}
		break;
	case CLUST_FL:
		if(whichClust==m_FLClust)
		{
			ShowIt=1;
			color=m_PalStack->GetSColor( (int) whichClust );
		}
		break;
	}

	if ( ShowIt )
	{
		CPen newPen(PS_SOLID,1,color);
		CPen *pOldPen=pDC->SelectObject(&newPen);
		int y, yPrev;
		float fy;
		float t=0;
		float tStep=(float) (Samples - 1)/DrawWin.Width();


		fy = Splint(xa, t/(Freq) ,(int)floor(t),(int)floor(t)+1,whichChanel,Samples);
		yPrev = DrawWin.bottom - fy*DrawWin.Height()/20 - DrawWin.Height()/2;
		for (int i=DrawWin.left+1;i<DrawWin.right;i++)
		{
			fy = Splint(xa, t/(Freq), (int)floor(t), (int)floor(t)+1, whichChanel, Samples);
			y = DrawWin.bottom - fy*DrawWin.Height()/20 - DrawWin.Height()/2;
			pDC->MoveTo(i-1,yPrev);
			pDC->LineTo(i,y);
			yPrev=y;

			t += tStep;
		}
		pDC->SelectObject(pOldPen);
	}
}

/////////////////////////////////////////////////////////////////////////////
int CVertex::DrawSpikes(CDC *pDC, float *xa, int Freq, int Samples, CRect DrawWin, int whichChanel)
{
	if(GetNoise()) return 0;

	if (m_Flag & 0x0004)
	{
		unsigned long color;

		int Index = *m_Vertex.begin();
		
		if(Index - 1 < 0) return 0;

		color=m_PalStack->GetSColor( m_MyClusts[Index-1] );

		CPen newPen(PS_SOLID,1,color);
		CPen *pOldPen=pDC->SelectObject(&newPen);
		int y, yPrev;
		float fy;
		float t=0;
		float tStep=(float) (Samples - 1)/DrawWin.Width();

		fy = Splint(xa, t/(Freq), (int)floor(t), (int)floor(t)+1, whichChanel, Samples);
		yPrev = DrawWin.bottom - fy*DrawWin.Height()/20 - DrawWin.Height()/2;
		for (int i=DrawWin.left+1;i<=DrawWin.right;i++)
		{
			fy = Splint(xa, t/(Freq), (int)floor(t), (int)floor(t)+1, whichChanel, Samples);
			y = DrawWin.bottom - fy*DrawWin.Height()/20 - DrawWin.Height()/2;
			pDC->MoveTo(i-1,yPrev);
			pDC->LineTo(i,y);
			yPrev=y;

			t += tStep;
		}
		pDC->SelectObject(pOldPen);
		return 1;
	}
	else return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::ClearHighDim(int Dim)
{
	while (m_Vertex.size() > Dim+1)
			m_Vertex.pop_back();
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::RemHighDim(int num)
{
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::CalcParam(CParamDim *param,float *xa,int Freq,int Samples, sSpike *spike)
{
	if ( param->GetType() == 2 )
	{	// Y-value in time = par1, chanel = par2
		float t = param->m_Par1 * Freq;
		float fy = Splint(xa, param->m_Par1, (int)floor(t), (int)floor(t)+1, (int) param->m_Par2, Samples);
		AddPnt(fy);
	}	
	else if ( param->GetType() == 3 )
	{	// dY-value in time = par1, chanel = par2
		float t = param->m_Par1 * Freq;
		float fy = Splope(xa, param->m_Par1, (int)floor(t), (int)floor(t)+1, (int) param->m_Par2, Samples)/1000;
		AddPnt(fy);
	}
	else if ( param->GetType() == 4 )
	{	// Energy-value in t1 = par1, t2 = par2, chanel = par2
		int t1 = (int) floor (param->m_Par1*Freq);
		int t2 = (int) ceil (param->m_Par2*Freq);
		if (t2 > (Samples-1)) 
			t2 = Samples-1;
		int shift = Samples * param->m_Par3, iVals = 0;
		float fy = 0.0f;
		for (int i=t1; i<t2; i++, iVals++)
		{
			fy += m_YValues[shift+i] * m_YValues[shift+i];
		}
		fy = sqrt(fy);
		
		if(iVals) fy /= iVals;

		AddPnt(fy);
	}
	else if ( param->GetType() == 5 )
	{ // WCP external parameter: par1 = index in names, par2 = channel, par3 = index in spike->paramFile
		float fy = (float) *( spike->fileParams.begin() + (int) param->GetParam( 3 ) );
		AddPnt(fy);
	}
	else if ( param->GetType() == 6 )
	{
		int isz = param->m_Par2 , i = 0;
		int iS = 0, iSamps = CeilPowOf2(m_YValues.size());
		vector<double> vals(iSamps);
		copy(m_YValues.begin(),m_YValues.end(),vals.begin());
		int iDimsAdded = 0;
		simple_haar haar;
		haar.wavelet_calc(vals);
		int iA,iC;
		for(iA=0;iA<haar.coef.size() && iDimsAdded < isz;iA++)
		{	for(iC=0;iC<haar.coef[iA].size() && iDimsAdded < isz;iC++)
			{	iDimsAdded++;
				AddPnt(haar.coef[iA][iC]);
			}
		}		
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CVertex::CheckNoise(int iChannels,int iSamples,double dLowSat,double dHighSat,double dFracSat,double dMinSpikeAmp)
{
// NOISE DISABLED
//	if ( timeMax > 100 )
//		SetNoise(1);
//	else
//		if ( timeMin > 100 )
//			SetNoise(1);
//		else
//			SetNoise(0);

//	SetNoise(0);
	SetNoise(0);

	bool bIsNoise = false;
	static int iSM = 0;
	int iC = 0 , iSaturate = 0;
	bool bSpikeGreaterMinAmp = false;
	for(;iC<iChannels;iC++)	//do saturation check
	{
		int iSat = 0;
		float* y = &m_YValues[iSamples*iC];
		int i=0;
		for(;i<iSamples;i++)
		{	//max val has abs val of 10, allow slightly looser measure
			if(y[i] > dHighSat || y[i] < dLowSat) 
			{
				iSat++; iSaturate++;
			}
			if(y[i] > dMinSpikeAmp) bSpikeGreaterMinAmp = true;
		}
		if(iSaturate>iSM)
			iSM=iSaturate;
		//if 33% of values too high or too low
		//means spike is probably noise
		if(static_cast<double>(iSat)/iSamples >= dFracSat)	
		{
			bIsNoise=true;
			break;
		}
	}
	if(bIsNoise || !bSpikeGreaterMinAmp)
	{
		SetNoise(1);
		//*m_Vertex.begin()=0;
		return true;
	}
	return false;
}

prob_t CVertex::GetEnergy(int iSamples,int iChannel)
{
	prob_t dEnergy = 0.0;

	float* y = &m_YValues[iSamples*iChannel];

	int i;
	for(i=0;i<iSamples;i++)
		dEnergy += y[i]*y[i];
	dEnergy = sqrt(dEnergy);

	if(iSamples) 
		dEnergy /= iSamples;

	return dEnergy;
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::Swap(int channels, int samples)
{
	for (int j = 0; j< channels * samples; j++)
	{
		m_YValues[j] = - m_YValues[j];
		m_d2YValues[j] = - m_d2YValues[j];
	}

	float Stack1Value;
	//bad - hard coded 8
	//caused crash when reduced # of params from 17 to 12
	//couldn't find bug for hours :( :( :(
	//don't want to hard code another number
	//what if # of dimensions is even??
	for (char i = 0; i<8; i++)
	{
		Stack1Value = *(m_Vertex.begin() + i + 1);
		*(m_Vertex.begin() + i + 1) = *(m_Vertex.begin() + i + 8 + 1);
		*(m_Vertex.begin() + i + 8 + 1) = Stack1Value;
	}
	char toSwap = timeMax; timeMax = timeMin; timeMin = toSwap;
}

/////////////////////////////////////////////////////////////////////////////
// Written by Andre Fenton
void CVertex::Spline(float *x,float *y,int n,float *d2y,float *u)
{
	register int	i;
	register float	p, sig;

	d2y[0] = u[0] = 0.0;

	for (i = 1; i < n - 1; i++) {
		sig = (x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
		p = sig * d2y[i - 1] + 2.0;
		d2y[i] = (sig - 1.0) / p;
		u[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
		u[i] = (6.0 * u[i] / (x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
	}

	d2y[n - 1] = 0.0;

	for (i = n - 1; i--; )
		d2y[i] = d2y[i] * d2y[i + 1] + u[i];

	return;
}

void CVertex::Spline(float *x,int n,float *u)
{
	register int	i;
	register float	p, sig;
	float *y,*d2y;
	y = &m_YValues[0];
	d2y = &m_d2YValues[0];

	d2y[0] = u[0] = 0.0;

	for (i = 1; i < n - 1; i++) {
		sig = (x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
		p = sig * d2y[i - 1] + 2.0;
		d2y[i] = (sig - 1.0) / p;
		u[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
		u[i] = (6.0 * u[i] / (x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
	}

	d2y[n - 1] = 0.0;

	for (i = n - 1; i--; )
		d2y[i] = d2y[i] * d2y[i + 1] + u[i];

	return;
}


/////////////////////////////////////////////////////////////////////////////
// Written by Andre Fenton
float CVertex::Splope(float *xa,float *ya,float *d2y,float x,int lo_pt,int hi_pt)
{
	register float a, b, c, d, e, f, g, h;

	a = xa[lo_pt];
	b = xa[hi_pt];
	c = ya[lo_pt];
	d = ya[hi_pt];
	e = d2y[lo_pt];
	f = d2y[hi_pt];
	g = b - a;

	h = (x * (e * b - f * a) + d - c) / g;
	h += (x * x * (f - e) + f * a * a - e * b * b) / (2.0 * g);
	h += g * (e - f) / 6.0;

	return(h);
}

float CVertex::Splope(float *xa,float x,int lo_pt,int hi_pt)
{
	float *ya,*d2y;
	ya = &m_YValues[0];
	d2y = &m_d2YValues[0];
	
	register float a, b, c, d, e, f, g, h;

	a = xa[lo_pt];
	b = xa[hi_pt];
	c = ya[lo_pt];
	d = ya[hi_pt];
	e = d2y[lo_pt];
	f = d2y[hi_pt];
	g = b - a;

	h = (x * (e * b - f * a) + d - c) / g;
	h += (x * x * (f - e) + f * a * a - e * b * b) / (2.0 * g);
	h += g * (e - f) / 6.0;

	return(h);
}

float CVertex::Splope(float *xa,float x,int lo_pt,int hi_pt,int channel,int Samples)
{
	float *ya,*d2y;
	ya = &m_YValues[channel*Samples];
	d2y = &m_d2YValues[channel*Samples];
	
	register float a, b, c, d, e, f, g, h;

	a = xa[lo_pt];
	b = xa[hi_pt];
	c = ya[lo_pt];
	d = ya[hi_pt];
	e = d2y[lo_pt];
	f = d2y[hi_pt];
	g = b - a;

	h = (x * (e * b - f * a) + d - c) / g;
	h += (x * x * (f - e) + f * a * a - e * b * b) / (2.0 * g);
	h += g * (e - f) / 6.0;

	return(h);
}

/////////////////////////////////////////////////////////////////////////////
// Written by Andre Fenton
float CVertex::Splint(float *xa,float *ya,float *d2y,float x,int lo_pt,int hi_pt)
{
	register float	h, b, a;
	float	y;

	h = xa[hi_pt] - xa[lo_pt];
	a = (xa[hi_pt] - x) / h;
	b = (x - xa[lo_pt]) / h;

	y = a * ya[lo_pt] + b * ya[hi_pt] + ((a * a * a - a) * d2y[lo_pt] + (b * b * b - b) * d2y[hi_pt]) * (h * h) / 6.0;

	return(y);
}

float CVertex::Splint(float *xa,float x,int lo_pt,int hi_pt)
{
	float *ya,*d2y;
	ya = &m_YValues[0];
	d2y = &m_d2YValues[0];
		
	register float	h, b, a;
	float	y;

	h = xa[hi_pt] - xa[lo_pt];
	a = (xa[hi_pt] - x) / h;
	b = (x - xa[lo_pt]) / h;

	y = a * ya[lo_pt] + b * ya[hi_pt] + ((a * a * a - a) * d2y[lo_pt] + (b * b * b - b) * d2y[hi_pt]) * (h * h) / 6.0;

	return(y);
}

float CVertex::Splint(float *xa,float x,int lo_pt,int hi_pt,int channel,int Samples)
{
	float *ya,*d2y;
	ya = &m_YValues[channel*Samples];
	d2y = &m_d2YValues[channel*Samples];
		
	register float	h, b, a;
	float	y;

	h = xa[hi_pt] - xa[lo_pt];
	a = (xa[hi_pt] - x) / h;
	b = (x - xa[lo_pt]) / h;

	y = a * ya[lo_pt] + b * ya[hi_pt] + ((a * a * a - a) * d2y[lo_pt] + (b * b * b - b) * d2y[hi_pt]) * (h * h) / 6.0;

	return(y);
}

/////////////////////////////////////////////////////////////////////////////
float CVertex::GetValue(int mIndex,MY_STACK::iterator whichVerx)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CVertexStack
CVerxStack::CVerxStack()
{ 
	AreData = 0;  
	m_NumVerx = 0;
	Dimension = 0;
	BASE_DIMENSION = 0;
	RESOLUTION = 16;
	NUM_CHANNELS = 4; 
	NUM_SAMPLES = 64;
	SAMPLE_FREQ = 32000; 
	whichDraw = 1;
	m_FakeSpikes = false;
	m_x.resize(NUM_SAMPLES);
	CVertex::VX.resize(NUM_SAMPLES);
	for (int i=0;i<NUM_SAMPLES;i++)
		CVertex::VX[i] = m_x[i] = (float) i/SAMPLE_FREQ;
	m_MainSyncStack.SetMyPalette(m_PalStack);
	m_bShowXYSpikes = true;
	m_bLoadedXYSpikes = false;
	m_iPosCollapse = 8;
	m_NumOriginalCl = 0;
	m_bAutoCheckNoise = false;

	m_bCombinePCA = false;
	m_bUsePCA = false;
	m_iNumPCsToStore = 3;
	m_iPCAStartDim = -1;
	m_iPCAEndDim = -1;

	m_bUseEEG = true;
	m_bNormFloatV = true;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::SetPalette(CPaletteStack *pal)
{
	SetMyPalette(pal);
	m_MainSyncStack.SetMyPalette(pal);
	m_MainEEGStack.SetMyPalette(pal);
}

int CVerxStack::NumTopDims()
{
	int iTopD = 1;//Time[s]
	if(m_bLoadedXYSpikes)//x,y location
		iTopD += 2;
	return iTopD;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::ClearHighDim()
{
	if(Dimension <= BASE_DIMENSION)
		return;

	TopDimPopPush PopPush(this);
	
	MY_STACK::iterator Index;

	Dimension = BASE_DIMENSION;
	for (Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++)
	{
		CVertex *Verx;
		Verx = (CVertex*) *Index;
		Verx->ClearHighDim(Dimension);
	}

	CString *myTxt;
	while (m_AxesStack.size()>Dimension)
	{
		myTxt = *(m_AxesStack.end() - 1);
		delete myTxt;
		m_AxesStack.pop_back();
	}
}

void CVerxStack::CalcDimStats()
{
	MY_STACK::iterator iVerx;
	
	m_MainMin.clear();
	m_MainMax.clear();

	m_MainMean = VERTEX(Dimension+1,0.0f);
	m_MainStdev = VERTEX(Dimension+1,0.0f);
	m_MainEntropy = VERTEX(Dimension+1,0.0f);
	m_MainRange = VERTEX(Dimension+1,0.0f);

	for (int i=0;i<=Dimension;i++)
	{	m_MainMin.push_back((float) 2e+20);
		m_MainMax.push_back((float)-2e+20);
	}
	
	for (iVerx = m_VerxStack.begin(); iVerx != m_VerxStack.end(); iVerx++)
	{	CVertex* verx = (CVertex*) *iVerx;		
		for (i=1;i<=Dimension;i++)			// noised signal
		{	float val = verx->GetValue(i);
			if ( val < m_MainMin[i] )
				m_MainMin[i] = val;
			if ( val > m_MainMax[i] )
				m_MainMax[i] = val;
			m_MainMean[i] += val;
		}
	}

	for(i=1;i<=Dimension;i++) 
	{
		m_MainMean[i] /= (float) m_VerxStack.size(); // calc mean
		m_MainRange[i] = m_MainMax[i] - m_MainMin[i];
	}

	for (iVerx = m_VerxStack.begin(); iVerx != m_VerxStack.end(); iVerx++)
	{	CVertex* verx = (CVertex*) *iVerx;		
		for (i=1;i<=Dimension;i++)			// noised signal
		{	float val = verx->GetValue(i) - m_MainMean[i];
			m_MainStdev[i] += (val*val);
		}
	}

	for(i=1;i<=Dimension;i++) m_MainStdev[i] = sqrt(m_MainStdev[i] / (float) m_VerxStack.size());

	
	for(i=1;i<=Dimension;i++)
	{	//compute entropy of each dimension (using 30 bins)
		const int iBins = 30;
		vector<int> vHist(iBins,0);
		for (iVerx = m_VerxStack.begin(); iVerx != m_VerxStack.end(); iVerx++)
		{	CVertex* verx = (CVertex*) *iVerx;
			float val = (verx->GetValue(i) - m_MainMin[i]) / m_MainRange[i];
			int bin = (iBins-1.0)*val;
			vHist[bin]++;
		}
		int j = 0;
		for(j = 0; j<iBins; j++)
		{	if(!vHist[j])continue;
			float prob = vHist[j]/(float)m_VerxStack.size();
			m_MainEntropy[i] -= prob * log2(prob);
		}
	}

//	for(i=1;i<=Dimension;i++)
//	{	//compute entropy of each dimension
//		const float fBinWidth = 0.5;
//		VHist oH(m_MainMin[i],m_MainMax[i],fBinWidth);
//		for (iVerx = m_VerxStack.begin(); iVerx != m_VerxStack.end(); iVerx++)
//		{	CVertex* verx = (CVertex*) *iVerx;
//			oH.Inc(verx->GetValue(i));
//		}
//		m_MainEntropy[i] = oH.Entropy();
//	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcMinMax()
{
	MY_STACK::iterator iVerx;
	
	m_MainMin.clear();
	m_MainMax.clear();
	m_MainNoisedMin.clear();
	m_MainNoisedMax.clear();

	for (int i=0;i<=Dimension;i++)
	{
		m_MainMin.push_back((float) 2e+20);
		m_MainMax.push_back((float)-2e+20);
		m_MainNoisedMin.push_back((float) 2e+20);
		m_MainNoisedMax.push_back((float)-2e+20);
	}
	
	CVertex *verx;
	for (iVerx = m_VerxStack.begin(); iVerx != m_VerxStack.end(); iVerx++)
	{
		verx = (CVertex*) *iVerx;
		
		if ( verx->GetNoise() == 0 )
		{	// signal
			for (i=1;i<=Dimension;i++)
			{
				if ( verx->GetValue(i) < *(m_MainMin.begin()+i) )
					*(m_MainMin.begin()+i) = verx->GetValue(i);
				if ( verx->GetValue(i) > *(m_MainMax.begin()+i) )
					*(m_MainMax.begin()+i) = verx->GetValue(i);
			}
		}
		// noised signal
		for (i=1;i<=Dimension;i++)
		{
			if ( verx->GetValue(i) < *(m_MainNoisedMin.begin()+i) )
				*(m_MainNoisedMin.begin()+i) = verx->GetValue(i);
			if ( verx->GetValue(i) > *(m_MainNoisedMax.begin()+i) )
				*(m_MainNoisedMax.begin()+i) = verx->GetValue(i);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcMinMaxLast()
{
	MY_STACK::iterator iVerx;

	m_MainMin.push_back((float) 2e+20);
	m_MainMax.push_back((float)-2e+20);
	m_MainNoisedMin.push_back((float) 2e+20);
	m_MainNoisedMax.push_back((float)-2e+20);
	
	CVertex *verx;
	for (iVerx = m_VerxStack.begin(); iVerx != m_VerxStack.end(); iVerx++)
	{
		verx = (CVertex*) *iVerx;
		if ( verx->GetNoise() == 0 )
		{	// signal
			if ( verx->GetValue(Dimension) < *(m_MainMin.begin()+Dimension) )
				*(m_MainMin.begin()+Dimension) = verx->GetValue(Dimension);
			if ( verx->GetValue(Dimension) > *(m_MainMax.begin()+Dimension) )
				*(m_MainMax.begin()+Dimension) = verx->GetValue(Dimension);
		}

		if ( verx->GetValue(Dimension) < *(m_MainNoisedMin.begin()+Dimension) )
			*(m_MainNoisedMin.begin()+Dimension) = verx->GetValue(Dimension);
		if ( verx->GetValue(Dimension) > *(m_MainNoisedMax.begin()+Dimension) )
			*(m_MainNoisedMax.begin()+Dimension) = verx->GetValue(Dimension);
	
	}
}


/////////////////////////////////////////////////////////////////////////////
float CVerxStack::GetMin(int Index)
{ 
	if (Index<=Dimension) 
		return *(m_MainMin.begin()+Index);
	else 
		return -10; 
}

/////////////////////////////////////////////////////////////////////////////
float CVerxStack::GetMax(int Index)
{ 
	if (Index<=Dimension)
		return *(m_MainMax.begin()+Index);
	else 
		return 10;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::AddVrx(CMyObject *toStore)
{
	m_VerxStack.push_back(toStore);
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::GetClust(int NoOfVerx)
{
	if (NoOfVerx < m_NumVerx)
	{
		return ((CVertex*)*(m_VerxStack.begin() + NoOfVerx))->GetClust();
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::CheckNoise(double dLowSat,double dHighSat,double dFracSat,double dMinSpikeAmp)
{
	int iCount = 0;
	MY_STACK::iterator iVerx;
	CVertex *verx;
	for (iVerx = m_VerxStack.begin(); iVerx != m_VerxStack.end(); iVerx++)
	{
		verx = (CVertex*) *iVerx;
		if(verx->CheckNoise(NUM_CHANNELS,NUM_SAMPLES,dLowSat,dHighSat,dFracSat,dMinSpikeAmp)) iCount++;
	}
	return iCount;
}

void CVerxStack::SplineUpSample(CVertex* verx,float* vout,int iChannel,int iINC)
{
	//int iFullSize = RESOLUTION*NUM_SAMPLES+1;
	//vout.resize(RESOLUTION*NUM_SAMPLES+1);
	//vout.resize(8*NUM_SAMPLES+1);
	int iK=0 , idx = 0, iL = 0;
	//float dYt1 = verx->Splope(&m_x[0], m_x[iK], iK+0, iK+1, iChannel, NUM_SAMPLES);
	for (iK=0;iK<NUM_SAMPLES-1;++iK)
		for (iL=0; iL<RESOLUTION; iL+=iINC)
			vout[idx++]=verx->Splint(&m_x[0], m_x[iK]+(float)iL/(SAMPLE_FREQ*RESOLUTION), iK+0, iK+1, iChannel, NUM_SAMPLES);
	vout[idx++]=verx->GetYValues(iChannel*NUM_SAMPLES+NUM_SAMPLES-1);
//	Write2Log("final idx = %d",idx);
}

int CVerxStack::MaxIndex(CVertex* verx,int iChannel,int iINC)
{
	/*int iSz = NUM_SAMPLES , i = 0 , maxind = 0;
	float maxval = verx->GetYValues(iChannel*NUM_SAMPLES);
	float tmpval = maxval;
	for(i=1;i<iSz;i++)
		if((tmpval=verx->GetYValues( iChannel * NUM_SAMPLES + i))>maxval)
		{	maxval=tmpval; maxind=i; }
	return maxind;*/

	int m_iK;
	int lo_ptMax;
	int lo_ptMaxDef;
	float m_max,mTMax;
	float m_maxDef;
	
	m_max=(float)-2e+20;
						
	int iQ = iChannel;
							
	m_iK=0;
	float dYt1 = verx->Splope(&m_x[0], m_x[m_iK], m_iK+0, m_iK+1, iQ, NUM_SAMPLES);
	for (m_iK=0;m_iK<(NUM_SAMPLES-2);++m_iK)
	{
		float dYt2 = verx->Splope(&m_x[0], m_x[m_iK+1], m_iK+1, m_iK+2, iQ, NUM_SAMPLES);
		if (dYt1*dYt2<=0)
		{
			for (int iL=0; iL<RESOLUTION; iL+=iINC)
			{
				float Value = verx->Splint(&m_x[0], m_x[m_iK]+(float)iL/(SAMPLE_FREQ*RESOLUTION), m_iK+0, m_iK+1, iQ, NUM_SAMPLES);
				if (m_max<Value)
				{
					m_max=Value;
					mTMax=m_x[m_iK]+(float)iL/(SAMPLE_FREQ*RESOLUTION);
					lo_ptMax=m_iK*RESOLUTION/iINC + iL/iINC;
				}
			} // 
		} // If is extreme, change resolution
		dYt1 = dYt2;
	} // For each 64 (32,...) samples

	return lo_ptMax;
}
/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcOneSpike(CVertex *verx)
{
	int m_iK;
	int lo_ptMin[4],lo_ptMax[4];
	int lo_ptMinDef,lo_ptMaxDef;
	float m_min[4],m_max[4],mTMin[4],mTMax[4];
	float m_minDef,m_maxDef;
	
	for (int iQ=0;iQ<NUM_CHANNELS;iQ++)
	{
		m_min[iQ]=(float) 2e+20;
		m_max[iQ]=(float)-2e+20;
	}
						
	float dYt1,dYt2;
							
	for (iQ=0;iQ<NUM_CHANNELS;iQ++)
	{
		m_iK=0;
		dYt1 = verx->Splope(&m_x[0], m_x[m_iK], m_iK+0, m_iK+1, iQ, NUM_SAMPLES);
		for (m_iK=0;m_iK<(NUM_SAMPLES-2);++m_iK)
		{
			dYt2 = verx->Splope(&m_x[0], m_x[m_iK+1], m_iK+1, m_iK+2, iQ, NUM_SAMPLES);
			if (dYt1*dYt2<=0)
			{
				for (int iL=0; iL<RESOLUTION; iL++)
				{
					float Value = verx->Splint(&m_x[0], m_x[m_iK]+(float)iL/(SAMPLE_FREQ*RESOLUTION), m_iK+0, m_iK+1, iQ, NUM_SAMPLES);
					if (Value >= m_max[iQ] || Value <= m_min[iQ])
					{
						if (m_min[iQ]>Value)		
						{
							m_min[iQ]=Value;
							mTMin[iQ]=m_x[m_iK]+(float)iL/(SAMPLE_FREQ*RESOLUTION);
							lo_ptMin[iQ]=m_iK;
						}
						if (m_max[iQ]<Value)
						{
							m_max[iQ]=Value;
							mTMax[iQ]=m_x[m_iK]+(float)iL/(SAMPLE_FREQ*RESOLUTION);
							lo_ptMax[iQ]=m_iK;
						}
					}
				} // 
			} // If is extreme, change resolution
			dYt1 = dYt2;
		} // For each 64 (32,...) samples
	} //  Repeat for each canal


	float MainTMin = mTMin[0];
	float MainTMax = mTMax[0];

	m_minDef = m_min[0];
	m_maxDef = m_max[0];
	lo_ptMinDef = lo_ptMin[0];
	lo_ptMaxDef = lo_ptMax[0];

	for (int iN=1;iN<NUM_CHANNELS;iN++)
	{
		if (m_min[iN]<m_minDef)
		{ 
			m_minDef = m_min[iN];
			lo_ptMinDef = lo_ptMin[iN];
			MainTMin = mTMin[iN];
		}
		if (m_max[iN]>m_maxDef)
		{ 
			m_maxDef = m_max[iN];
			lo_ptMaxDef = lo_ptMax[iN];
			MainTMax = mTMax[iN];
		}
	}
	verx->SetTimeMin((unsigned char) (MainTMin * 100000) );
	verx->SetTimeMax((unsigned char) (MainTMax * 100000) );
	if(m_bAutoCheckNoise) verx->CheckNoise(NUM_CHANNELS,NUM_SAMPLES);

	// Filling with PEAKs & VALLEYs & ... (default) parameters
	float minToStore,maxToStore;
	int iP=0;
	for (iP=0;iP<NUM_CHANNELS;iP++)
	{ 	
		verx->AddPnt(m_max[iP]);		// Tx_peak
	}
	for (iP=0;iP<NUM_CHANNELS;iP++)
	{ 	
		maxToStore = verx->Splint(&m_x[0], MainTMax, lo_ptMaxDef, lo_ptMaxDef+1, iP, NUM_SAMPLES);
		verx->AddPnt(maxToStore);	// Tx-V(peak)
	}
		
	for (iP=0;iP<NUM_CHANNELS;iP++)
	{
		verx->AddPnt(m_min[iP]);		// Tx-valley
	}

	for (iP=0;iP<NUM_CHANNELS;iP++)
	{
		minToStore = verx->Splint(&m_x[0], MainTMin, lo_ptMinDef, lo_ptMinDef+1, iP, NUM_SAMPLES);
		verx->AddPnt(minToStore);	// Tx-V(valley)
	}

	//store energy
	for(iP=0;iP<NUM_CHANNELS;iP++)
	{	prob_t dEnergy = verx->GetEnergy(NUM_SAMPLES,iP);
		verx->AddPnt(dEnergy);
	}
	//verx->AddPnt( (float) verx->GetTimeStamp()/10000 );//TimeStamp
}

//pop dims off vertices (& onto temp stack)
void CVerxStack::PopDims(int iDims)
{
	int iStart = 0;
	if(m_vPopped.size() && m_vPopped[0].size())
	{
		iStart = m_vPopped[0].size();
		int i;
		for(i=0;i<m_vPopped.size();i++)
			m_vPopped[i].resize(m_vPopped[i].size()+iDims);
	}
	else
		m_vPopped = vector< vector<float> >(m_VerxStack.size(),vector<float>(iDims));

	int iV = 0;
	MY_STACK::iterator Index;
	for (Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++, iV++)
	{
		CVertex* Verx = (CVertex*) *Index;
		int i , j = Verx->m_Vertex.size() - iDims;
		for(i=0;i<iDims;i++,j++)
			m_vPopped[iV][i+iStart] = Verx->m_Vertex[j];
		Verx->m_Vertex.resize(Verx->m_Vertex.size()-iDims);
	}

	int i , j = m_AxesStack.size() - iDims;
	for(i=0;i<iDims;i++,j++)
	{
		CString* myTxt = m_AxesStack[j];
		m_AxPopped.push_back(myTxt);
	}
	m_AxesStack.resize(m_AxesStack.size()-iDims);

	Dimension -= iDims;
	BASE_DIMENSION -= iDims;

	CalcMinMax();
}

//push popped dims off temp stack back onto vertices
void CVerxStack::PushDims(int iDims)
{
	MY_STACK::iterator Index;

	int iV = 0 , i = 0;
	for (Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++, iV++)
	{
		CVertex* Verx = (CVertex*) *Index;
		for(i=0;i<iDims;i++)
			Verx->AddPnt(m_vPopped[iV][i]);
		m_vPopped[iV].resize(m_vPopped[iV].size()-iDims);
	}

	if(m_vPopped[0].size()==0) m_vPopped.resize(0);

	for(i=0;i<iDims;i++)
		m_AxesStack.push_back(m_AxPopped[i]);
	m_AxPopped.resize(m_AxPopped.size()-iDims);

	Dimension += iDims;
	BASE_DIMENSION += iDims;

	CalcMinMax();
}

bool CVerxStack::AdjustTimeStamps()
{
	if(m_SpikesStack.size() != m_VerxStack.size())
		return false;

	CVertex *verx;
	MY_STACK::iterator Index = m_VerxStack.begin();
	MY_SPIKE_STACK::iterator SI = m_SpikesStack.begin();
	for (; Index!=m_VerxStack.end(); Index++,SI++)
	{
		verx = (CVertex*) *Index;
		verx->SetTimeStamp( verx->GetTimeStamp() + verx->GetTimeMax() ); // do adjustment here
		SI->TimeStamp = verx->GetTimeStamp();
	}

	return true;
}

void CVerxStack::AddVertexTimeStamps()
{
	CVertex *verx;
	MY_STACK::iterator Index;
	for (Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++)
	{
		verx = (CVertex*) *Index;
		verx->AddPnt( (float) verx->GetTimeStamp()/10000 );//TimeStamp
	}
}

#ifdef _DEBUG
 int not_main(int argc, char** argv);
#endif

 int CVerxStack::PCASampleSize()
 {
	 return m_vPCA[0].dim();
 }

 void CVerxStack::AppendPCAProjections(CVertex* p)
 {
	if(m_bCombinePCA)
	{
		int iSamp = m_vPCA[0].dim() , i = 0;
		vector<double> interm, vProj(m_iNumPCsToStore,0.0f);
		GetPCADataVec(p,interm);

		m_vPCA[0].transform(&interm[0],interm.size(),&vProj[0],m_iNumPCsToStore);

		//store projections of row-points on principle components in vertices
		for(i=0;i<m_iNumPCsToStore;i++)
			p->AddPnt(vProj[i]);
	}
	else
	{	int iChan = 0 , iMaxChan = 4, i;
		vector< vector<double> > v(iMaxChan, vector<double>(NUM_SAMPLES) );
		A2D<double> vProj(iMaxChan,m_iNumPCsToStore);
		for(iChan=0;iChan<iMaxChan;iChan++)
			GetPCADataVec(p,v[iChan],iChan);
		for(iChan=0;iChan<iMaxChan;iChan++) m_vPCA[iChan].transform(&v[iChan][0],v[iChan].size(),vProj[iChan],m_iNumPCsToStore);
		for(i=0;i<m_iNumPCsToStore;i++)
			for(iChan=0;iChan<iMaxChan;iChan++)
				p->AddPnt(vProj[i][iChan]);
	}
 }

void CVerxStack::GetPCADataVec(CVertex* pVertex,vector<double>& v,int iChannel)
{
	const vector<double>& mean = m_vPCA[iChannel].mean();
	prob_t dEnergy = pVertex->GetEnergy(NUM_SAMPLES,iChannel);
	int i = 0  , iSamps = m_vPCA[iChannel].dim();
	if(dEnergy <= 0.0) dEnergy = 1.0;
	v.resize(iSamps);
	for(i=0;i<iSamps;i++)
		v[i] = pVertex->GetYValues((iChannel*NUM_SAMPLES)+i) / dEnergy;
}

 void CVerxStack::GetPCADataVec(CVertex* pVertex,vector<double>& v)
 {	 
	const vector<double>& mean = m_vPCA[0].mean();
	int iSamps = m_vPCA[0].dim();
	v.resize(iSamps);
	int i = 0;
	for(i=0;i<iSamps;i++)
		v[i] = pVertex->GetYValues(i-1);
	double dE = Energy(v); 
	if(dE > 0.0) for(i=0;i<iSamps;i++)
		v[i] /= dE;
 }

void CVerxStack::DoVertexPCA(bool bForIsolationDist)
{
	if(m_bCombinePCA && !bForIsolationDist)
	{
		int iSamps = NUM_SAMPLES * NUM_CHANNELS;
		PCA& oPCA = m_vPCA[0]; oPCA.init(iSamps);

		MY_STACK::iterator Index;

		int iV = 0;

		vector<double> vtmp(iSamps), vEnergyNorm(m_VerxStack.size());

		//copy vertex data to pca format
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
		{	CVertex* verx = (CVertex*) *Index;
			int i = 0;
			for(i=0;i<iSamps;i++) vtmp[i]=verx->GetYValues(i);
			double dEnergyNorm = Energy(vtmp) * iSamps;
			vEnergyNorm[iV] = dEnergyNorm;
			for(i=0;i<iSamps;i++) vtmp[i] /= dEnergyNorm;
			oPCA.putData(vtmp);
		}

		oPCA.dataEnd();
		oPCA.calcPCA();
		
		iV=0;
		vector<double> vout(m_iNumPCsToStore,0.0);

		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
		{	CVertex* verx = (CVertex*) *Index;
			int iP , i;
			for(i=0;i<iSamps;i++) vtmp[i]=verx->GetYValues(i) / vEnergyNorm[iV];
			fill(vout.begin(),vout.end(),0.0f);
			oPCA.transform(&vtmp[0],iSamps,&vout[0],m_iNumPCsToStore);
			for(iP=0;iP<m_iNumPCsToStore;iP++)//store projections of spikes on PCs
				verx->AddPnt(vout[iP]);
		}
	}
	else
	{	//storage for principle components
		//later will move it to the vertices
		//only used to keep a specific ordering on the data
		const int iParams = bForIsolationDist ? 1 : m_iNumPCsToStore;
		A2D<float> vStore(m_VerxStack.size(),iParams*NUM_CHANNELS);

		int iChannel = 0;
		for(iChannel=0;iChannel<NUM_CHANNELS;iChannel++)
		{	int iV = 0, iMaxIDX = 0, iMaxShift = 0, iAlignPad = 0;
			const int iResRedFctr = 8; //resolution reduction factor
			MY_STACK::iterator Index;
			vector<int> vMaxIDX(m_VerxStack.size()) , vMaxHist(RESOLUTION*NUM_SAMPLES/iResRedFctr+1,0);
			const bool bAlign = false , bUpsample = false;//don't ever upsample & align for Iso-D,L-Ratio until proven to be better
			if(bAlign)
			{	for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
				{	CVertex* verx = (CVertex*) *Index;
					vMaxIDX[iV] = MaxIndex(verx,iChannel,iResRedFctr); //x location of peak for spike iV
					if(vMaxIDX[iV]<vMaxHist.size()) vMaxHist[vMaxIDX[iV]]++;
				}
				iMaxIDX = GetMaxIndex(vMaxHist); // most common x location of peak for all spikes on channel				
				vector<int> vShiftHist(2*RESOLUTION*NUM_SAMPLES/iResRedFctr+2,0);
				for(iV=0;iV<vMaxIDX.size();iV++)
				{	int tmp;
					if((tmp=abs(vMaxIDX[iV]-iMaxIDX))>iMaxShift)
						iMaxShift=tmp;
					if(tmp<vShiftHist.size()) vShiftHist[tmp]++;
				}
				iAlignPad = max(4*GetMaxIndex(vShiftHist),32);
			}

			int iPCASz = bUpsample ? NUM_SAMPLES*RESOLUTION/iResRedFctr+1 : NUM_SAMPLES;
			if(bAlign) iPCASz += 2*iAlignPad;
			A2D<PCA_T> vChanData(m_VerxStack.size(),iPCASz);
			m_vPCA[iChannel].init(iPCASz);
			PCA& oPCA = m_vPCA[iChannel];

			iV=0;
			if(bAlign)
			{	if(bUpsample)
				{	iV = 0; ScopedTimer S("SplineUpsample");
					vector<PCA_T> vUp(NUM_SAMPLES*RESOLUTION/iResRedFctr+1,0.0f);
					Write2Log("iPCASz=%d vUp.size()=%d",iPCASz,vUp.size());
					for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
					{	CVertex* verx = (CVertex*) *Index;
						PCA_T* vData = vChanData[iV];
						SplineUpSample(verx,&vUp[0],iChannel,iResRedFctr);
						float dEnergyNorm = Energy(vUp) * vUp.size();
						if(dEnergyNorm<=0.0) dEnergyNorm=1.0;
						int i = 0, iOffset = iAlignPad;
						if( abs(iMaxIDX - vMaxIDX[iV]) < iAlignPad)
							iOffset += (iMaxIDX - vMaxIDX[iV]);
						for(i=0;i<iAlignPad;i++)	//fill with 1st value
							vData[i] = (vUp[0] / dEnergyNorm);
						for(i=0;i<vUp.size();i++)
							if(i+iOffset>=0 && i+iOffset<iPCASz)
								vData[i+iOffset] = (vUp[i] / dEnergyNorm);
						for(;i<vUp.size()+iAlignPad;i++)
							if(i+iOffset<iPCASz) vData[i+iOffset]=(vUp[vUp.size()-1] / dEnergyNorm);
					}
				}
				else	//copy vertex data to pca format
				{	for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
					{	PCA_T* vData = vChanData[iV];
						CVertex* verx = (CVertex*) *Index;
						prob_t dEnergyNorm = verx->GetValue(17+iChannel) * iPCASz;
						int i = 0, iOffset = iAlignPad;
						if( abs(iMaxIDX - vMaxIDX[iV]) < iAlignPad)
							iOffset += (iMaxIDX - vMaxIDX[iV]);
						if(dEnergyNorm <= 0.0) dEnergyNorm = 1.0;
						for(i=0;i<iAlignPad;i++)	//fill with 1st value
							vData[i] = (verx->GetYValues((iChannel*NUM_SAMPLES)) / dEnergyNorm);
						for(i=0;i<NUM_SAMPLES;i++)
							vData[i+iOffset] = (verx->GetYValues((iChannel*NUM_SAMPLES)+i) / dEnergyNorm);
						for(;i<NUM_SAMPLES+iAlignPad;i++)
							vData[i]=(verx->GetYValues((iChannel*NUM_SAMPLES)+NUM_SAMPLES-1) / dEnergyNorm);
					}
				}
			}
			else	//copy vertex data to pca format
			{	for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
				{	PCA_T* vData = vChanData[iV];
					CVertex* verx = (CVertex*) *Index;
					prob_t dEnergyNorm = verx->GetValue(17+iChannel) * NUM_SAMPLES;
					int i = 0;
					if(dEnergyNorm <= 0.0) dEnergyNorm = 1.0;
					for(i=0;i<NUM_SAMPLES;i++)
						vData[i] = verx->GetYValues((iChannel*NUM_SAMPLES)+i)   / dEnergyNorm;
				}
			}	
			for(iV=0;iV<vChanData.Rows();iV++)
				oPCA.putData(vChanData[iV]);
			oPCA.dataEnd();
			{ScopedTimer S("oPCA.calcPCA()"); oPCA.calcPCA();}

			const bool bOutD = false;
			if(bOutD)
			{	char fn[1024]; 
				sprintf(fn,"chan_%d_spikes.txt",iChannel); FILE* fp = fopen(fn,"w");
				for(iV=0;iV<vChanData.Rows();iV++)
				{	int ii = 0;
					for(;ii<iPCASz;ii++)
						fprintf(fp,"%.4f ",vChanData[iV][ii]);
					fprintf(fp,"\n");
				}
				fclose(fp);
			}
			//store projections of row-points on first 3 prin. comps.
			vector<PCA_T> vTrans(iParams);
			for(iV=0;iV<m_VerxStack.size();iV++)
			{	oPCA.transform(vChanData[iV],iPCASz,&vTrans[0],iParams);
				int iPC = 0;
				for(iPC = 0; iPC < iParams; iPC++)
					vStore[iV][iChannel*iParams+iPC] = vTrans[iPC];
			}
		}

		if(bForIsolationDist)	//store energy + 1st principle component for each channel 
		{	m_vIsolationDistData.resize(m_VerxStack.size()*NUM_CHANNELS*2);
			int iV = 0, iVIDX = 0;
			MY_STACK::iterator Index;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{	CVertex* verx = (CVertex*) *Index;
				int iCH = 0;
				for(;iCH<NUM_CHANNELS;iCH++)//energy on each tetrode in order
					m_vIsolationDistData[iVIDX++]=verx->GetValue(17+iCH);
				for(iCH=0;iCH<NUM_CHANNELS;iCH++)//energy-normalized PC1 on each tetrode in order
					m_vIsolationDistData[iVIDX++]=vStore[iV][iCH]; 
			}
		}
		else	//store the projection onto the principle components in vertices
		{	int iV=0;
			MY_STACK::iterator Index;
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{	CVertex* verx = (CVertex*) *Index;
				int iP;
				for(iP=0;iP<iParams;iP++)
					for(iChannel=0;iChannel<NUM_CHANNELS;iChannel++)
						verx->AddPnt(vStore[iV][iParams*iChannel+iP]);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CVerxStack::AddVertexLocations()
{
	if(!m_MainSyncStack.SyncStack.size())
		return false;

//#ifndef _DEBUG
	//off for now
//	return false;
//#endif

	int timeWTimes = 10; // this is default, it can be arbitrary
	int MIN_TIME_WINDOW = NUM_SAMPLES*1000 / SAMPLE_FREQ ;
	if (MIN_TIME_WINDOW == 0)
		MIN_TIME_WINDOW = 1;
	int MIN_STEP_WINDOW = MIN_TIME_WINDOW;

	int timeStampFirst = m_SpikesStack.begin()->TimeStamp - NUM_SAMPLES*10000 / SAMPLE_FREQ;
	int TSLast = (m_SpikesStack.end() - 1)->TimeStamp;
	if ( (InBpfExist & 3) || (LoadedAdd & 3) )
	{
		if ( ((CSync*)*(m_MainSyncStack.SyncStack.begin()))->GetTimeStamp() < timeStampFirst )
			timeStampFirst = ((CSync*)*(m_MainSyncStack.SyncStack.begin()))->GetTimeStamp();
		if ( ((CSync*)*(m_MainSyncStack.SyncStack.begin() + m_MainSyncStack.SyncStack.size() - 1))->GetTimeStamp() > (m_SpikesStack.end()-1)->TimeStamp)
			TSLast = ((CSync*)*(m_MainSyncStack.SyncStack.begin() + m_MainSyncStack.SyncStack.size() - 1))->GetTimeStamp();
	}

	int MAX_TIME_WINDOW = ( TSLast - timeStampFirst) / 10 + 5 * MIN_STEP_WINDOW;
	int MAX_STEP_TIMES = MAX_TIME_WINDOW / MIN_STEP_WINDOW + 1;
	if ( MAX_STEP_TIMES > 32767 )  // see Help of CScrollBar -> SetScrollBarRange, it cannot be grater than 32.767
	{
		MIN_STEP_WINDOW = MAX_TIME_WINDOW / 32767 + ( (MAX_TIME_WINDOW % 32767) ? 1 : 0 );  
		MAX_STEP_TIMES = MAX_TIME_WINDOW / MIN_STEP_WINDOW + MIN_STEP_WINDOW;
	}
	int MAX_TIME_TIMES = MAX_TIME_WINDOW / MIN_TIME_WINDOW;
	
	int timeWindow = timeWTimes * MIN_TIME_WINDOW;  // initialization
	float EditTime = float(timeWindow) / 10;
	

	int startStepTimes = 0;
	int startTime = startStepTimes * MIN_STEP_WINDOW;

	int pStartTimeTimes = startTime / MIN_TIME_WINDOW;
	char pMode = 0;
	int pAddTimes = 1;

	MY_SYNC_STACK& SyncStack = m_MainSyncStack.SyncStack;

	vector<unsigned char> vX(m_VerxStack.size()), vY(m_VerxStack.size());
	vector<bool> vAssigned(m_VerxStack.size());

	for(;pStartTimeTimes < MAX_TIME_TIMES - timeWTimes ; pStartTimeTimes += pAddTimes)
	{
		startTime = pStartTimeTimes * MIN_TIME_WINDOW;
		startStepTimes = startTime / MIN_STEP_WINDOW;

		//char params = 4; // iixxxooo, xxx - points/circles/vectors, ooo - arena/room
		//params += 8 * m_wndCmbRoom.GetCurSel();
		//m_MainDataStack->m_MainSyncStack.DrawArenaRoom(&dc,m_RoomRect, timeStampFirst + 10*startTime, timeWindow, params, clView, m_MainDataStack->whichDraw);

		int firstTS = timeStampFirst + 10 * startTime;

		// find the FIRST frame
		int timeWin = timeWindow;
		timeWin *= 10;
		int minT,maxT;
		int indexFrame = SyncStack.size() / 2 ; 
		int increment = indexFrame;
		minT = 0; maxT = SyncStack.size() - 1;
		char finishedFind = 0;
		while (increment > 0)
		{
			if ( firstTS > ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() )
			{
				minT = indexFrame;
				increment = (maxT - minT)/2;
				indexFrame += increment;
			}
			else{
				if ( firstTS < ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() )
				{
					maxT = indexFrame;
					increment = (maxT - minT)/2;
					indexFrame -= increment;
				}
				else{
					// ==
					finishedFind = 1;
					increment = 0;
				}
			}
		}
		
		if ( ! finishedFind )
		{
			indexFrame = minT;
			if ( firstTS > ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() )
			{
				indexFrame = maxT;
			}
			else indexFrame = minT;
		}
		
		
		unsigned char x = 0, y = 0;
		char fine = 0;

		if (indexFrame >= SyncStack.size())
			fine = 1;

		if ( ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() >= firstTS )
		{ 
			int ai = 0;
			while ( ! fine && ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() < firstTS + timeWin )
			{
				/*if ( params & 2 )
				{	// Arena
					if ( typeOfData == 1 )
					{	// BPF
						x = ((CSyncBPF*) *(SyncStack.begin() + indexFrame))->GetArenaX() /2;
						y = ((CSyncBPF*) *(SyncStack.begin() + indexFrame))->GetArenaY() /2;
					}
					if ( typeOfData == 2 )
					{	// UFF
						x = ((CSyncUFF*) *(SyncStack.begin() + indexFrame))->GetRedX() /2;
						y = ((CSyncUFF*) *(SyncStack.begin() + indexFrame))->GetRedY() /2;
					}

				}*/
				//if ( params & 4 )
				{	// Room
					x = ((CSyncBPF*) *(SyncStack.begin() + indexFrame))->GetRoomX() / m_iPosCollapse;
					y = ((CSyncBPF*) *(SyncStack.begin() + indexFrame))->GetRoomY();
					y = (255 - y) / m_iPosCollapse;
				}

				indexFrame++;
				if (indexFrame >= SyncStack.size())
				{
					indexFrame--;
					fine = 1;
				}
			}
		}

		int indexSpike = m_SpikesStack.size() / 2 ; 
		increment = indexSpike;
		minT = 0; maxT = m_SpikesStack.size() - 1;
		finishedFind = 0;

		// find the FIRST spike
		timeWin = timeWindow;
		timeWin *= 10;
		int TSFirst = timeStampFirst;
		int startTS = startTime * 10 + TSFirst;

		while (increment > 0)
		{
			if ( startTS > (m_SpikesStack.begin() + indexSpike)->TimeStamp )
			{
				minT = indexSpike;
				increment = (maxT - minT)/2;
				indexSpike += increment;
			}
			else{
				if ( startTS < (m_SpikesStack.begin() + indexSpike)->TimeStamp )
				{
					maxT = indexSpike;
					increment = (maxT - minT)/2;
					indexSpike -= increment;
				}
				else{
					// ==
					finishedFind = 1;
					increment = 0;
				}
			}
		}
		if ( ! finishedFind )
		{
			indexSpike = minT;
			if ( startTS > (m_SpikesStack.begin() + indexSpike)->TimeStamp )
			{
				indexSpike = maxT;
			}
			else indexSpike = minT;
		}

		fine = 0;

		if (indexSpike >= m_SpikesStack.size())
		{
	//		indexSpike = m_SpikesStack.size();
			fine = 1;
		}
		if ((m_SpikesStack.begin() + indexSpike)->TimeStamp >= startTS )
		{ 
			while ( ! fine && (m_SpikesStack.begin() + indexSpike)->TimeStamp < startTS + timeWin )
			{
				sSpike& pSpike = *(m_SpikesStack.begin()+indexSpike);
				//if not in memory, then skip it
				if(!(pSpike.type & 1))
				{
					indexSpike++;
					if (indexSpike >= m_SpikesStack.size())
					{
						indexSpike--;
						fine = 1;
					}
					continue;
				}

				if(!vX[pSpike.offset] || !vY[pSpike.offset])
				{
					vX[pSpike.offset] = x;
					vY[pSpike.offset] = y;
					vAssigned[pSpike.offset] = true;
				}

 //				CVertex* verx = (CVertex*) *(m_VerxStack.begin() + pSpike->offset);

//				verx->AddPnt((float)x);
//				verx->AddPnt((float)y);

				indexSpike++;
				if (indexSpike >= m_SpikesStack.size())
				{
					indexSpike--;
					fine = 1;
				}
			}
		}
	}

	int iV = 0;
	bool bGood = true;
	int iA = 0,iNA=0;
	for(iV=0;iV<vAssigned.size();iV++)
	{
		if(!vAssigned[iV]) 
		{
			iNA++;
			bGood = false;
		}
		else iA++;
	}

	if(bGood)
	{
		iV = 0;
		MY_STACK::iterator Index;
		for(Index = m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
		{
			CVertex* verx = (CVertex*) *Index;
			verx->AddPnt(vX[iV]);
			verx->AddPnt(vY[iV]);
		}
	}

//#ifdef _DEBUG
	if(!bGood)
		Write2Log("error loading room x,y : iA = %d , iNA = %d",iA,iNA);
//#endif
	return bGood;
}

void InitProbs(int iMaxNumElems);

void CVerxStack::RemoveADim(int iDim)
{
	if(iDim<m_AxesStack.size())
	{
		MY_STACK::iterator IT;
		for(IT=m_VerxStack.begin();IT!=m_VerxStack.end();IT++)
		{
			CVertex* pVert = (CVertex*) *IT;
			pVert->m_Vertex.erase( (pVert->m_Vertex.begin()+iDim));
		}

		MY_STR_STACK::iterator ITA = (m_AxesStack.begin() + iDim - 1);
		CString* myTxt = *ITA;

#ifdef _DEBUG
		CString str;
		str.Format("removing axis %s , m_AxesStack.size()=%d",myTxt->GetBuffer(),m_AxesStack.size());
		MessageBox(0,str.GetBuffer(),str.GetBuffer(),0);
#endif

		delete myTxt;
		m_AxesStack.erase( ITA );

		Dimension -= 1;
		BASE_DIMENSION -= 1;
	}
	else
	{
		CString str;
		str.Format("Tell Sam to fix this RemoveADim bug! give him this info + .cl & .bpf file: iDim=%d m_AxesStack.size()=%d",iDim,m_AxesStack.size());
		MessageBox(0,str.GetBuffer(),"WClust - error!",0);
	}
}

int CVerxStack::NumPCADims()
{
	return m_iPCAEndDim - m_iPCAStartDim + 1;
}

void CVerxStack::RemovePCAAxes()
{
	int iNumPCADims = NumPCADims();
	int i = 0;
	for(i=0;i<iNumPCADims && m_AxesStack.size();i++)
	{
		MY_STR_STACK::iterator IT = (m_AxesStack.begin() + m_iPCAStartDim - 1);
		CString* myTxt = *IT;
		delete myTxt;
		m_AxesStack.erase( IT );
	}

	Dimension -= iNumPCADims;
	BASE_DIMENSION -= iNumPCADims;
}

void CVerxStack::RemoveVertexPCA()
{
	MY_STACK::iterator IT;
	for(IT=m_VerxStack.begin();IT!=m_VerxStack.end();IT++)
	{
		CVertex* pVert = (CVertex*) *IT;
		int i=0;
		for(i=m_iPCAStartDim;i<=m_iPCAEndDim;i++)
			pVert->m_Vertex.erase( (pVert->m_Vertex.begin()+m_iPCAStartDim));
	}
}

void CVerxStack::AddPCAAxes()
{
	if(m_bUsePCA)
	{
		m_iPCAStartDim = Dimension + 1; //21

		if(m_bCombinePCA)
		{
			Dimension += m_iNumPCsToStore;
			m_iPCAEndDim = m_iPCAStartDim + (m_iNumPCsToStore - 1);

			int iPC = 1;
			char strTmp[64];
			//D21 and up
			for(iPC = 1; iPC <= m_iNumPCsToStore; iPC++)
			{
				sprintf(strTmp,"PC%d",iPC);
				CString* pom = new CString(strTmp);
				AddAxes(pom);
			}
		}
		else
		{
			Dimension += NUM_CHANNELS * m_iNumPCsToStore;
			m_iPCAEndDim = m_iPCAStartDim + (NUM_CHANNELS * m_iNumPCsToStore - 1);

			int iPC = 1;
			char strTmp[64];
			//D21 and up
			for(iPC = 1; iPC <= m_iNumPCsToStore; iPC++)
			{
				int iChan = 1;
				for(iChan=1;iChan<=NUM_CHANNELS;iChan++)
				{
					sprintf(strTmp,"T%d-PC%d",iChan,iPC);
					CString* pom = new CString(strTmp);
					AddAxes(pom);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcAfterLoad()
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	MY_STACK::iterator Index;
	for (Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++)
	{
		CVertex* NewVerx = (CVertex*) *Index;
//#ifdef _DEBUG
		if(NewVerx->GetOrigClust() < 0 ||
		   NewVerx->GetNoise())
		{
			NewVerx->SetNoise(1);
		}
//#endif
		CalcOneSpike(NewVerx);
	}

	//do pca stuff here
	if(m_bUsePCA)
		DoVertexPCA();

	//add vertex locations if user likes it that way...
	m_bLoadedXYSpikes = m_bShowXYSpikes && AddVertexLocations();	

	//add timestamps
	AddVertexTimeStamps();

	//	CalcMinMax();
	CString *pom;		
										  //Dimension			
	pom = new CString(LPCSTR("T1-peak")); //D1
	AddAxes(pom);
	pom = new CString(LPCSTR("T2-peak")); //D2
	AddAxes(pom);
	pom = new CString(LPCSTR("T3-peak")); //D3
	AddAxes(pom);
	pom = new CString(LPCSTR("T4-peak")); //D4 
	AddAxes(pom);
	
	//////////////////////////////////////////////////
	//D5 - D8 shouldn't be used for auto-clustering
	//but should still be displayed to user
	pom = new CString(LPCSTR("T1-V(peak)"));//D5
	AddAxes(pom);
	pom = new CString(LPCSTR("T2-V(peak)"));//D6
	AddAxes(pom);
	pom = new CString(LPCSTR("T3-V(peak)"));//D7
	AddAxes(pom);
	pom = new CString(LPCSTR("T4-V(peak)"));//D8
	AddAxes(pom);
	//////////////////////////////////////////////////

	pom = new CString(LPCSTR("T1-valley")); //D9
	AddAxes(pom);
	pom = new CString(LPCSTR("T2-valley")); //D10
	AddAxes(pom);
	pom = new CString(LPCSTR("T3-valley")); //D11
	AddAxes(pom);
	pom = new CString(LPCSTR("T4-valley")); //D12
	AddAxes(pom);

	//////////////////////////////////////////////////
	//D13 - D16 shouldn't be used for auto-clustering
	//but should still be displayed to user
	pom = new CString(LPCSTR("T1-V(valley)")); //D13
	AddAxes(pom);
	pom = new CString(LPCSTR("T2-V(valley)")); //D14
	AddAxes(pom);
	pom = new CString(LPCSTR("T3-V(valley)")); //D15
	AddAxes(pom);
	pom = new CString(LPCSTR("T4-V(valley)")); //D16
	AddAxes(pom);
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	//energy
	pom = new CString(LPCSTR("T1-Energy")); //D17
	AddAxes(pom);
	pom = new CString(LPCSTR("T2-Energy")); //D18
	AddAxes(pom);
	pom = new CString(LPCSTR("T3-Energy")); //D19
	AddAxes(pom);
	pom = new CString(LPCSTR("T4-Energy")); //D20
	AddAxes(pom);
	//////////////////////////////////////////////////

	Dimension = 20;
	
	if(m_bUsePCA)
		AddPCAAxes();
	
	Dimension += 1; // time 
	
	if(m_bLoadedXYSpikes) Dimension += 2; //x,y locations

	BASE_DIMENSION = Dimension;

	//"top" dims (x,y,time)
	if(m_bLoadedXYSpikes)
	{
		pom = new CString(LPCSTR("RoomX"));
		AddAxes(pom);
		pom = new CString(LPCSTR("RoomY"));
		AddAxes(pom);
	}
	pom = new CString(LPCSTR("Time[s]"));	//D33 - last dim not used for auto-clustering
	AddAxes(pom);

	AreData = 1;	

#ifdef _DEBUG
	CalcMinMax();	
#endif
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::LoadUFF(CFile *file, char *buffer, int nRead)
{
	int		i;
	int		drdb_size;
	char	*dp, *first_rec;
	int		rec_sizes[256];
	int		current_byte;
	char	type;
	
	m_FakeSpikes = false;
	FileType = 2;

	dp = buffer;

	drdb_size = (int) *( (unsigned short*) (dp + HEADER_DRDB_SIZE_OFFSET) ) + PAD_BYTES;
	dp += HEADER_SIZE + PAD_BYTES;

	(void) memset((void *) rec_sizes, 0, (size_t) 256 * sizeof(int));
	while (TRUE) {
		if (dp[0] != DRDB_BYTE_0 || dp[1] != DRDB_BYTE_1)
			break; 
		rec_sizes[dp[DRDB_REC_TYPE_OFFSET]] = (int) *( (int*) (dp + DRDB_REC_SIZE_OFFSET) ) + PAD_BYTES; //reverse_ui4(dp + DRDB_REC_SIZE_OFFSET) + PAD_BYTES;
		dp += drdb_size;
	}

	m_NumVerx = 0;
	first_rec = dp; 
	current_byte = dp - buffer;
	type = *dp;

	
	// inspection of file
	const char max_elde = 3;
	int ChannelNo[max_elde*256];

	for (i=0; i<max_elde; i++)
		for (int j=0; j<256; j++)
		{
			ChannelNo[i*256+j] = 0;
		}

	do {
		type = *dp;
		switch (type) {
			case SINGLE_ELECTRODE_SPIKE_REC:
				ChannelNo[0*256 + *(dp + RECORD_PROBE_OFFSET)] += 1;
				break;
			case STEREOTRODE_SPIKE_REC:
				ChannelNo[1*256 + *(dp + RECORD_PROBE_OFFSET)] += 1;
				break;
			case TETRODE_SPIKE_REC:
				ChannelNo[2*256 + *(dp + RECORD_PROBE_OFFSET)] += 1;
				break;
			case CONTINUOUS_DATA_REC:
				//rp->probe = *(dp + RECORD_PROBE_OFFSET);
				break;
			case POSITION_REC:
				//rp->red_x = *(dp + POS_REC_RED_X_OFFSET);
				//rp->red_y = *(dp + POS_REC_RED_Y_OFFSET);
				//rp->green_x = *(dp + POS_REC_GREEN_X_OFFSET);
				//rp->green_y = *(dp + POS_REC_GREEN_Y_OFFSET);
				break;
			case EVENT_FLAG_REC:
				//rp->probe = *(dp + EV_FLAG_REC_TYPE_OFFSET);
				break;						
		}								
		dp += rec_sizes[type];
		current_byte += rec_sizes[type];
		if (rec_sizes[type] == 0)
		{
			return 2;
		}

	}while (current_byte < nRead);

	// Selecting of electrode & channel by user
	CSelectTT SelTTDial;
	SelTTDial.ChannelNo = ChannelNo;
	SelTTDial.max_elde = max_elde;
	SelTTDial.FileType = FileType;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SelTTDial.DoModal();
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	m_ElectrodeChannel = SelTTDial.EldeCh;
	m_ElectrodeType = SelTTDial.EldeType;

	switch (m_ElectrodeType)
	{
	case 1:
		NUM_CHANNELS = 4;
		break;
	case 2:
		NUM_CHANNELS = 4;
		break;
	case 3:
		NUM_CHANNELS = 4;
		break;

	}
	CVertex::SAMPLES = NUM_SAMPLES = SU_SPIKE_SIZE;
	CVertex::FREQ = SAMPLE_FREQ = 32000; 
	m_x.resize(NUM_SAMPLES); CVertex::VX.resize(NUM_SAMPLES);
	for (i=0;i<NUM_SAMPLES;i++)
		CVertex::VX[i] = m_x[i] = (float) i/SAMPLE_FREQ;

	dp = first_rec;	
	current_byte = dp - buffer;

	vector<float> m_u(NUM_SAMPLES);

	char *f;
	int mTS;
	sSpike *spikeInf;
	unsigned char greenX, greenY, redX, redY; 

	do {
		f = (char*) &mTS;
		*f = *(dp + RECORD_TIME_STAMP_OFFSET + 0); f++;
		*f = *(dp + RECORD_TIME_STAMP_OFFSET + 1); f++;
		*f = *(dp + RECORD_TIME_STAMP_OFFSET + 2); f++;
		*f = *(dp + RECORD_TIME_STAMP_OFFSET + 3);
		
		type = *dp;
		switch (type) {
			case SINGLE_ELECTRODE_SPIKE_REC:
				if ( m_ElectrodeType == 1 )
				{
					CVertex *NewVerx;
					char EldeChannel;
					EldeChannel = *(dp + RECORD_PROBE_OFFSET);

					// Add file portrait
					filePortraitStack.push_back(sSpikeInFile(1,EldeChannel));

					// We need this Channel
					if ( EldeChannel == m_ElectrodeChannel)
					{
						AreData = 1;
						m_NumVerx++;
					
						NewVerx = new CVertex(m_PalStack);
						NewVerx->SetOrigCl( *(dp + SPK_REC_CLUST_OFFSET) );
						NewVerx->SetTimeStamp( mTS );
						NewVerx->AddPnt( 0 );  //empty cluster's set
						NewVerx->AddClust(0);

					
						vector<float> mNewYVal(NUM_SAMPLES*NUM_CHANNELS);

						// conversion of values, and also reorganize matrix (Ch,S)->(S,Ch)
						for (int i = 0;i < (NUM_SAMPLES); i++)
						{
							mNewYVal[i] = (float) (10*( *((short*) (dp + SU_SPIKE_OFFSET + i*2 ) ) ))/4096;
							mNewYVal[i+1*NUM_SAMPLES] = mNewYVal[i + 2*NUM_SAMPLES] = mNewYVal[i + 3*NUM_SAMPLES] = mNewYVal[i];
						}
						NewVerx->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
						m_VerxStack.push_back(NewVerx);

						sSpike spikeInf;
						spikeInf.TimeStamp = NewVerx->GetTimeStamp();
						spikeInf.cluster = NewVerx->GetClust();
						spikeInf.origCluster = NewVerx->GetOrigClust();
						if ( NewVerx->GetOrigClust() > m_NumOriginalCl )
							m_NumOriginalCl = NewVerx->GetOrigClust();
						spikeInf.type = 1; //SPIKE_IN_MEMORY;
						spikeInf.offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;
						m_SpikesStack.push_back(spikeInf);
					}
				}
				break;
			case STEREOTRODE_SPIKE_REC:
				if ( m_ElectrodeType == 2 )
				{
					//CVertex *NewVerx;
					char EldeChannel;
					EldeChannel = *(dp + RECORD_PROBE_OFFSET);
					
					// Add file portrait
					filePortraitStack.push_back(sSpikeInFile(2,EldeChannel));

					// TODO stereode
				}
				break;
			case TETRODE_SPIKE_REC:
				if ( m_ElectrodeType == 3 )
				{
					CVertex *NewVerx;
					char EldeChannel;
					EldeChannel = *(dp + RECORD_PROBE_OFFSET);
					
					// Add file portrait
					filePortraitStack.push_back(sSpikeInFile(4,EldeChannel));

					// We need this Channel
					if ( EldeChannel == m_ElectrodeChannel)
					{
						AreData = 1;
						m_NumVerx++;
					
						NewVerx = new CVertex(m_PalStack);
						NewVerx->SetOrigCl( *(dp + SPK_REC_CLUST_OFFSET) );
						NewVerx->SetTimeStamp( mTS );
						NewVerx->AddPnt( 0 );  //empty cluster's set
						NewVerx->AddClust(0);
					
						vector<float> mNewYVal(NUM_SAMPLES*NUM_CHANNELS);

						// conversion of values, and also reorganize matrix (Ch,S)->(S,Ch)
						for (int i = 0;i < (NUM_SAMPLES); i++)
							for (int j = 0; j < NUM_CHANNELS; j++)
							{
								mNewYVal[i + j*NUM_SAMPLES] = (float) (10*( *((short*) (dp + SU_SPIKE_OFFSET + i*NUM_CHANNELS*2 + j*2) ) ))/4096;
							}
						NewVerx->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
						m_VerxStack.push_back(NewVerx);

						sSpike spikeInf;
						spikeInf.TimeStamp = NewVerx->GetTimeStamp();
						spikeInf.cluster = NewVerx->GetClust();
						spikeInf.origCluster = NewVerx->GetOrigClust();
						if ( NewVerx->GetOrigClust() > m_NumOriginalCl )
							m_NumOriginalCl = NewVerx->GetOrigClust();
						spikeInf.type = 1; //SPIKE_IN_MEMORY;
						spikeInf.offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;
						spikeInf.fileParams.clear();
						m_SpikesStack.push_back(spikeInf);
					}
				}
				break;
			case CONTINUOUS_DATA_REC:
				//rp->probe = *(dp + RECORD_PROBE_OFFSET);
				break;
			case POSITION_REC:
				InBpfExist |= 1; // Arena
				redX = *(dp + POS_REC_RED_X_OFFSET);
				redY = *(dp + POS_REC_RED_Y_OFFSET);
				greenX = *(dp + POS_REC_GREEN_X_OFFSET);
				greenY = *(dp + POS_REC_GREEN_Y_OFFSET);
				m_MainSyncStack.NewPosFromUFF(mTS, redX, redY, greenX, greenY);
		
				//rp->red_x = *(dp + POS_REC_RED_X_OFFSET);
				//rp->red_y = *(dp + POS_REC_RED_Y_OFFSET);
				//rp->green_x = *(dp + POS_REC_GREEN_X_OFFSET);
				//rp->green_y = *(dp + POS_REC_GREEN_Y_OFFSET);
				break;
			case EVENT_FLAG_REC:
				//rp->probe = *(dp + EV_FLAG_REC_TYPE_OFFSET);
				break;						
		}								
		dp += rec_sizes[type];
		current_byte += rec_sizes[type];
		if (rec_sizes[type] == 0)
		{
			m_u.clear();
			return 2;
		}

	}while (current_byte < nRead);

	m_u.clear();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
long CVerxStack::GetBPFDataPtr(CFile *file)
{
	//Searching of index of binary data
	int		m_j;
	int		FoundEnd;
	int		nRead;
	long	m_i;
	char	pomStr[40];
								
	vector<char> buffer(8192);
	
	nRead = file->Read(&buffer[0],8192);

	CString s;
	s.SetString(&buffer[0],8192);
	//file->Read(

	m_i=0;
	FoundEnd=0;
	sprintf(pomStr,"%%%%END_HEADER");
	while (m_i<nRead&&!FoundEnd)
	{
		m_j=0;
		while ((buffer[m_i+m_j]==pomStr[m_j])&&(m_j<12))
			m_j+=1;	
		
		if (m_j==12)
			FoundEnd=1;
		m_i+=1;

		/*if(m_j==12)
		{
			FoundEnd=1;
			break;
		}
		else m_i++;*/
	}


	if (!FoundEnd)
	{
		SetEmpty();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		m_FileName.Empty();
		return 0;
	}

	// Set file pointer to the first data identifier
	m_i+=13;

	return m_i;
}

/////////////////////////////////////////////////////////////////////////////
// from .x gets number
int GetNumber(char *buffer)
{
	int		No;
	char	*Ptr;

	Ptr = strstr(buffer,".");
	sscanf(Ptr + 1, "%d", &No);
	return No;
}

/////////////////////////////////////////////////////////////////////////////
// from ][x gets number
int GetNumberBr(char *buffer)
{
	int		No;
	char	*Ptr;
	char	toTest[4];
	sprintf(toTest,"][");
	
	Ptr = strstr(buffer,toTest);
	sscanf((Ptr + 2), "%d", &No);
	
	return No;
}

/////////////////////////////////////////////////////////////////////////////
// from (x gets number
int GetNumberRoundBr(char *buffer)
{
	int		No;
	char	*Ptr;
	char	toTest[4];
	sprintf(toTest,"(");
	
	Ptr = strstr(buffer,toTest);
	sscanf((Ptr + 1), "%d", &No);
	
	return No;
}

/////////////////////////////////////////////////////////////////////////////
int GetType(char *buffer)
{
	char	Type;
	char	*Ptr;

	Ptr = strstr(buffer,"(");
	sscanf(Ptr + 2, "%c", &Type);
	return Type;
}

/////////////////////////////////////////////////////////////////////////////
void GetParamName(char *buffer, CString *ParamName)
{
	char *Ptr;

	Ptr = strstr(buffer,"(");
	Ptr++;
	
	while ( *Ptr == ' ' )
	{
		Ptr++;
	}
	
	int i=0;
	while ( *Ptr != ')' && *Ptr != ' ')
	{
		ParamName->Insert(i,*Ptr);
		Ptr++;
		i++;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::PasteFileParamAsZeros( )
{
	MY_SPIKE_STACK::iterator indSpike = m_SpikesStack.begin();
	while ( indSpike != m_SpikesStack.end() )
	{
		sSpike& spike = *indSpike;

		spike.fileParams.push_back( (float) 0 );
		spike.fileParams.push_back( (float) 0 );
		spike.fileParams.push_back( (float) 0 );
		spike.fileParams.push_back( (float) 0 );

		indSpike++;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::PasteFileParameter( sWcpFiles *paramToAdd )
{

	int fileState;
	FILE *file = fopen( (LPCTSTR) paramToAdd->fileName, "r");

	if ( file != NULL )
	{
		char text[255], endOfHeader[40];
		sprintf(endOfHeader,"%%%%END_HEADER");		
		fileState = fscanf(file,"%s",&text);
		while ( fileState != EOF && strstr(text, endOfHeader) == NULL )
		{
			fileState = fscanf(file,"%s",&text);
		}
		if ( fileState != EOF )
		{
			fileState = fscanf(file,"%s",&text); // read WCLUST_PARAMETER_FILE

			float data[4];
			MY_FILE_PORTRAIT_STACK::iterator indFP = filePortraitStack.begin();
			MY_SPIKE_STACK::iterator indSpike = m_SpikesStack.begin();
			while ( fileState != EOF && indFP != filePortraitStack.end() )
			{
				sSpikeInFile& fp = *indFP;
				sSpike& spike = *indSpike;
				switch ( fp.electrode )
				{
				case 1:
					fileState = fscanf(file,"%f",&data[0]);
					if ( m_ElectrodeType == 1 && fp.channel == m_ElectrodeChannel )
					{
						spike.fileParams.push_back(data[0]);
						spike.fileParams.push_back(data[0]);
						spike.fileParams.push_back(data[0]);
						spike.fileParams.push_back(data[0]);

						indSpike++;
					}
					break;
				case 2:
					fileState = fscanf(file,"%f %f", &data[0], &data[1]);
					if ( m_ElectrodeType == 2 && fp.channel == m_ElectrodeChannel )
					{
						spike.fileParams.push_back(data[0]);
						spike.fileParams.push_back(data[1]);
						spike.fileParams.push_back(data[0]);
						spike.fileParams.push_back(data[1]);

						indSpike++;
					}
					break;
				case 4:
					fileState = fscanf(file,"%f %f %f %f", &data[0], &data[1], &data[2], &data[3]);
					if ( m_ElectrodeType == 3 && fp.channel == m_ElectrodeChannel )
					{
						spike.fileParams.push_back(data[0]);
						spike.fileParams.push_back(data[1]);
						spike.fileParams.push_back(data[2]);
						spike.fileParams.push_back(data[3]);
						indSpike++;
					}
					break;
				
				}// switch
				indFP++;
			} 
			fclose(file);
		}
	}
	
//	FILE *f=fopen("a.a","w");
//	MY_SPIKE_STACK::iterator indSpike;
//	sSpike *sp;
//	for ( indSpike = m_SpikesStack.begin(); indSpike != m_SpikesStack.end(); indSpike++ )
//	{
//		sp = (sSpike*) *indSpike;
//		fprintf(f,"%f \n", (float) *(sp->fileParams.begin() ) );
//	}
//	fclose(f);
}
	
void CVerxStack::RemoveExternalParam(int iNum)
{
	MY_SPIKE_STACK::iterator indSpike;
	for ( indSpike = m_SpikesStack.begin(); indSpike != m_SpikesStack.end(); indSpike++ )
	{
		indSpike->fileParams.erase( indSpike->fileParams.begin() + iNum );
	}
}


/////////////////////////////////////////////////////////////////////////////
// Looks for wcp files in directory
// wcpFileName ~ mask of 'name_of_bpf*.wcp'
// wcpFiles ~ stack for found parameters
void CVerxStack::CheckWcpFiles()
{
	bool  Done;				// Done searching for files?
	HANDLE  FndHnd = NULL;	// Handle to find data.
	struct _stat fileinfo;
	WIN32_FIND_DATA FindDat;// Info on file found.
	char temp[256];

	wcpFilesStack.clear();

	vector<char> fileMask(wcpFileNameMask.GetLength()+1);
	for (int iMask = 0; iMask < wcpFileNameMask.GetLength(); iMask++)
	{
		fileMask[iMask] = wcpFileNameMask.GetAt(iMask);
	}	
	fileMask[wcpFileNameMask.GetLength()] = '\0';

	FndHnd = FindFirstFile(&fileMask[0], &FindDat);
	
	//	Indicate we are done if nothing was found.
	Done = (FndHnd == INVALID_HANDLE_VALUE);

	while (!Done)
	{   // Print name and size of file found.
		sWcpFiles wcpFoundFile;

		strcpy(temp, FindDat.cFileName);
		_stat( temp, &fileinfo);

		int iName = 0;
		while (FindDat.cFileName[iName] != 0)
		{
			wcpFoundFile.fileName.Insert(iName,FindDat.cFileName[iName]);
			iName++;
		}
		wcpFoundFile.fileName.Insert(0,(LPCTSTR) wcpPath);
		wcpFoundFile.flag = 0;

		wcpFilesStack.push_back(wcpFoundFile);
		Done = !FindNextFile(FndHnd, &FindDat);
	}

	if (FndHnd != INVALID_HANDLE_VALUE)      // If there was anything found, then
		FindClose(FndHnd); // Close the find handle
	
	fileMask.clear();


	// Read parameter name of each file
	vector<char> buffer(1024);
	MY_WCP_FILES_STACK::iterator indFiles;
	for (indFiles = wcpFilesStack.begin(); indFiles != wcpFilesStack.end(); indFiles++)
	{
		CFile file;
		if ( file.Open( (LPCTSTR) ((sWcpFiles)*indFiles).fileName,CFile::modeRead) )
		{	// file is successfully opened
			int nRead = file.Read(&buffer[0],1024);
			if ( nRead == 0 )
			{
				file.Close();
			}
			else
			{	// header is successfully read

				char	*pdest, *formatInf;
				int		formatInfStart = 0;
				int		bpfEnd = 0;
	
				formatInf = strstr(&buffer[0], "WCLUST_PARAMETER_FILE");
				if (formatInf != NULL)
				{
					formatInfStart = formatInf - &buffer[0] + 1;
				
					pdest = strstr(formatInf,"ParameterName.0");
					if ( pdest != NULL )
					{	
						GetParamName(pdest,&indFiles->paramName);
						indFiles->flag = 1;
					} // if there is a ParameterName.0 flag
				} // if it is a wcp file
			} // header is succ. read
		} // file is open
		else
		{ // file is not open
		}
	} // for

	// Removing bad files from the list
	int iWcp = 0;
	while ( iWcp < wcpFilesStack.size() )
	{
		if ((wcpFilesStack.begin() + iWcp)->flag == 0)
		{
			wcpFilesStack.erase( wcpFilesStack.begin() + iWcp );
		}
		else
			iWcp++;
	}
	
//	FILE *f=fopen((LPCTSTR) ((sWcpFiles) (wcpFilesStack.begin())).fileName,"w+");
//	FILE *f=fopen("a.wcp","w+");
//	fprintf(f,"%%%%BEGIN_HEADER WCLUST_PARAMETER_FILE\n%%ParameterName.0 ( SpikeAsc )\n%%AmountElectrodes.0 (1)\n%%%%END_HEADER WCLUST_PARAMETER_FILE\n");
//	for (int i=0; i<15947; i++)
//		fprintf(f,"%d\n",i);
//	fclose(f);
}

void CVerxStack::AddFakeSpike( int TimeStamp )
{
	sSpike spikeInf;
	spikeInf.TimeStamp = TimeStamp;
	spikeInf.cluster = 0;
	spikeInf.origCluster = 0;
	spikeInf.fileParams.clear();
	spikeInf.type = 1; //SPIKE_IN_MEMORY;

	// Storing data to container
	CVertex *NewVerx;
	NewVerx=new CVertex(m_PalStack);
	NewVerx->SetOrigCl( 0 );
	NewVerx->SetTimeStamp( TimeStamp ); // 1 ~ [100us]
	NewVerx->AddPnt( 0 );  //empty cluster's set
								
	m_NumVerx++;

	// Convert to real value
	vector<float> m_u(NUM_SAMPLES);

	vector<float> mNewYVal(NUM_SAMPLES*NUM_CHANNELS);
	for (int m_l=0;m_l<(NUM_CHANNELS*NUM_SAMPLES);m_l++)
	{
		mNewYVal[m_l] = (float) 0;
	}
	NewVerx->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);

	NewVerx->AddClust(0);
	m_VerxStack.push_back(NewVerx);

	spikeInf.offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;

	m_SpikesStack.push_back(spikeInf);
}


/////////////////////////////////////////////////////////////////////////////
int CVerxStack::GetBPFHeader_SetupInformation(char *buffer, int nRead, char *errChar)
{
	char	*pdest, *formatInf;
	int		formatInfStart = 0;
	int		bpfEnd = 0;
	
	formatInf = strstr(buffer, SECTION_SETUP_INFORMATION);
	if (formatInf == NULL)
		return 0;
	formatInfStart = formatInf - buffer + 1;
	

	// SPIKE_SAMPLING_FREQUENCY
	pdest = strstr(formatInf,SPIKE_SAMPLING_FREQUENCY);
	if ( pdest == NULL )
	{	}
	else
	{	int tmp = GetNumberRoundBr(pdest);
		if(tmp > 0) //when tmp <= 0 no spikes no spikes recorded so use default value
			CVertex::FREQ = SAMPLE_FREQ = tmp;
	}

	// EEG_SAMPLING_FREQUENCY
	pdest = strstr(formatInf,EEG_SAMPLING_FREQUENCY);
	if ( pdest == NULL )
	{	}
	else
	{	
		EEG_SAMPLE_FREQ = GetNumberRoundBr(pdest);
	}

	//get # of channels
	pdest = strstr(formatInf,NUMBER_OF_EEG_CHANNELS);
	if(pdest)
		bpf.NumberOfEegChannels = GetNumberRoundBr(pdest);

	SetupEEGChanNames(formatInf);

	return 0;
}

void CVerxStack::SetupEEGChanNames(char* formatInf)
{
	//setup default channel names
	bpf.vEEGNames.clear();
	int j;
	for(j=0;j<bpf.NumberOfEegChannels;j++)
	{
		CString tmp;
		tmp.Format("eeg.%d",j);
		bpf.vEEGNames.push_back(string(tmp));
	}

	//look for channel names stored in bpf file
	string strFormatInf = formatInf, strSplit = CHANNEL_LABEL;
	strSplit += "0 ( ";
	char* pdest = strstr(formatInf,strSplit.c_str());
	int iChan = 0;
	while(pdest)
	{	pdest = strstr(pdest,"\"");
		string strName("");
		int idx = 1;
		while(pdest[idx]!='\"' && pdest[idx]!=0)
		{	strName += pdest[idx];
			idx++;
		}
		if(strName.length() && iChan<bpf.vEEGNames.size()) 
			bpf.vEEGNames[iChan++]=strName;
		pdest = strstr(pdest,strSplit.c_str());
	}	
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::GetBPFHeader(char *buffer, int nRead, char *errChar)
{
	char	*pdest, *formatInf;
	int		formatInfStart = 0;
	int		bpfEnd = 0;
	
	formatInf = strstr(buffer, SECTION_RECORD_FORMAT_INFORMATION);
	if (formatInf == NULL)
		return 0;
	formatInfStart = formatInf - buffer + 1;
	

	// EEG_TYPE
	pdest = strstr(formatInf,EEG_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// EEG_RECORD_FORMAT
		bpf.EEG_TYPE = GetType(pdest);
		pdest = strstr(pdest,EEG_RECORD_FORMAT);
		if ( pdest == NULL )
		{
			*errChar = 'e';
			return 0;
		}
		else
		{
			bpf.EEG_SIZE = GetNumber(pdest);
		}
	}

	// SYNC_TYPE
	pdest = strstr(formatInf,SYNC_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// SYNC_RECORD_FORMAT
		bpf.SYNC_TYPE = GetType(pdest);
		pdest = strstr(pdest,SYNC_RECORD_FORMAT);
		if ( pdest == NULL )
			return 0;
		else
		{
			bpf.SYNC_SIZE = GetNumber(pdest);
		}
	}
	
	// SINGLE_TYPE
	pdest = strstr(formatInf,SINGLE_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// SINGLE_RECORD_FORMAT
		bpf.SINGLE_TYPE = GetType(pdest);
		pdest = strstr(pdest,SINGLE_RECORD_FORMAT);
		if ( pdest == NULL )
			return 0;
		else
		{
			bpf.SINGLE_SIZE = GetNumber(pdest);
		}
	}

	// STEREO_TYPE
	pdest = strstr(formatInf,STEREO_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// STEREO_RECORD_FORMAT
		bpf.STEREO_TYPE = GetType(pdest);
		pdest = strstr(pdest,STEREO_RECORD_FORMAT);
		if ( pdest == NULL )
			return 0;
		else
		{
			bpf.STEREO_SIZE = GetNumber(pdest);
		}
	}

	// TETRODE_TYPE
	pdest = strstr(formatInf,TETRODE_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// TETRODE_RECORD_FORMAT
		bpf.TETRODE_TYPE = GetType(pdest);
		pdest = strstr(formatInf,TETRODE_RECORD_FORMAT);
		if ( pdest == NULL )
			return 0;
		else
		{
			bpf.TETRODE_SIZE = GetNumber(pdest);

			char VOLTAGE_ID[8];
			sprintf(VOLTAGE_ID,"Voltage");
			pdest = strstr(pdest,VOLTAGE_ID);
			if ( pdest == NULL )
				return 0;
			else
			{
				CVertex::SAMPLES = NUM_SAMPLES = GetNumberBr(pdest);
			}
		}
	}

	// ARENA_POS_TYPE
	pdest = strstr(formatInf,ARENA_POSITION_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// ARENA_POS_RECORD_FORMAT
		bpf.ARENA_POSITION_TYPE = GetType(pdest);
		pdest = strstr(formatInf,ARENA_POSITION_RECORD_FORMAT);
		if ( pdest == NULL )
		{	}
		else
		{
			bpf.ARENA_POSITION_SIZE = GetNumber(pdest);
		}
	}

	// ROOM_POS_TYPE
	pdest = strstr(formatInf,ROOM_POSITION_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// ROOM_POS_RECORD_FORMAT
		bpf.ROOM_POSITION_TYPE = GetType(pdest);
		pdest = strstr(formatInf,ROOM_POSITION_RECORD_FORMAT);
		if ( pdest == NULL )
		{	}
		else
		{
			bpf.ROOM_POSITION_SIZE = GetNumber(pdest);
		}
	}

	// KEY_EVENT_TYPE
	pdest = strstr(formatInf,KEY_EVENT_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// KEY_EVENT_RECORD_FORMAT
		bpf.KEY_EVENT_TYPE = GetType(pdest);
		pdest = strstr(pdest,KEY_EVENT_RECORD_FORMAT);
		if ( pdest == NULL )
			return 0;
		else
		{
			bpf.KEY_EVENT_SIZE = GetNumber(pdest);
		}
	}

	// INPUT_EVENT_TYPE
	pdest = strstr(formatInf,INPUT_EVENT_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// INPUT_EVENT_RECORD_FORMAT
		bpf.INPUT_EVENT_TYPE = GetType(pdest);
		pdest = strstr(pdest,INPUT_EVENT_RECORD_FORMAT);
		if ( pdest == NULL )
			return 0;
		else
		{
			bpf.INPUT_EVENT_SIZE = GetNumber(pdest);
		}
	}

	// OUTPUT_EVENT_TYPE
	pdest = strstr(formatInf,OUTPUT_EVENT_ID);
	if ( pdest == NULL )
	{	}
	else
	{	// OUTPUT_EVENT_RECORD_FORMAT
		bpf.OUTPUT_EVENT_TYPE = GetType(pdest);
		pdest = strstr(pdest,OUTPUT_EVENT_RECORD_FORMAT);
		if ( pdest == NULL )
			return 0;
		else
		{
			bpf.OUTPUT_EVENT_SIZE = GetNumber(pdest);
		}
	}
	
	// Find END_HEADER
	pdest = strstr(buffer, HEADER_END_LABEL);
	if ( pdest == NULL )
		return 0;
	bpfEnd = pdest - buffer + 1 + 13;
	return bpfEnd;
}

void CVerxStack::ClearVertexWaveData()
{
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		((CVertex*)*Index)->ClearWaveformData();
	}
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::LoadBPF(CFile *file, char *buffer, int nRead, char *errChar, long *errOffset, BatchRec* pQB)
{
	FileType = 1;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	buffer[8191] = '\0';
	CVertex::CHANNELS = NUM_CHANNELS=4; 
	CVertex::SAMPLES = NUM_SAMPLES=64;
	EEG_SAMPLE_FREQ = 2000;
	CVertex::FREQ = SAMPLE_FREQ=32000; 

	//Searching of index of binary data
	long m_i = 0; 
	long startPos;

	GetBPFHeader_SetupInformation(buffer, nRead, errChar);

	m_i = GetBPFHeader(buffer, nRead, errChar);
	
	startPos = m_i;

	if (m_i == 0 )
	{
		file->Close();
		SetEmpty();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		m_FileName.Empty();
		return 2;
	}

	m_x.resize(NUM_SAMPLES);
	CVertex::VX.resize(NUM_SAMPLES);
	for (int i=0;i<NUM_SAMPLES;i++)
		CVertex::VX[i] = m_x[i] = (float) i/SAMPLE_FREQ;

	// Main reading of data
	char	m_Identif;
	char	ElChannel;
	vector<short>	m_buffer;
	vector<float>	m_u,m_dY;
	CVertex *NewVerx;
		
	// inspection in file
	// Number of spikes in each channel
	const char max_elde = 3;
	int ChannelNo[max_elde*256];
	for (i=0; i<max_elde; i++)
		for (int j=0; j<256; j++)
			ChannelNo[i*256+j] = 0;

	// for making scope in SYNC 
	const	ResolutionHistogram = 10; ;
	int		NthToHistogram = 0;
	int		TimeStamp;
	char	isLoaded;	// when typical character found ( Y, S, U, T, A, R, E,....)
	UINT	bLoaded;		// amount of loaded bytes

	// prepare tables
	vector<char> SyncValues(2*1000);
	vector<int> histogram(256);
	std::fill(histogram.begin(),histogram.end(),0);
	
	// data for diagnostic
	dFileOK = 0;
	dLastType = 0;
	dLastPos = 0;
	dFileLength = file->GetLength();

	// go to the start of data flow
	file->Seek(m_i,CFile::begin);
	
	// main loop of pre-reading of BPF
	while (m_i<file->GetLength())
	{
		isLoaded = 0;
		file->Seek(m_i,CFile::begin);
		file->Read(&m_Identif,1);
		
		if ( m_Identif == bpf.EEG_TYPE )
		{	// EEG
			isLoaded = 1;
			*errChar = 'E'; *errOffset = m_i;
			dLastType = 'E'; dLastPos = m_i;
			
			m_i += bpf.EEG_SIZE;
		}
		else if ( m_Identif == bpf.KEY_EVENT_TYPE )
		{	// KEY EVENT
			isLoaded = 1;
			*errChar = 'K'; *errOffset = m_i;
			dLastType = 'K'; dLastPos = m_i;
			
			m_i += bpf.KEY_EVENT_SIZE;
		}		
		else if ( m_Identif == bpf.SYNC_TYPE )
		{	// SYNC
			isLoaded = 1;
			*errChar = 'Y'; *errOffset = m_i;
			dLastType = 'Y'; dLastPos = m_i;

			if (NthToHistogram == ResolutionHistogram)
			{
				
				bLoaded = file->Read(&TimeStamp,4);
				bool bWarned = false;
				if ( bLoaded == 4 )
				{
					bLoaded = file->Read(&SyncValues[0],2000);	// 2000 - SyncData
					if ( bLoaded == 2000 )
					{
						for (i = 0; i<1000; i++)		// 1000	- SyncData
						{	int idx = SyncValues[2*i+1]+127;
							if(idx < 0 || idx >= histogram.size())
							{	if(!bWarned)
								{	Write2Log("WARNING: bad sync value %d , index out of bounds %d !!!",SyncValues[2*i+1],idx);
									bWarned=true;
								}
							}
							else
								histogram[idx]++; // (char)floor(SyncValues[i]/256)
						}
						NthToHistogram = 0;
					}
				}
			}
			NthToHistogram++;
			m_i += bpf.SYNC_SIZE;
		}		
		else if ( m_Identif == bpf.SINGLE_TYPE )
		{	// SINGLE
			isLoaded = 1;
			*errChar = 'U'; *errOffset = m_i;
			dLastType = 'U'; dLastPos = m_i;
		
			
			file->Seek(5,CFile::current);
			bLoaded = file->Read(&ElChannel,1);
			ChannelNo[0*256 + ElChannel] += 1;

			m_i += bpf.SINGLE_SIZE;
		}		
		else if ( m_Identif == bpf.STEREO_TYPE )
		{	// STEREO
			isLoaded = 1;
			*errChar = 'S'; *errOffset = m_i;
			dLastType = 'S'; dLastPos = m_i;
		
			
			file->Seek(5,CFile::current);
			file->Read(&ElChannel,1);
			ChannelNo[1*256 + ElChannel] += 1;

			m_i += bpf.STEREO_SIZE;
		}		
		else if ( m_Identif == bpf.TETRODE_TYPE )
		{	// TETRODE		
			isLoaded = 1;
			*errChar = 'T'; *errOffset = m_i;
			dLastType = 'T'; dLastPos = m_i;
			
			
			file->Seek(4,CFile::current);
			file->Read(&ElChannel,1);
			ChannelNo[2*256 + ElChannel] += 1;

			m_i += bpf.TETRODE_SIZE;
		}		
		else if ( m_Identif == bpf.ARENA_POSITION_TYPE )
		{	// ARENA
			isLoaded = 1;
			*errChar = 'A'; *errOffset = m_i;
			dLastType = 'A'; dLastPos = m_i;
		

			InBpfExist |= 1;
			m_i += bpf.ARENA_POSITION_SIZE;
		}		
		else if ( m_Identif == bpf.ROOM_POSITION_TYPE )
		{	// ROOM		
			isLoaded = 1;
			*errChar = 'R'; *errOffset = m_i;
			dLastType = 'R'; dLastPos = m_i;
			

			InBpfExist |= 2;
			m_i += bpf.ROOM_POSITION_SIZE;
		}		
		else if ( m_Identif == bpf.INPUT_EVENT_TYPE )
		{	// INPUT		
			isLoaded = 1;
			*errChar = 'I'; *errOffset = m_i;
			dLastType = 'I'; dLastPos = m_i;
			
			m_i += bpf.INPUT_EVENT_SIZE;
		}
		else if ( m_Identif == bpf.OUTPUT_EVENT_TYPE )
		{	// OUTPUT
			isLoaded = 1;
			*errChar = 'O'; *errOffset = m_i;
			dLastType = 'O'; dLastPos = m_i;
			
			m_i += bpf.OUTPUT_EVENT_SIZE;
		}
		
		if ( isLoaded == 0 )
		{
			if(m_i+1 == dFileLength){ //allow 1 extra junk char @ end
				m_i += 1;
			}
			else {			
				file->Close();
				SetEmpty();
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				m_FileName.Empty();
				return 2;
			}
		}
	}

	if ( m_i == dFileLength )
	{
		dFileOK = 1;
	}
	else
	{
		
	}

/*	char txt[30];
	FILE *f=fopen("a.a","a");
	fprintf(f,"%d\t%d\t%d...%c...%x\n",dFileOK,dLastPos,dFileLength,dLastType,dLastPos);
	fclose(f);
*/
	const DELTA = 10;
	char HminI=0,HmaxI=0; // histogram min/max index
	int Hmin = 0,Hmax = 0;
	char minFound = 0;

	int histMaxI = 0;
	for (i = 0; i<256; i++)	// find hist. maximum
		if (histogram[i] > histogram[histMaxI])
			histMaxI = i;
	int HpeakI = 0;
	for (i = 0; i<256; i++)
	{
		if ( abs(i-histMaxI) > DELTA && histogram[i]> histogram[HpeakI])
			HpeakI = i;						
	}

	histogram.clear();
	SyncValues.clear();

	// Set the middle 
	m_MainSyncStack.SetMiddle( (HpeakI + (histMaxI-HpeakI)/2 - 127)*256 );
	
	// Check if there are electrode data in BPF 	
	i = 0;
	bpf.NumberOfTetrodeChannels = 0;
	int total_channel_data = max_elde * 256;
	for(i=0;i<total_channel_data;i++) //counts # of channels
		if(ChannelNo[i])
			bpf.NumberOfTetrodeChannels++;
	i = 0;
	while ( i < total_channel_data && ChannelNo[i] == 0)
	{	//gets first channel that has spikes
		i++;
	}
	
	bool make_fake_spike = false;
	// If there are electrode data, show a dialog for choosing	
	if ( i < total_channel_data )
	{	
		// Dialog of selecting electrode and channel
		CSelectTT SelTTDial;
		SelTTDial.ChannelNo = ChannelNo;
		SelTTDial.max_elde = max_elde;
		SelTTDial.FileType = FileType;
		SelTTDial.dFileOK = dFileOK;
		SelTTDial.dLastPos = dLastPos;
		SelTTDial.dLastType = dLastType;

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// Main selection
		if(pQB)
		{
			m_ElectrodeChannel = pQB->iTetrode;
			AmountDataFromFile = static_cast<int>(100.0*pQB->dPrct);
			m_bUseEEG = false;
			m_ElectrodeType = 3; //tetrode
		}
		else
		{
			SelTTDial.DoModal();
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
			// Get values
			m_ElectrodeChannel = SelTTDial.EldeCh;
			AmountDataFromFile = SelTTDial.GetAmount();//SelTTDial.Amount;
			m_bUseEEG =	SelTTDial.m_bUseEEG;
			m_ElectrodeType = SelTTDial.EldeType;
		}
		//these are at defaults in batch mode, and set by user in non-batch mode
		m_FakeSpikes = false;
		m_bUsePCA = SelTTDial.DoPCA();
		m_bCombinePCA = SelTTDial.CatPCA();
		m_iNumPCsToStore = SelTTDial.NumPCs();

//		UnitOfAmount = SelTTDial.UnitOfAmount;
//		int maxSpikes = SelTTDial.EldeMaxAmount;
	} else
	{	
//		int maxSpikes = 1;
		m_FakeSpikes = true;
	}	
		
//	m_MainEEGStack.SetParams( (bpf.EEG_SIZE - 5) / 2000, 1000);
	int iSampBytes = bpf.EEG_SIZE - 5;//# of bytes in an 'E' record, without timestamp and identifier
	int iEEGChans = iSampBytes / EEG_SAMPLE_FREQ;//# of eeg channels
	int iTotalEEGBytes = EEG_SAMPLE_FREQ * 2 * iEEGChans; //total # sampled bytes using full EEG_SAMPLE_FREQ

	if(iEEGChans != bpf.NumberOfEegChannels)
	{	Write2Log("Error with %s bpf file : iEEGChans=%d != bpf.NumberOfEEGChannels=%d",file,iEEGChans,bpf.NumberOfEegChannels);
	}
	
	m_MainEEGStack.SetParams(iEEGChans,((double)iSampBytes/iTotalEEGBytes)*EEG_SAMPLE_FREQ);
	m_MainEEGStack.SetFrequency( EEG_SAMPLE_FREQ );

	m_i = startPos;

	m_buffer = vector<short>(NUM_CHANNELS*NUM_SAMPLES);
	m_u = vector<float>(NUM_SAMPLES);
	m_dY = vector<float>(NUM_SAMPLES);
	vector<float> mNewYVal(NUM_SAMPLES*NUM_CHANNELS);

		// *****************************************************
	srand( (unsigned)time( NULL ) );	
	//srand( 0 );
		// *****************************************************

	float NumbLoaded = 0;
	float Increase = (float) (100 / (float) AmountDataFromFile); // perc.
	int PlusRand = (int) floor(Increase) - 1;
	float IndexNext = Increase;
	int Index = 0;
	int IndexSpike = (int) ( (float) PlusRand*rand()/32768);;
	char isSpike = 0;
	bool bRoomHasZ = bpf.ROOM_POSITION_SIZE > 9,
		 bArenaHasZ = bpf.ARENA_POSITION_SIZE > 9;
	
	while ( m_i < dFileLength )
	{
		isLoaded = 0;
		
		file->Seek(m_i,0);
		file->Read(&m_Identif,1);
			
		if ( m_Identif == bpf.ARENA_POSITION_TYPE ) 
		{	// Arena
			isLoaded = 1;
			m_MainSyncStack.LoadArena(file,bArenaHasZ);
			m_i += bpf.ARENA_POSITION_SIZE;
		}
		else if ( m_Identif == bpf.ROOM_POSITION_TYPE ) 
		{	// Room
			isLoaded = 1;
			m_MainSyncStack.LoadRoom(file,bRoomHasZ);
			m_i += bpf.ROOM_POSITION_SIZE;
		}		
		else if ( m_Identif == bpf.EEG_TYPE ) 
		{	// EEG
			isLoaded = 1;
			if(m_bUseEEG)
				m_MainEEGStack.LoadEEG(file);
			m_i += bpf.EEG_SIZE;
		}			
		else if ( m_Identif == bpf.SYNC_TYPE ) 
		{	//Synchronize	
			isLoaded = 1;
			m_i += bpf.SYNC_SIZE;
			m_MainSyncStack.LoadSync(file);			
		}		
		else if ( m_Identif == bpf.KEY_EVENT_TYPE )
		{	// KEY EVENT
			isLoaded = 1;			
			m_i += bpf.KEY_EVENT_SIZE;
		}
		else if ( m_Identif == bpf.SINGLE_TYPE ) 
		{	// SINGLE
			isLoaded = 1;
			file->Read(&TimeStamp,4);
			file->Read(&ElChannel,1);

			// Add file portrait
			filePortraitStack.push_back(sSpikeInFile(1,ElChannel));

			m_i += bpf.SINGLE_SIZE;
		}
		else if ( m_Identif == bpf.STEREO_TYPE ) 
		{	// STEREO
			isLoaded = 1;
			file->Read(&TimeStamp,4);
			file->Read(&ElChannel,1);
			
			// Add file portrait
			filePortraitStack.push_back(sSpikeInFile(2,ElChannel));

			m_i += bpf.STEREO_SIZE;
		}
		else if ( m_Identif == bpf.TETRODE_TYPE ) 
		{	// TETRODE
			char OrigCl;
			int TimeStamp;

			isLoaded = 1;
			file->Read(&TimeStamp,sizeof(TimeStamp));
			file->Read(&ElChannel,sizeof(ElChannel));
			file->Read(&OrigCl,sizeof(OrigCl));
			
			// Add file portrait
			filePortraitStack.push_back(sSpikeInFile(4,ElChannel));

			// we need this channel
			if ( ElChannel == m_ElectrodeChannel )
			{
				sSpike spikeInf;
				spikeInf.TimeStamp = TimeStamp;
				spikeInf.offset = m_i;
				spikeInf.type = 0; //SPIKE_ON_DISK; // on disk
				spikeInf.cluster = OrigCl;
				spikeInf.origCluster = OrigCl;
				spikeInf.fileParams.clear();
				if ( OrigCl > m_NumOriginalCl )
					m_NumOriginalCl = OrigCl;

				if ( Index >= IndexSpike && ! isSpike )
				{
					isSpike = 1;						

					// Storing data to container
					NewVerx=new CVertex(m_PalStack);
					NewVerx->SetOrigCl(OrigCl);
					NewVerx->SetTimeStamp( TimeStamp ); // 1 ~ [100us]
					NewVerx->AddPnt( 0 );  //empty cluster's set
												
					file->Read(&m_buffer[0],m_buffer.size()*sizeof(short));
					m_NumVerx++;
	
					// Convert to real value
					for (int m_l=0;m_l<m_buffer.size();m_l++)
					{
						mNewYVal[m_l] = (float) (10*m_buffer[m_l])/SHRT_MAX;
					}
					NewVerx->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
				
					NewVerx->AddClust(0);
					m_VerxStack.push_back(NewVerx);

					spikeInf.type = 1; //SPIKE_IN_MEMORY;
					spikeInf.offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;
				}
				Index++;
				if (Index >= IndexNext)
				{
					IndexSpike =  (IndexNext +  (float) PlusRand * rand() /32768  );
					IndexNext += Increase;
					isSpike = 0;
				}
				m_SpikesStack.push_back(spikeInf);
			}
			m_i += bpf.TETRODE_SIZE;
		} // end loading T param
		else if ( m_Identif == bpf.INPUT_EVENT_TYPE ) 
		{	// INPUT EVENT
			isLoaded = 1;
			m_i += bpf.INPUT_EVENT_SIZE;
		}
		else if ( m_Identif == bpf.OUTPUT_EVENT_TYPE )
		{	// OUTPUT EVENT
			isLoaded = 1;
			m_i += bpf.OUTPUT_EVENT_SIZE;
		}
	
		if ( isLoaded == 0 )
		{
			if(m_i+1==dFileLength)
			{
				m_i+=1;
			}
			else
			{
				file->Close();
				SetEmpty();
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

				m_FileName.Empty();
				return 2;
			}
		}
	} //while not eof
			
	if ( m_FakeSpikes && m_bUseEEG)
	{		
		// Adding 2 spikes, 
		AddFakeSpike( m_MainEEGStack.GetFirstTimeStamp() );
		AddFakeSpike( m_MainEEGStack.GetLastTimeStamp() + 5000);
	}

	if ( InBpfExist == 0 )
		m_MainSyncStack.CreateSync();

	file->Close();
	
	return 0;			
}

bool CVerxStack::ImportSpikes(char* path)
{	try{
	ClearHighDim();
	SetEmpty();

	CVertex::CHANNELS = NUM_CHANNELS = 4; 
	CVertex::SAMPLES = NUM_SAMPLES = 32;

	int i = 0;

	m_x.resize(NUM_SAMPLES);
	CVertex::VX.resize(NUM_SAMPLES);
	for (i=0;i<NUM_SAMPLES;i++)
		CVertex::VX[i] = m_x[i] = (float) i/SAMPLE_FREQ;

	// do actual loading here
	// line 1 - # of tetrodes
	// line 2 - # of samples per spike on single wire
	// line 3... n - waveform record
	//record structure : clusterID numsamples*numtetrodes
	//first waveform for tetrode 1 , then tetrode 2 , then 3, then 4
	std::ifstream f;
	f.open(path);
	if(!f.is_open())
		return false;
	f >> CVertex::CHANNELS;
	NUM_CHANNELS = CVertex::CHANNELS;
	f >> CVertex::SAMPLES;
	NUM_SAMPLES = CVertex::SAMPLES;
	int samps = NUM_CHANNELS*NUM_SAMPLES;
	if(!samps)
		return false;
	vector<float> v(samps,0.0) , m_u(NUM_SAMPLES);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	while(true)
	{
		int cid;
		f >> cid;
		if(f.eof())
			break;
		for(i=0;i<samps && !f.eof();i++)
			f >> v[i];

		if(i<samps && f.eof()) // full waveform not read in
			break;

		// TETRODE
		char OrigCl = cid;
		int TimeStamp = 0;
		int ElChannel = 0;
		
		// Add file portrait
		filePortraitStack.push_back(sSpikeInFile(4,ElChannel));

		sSpike spikeInf;
		spikeInf.TimeStamp = TimeStamp;
		spikeInf.offset = 0;
		spikeInf.type = 1; //SPIKE_IN_MEMORY;
		spikeInf.cluster = OrigCl;
		spikeInf.origCluster = OrigCl;
		spikeInf.fileParams.clear();
		if ( OrigCl > m_NumOriginalCl )
			m_NumOriginalCl = OrigCl;

		// Storing data to container
		CVertex* NewVerx=new CVertex(m_PalStack);
		NewVerx->SetOrigCl(OrigCl);
		NewVerx->SetTimeStamp( TimeStamp ); // 1 ~ [100us]
		NewVerx->AddPnt( 0 );  //empty cluster's set
									
		m_NumVerx++;

		NewVerx->SetYValues(v, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
	
		NewVerx->AddClust(0);
		m_VerxStack.push_back(NewVerx);
		
		spikeInf.offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;

		m_SpikesStack.push_back(spikeInf);

		if(f.eof())
			break;
	}
	f.close();

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	//check if spikes were loaded
	if (m_NumVerx == 0)
	{
		AreData=0;
		return false;
	}
	else
	{
		m_NumVerxAll = m_SpikesStack.size();
		m_MainSyncStack.SetFileType(FileType);
		CalcAfterLoad();
		m_FileName = path;
		Write2Log("Imported %d spikes. samples=%d channels=%d",m_NumVerx,NUM_SAMPLES,NUM_CHANNELS);
		return true;
	}}
catch(...){
	AreData=0;
	Write2Log("Exception in ImportSpikes!");
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return false;
}
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::LoadVerx( char *errChar, long *errOffset, BatchRec* pQB)
{
	m_MainSyncStack.SetFileType(0);
	whichDraw = 1;
	CString cstrBPFPath;
	if(pQB)
		cstrBPFPath=pQB->strBPF.c_str();
	else
	{
	    static char BASED_CODE szFilter[] = "Brain potential files (*.bpf, *.uff)|*.bpf;*.uff|All Files (*.*)|*.*||";
		CFileDialog dlg(TRUE, "bpf", "*.bpf;*.uff",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
		if(dlg.DoModal()!=IDOK) return 3;
		cstrBPFPath=dlg.GetPathName();
	}

	CFile file;
	if (!file.Open(cstrBPFPath,CFile::modeRead))
	{
		Write2Log("LoadVerx ERR: Couldn't open %s !",cstrBPFPath.GetBuffer());
		*errChar = 'o';
		*errOffset = 0;
		return 2;
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));		

	m_FileName=m_TmpName=cstrBPFPath;
	m_FileNameWithoutExt=GetFilePath(cstrBPFPath,false);
	CString cstrExt = GetFileExt(cstrBPFPath); //extension with "."

	// Set TMP file name			
	int i = m_FileName.GetLength();
	m_TmpName.SetAt(i-3,'T');
	m_TmpName.SetAt(i-2,'M');
	m_TmpName.SetAt(i-1,'P');

	// Remove old data from memory
	SetEmpty();

	// Creating name of WCP files (WClust Parameter)
	wcpFileNameMask = "*.wcp";
	int length = m_FileName.GetLength() - cstrExt.GetLength();
	for (i = 0; i < length; i++)
		wcpFileNameMask.Insert(i, cstrBPFPath.GetAt(i));

	wcpPath.Empty();
	wcpPath = GetDir(cstrBPFPath);

	// Finding WCP files
	CheckWcpFiles();

	// Loading first bytes from file
	vector<char> buffer(8192);
	if(buffer.size()!=8192)
	{	//CWnd::MessageBox(TEXT("Error: Out of memory"),NULL,MB_OK|MB_ICONERROR);
		m_FileName.Empty();
		FileType = 0;
		file.Close();
		return 1;
	}

	int nRead=file.Read(&buffer[0],8192);
	if ( nRead == 0 )
	{
		file.Close();
		FileType = 0;
		return 2;
	}

	int errorNO;
	if ( buffer[0] == HEADER_BYTE_0 && buffer[1] == HEADER_BYTE_1 )
	{	// UFF file
		// Prepare memory for file
		buffer.resize(file.GetLength()+1);
		if (buffer.size()!=file.GetLength()+1)
		{	
			m_FileName.Empty();
			FileType = 0;
			file.Close();
			return 1;
		}
		// Loading file to memory
		file.Seek(0,CFile::begin);
		int nRead=file.Read(&buffer[0],file.GetLength());
		if ( nRead == 0 )
		{
			file.Close();
			SetEmpty();
			FileType = 0;
			return 2;
		}

		// get data from buffer
		errorNO = LoadUFF(&file, &buffer[0], nRead);
		if (errorNO != 0)
		{
			FileType = 0;
			file.Close();
			return errorNO;
		}
		file.Close();
	}
	else
	{	// BPF file
		errorNO = LoadBPF(&file, &buffer[0], nRead, errChar, errOffset, pQB);
		if (errorNO != 0)
		{
			FileType = 0;
			return errorNO;
		}
	}
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	
	if (m_NumVerx == 0)
	{
		AreData=0;
	}
	else
	{
		m_NumVerxAll = m_SpikesStack.size();
		m_MainSyncStack.SetFileType(FileType);
		CalcAfterLoad();
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust)
{
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		(*Index)->Draw(pDC,DrawWin,whichValues,whichAxes,Clust);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::DrawAutoC(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize,char whichAutoC)
{
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		((CVertex*)*Index)->DrawAutoC(pDC,DrawWin,whichValues,whichAxes,Clust,PntSize,whichAutoC);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize)
{
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		(*Index)->Draw(pDC,DrawWin,whichValues,whichAxes,Clust,PntSize);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::DrawOrig(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust)
{
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
	//	((CVertex*)*Index)->DrawOrig(pDC,DrawWin,whichValues,whichAxes,Clust);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVerxStack::DrawOrig(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize)
{
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		((CVertex*)*Index)->DrawOrig(pDC,DrawWin,whichValues,whichAxes,Clust,PntSize);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVerxStack::DrawSpikes(CDC *pDC,CRect DrawWin,int whichChanel,int whichClust)
{	// Spikes in small window at once
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		((CVertex*)*Index)->DrawSpikes(pDC, &m_x[0],SAMPLE_FREQ, NUM_SAMPLES, DrawWin, whichChanel, whichClust, whichDraw);
	}
	
	int Numb=0;
	float y, yPrev;
	vector<float> average(DrawWin.Width());
	std::fill(average.begin(),average.end(),0);
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		((CVertex*)*Index)->CalcAvgSpike(&m_x[0], SAMPLE_FREQ, NUM_SAMPLES, DrawWin,whichChanel, whichClust, &average[0], &Numb, whichDraw);
	}
	
	if (Numb > 0)
	{
		for (int i = 0; i<DrawWin.Width(); i++)
		{
			average[i] /= Numb;
		}

		CPen newPen(PS_SOLID,1,RGB(0,0,0));
		CPen *pOldPen=pDC->SelectObject(&newPen);

		yPrev = DrawWin.bottom - (average[0])*DrawWin.Height()/20 - DrawWin.Height()/2;
		for (i=DrawWin.left+1;i<DrawWin.right;i++)
		{
			y = DrawWin.bottom - average[i-DrawWin.left]*DrawWin.Height()/20 - DrawWin.Height()/2;
			pDC->MoveTo(i,yPrev);
			pDC->LineTo(i+1,y);
			yPrev=y;
		}
		pDC->SelectObject(pOldPen);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVerxStack::DrawSpikes(CDC *pDC,CRect DrawWin,int whichChanel,int Start,int Stop)
{	// Spikes in play window after resizing
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin()+Start;Index!=m_VerxStack.begin()+Stop;Index++)
	{
		((CVertex*)*Index)->DrawSpikes(pDC, &m_x[0], SAMPLE_FREQ, NUM_SAMPLES, DrawWin, whichChanel);
	}
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::DrawSpikes(CDC *pDC,CRect DrawWin,int whichChanel,int Start, int HowMany, int *Last)
{	// Spikes in play window after play
	MY_STACK::iterator Index;	
	int Numb = 0;
	*Last = 0;
	
	Index = (m_VerxStack.begin() + Start);
	
	while ( Index != m_VerxStack.end() && Numb < HowMany )
	{
		if ( ((CVertex*)*Index)->DrawSpikes(pDC, &m_x[0], SAMPLE_FREQ, NUM_SAMPLES, DrawWin, whichChanel) )
			Numb++;
		Index++;
		*Last += 1;
	}
	if ( Index == m_VerxStack.end() )
		return 1; // all points were drawn
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::DrawTimeSpikes(CDC *pDC, CRect drawWin, int startTime, int timeWindow, int TSFirst, char params, unsigned char *clView, CFile *file)
{
	// find the FIRST spike
	timeWindow *= 10;
	int minT,maxT;	
	int startTS = startTime * 10 + TSFirst;
	int indexSpike = m_SpikesStack.size() / 2 ; 
	int increment = indexSpike;
	minT = 0; maxT = m_SpikesStack.size() - 1;
	char finishedFind = 0;
	while (increment > 0)
	{
		if ( startTS > (m_SpikesStack.begin() + indexSpike)->TimeStamp )
		{
			minT = indexSpike;
			increment = (maxT - minT)/2;
			indexSpike += increment;
		}
		else{
			if ( startTS < (m_SpikesStack.begin() + indexSpike)->TimeStamp )
			{
				maxT = indexSpike;
				increment = (maxT - minT)/2;
				indexSpike -= increment;
			}
			else{
				// ==
				finishedFind = 1;
				increment = 0;
			}
		}
	}
	if ( ! finishedFind )
	{
		indexSpike = minT;
		if ( startTS > (m_SpikesStack.begin() + indexSpike)->TimeStamp )
		{
			indexSpike = maxT;
		}
		else indexSpike = minT;
	}
	

	// Draw
	int x1,x2;
	char fine = 0;
	float ratioTS = (float)drawWin.Width() / (float)timeWindow;
	int indexFirst = indexSpike;
	if (indexSpike >= m_SpikesStack.size())
	{
//		indexSpike = m_SpikesStack.size();
		fine = 1;
	}
	if ( (m_SpikesStack.begin() + indexSpike)->TimeStamp >= startTS )
	{ 
		unsigned char	toDraw = 0;
		unsigned char	bite = 4;
		for ( int i = 0; i < NUM_CHANNELS; i++ )
		{
			if ( params & ( bite << i ) )
				toDraw++;
		}
		int		ySize1Spike = drawWin.Height() / toDraw;

//		if ( (m_SpikesStack.begin() + indexSpike) != m_SpikesStack.begin() )
//		{
//			indexSpike--;
//		}

		

		while ( ! fine && (m_SpikesStack.begin() + indexSpike)->TimeStamp < startTS + timeWindow )
		{
			char draw = (whichDraw == 0) ? clView[(m_SpikesStack.begin() + indexSpike)->cluster] & 1 : clView[(m_SpikesStack.begin() + indexSpike)->origCluster] & 1;
			if ( draw )
			{
				x1 = drawWin.left + ratioTS * ((m_SpikesStack.begin() + indexSpike)->TimeStamp - startTS);
				x2 = drawWin.left + ratioTS * ((m_SpikesStack.begin() + indexSpike)->TimeStamp - startTS + NUM_SAMPLES*10000/SAMPLE_FREQ);
//char a[120];
//sprintf(a,"%x",params);
//pDC->TextOut(10,25,a);

				int	yWin = 0;
				if ( (params & 3) == 0 )
				{	// RASTER
					COLORREF color = ( whichDraw == 0 ) ? (m_SpikesStack.begin() + indexSpike)->cluster : (m_SpikesStack.begin() + indexSpike)->origCluster;
				
					CPen newPen(PS_SOLID,1, m_PalStack->GetSColor( color ));
					CPen *pOldPen = pDC->SelectObject(&newPen);
					for ( int nextCh = 0; nextCh < toDraw; nextCh++ )
					{
						pDC->MoveTo(x1,drawWin.top + nextCh * ySize1Spike + ySize1Spike / 10);
						pDC->LineTo(x1,drawWin.top + ( nextCh + 1 ) * ySize1Spike);
					}
					pDC->SelectObject(pOldPen);
				}
				if ( (params & 3) == 1 )
				{	// RASTER/SPIKE
					if ( (m_SpikesStack.begin() + indexSpike)->type & 1 )
					{	// Spike in memory -> SPIKE
						CRect spikeWin;
						for ( int nextCh = 0; nextCh < NUM_CHANNELS; nextCh++ )
						{
							if ( params & ( bite << nextCh ) )
							{
								spikeWin.left = x1;
								spikeWin.right = x2;
								spikeWin.top = drawWin.top + yWin * ySize1Spike;
								spikeWin.bottom = drawWin.top + ( yWin + 1 ) * ySize1Spike;
								((CVertex*) *(m_VerxStack.begin() + ( (m_SpikesStack.begin() + indexSpike))->offset))->DrawTimeSpikes(pDC, &m_x[0], SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
								yWin++;
							}
						}
					}	
					else{ // Spike on disk -> RASTER
						COLORREF color = ( whichDraw == 0 ) ? ((m_SpikesStack.begin() + indexSpike))->cluster : ((m_SpikesStack.begin() + indexSpike))->origCluster;
						CPen newPen(PS_SOLID,1, m_PalStack->GetSColor( color ));
						CPen *pOldPen = pDC->SelectObject(&newPen);
						for ( int nextCh = 0; nextCh < toDraw; nextCh++ )
							{
							pDC->MoveTo(x1,drawWin.top + nextCh * ySize1Spike + ySize1Spike / 10);
							pDC->LineTo(x1,drawWin.top + ( nextCh + 1 ) * ySize1Spike);
						}
						pDC->SelectObject(pOldPen);
					}
				}
				if ( (params & 3) == 2 )
				{	// SPIKE
					if ( ((m_SpikesStack.begin() + indexSpike))->type & 1 )
					{	// Spike in memory -> SPIKE
						CRect spikeWin;
						for ( int nextCh = 0; nextCh < NUM_CHANNELS; nextCh++ )
						{
							if ( params & ( bite << nextCh ) )
							{
								spikeWin.left = x1;
								spikeWin.right = x2;
								spikeWin.top = drawWin.top + yWin * ySize1Spike;
								spikeWin.bottom = drawWin.top + ( yWin + 1 ) * ySize1Spike;
								((CVertex*) *(m_VerxStack.begin() + ((m_SpikesStack.begin() + indexSpike))->offset))->DrawTimeSpikes(pDC, &m_x[0], SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
								yWin++;
							}
						}
					}	
					else{ // Spike on disk -> SPIKE
						if ( file == NULL )
						{
							COLORREF color = ( whichDraw == 0 ) ? ((m_SpikesStack.begin() + indexSpike))->cluster : ((m_SpikesStack.begin() + indexSpike))->origCluster;
							CPen newPen(PS_SOLID,1, m_PalStack->GetSColor( color ));
							CPen *pOldPen = pDC->SelectObject(&newPen);
							for ( int nextCh = 0; nextCh < toDraw; nextCh++ )
								{
								pDC->MoveTo(x1,drawWin.top + nextCh * ySize1Spike + ySize1Spike / 10);
								pDC->LineTo(x1,drawWin.top + ( nextCh + 1 ) * ySize1Spike);
							}
							pDC->SelectObject(pOldPen);
						}
						else 
						{
							CVertex *verx;
							verx = (CVertex*) new CVertex(m_PalStack);
							LoadBPFElde(verx, file, ((m_SpikesStack.begin() + indexSpike))->offset);
						
							CRect spikeWin;
							for ( int nextCh = 0; nextCh < NUM_CHANNELS; nextCh++ )
							{
								if ( params & ( bite << nextCh ) )
								{
									spikeWin.left = x1;
									spikeWin.right = x2;
									spikeWin.top = drawWin.top + yWin * ySize1Spike;
									spikeWin.bottom = drawWin.top + ( yWin + 1 ) * ySize1Spike;
									((CVertex*) verx)->DrawTimeSpikes(pDC, &m_x[0], SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
									yWin++;
								}
							}
							delete verx;
						}
					}
				}
			}

			indexSpike++;
			if (indexSpike >= m_SpikesStack.size())
			{
				indexSpike--;
				fine = 1;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::DrawTimeSpikesAutoC(CDC *pDC, CRect drawWin, int startTime, int timeWindow, int TSFirst, char params, unsigned char *clView, CFile *file)
{
	// find the FIRST spike
	timeWindow *= 10;
	int minT,maxT;	
	int startTS = startTime * 10 + TSFirst;
	int indexSpike = m_SpikesStack.size() / 2 ; 
	int increment = indexSpike;
	minT = 0; maxT = m_SpikesStack.size() - 1;
	char finishedFind = 0;
	while (increment > 0)
	{
		if ( startTS > ((m_SpikesStack.begin() + indexSpike))->TimeStamp )
		{
			minT = indexSpike;
			increment = (maxT - minT)/2;
			indexSpike += increment;
		}
		else{
			if ( startTS < ((m_SpikesStack.begin() + indexSpike))->TimeStamp )
			{
				maxT = indexSpike;
				increment = (maxT - minT)/2;
				indexSpike -= increment;
			}
			else{
				// ==
				finishedFind = 1;
				increment = 0;
			}
		}
	}
	if ( ! finishedFind )
	{
		indexSpike = minT;
		if ( startTS > ((m_SpikesStack.begin() + indexSpike))->TimeStamp )
		{
			indexSpike = maxT;
		}
		else indexSpike = minT;
	}
	

	// Draw
	int x1,x2;
	char fine = 0;
	float ratioTS = (float)drawWin.Width() / (float)timeWindow;
	int indexFirst = indexSpike;
	if (indexSpike >= m_SpikesStack.size())
	{
//		indexSpike = m_SpikesStack.size();
		fine = 1;
	}
	if ( ((m_SpikesStack.begin() + indexSpike))->TimeStamp >= startTS )
	{ 
		unsigned char	toDraw = 0;
		unsigned char	bite = 4;
		for ( int i = 0; i < NUM_CHANNELS; i++ )
		{
			if ( params & ( bite << i ) )
				toDraw++;
		}
		int		ySize1Spike = drawWin.Height() / toDraw;

//		if ( (m_SpikesStack.begin() + indexSpike) != m_SpikesStack.begin() )
//		{
//			indexSpike--;
//		}

		

		while ( ! fine && ((m_SpikesStack.begin() + indexSpike))->TimeStamp < startTS + timeWindow )
		{
			sSpike& pSpike = *(m_SpikesStack.begin()+indexSpike);
			//if not in memory, then skip it
			if(!(pSpike.type & 1))
			{
				indexSpike++;
				if (indexSpike >= m_SpikesStack.size())
				{
					indexSpike--;
					fine = 1;
				}
				continue;
			}
			CVertex* verx = (CVertex*) *(m_VerxStack.begin() + pSpike.offset);
			char draw = 0;
			int iMyC = 0;
			switch(whichDraw)
			{
			case CLUST_KM:
				iMyC = verx->GetKmeansClust();
				draw = clView[iMyC] & 1;
				break;
			case CLUST_AP:
				iMyC = verx->GetAPClust();
				draw = clView[iMyC] & 1;
				break;
			case CLUST_INFO:
				iMyC = verx->GetInfoClust();
				draw = clView[iMyC] & 1;
				break;
			case CLUST_KK:
				iMyC = verx->GetKKClust();
				draw = clView[iMyC] & 1;
				break;
			case CLUST_FL:
				iMyC = verx->GetFLClust();
				draw = clView[iMyC] & 1;
				break;
			}

			if ( draw )
			{
				x1 = drawWin.left + ratioTS * (pSpike.TimeStamp - startTS);
				x2 = drawWin.left + ratioTS * (pSpike.TimeStamp - startTS + NUM_SAMPLES*10000/SAMPLE_FREQ);

				int	yWin = 0;
				if ( (params & 3) == 0 )
				{	// RASTER
					COLORREF color = iMyC > 0 ? iMyC : 255;
				
					CPen newPen(PS_SOLID,1, m_PalStack->GetSColor( color ));
					CPen *pOldPen = pDC->SelectObject(&newPen);
					for ( int nextCh = 0; nextCh < toDraw; nextCh++ )
					{
						pDC->MoveTo(x1,drawWin.top + nextCh * ySize1Spike + ySize1Spike / 10);
						pDC->LineTo(x1,drawWin.top + ( nextCh + 1 ) * ySize1Spike);
					}
					pDC->SelectObject(pOldPen);
				}
				if ( (params & 3) == 1 )
				{	// RASTER/SPIKE
					CRect spikeWin;
					for ( int nextCh = 0; nextCh < NUM_CHANNELS; nextCh++ )
					{
						if ( params & ( bite << nextCh ) )
						{
							spikeWin.left = x1;
							spikeWin.right = x2;
							spikeWin.top = drawWin.top + yWin * ySize1Spike;
							spikeWin.bottom = drawWin.top + ( yWin + 1 ) * ySize1Spike;
							verx->DrawTimeSpikes(pDC, &m_x[0], SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
							yWin++;
						}
					}
				}
				if ( (params & 3) == 2 )
				{	// SPIKE
					CRect spikeWin;
					for ( int nextCh = 0; nextCh < NUM_CHANNELS; nextCh++ )
					{
						if ( params & ( bite << nextCh ) )
						{
							spikeWin.left = x1;
							spikeWin.right = x2;
							spikeWin.top = drawWin.top + yWin * ySize1Spike;
							spikeWin.bottom = drawWin.top + ( yWin + 1 ) * ySize1Spike;
							verx->DrawTimeSpikes(pDC, &m_x[0], SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
							yWin++;
						}
					}
				}
			}

			indexSpike++;
			if (indexSpike >= m_SpikesStack.size())
			{
				indexSpike--;
				fine = 1;
			}
		}
	}
}

\
/////////////////////////////////////////////////////////////////////////////
void CVerxStack::ClearExternalParamData()
{
	MY_SPIKE_STACK::iterator indSp;
	for (indSp = m_SpikesStack.begin(); indSp != m_SpikesStack.end(); indSp++)
	{
		indSp->fileParams.clear();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::SetEmpty()
{
	// Remove vectors
	MY_STACK::iterator Index;
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		CVertex* verx = (CVertex*)*Index;
		delete verx;
	}
	m_VerxStack.clear();
	
	InBpfExist = 0;
	LoadedAdd = 0;
	m_MainSyncStack.SetEmpty();
	m_MainEEGStack.SetEmpty();

	int i;
	for(i=0;i<m_AxPopped.size();i++)
		delete m_AxPopped[i];
	m_AxPopped.clear();
	m_vPopped.clear();

	// Remove axes names
	while (m_AxesStack.size()>0)
	{
		CString* myTxt = *(m_AxesStack.end()-1);
		delete myTxt;
		m_AxesStack.pop_back();
	}
	m_AxesStack.clear();
	
	// Remove spikes indices...
	m_SpikesStack.clear();
	filePortraitStack.clear();

	Dimension = 0;
	m_NumVerx = 0;
	m_NumVerxAll = 0;
	m_NumOriginalCl = 0;
	AreData = 0;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcViewVx(int FirstEv,int LastEv,int PercEv)
{
	// Calculate, which vectors will be displayed
	MY_STACK::iterator Index;

	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
		((CVertex*)*Index)->SetDraw(0);
	
	float StackPtr = FirstEv-1;
	float Increase = 100/PercEv;
	int PlusRand = (int) floor(Increase) - 1;
	int IntIndex;
	int maxInd = m_VerxStack.end()-m_VerxStack.begin();
	
	srand( (unsigned)time( NULL ) );

	while (StackPtr<=LastEv-1)
	{
		IntIndex = (int) (StackPtr + (float) PlusRand*rand()/32768);
		if (IntIndex<maxInd)
			((CVertex*) *(m_VerxStack.begin()+IntIndex))->SetDraw(1);
		StackPtr += Increase;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcViewSpike(int FirstEv,int LastEv,int PercEv)
{
	// Calculate, which Spikes will be displayed
	MY_STACK::iterator Index;
	const int iMaxSteps = 100;
	float steps[iMaxSteps];
	float Increase = (float)iMaxSteps/PercEv;

	for (int i=0;i<iMaxSteps;i++)
		steps[i] = Increase;

	CVertex *m_vx;
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		m_vx = (CVertex*)*Index;
		m_vx->SetDrawSpike(0);
	}

	int indI;
	for (Index=m_VerxStack.begin()+FirstEv-1;Index!=m_VerxStack.begin()+LastEv;Index++)
	{
		m_vx = (CVertex*)*Index;
		
		//don't want cluster index out of bounds of steps array
		if(m_vx->GetNoise()) continue;
		
		indI = 	m_vx->GetClust(m_vx->GetValue(0)-1);

		//make sure index doesn't go out of bounds of steps
		if(indI < 0 || indI >= iMaxSteps) continue;

		steps[indI] += 1;
		if ( steps[indI] > Increase )
		{
			m_vx->SetDrawSpike(1);
			steps[indI] -= Increase;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcViewSpike(int FirstEv,int LastEv,int PercEv, unsigned char *clView, char numb)
{
	// Calculate, which Spikes will be displayed
	MY_STACK::iterator Index;
	const int iMaxSteps = 100;
	float steps[iMaxSteps];
	float Increase = (float)iMaxSteps/PercEv;

	for (int i=0;i<iMaxSteps;i++)
		steps[i] = Increase;

	CVertex *m_vx;
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		m_vx = (CVertex*)*Index;
		m_vx->SetDrawSpike(0);
	}

	int indI;
	for (Index=m_VerxStack.begin()+FirstEv-1;Index!=m_VerxStack.begin()+LastEv;Index++)
	{
		m_vx = (CVertex*)*Index;
		
		//don't want cluster index out of bounds of steps array
		if(m_vx->GetNoise()) continue;

		indI = 	m_vx->GetClust(m_vx->GetValue(0)-1);

		//make sure index doesn't go out of bounds
		if(indI < 0 || indI >= iMaxSteps) continue;

		steps[indI] += 1;
		if ( steps[indI] > Increase )
		{
			if ( clView[indI] & numb )
				m_vx->SetDrawSpike(1);
			steps[indI] -= Increase;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcViewSpike(int FirstEv,int LastEv,int PercEv, int Clust)
{
	// Calculate, which Spikes will be displayed

	MY_STACK::iterator Index;
	const int iMaxSteps = 100;
	float steps[iMaxSteps];

	float Increase = (float)iMaxSteps/PercEv;

	for (int i=0;i<iMaxSteps;i++)
		steps[i] = Increase;

	CVertex *m_vx;
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		m_vx = (CVertex*)*Index;
		m_vx->SetDrawSpike(0);
	}

	int indI;
	for (Index=m_VerxStack.begin()+FirstEv-1; Index!=m_VerxStack.begin()+LastEv; Index++)
	{
		m_vx = (CVertex*)*Index;

		//don't want cluster index out of bounds of steps array
		if(m_vx->GetNoise()) continue;

		indI = 	m_vx->GetClust(m_vx->GetValue(0)-1);

		if(indI < 0 || indI >= iMaxSteps) continue;

		if (indI == Clust)
		{
			steps[indI] += 1;
			if ( steps[indI] > Increase )
			{
				m_vx->SetDrawSpike(1);
				steps[indI] -= Increase;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::StoreBPFAddition()
{
	forStoreBPF_file.Seek(0,CFile::end);
	if ( LoadedAdd & 1 )
	{
		// Arena
		m_MainSyncStack.SaveArenaFirst(&forStoreBPF_file);
		InBpfExist |= 1;
		LoadedAdd &= 0xFE;
	}
	if ( LoadedAdd & 2 )
	{
		// Arena
		m_MainSyncStack.SaveRoomFirst(&forStoreBPF_file);
		InBpfExist |= 2;
		LoadedAdd &= 0xFD;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::PrepareStoreBPF()
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	forStoreBPF_buffer = vector<short>(NUM_CHANNELS*NUM_SAMPLES);
	if ( !forStoreBPF_buffer.size() )
		return 2;

	forStoreBPF_u = vector<float>(NUM_SAMPLES);
	if ( !forStoreBPF_u.size() )
		return 2;

	if (!forStoreBPF_file.Open(m_FileName, CFile::modeReadWrite))
	{
		forStoreBPF_buffer.clear();
		forStoreBPF_u.clear();
		return 1;
	}

	forStoreBPF_IndexFile = GetBPFDataPtr( &forStoreBPF_file );
	if (forStoreBPF_IndexFile == 0)
	{
		forStoreBPF_buffer.clear();
		forStoreBPF_u.clear();
		forStoreBPF_file.Close();
		return 3;
	}
	
	forStoreBPF_IndexVx = m_VerxStack.begin();
	forStoreBPF_ArenaIndex = 0;
	forStoreBPF_RoomIndex = 0;
	forStoreBPF_SpikeIndex = -1;
	forStoreBPF_Spike = &*m_SpikesStack.begin();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::FinishStoreBPF()
{
	forStoreBPF_buffer.clear();
	forStoreBPF_u.clear();
	forStoreBPF_file.Close();
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::LoadBPFElde(CVertex *pnt, CFile *file, int offset)
{	
	int		TimeStamp;
	char	Channel;
	char	OrigCl;

	vector<short> m_buffer(NUM_CHANNELS*NUM_SAMPLES);
	vector<float> m_u(NUM_SAMPLES);

	file->Seek(offset+1,CFile::begin);
	file->Read(&TimeStamp,4);
	file->Read(&Channel,1);
	file->Read(&OrigCl,1);
	// Storing data to container
	pnt->SetOrigCl(OrigCl);
	pnt->SetTimeStamp(float( TimeStamp));
	pnt->AddPnt( 0 );  //empty cluster's set
												
	if (m_ElectrodeType == 1)
	{ // Single
	}
	
	if (m_ElectrodeType == 2)
	{ //Stereode
	}

	if (m_ElectrodeType == 3)
	{	// Tetrode
		file->Read(&m_buffer[0],NUM_CHANNELS*NUM_SAMPLES*sizeof(short));
	}
				
	// Convert to real value
	vector<float> mNewYVal(NUM_SAMPLES*NUM_CHANNELS);
	for (int m_l=0;m_l<(NUM_CHANNELS*NUM_SAMPLES);m_l++)
		mNewYVal[m_l] = (float) (10*( m_buffer[m_l] ))/SHRT_MAX;

	pnt->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
	pnt->AddClust(0);
	CalcOneSpike(pnt);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::StoreBPFElde()
{
	char DiskrUnit;

	if (m_ElectrodeType == 1)
	{ // Single
		forStoreBPF_file.Seek(forStoreBPF_IndexFile + 6,0);
		DiskrUnit = forStoreBPF_NonClVx->GetNoise() ? 0 : LookupVClust(forStoreBPF_SpikeIndex,whichDraw);
		forStoreBPF_file.Write(&DiskrUnit,1);
		forStoreBPF_IndexFile += bpf.SINGLE_SIZE;
	}
	
	if (m_ElectrodeType == 2)
	{ //Stereode
		forStoreBPF_file.Seek(forStoreBPF_IndexFile + 6,0);
		DiskrUnit = forStoreBPF_NonClVx->GetNoise() ? 0 : LookupVClust(forStoreBPF_SpikeIndex,whichDraw);
		forStoreBPF_file.Write(&DiskrUnit,1);
		forStoreBPF_IndexFile += bpf.STEREO_SIZE;
	}
	
	if (m_ElectrodeType == 3)
	{ // Tetrode
		static int iCount = 0;
		forStoreBPF_file.Seek(forStoreBPF_IndexFile + 6,0);
		DiskrUnit = forStoreBPF_NonClVx->GetNoise() ? 0 : LookupVClust(forStoreBPF_SpikeIndex,whichDraw);
		/*FILE* fp;
		if(iCount==0)
			fp = fopen("ids.txt","w+");
		else
			fp = fopen("ids.txt","a+");
		fprintf(fp,"idx%d du%d lvc%d cnt=%d\n", forStoreBPF_SpikeIndex,DiskrUnit,LookupVClust(forStoreBPF_SpikeIndex),++iCount);
		fclose(fp);*/
		forStoreBPF_file.Write(&DiskrUnit,1);
		forStoreBPF_IndexFile += bpf.TETRODE_SIZE;
	}
	
	delete forStoreBPF_NonClVx;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::StoreBPF(char *State,bool bTSAdjust)
{
	char DiskrUnit;
	CVertex *verx;

	char	isInFile;
	char	m_Identif;
	char	OrigCl;
	long	IndexFile;

	IndexFile = forStoreBPF_IndexFile;

	// Main reading of data
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	while ( IndexFile < forStoreBPF_file.GetLength() )
	{
		isInFile = 0;
		forStoreBPF_file.Seek( IndexFile, 0 );
		forStoreBPF_file.Read( &m_Identif, 1 );
			
		if (m_Identif == bpf.ARENA_POSITION_TYPE) 
		{
		// Arena
			isInFile = 1;
			IndexFile += bpf.ARENA_POSITION_SIZE;
			int TS = m_MainSyncStack.GetTimeStamp(forStoreBPF_ArenaIndex); //
			forStoreBPF_file.Write(&TS,4); //
			forStoreBPF_IndexFile = IndexFile;
			forStoreBPF_ArenaIndex++; //
			InBpfExist |= 1;
		}
		if (m_Identif == bpf.ROOM_POSITION_TYPE) 
		{
			// Room
			isInFile = 1;
			IndexFile += bpf.ROOM_POSITION_SIZE;
			int TS = m_MainSyncStack.GetTimeStamp(forStoreBPF_RoomIndex); //
			forStoreBPF_file.Write(&TS,4); //
			forStoreBPF_IndexFile = IndexFile;
			forStoreBPF_RoomIndex++; //
			InBpfExist |= 2;
		}
		if (m_Identif == bpf.KEY_EVENT_TYPE) 
		{
			// KEY_EVENT
			isInFile = 1;
			IndexFile += bpf.KEY_EVENT_SIZE;
			forStoreBPF_IndexFile = IndexFile;
		}
		if (m_Identif == bpf.INPUT_EVENT_TYPE) 
		{
			// INPUT_EVENT
			isInFile = 1;
			IndexFile += bpf.INPUT_EVENT_SIZE;
			forStoreBPF_IndexFile = IndexFile;
		}
		if (m_Identif == bpf.OUTPUT_EVENT_TYPE) 
		{
			// OUTPUT_EVENT
			isInFile = 1;
			IndexFile += bpf.OUTPUT_EVENT_SIZE;
			forStoreBPF_IndexFile = IndexFile;
		}
		
		if (m_Identif == bpf.EEG_TYPE) 
		{
			// EEG
			isInFile = 1;
			IndexFile += bpf.EEG_SIZE;
			forStoreBPF_IndexFile = IndexFile;
		}
		if (m_Identif == bpf.SYNC_TYPE) 
		{
			//Synchronize
			isInFile = 1;
			IndexFile += bpf.SYNC_SIZE;
			forStoreBPF_IndexFile = IndexFile;
		}
		if (m_Identif == bpf.TETRODE_TYPE) 
		{	//Spikes
			int		TimeStamp;
			char	Channel;
			isInFile = 1;
			//bTSAdjust;
			ULONGLONG TSPos = forStoreBPF_file.GetPosition(); //position of timestamp in file
			forStoreBPF_file.Read(&TimeStamp,4);
			forStoreBPF_file.Read(&Channel,1);
			ULONGLONG CurPos = forStoreBPF_file.GetPosition(); //current position for writing in file
			if (Channel == m_ElectrodeChannel)
			{
				forStoreBPF_SpikeIndex++;
				forStoreBPF_Spike = &*( m_SpikesStack.begin() + forStoreBPF_SpikeIndex );

				int TS;
				if ( forStoreBPF_IndexVx == m_VerxStack.end() )
				{
					verx = (CVertex*) *forStoreBPF_IndexVx;
					TS = verx->GetTimeStamp();
				}
				else 
				{
					TS = 0;
					if(bTSAdjust)
					{	//if timestamp batch readjust mode, rewrite timestamp
						forStoreBPF_file.Seek(TSPos,CFile::begin);
						int TSNew = forStoreBPF_Spike->TimeStamp;
						forStoreBPF_file.Write(&TSNew , 4);
						forStoreBPF_file.Seek(CurPos,CFile::begin);
					}
				}
				if ( TimeStamp == TS )
				{
					DiskrUnit = verx->GetNoise() ? 0 : LookupVClust(forStoreBPF_SpikeIndex,whichDraw);
					forStoreBPF_file.Write(&DiskrUnit,1);
					forStoreBPF_IndexVx ++;
					
					IndexFile += bpf.TETRODE_SIZE;
					forStoreBPF_IndexFile = IndexFile;
				}
				else
				{
					forStoreBPF_file.Read(&OrigCl,1);
					// Storing data to container
					forStoreBPF_NonClVx = new CVertex(m_PalStack);
					forStoreBPF_NonClVx->SetOrigCl(OrigCl);
					forStoreBPF_NonClVx->SetTimeStamp(float( TimeStamp));
					forStoreBPF_NonClVx->AddPnt( 0 );  //empty cluster's set
												
					forStoreBPF_file.Read(&forStoreBPF_buffer[0],NUM_CHANNELS*NUM_SAMPLES*sizeof(short));
						
					// Convert to real value
					vector<float> mNewYVal(NUM_SAMPLES*NUM_CHANNELS);
					for (int m_l=0;m_l<(NUM_CHANNELS*NUM_SAMPLES);m_l++)
						mNewYVal[m_l] = (float) (10 * forStoreBPF_buffer[m_l])/SHRT_MAX;

					forStoreBPF_NonClVx->SetYValues(mNewYVal, m_x, forStoreBPF_u, NUM_CHANNELS, NUM_SAMPLES);
					forStoreBPF_NonClVx->AddClust(0);
					CalcOneSpike(forStoreBPF_NonClVx);
					
					if(m_bUsePCA)
						AppendPCAProjections(forStoreBPF_NonClVx);

					return 5;
				}
			}
			else
			{
				IndexFile += bpf.TETRODE_SIZE;
				forStoreBPF_IndexFile = IndexFile;
			}
		} // end loading T param
		if ( !isInFile )
		{
			return 2;
		}
	} //while not eof

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::StoreUFF(CFile *file)
{
	MY_STACK::iterator Index;
	int		drdb_size;
	char	*dp, *first_rec;
	int		rec_sizes[256];
	int		current_byte;
	char	type;
	char	*buffer;
	char	DiskrUnit;
	CVertex *verx;
	long	fileLength = file->GetLength();
	long	lActual;

	buffer = new char[file->GetLength()];
	if (buffer==NULL)
		return 1;

	// Loading file to memory
	int nRead=file->Read(buffer,file->GetLength());
	if ( nRead == 0 )
	{
		delete [] buffer;
		return 2;
	}

	dp = buffer;

	drdb_size = (int) *( (unsigned short*) (dp + HEADER_DRDB_SIZE_OFFSET) ) + PAD_BYTES;
	dp += HEADER_SIZE + PAD_BYTES;
	file->Seek(HEADER_SIZE + PAD_BYTES, CFile::begin);

	(void) memset((void *) rec_sizes, 0, (size_t) 256 * sizeof(int));
	while (TRUE) {
		if (dp[0] != DRDB_BYTE_0 || dp[1] != DRDB_BYTE_1)
			break; 
		rec_sizes[dp[DRDB_REC_TYPE_OFFSET]] = (int) *( (int*) (dp + DRDB_REC_SIZE_OFFSET) ) + PAD_BYTES; //reverse_ui4(dp + DRDB_REC_SIZE_OFFSET) + PAD_BYTES;
		dp += drdb_size;
		lActual = file->Seek(drdb_size,CFile::current);
	}
		

	m_NumVerx = 0;
	first_rec = dp; 
	current_byte = dp - buffer;
	type = *dp;
	
	Index = m_VerxStack.begin();
	do {
		type = *dp;
		switch (type) {
			case SINGLE_ELECTRODE_SPIKE_REC:
				if ( m_ElectrodeType == 1 )
				{
					char EldeChannel;
					EldeChannel = *(dp + RECORD_PROBE_OFFSET);
					if ( EldeChannel == m_ElectrodeChannel)
					{
						verx = (CVertex*) *Index;
						DiskrUnit = verx->GetClust(verx->GetValue(0)-1);
						*(dp + SPK_REC_CLUST_OFFSET) = DiskrUnit;
					
						file->Seek(SPK_REC_CLUST_OFFSET, CFile::current);
						file->Write(&DiskrUnit,1);
					
						//NewVerx->SetOrigCl( *(dp + SPK_REC_CLUST_OFFSET) );
						//file->Write(&DiskrUnit,1);
						Index++;
					}
				}
				break;
			case STEREOTRODE_SPIKE_REC:
				break;
			case TETRODE_SPIKE_REC:
				if ( m_ElectrodeType == 3 )
				{
					char TetrChannel;
					TetrChannel = *(dp + RECORD_PROBE_OFFSET);
					if ( TetrChannel == m_ElectrodeChannel)
					{
						verx = (CVertex*) *Index;
						DiskrUnit = verx->GetClust(verx->GetValue(0)-1);
						*(dp + SPK_REC_CLUST_OFFSET) = DiskrUnit;
					
						file->Seek(SPK_REC_CLUST_OFFSET, CFile::current);
						file->Write(&DiskrUnit,1);
					
						//NewVerx->SetOrigCl( *(dp + SPK_REC_CLUST_OFFSET) );
						//file->Write(&DiskrUnit,1);
						Index++;
					}
				}
				break;
			case CONTINUOUS_DATA_REC:
				//rp->probe = *(dp + RECORD_PROBE_OFFSET);
				break;
			case POSITION_REC:
				//rp->red_x = *(dp + POS_REC_RED_X_OFFSET);
				//rp->red_y = *(dp + POS_REC_RED_Y_OFFSET);
				//rp->green_x = *(dp + POS_REC_GREEN_X_OFFSET);
				//rp->green_y = *(dp + POS_REC_GREEN_Y_OFFSET);
				break;
			case EVENT_FLAG_REC:
				//rp->probe = *(dp + EV_FLAG_REC_TYPE_OFFSET);
				break;						
		}								
		dp += rec_sizes[type];
		lActual = file->Seek(lActual+rec_sizes[type], CFile::begin);
					
		current_byte += rec_sizes[type];
		if (rec_sizes[type] == 0)
		{
			return 2;
		}

	}while (current_byte < fileLength);



	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::StoreClMarks()
{
	CFile file;
		
	if (!file.Open(m_FileName,CFile::modeReadWrite))
			//CWnd::MessageBox(TEXT("Error: Open brain potential file"),NULL,MB_OK|MB_ICONERROR);
		return 2;
	else
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		
		int errorNo;
		if (FileType == 1)	// BPF
		{
			// Storing of BPF is solved in another place
		}
		
		if (FileType == 2)	// UFF
		{
			errorNo = StoreUFF(&file);
		}

		
		file.Close();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		return errorNo;
	} 
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CVerxStack::SwapPolarity()
{
	ClearHighDim();
	MY_STACK::iterator Index;
	for (Index = m_VerxStack.begin(); Index != m_VerxStack.end(); Index++)
	{
		((CVertex*)*Index)->Swap(NUM_CHANNELS,NUM_SAMPLES);
	}
}

/////////////////////this header is for when exporting features//////////////
void CVerxStack::CreateHeaderForExport(FILE *file,CString& oStrExtra)
{
	char text[100];
	MY_STR_STACK::iterator index;

	fprintf(file,"%%%%BEGIN_HEADER PARAMETER_EXPORT Version 1.0\n\n");
	fprintf(file,"%%%%BEGIN RECORD_FORMAT_INFORMATION\n");

	//////////////
	fprintf(file,"%ParameterNames.0 ( TimeStamp Cluster ");
	for ( index = m_AxesStack.begin(); index != m_AxesStack.end(); index++ )
	{
		sprintf(text,"%s",(LPCTSTR) *((CString*)*index));
		fprintf(file,"%s ",text);
	}
	fprintf(file,")\n");
	
	//////////////////
	fprintf(file,"%RecordFormat.0 ( TimeStamp Cluster ");
	for ( index = m_AxesStack.begin(); index != m_AxesStack.end(); index++ )
	{
		sprintf(text,"%s",(LPCTSTR) *((CString*)*index));
		fprintf(file,"%s ",text);
	}
	fprintf(file,")\n");

	///////////

	fprintf(file,"%%%%END RECORD_FORMAT_INFORMATION\n\n");

	//print any additional header information
	if(oStrExtra.GetLength()) fprintf(file,"%s",oStrExtra.GetString());

	CString strV = GetWClustVersionString();
	fprintf(file,"%%%%WClust version %s\n\n",strV);

	fprintf(file,"%%%%END_HEADER\n");
}

bool CVerxStack::ExportSpikes(char* fname,bool bUpsample)
{
	FILE* fp = fopen(fname,"w");
	if(!fp)
		return false;
	MY_STACK::iterator Index;
	int i,j;
	fprintf(fp,"Cluster ");//first column is cluster id, rest of columns are spike waveforms
	if(bUpsample)
	{	int iResRedFctr = 4;
		vector<PCA_T> vUp(NUM_SAMPLES*RESOLUTION/iResRedFctr+1,0.0f);//vector storing upsampled spike
		for(i=0;i<NUM_CHANNELS;i++)//write header for NQS (or other, can get rid of before load into matlab)
		{	for(j=0;j<vUp.size();j++)
			{	fprintf(fp,"C%dS%d ",i,j);
			}
		}
		fprintf(fp,"\n");
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
		{	CVertex* verx = (CVertex*) *Index;
			fprintf(fp,"%d ",GetVClust(verx,whichDraw));//first column cluster
			for(i=0;i<NUM_CHANNELS;i++)//rest of columns spike waveforms
			{	SplineUpSample(verx,&vUp[0],i,iResRedFctr);
				for(j=0;j<vUp.size();j++) fprintf(fp,"%g ",vUp[j]);
			}
			fprintf(fp,"\n");
		}
	}
	else
	{
		//write header for NQS (or other, can get rid of before load into matlab)
		for(i=0;i<NUM_CHANNELS;i++)
		{	for(j=0;j<NUM_SAMPLES;j++)
			{	fprintf(fp,"C%dS%d ",i,j);
			}
		}
		fprintf(fp,"\n");
		for(Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++)
		{	
			CVertex* verx = (CVertex*) *Index;
			fprintf(fp,"%d ",GetVClust(verx,whichDraw));//first column cluster
			for(i=0;i<NUM_CHANNELS;i++)//rest of clumns spike waveforms
			{	for(j=0;j<NUM_SAMPLES;j++)
				{	fprintf(fp,"%.2f ",verx->GetYValues(NUM_SAMPLES*i+j));
				}
			}
			fprintf(fp,"\n");
		}
	}
	fclose(fp);
	return true;
}

bool CVerxStack::ExportHQPC1Energy(char* fname)
{
	FILE* fp = fopen(fname,"w");
	if(!fp)
	{	Write2Log("CVerxStack::ExportHQPC1Energy : Couldn't open %s !",fname);
		return false;
	}

	DoVertexPCA(true);// used to spline-upsample then peak-align then energy normalize then do PCA
					  // now just does regular pca with energy normalized waveforms

	vector<double>& vICED = ICED();

	if(!vICED.size())
	{	Write2Log("CVerxStack::ExportHQPC1Energy : vICED.size()==0!!!");
		fclose(fp); 
		return false;
	}

	int iSZ = m_VerxStack.size() , idx = 0 , jdx = 0;
	MY_STACK::iterator Index;
	for(Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++)
	{	CVertex* verx = (CVertex*) *Index;
		fprintf(fp,"%d ",verx->GetClust());
		for(jdx=0;jdx<8;jdx++)
			fprintf(fp,"%g ",vICED[idx++]);
		fprintf(fp,"\n");
	}

	fclose(fp);
	
	ClearICED(); //free memory once its no longer needed
	
	return true;
}
