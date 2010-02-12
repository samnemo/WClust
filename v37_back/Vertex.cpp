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
#include <math.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <errno.h> 


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
void CVertex::ExportValues(FILE *file,int* pOverRideCluster)
{
	if(pOverRideCluster)
		fprintf(file,"%d\t%d\t", GetTimeStamp(), pOverRideCluster[0] );
	else
		fprintf(file,"%d\t%d\t", GetTimeStamp(), GetClust() );
	
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
void CVertex::AddClust(int toStore)
{
 	int HowMany = GetValue(0);
	int *newMyClusts;

	newMyClusts = (int*) malloc ((HowMany+1)*sizeof(int));

	for (int i=0; i<HowMany; i++)
		*(newMyClusts+i) = *(m_MyClusts+i);
	*(newMyClusts+HowMany) = toStore;

	if (HowMany!=0)
		free(m_MyClusts);
	m_MyClusts = newMyClusts;
	SetValue(0,HowMany+1);

//	if(toStore<0) 
//		SetNoise(1); 
//	else 
//		SetNoise(0);
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
		m_KmeansClust = -1;
		m_InfoClust = -1;
		m_APClust = -1;
		m_KKClust = -1;
	}
	else
	{
		m_Flag &= 0xFFFFFF7F; 
		if( m_KmeansClust < 0 ) m_KmeansClust = 0;
		if( m_InfoClust < 0 ) m_InfoClust = 0;
		if( m_APClust < 0 ) m_APClust = 0;
		if( m_KKClust < 0 ) m_KKClust = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::SetYValues(float *mNew, float *x,float *u, int channels, int samples)
{
	if (m_Flag&0x0002)
	{
		free(m_YValues);
		free(m_d2YValues);
	}
	m_YValues = mNew;
	m_d2YValues = (float*) malloc(channels * samples * sizeof(float));
	for (int i=0; i<channels; i++)
		Spline(x,&m_YValues[samples*i],samples,&m_d2YValues[samples*i],u);

	m_Flag |= 0x0002;
}

/////////////////////////////////////////////////////////////////////////////
float CVertex::GetYValues(int Index)
{ 
	if ( Index>=0 /*&& Index<256*/ && m_Flag&0x0002 ) 
		return *(m_YValues+Index); 
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
	float fx = *(m_Vertex.begin()+whichAxes.x);
	float fy = *(m_Vertex.begin()+whichAxes.y);
	if (whichValues->IsInRect(fx,fy))
	{
		int x=DrawWin.left+(fx-whichValues->GetLeft())*(DrawWin.right-DrawWin.left)/whichValues->GetSizeX();
		int y=DrawWin.bottom+(fy-whichValues->GetBottom())*(DrawWin.top-DrawWin.bottom)/whichValues->GetSizeY();

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
	if ( m_Flag & 0x0001 ) 
	{
		if(GetNoise() && !(Clust[255] & 1)) return;

		if(false && GetNoise())
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
			if ( (*(Clust+(int) *(m_MyClusts+Index))) & 1 )
			{
				unsigned long color=m_PalStack->GetSColor( (int) *(m_MyClusts+Index) );
				if(GetNoise()) color = RGB(204,204,204);
				DoDraw(pDC,DrawWin,whichValues,whichAxes,PntSize,color);
			}
		}
	}	
}


/////////////////////////////////////////////////////////////////////////////
void CVertex::DrawOrig(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize)
{
	int Index;
	unsigned long color;

	int ShowIt=0;
	Index = *m_Vertex.begin();

#ifdef _DEBUG
	if(m_OrigClust < 0)
		int moo = 9;
#endif

	if ( m_Flag & 0x0001 ) 
	{
		while (Index != 0 && !ShowIt)
		{
			Index--;
			if ( (*(Clust+(int) *(m_MyClusts+Index))) & 1 )
			{
				ShowIt=1;
				color=m_PalStack->GetSColor( (int) (m_OrigClust) );
				if ((int)m_OrigClust == 255)
					MessageBeep(0);
			}
		}
	}

	if (m_OrigClust < 0 || GetNoise())
		color = RGB(204,204,204);

	if ( ShowIt )
	{
		DoDraw(pDC,DrawWin,whichValues,whichAxes,PntSize,color);
	}
	else
	{
		int moo=0;
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
			else
				c = m_KKClust;
			
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
		if ( *(m_MyClusts+Index)==whichClust ) CountIt=1;
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
		color = ( whichSpDraw == 0 ) ? m_PalStack->GetSColor( (int) *(m_MyClusts+Index-1) ) : m_PalStack->GetSColor( GetOrigClust() );
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

	if(GetNoise() && whichDrawMode!=CLUST_USER && whichDrawMode!=CLUST_ORIG)
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
			if ( *(m_MyClusts+Index)==whichClust )
			{
				ShowIt=1;
				color=m_PalStack->GetSColor( (int) *(m_MyClusts + Index) );
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
		int Index;
		unsigned long color;

		Index = *m_Vertex.begin();

		color=m_PalStack->GetSColor( (int) *(m_MyClusts+Index-1) );

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
		float fy;
		float t = param->m_Par1 * Freq;
		fy = Splint(xa, param->m_Par1, (int)floor(t), (int)floor(t)+1, (int) param->m_Par2, Samples);
		AddPnt(fy);
	}	
	else if ( param->GetType() == 3 )
	{	// dY-value in time = par1, chanel = par2
		float fy;
		float t = param->m_Par1 * Freq;
		fy = Splope(xa, param->m_Par1, (int)floor(t), (int)floor(t)+1, (int) param->m_Par2, Samples)/1000;
		AddPnt(fy);
	}
	else if ( param->GetType() == 4 )
	{	// Energy-value in t1 = par1, t2 = par2, chanel = par2
		float fy;
		int t1, t2, shift;
		t1 = (int) floor (param->m_Par1);
		t2 = (int) ceil (param->m_Par2);
		if (t2 > (Samples-1)) 
			t2 = Samples-1;
		shift = Samples * param->m_Par3;
			fy = 0;
		for (int i=t1; i<t2; i++)
		{
			fy += m_YValues[shift+i] * m_YValues[shift+i];
		}

		fy = sqrt(fy);

		AddPnt(fy);
	}
	else if ( param->GetType() == 5 )
	{ // WCP external parameter: par1 = index in names, par2 = channel, par3 = index in spike->paramFile
		float fy;
//	FILE *f = fopen("a.a","a+");
//	fprintf(f,"!%d,%d",(int) param->GetParam( 3 ), spike->fileParams.size() );
//	fclose(f);
		fy = (float) *( spike->fileParams.begin() + (int) param->GetParam( 3 ) );
//	f = fopen("a.a","a+");
//	fprintf(f,"?" );
//	fclose(f);
		AddPnt(fy);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVertex::CheckNoise()
{
//	if(this->m_KmeansClust < 0 ||
//	   this->m_InfoClust < 0 ||
//	   this->m_OrigClust < 0 ||
//	   GetNoise())
//		SetNoise(1);

// NOISE DISABLED
//	if ( timeMax > 100 )
//		SetNoise(1);
//	else
//		if ( timeMin > 100 )
//			SetNoise(1);
//		else
//			SetNoise(0);

	SetNoise(0);
}

double CVertex::GetEnergy(int iSamples,int iChannel)
{
	double dEnergy = 0.0;

	float* y = &m_YValues[iSamples*iChannel];

	int i;

	for(i=0;i<iSamples;i++) dEnergy += ( y[i] < 0.0 ? -y[i] : y[i]);

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
	CheckNoise();
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
	y = m_YValues;
	d2y = m_d2YValues;

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
	ya = m_YValues;
	d2y = m_d2YValues;
	
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
	ya = m_YValues;
	d2y = m_d2YValues;
		
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
	RESOLUTION = 16;
	NUM_CHANNELS = 4; 
	NUM_SAMPLES = 64;
	SAMPLE_FREQ = 32000; 
	whichDraw = 1;
	m_FakeSpikes = false;
	m_x=(float*)malloc(NUM_SAMPLES*sizeof(float));
	for (int i=0;i<NUM_SAMPLES;i++)
		m_x[i] = (float) i/SAMPLE_FREQ;
	m_MainSyncStack.SetMyPalette(m_PalStack);
	m_bShowXYSpikes = true;
	m_bLoadedXYSpikes = false;
	m_iPosCollapse = 8;
	m_NumOriginalCl = 0;
	m_bCombinePCA = false;
	m_bUsePCA = true;
	m_iNumPCsToStore = 3;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::SetPalette(CPaletteStack *pal)
{
	SetMyPalette(pal);
	m_MainSyncStack.SetMyPalette(pal);
	m_MainEEGStack.SetMyPalette(pal);
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::ClearHighDim()
{
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
void CVerxStack::CheckNoise()
{
	MY_STACK::iterator iVerx;


	CVertex *verx;
	for (iVerx = m_VerxStack.begin(); iVerx != m_VerxStack.end(); iVerx++)
	{
		verx = (CVertex*) *iVerx;
		verx->CheckNoise();
	}
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
		dYt1 = verx->Splope(m_x, m_x[m_iK], m_iK+0, m_iK+1, iQ, NUM_SAMPLES);
		for (m_iK=0;m_iK<(NUM_SAMPLES-2);++m_iK)
		{
			dYt2 = verx->Splope(m_x, m_x[m_iK+1], m_iK+1, m_iK+2, iQ, NUM_SAMPLES);
			if (dYt1*dYt2<=0)
			{
				for (int iL=0; iL<RESOLUTION; iL++)
				{
					float Value = verx->Splint(m_x, m_x[m_iK]+(float)iL/(SAMPLE_FREQ*RESOLUTION), m_iK+0, m_iK+1, iQ, NUM_SAMPLES);
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
	verx->CheckNoise();

	// Filling with PEAKs & VALLEYs & ... (default) parameters
	float minToStore,maxToStore;
	int iP=0;
	for (iP=0;iP<NUM_CHANNELS;iP++)
	{ 	
		verx->AddPnt(m_max[iP]);		// Tx_peak
	}
	for (iP=0;iP<NUM_CHANNELS;iP++)
	{ 	
		maxToStore = verx->Splint(m_x, MainTMax, lo_ptMaxDef, lo_ptMaxDef+1, iP, NUM_SAMPLES);
		verx->AddPnt(maxToStore);	// Tx-V(peak)
	}
		
	for (iP=0;iP<NUM_CHANNELS;iP++)
	{
		verx->AddPnt(m_min[iP]);		// Tx-valley
	}

	for (iP=0;iP<NUM_CHANNELS;iP++)
	{
		minToStore = verx->Splint(m_x, MainTMin, lo_ptMinDef, lo_ptMinDef+1, iP, NUM_SAMPLES);
		verx->AddPnt(minToStore);	// Tx-V(valley)
	}

	//store energy
	for(iP=0;iP<NUM_CHANNELS;iP++)
	{
		double dEnergy = verx->GetEnergy(NUM_SAMPLES,iP);
		verx->AddPnt(dEnergy);
	}

	//verx->AddPnt( (float) verx->GetTimeStamp()/10000 );//TimeStamp
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

void CVerxStack::DoVertexPCA()
{
#ifdef _DEBUG	
//	char* argv[5] = {"jnk","c:\\users\\samn\\documents\\neurosim\\pca\\test_input.txt",
//  char* argv[5] = {"jnk","c:\\users\\samn\\documents\\neurosim\\spikeclust\\chan_0_data.txt",
//		"166","64","R"};
//	not_main(5,argv);
#endif
	
	if(m_bCombinePCA)
	{
		int i;

		int iSamps = NUM_SAMPLES * NUM_CHANNELS;

		// Storage allocation for input data
		vector< vector<PCA_T> > data(m_VerxStack.size()+1, vector<PCA_T>(iSamps+1));

		// Allocation of correlation (etc.) matrix
		vector< vector<PCA_T> > symmat(iSamps+1, vector<PCA_T>(iSamps+1));

		MY_STACK::iterator Index;
		CVertex *verx;	    

		int iV = 1;

		//copy vertex data to pca format
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
		{
			verx = (CVertex*) *Index;
			int i = 0;
			for(i=1;i<=iSamps;i++)
			{
				data[iV][i] = verx->GetYValues(i-1);
			}
		}

		corcol(data, m_VerxStack.size(), iSamps, symmat);

		// Allocate storage for dummy and new vectors.
		//Storage alloc. for vector of eigenvalues
		vector<PCA_T> evals(iSamps+1);

		// Storage alloc. for 'intermediate' vector
		vector<PCA_T> interm(iSamps+1);
		
		int j,k,k2, m = iSamps, n = m_VerxStack.size();
		
		// Duplicate of correlation (etc.) matrix
		//	vector< vector<double> > symmat2(symmat);
		    
		tred2(symmat, m, evals, interm);  // Triangular decomposition
		tqli(evals, interm, m, symmat);   // Reduction of sym. trid. matrix
		//evals now contains the eigenvalues,
		//columns of symmat now contain the associated eigenvectors.

		// Form projections of row-points on first three prin. components.
		// Store in 'data', overwriting original data.
		for (i = 1; i <= n; i++) 
		{     
			for (j = 1; j <= m; j++) 
			{       
				interm[j] = data[i][j]; 
			}   
			// data[i][j] will be overwritten
			for (k = 1; k <= m_iNumPCsToStore; k++) 
			{          
				data[i][k] = 0.0;          
				for (k2 = 1; k2 <= m; k2++) 
				{            
					data[i][k] += interm[k2] * symmat[k2][m-k+1]; 
				}
			}
		}

		//store the principle components in vertices
		iV=1;
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
		{
			CVertex* verx = (CVertex*) *Index;
			int iP;
			for(iP=1;iP<=m_iNumPCsToStore;iP++)
			{
				verx->AddPnt(data[iV][iP]);
			}
		}
	}
	else
	{
		//storage for principle components
		//later will move it to the vertices
		//only used to keep a specific ordering on the data
		const int iParams = m_iNumPCsToStore;
		vector< vector< float > > vStore(m_VerxStack.size(), vector<float>(iParams*NUM_CHANNELS));

		int i;

		// Storage allocation for input data
		vector< vector<PCA_T> > data(m_VerxStack.size()+1, vector<PCA_T>(NUM_SAMPLES+1));

		// Allocation of correlation (etc.) matrix
		vector< vector<PCA_T> > symmat(NUM_SAMPLES+1, vector<PCA_T>(NUM_SAMPLES+1));

		int iChannel = 0;
		for(iChannel=0;iChannel<NUM_CHANNELS;iChannel++)
		{
			MY_STACK::iterator Index;
			CVertex *verx;	    

			int iV = 1;

			//copy vertex data to pca format
			for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
			{
				verx = (CVertex*) *Index;
				double dEnergy = verx->GetValue(17+iChannel);
				int i = 0;
				if(dEnergy <= 0.0) dEnergy = 1.0;
				for(i=1;i<=NUM_SAMPLES;i++)
				{
					data[iV][i] = verx->GetYValues((iChannel*NUM_SAMPLES)+i-1) / dEnergy;
				}
			}

	#ifdef _DEBUG
			char sname[1024];
			sprintf(sname,"chan_%d_data.txt",iChannel);
			FILE* fp = fopen(sname,"w");
			int ii,jj;
			for(ii=1;ii<=m_VerxStack.size();ii++)
			{
				for(jj=1;jj<=NUM_SAMPLES;jj++)
				{
					fprintf(fp,"%f ",data[ii][jj]);
				}
				fprintf(fp,"\n");
			}
			fclose(fp);
	#endif

			corcol(data, m_VerxStack.size(), NUM_SAMPLES, symmat);

			// Allocate storage for dummy and new vectors.
			//Storage alloc. for vector of eigenvalues
			vector<PCA_T> evals(NUM_SAMPLES+1);

			// Storage alloc. for 'intermediate' vector
			vector<PCA_T> interm(NUM_SAMPLES+1);
			
			int j,k,k2, m = NUM_SAMPLES, n = m_VerxStack.size();
			
			// Duplicate of correlation (etc.) matrix
		//	vector< vector<double> > symmat2(symmat);
		    
			tred2(symmat, m, evals, interm);  // Triangular decomposition
			tqli(evals, interm, m, symmat);   // Reduction of sym. trid. matrix
			//evals now contains the eigenvalues,
			//columns of symmat now contain the associated eigenvectors.

			// Form projections of row-points on first m_iNumPCsToStore prin. components.
			// Store in 'data', overwriting original data.
			for (i = 1; i <= n; i++) 
			{     
				for (j = 1; j <= m; j++) 
				{       
					interm[j] = data[i][j]; 
				}   
				// data[i][j] will be overwritten
				for (k = 1; k <= m_iNumPCsToStore; k++) 
				{          
					data[i][k] = 0.0;          
					for (k2 = 1; k2 <= m; k2++) 
					{            
						data[i][k] += interm[k2] * symmat[k2][m-k+1]; 
					}
				}
			}

			//store projections of row-points on first 3 prin. comps.
			for(iV=1;iV<=m_VerxStack.size();iV++)
			{
				int iPC = 0;
				for(iPC = 0; iPC < m_iNumPCsToStore; iPC++)
				{
					vStore[iV-1][iChannel*iParams+iPC] = data[iV][1];
				}
			}
		}

		data.clear();
		symmat.clear();

		//store the energy & 1st 3 principle components in vertices
		int iV=0;
		MY_STACK::iterator Index;
		for(Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++,iV++)
		{
			CVertex* verx = (CVertex*) *Index;
			int iP;
			for(iP=0;iP<iParams;iP++)
			{
				for(iChannel=0;iChannel<NUM_CHANNELS;iChannel++)
				{
					verx->AddPnt(vStore[iV][iParams*iChannel+iP]);
				}
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

	int timeStampFirst = ((sSpike*)*(m_SpikesStack.begin()))->TimeStamp - NUM_SAMPLES*10000 / SAMPLE_FREQ;
	int TSLast = ((sSpike*)*(m_SpikesStack.end() - 1))->TimeStamp;
	if ( (InBpfExist & 3) || (LoadedAdd & 3) )
	{
		if ( ((CSync*)*(m_MainSyncStack.SyncStack.begin()))->GetTimeStamp() < timeStampFirst )
			timeStampFirst = ((CSync*)*(m_MainSyncStack.SyncStack.begin()))->GetTimeStamp();
		if ( ((CSync*)*(m_MainSyncStack.SyncStack.begin() + m_MainSyncStack.SyncStack.size() - 1))->GetTimeStamp() > ((sSpike*)*(m_SpikesStack.end()-1))->TimeStamp)
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
			if ( startTS > ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
			{
				minT = indexSpike;
				increment = (maxT - minT)/2;
				indexSpike += increment;
			}
			else{
				if ( startTS < ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
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
			if ( startTS > ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
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
		if ( ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp >= startTS )
		{ 
			while ( ! fine && ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp < startTS + timeWin )
			{
				sSpike* pSpike = (sSpike*) *(m_SpikesStack.begin()+indexSpike);
				//if not in memory, then skip it
				if(!(pSpike->type & 1))
				{
					indexSpike++;
					if (indexSpike >= m_SpikesStack.size())
					{
						indexSpike--;
						fine = 1;
					}
					continue;
				}

				if(!vX[pSpike->offset] || !vY[pSpike->offset])
				{
					vX[pSpike->offset] = x;
					vY[pSpike->offset] = y;
					vAssigned[pSpike->offset] = true;
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
	{
		char msg[1024];
		sprintf(msg,"iA = %d , iNA = %d",iA,iNA);
		MessageBox(0,msg,"WClust - error loading room x,y",0);
	}
//#endif

	return bGood;
}

void InitProbs(int iMaxNumElems);

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::CalcAfterLoad()
{
	MY_STACK::iterator Index;

	CVertex *NewVerx;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	for (Index=m_VerxStack.begin(); Index!=m_VerxStack.end(); Index++)
	{
		NewVerx = (CVertex*) *Index;
//#ifdef _DEBUG
		if(NewVerx->GetOrigClust() < 0 ||
		   NewVerx->GetNoise())
		{
			NewVerx->SetNoise(1);
			int moo=0;
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

	BASE_DIMENSION = 20 + 1; //+1 for time
	if(m_bLoadedXYSpikes) BASE_DIMENSION += 2;
	if(m_bUsePCA)
	{
		if(m_bCombinePCA)
			BASE_DIMENSION += m_iNumPCsToStore;
		else
			BASE_DIMENSION += NUM_CHANNELS * m_iNumPCsToStore;
	}
	Dimension = BASE_DIMENSION;
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

	if(m_bUsePCA)
	{
		if(m_bCombinePCA)
		{
			int iPC = 1;
			char strTmp[64];
			//D21 and up
			for(iPC = 1; iPC <= m_iNumPCsToStore; iPC++)
			{
				sprintf(strTmp,"PC%d",iPC);
				pom = new CString(strTmp);
				AddAxes(pom);
			}
		}
		else
		{
			int iPC = 1;
			char strTmp[64];
			//D21 and up
			for(iPC = 1; iPC <= m_iNumPCsToStore; iPC++)
			{
				int iChan = 1;
				for(iChan=1;iChan<=NUM_CHANNELS;iChan++)
				{
					sprintf(strTmp,"T%d-PC%d",iChan,iPC);
					pom = new CString(strTmp);
					AddAxes(pom);
				}
			}
		}
	}

	if(m_bLoadedXYSpikes)
	{
		pom = new CString(LPCSTR("RoomX"));
		AddAxes(pom);
		pom = new CString(LPCSTR("RoomY"));
		AddAxes(pom);
	}

	pom = new CString(LPCSTR("Time[s]"));	//D33 - last dim not used for auto-clustering
	AddAxes(pom);

//	DWORD S = GetTickCount();
	InitProbs(m_VerxStack.size()+1);
//	DWORD T = GetTickCount();
//	DWORD E = T - S;
//	char st[32]={0};
//	sprintf(st,"init prob time=%d",E);
//	MessageBox(0,st,st,0);

	AreData = 1;	

#ifdef _DEBUG
	CalcMinMax();
	
/*	float t1pc1range = m_MainMax[17] - m_MainMin[17];
	float t1pc2range = m_MainMax[21] - m_MainMin[21];
	float t1pc3range = m_MainMax[25] - m_MainMin[25];

	float t2pc1range = m_MainMax[18] - m_MainMin[18];
	float t2pc2range = m_MainMax[22] - m_MainMin[22];
	float t2pc3range = m_MainMax[26] - m_MainMin[26];

	float t3pc1range = m_MainMax[19] - m_MainMin[19];
	float t3pc2range = m_MainMax[23] - m_MainMin[23];
	float t3pc3range = m_MainMax[27] - m_MainMin[27];

	float t4pc1range = m_MainMax[20] - m_MainMin[20];
	float t4pc2range = m_MainMax[24] - m_MainMin[24];
	float t4pc3range = m_MainMax[28] - m_MainMin[28];

	float ttt = t1pc1range;*/
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
	int *ChannelNo;
	char max_elde = 3;
	ChannelNo = (int*) malloc(max_elde * 256 * sizeof(int));

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
			free(ChannelNo);
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

	free(ChannelNo);


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
	NUM_SAMPLES = SU_SPIKE_SIZE;
	SAMPLE_FREQ = 32000; 
	free(m_x);
	m_x=(float*)malloc(NUM_SAMPLES*sizeof(float));
	for (i=0;i<NUM_SAMPLES;i++)
		m_x[i] = (float) i/SAMPLE_FREQ;

	dp = first_rec;	
	current_byte = dp - buffer;

	float *m_u;
	m_u=(float*)malloc(NUM_SAMPLES * sizeof(float));

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
					sSpikeInFile *spikePortrait;
					spikePortrait = (sSpikeInFile*) new sSpikeInFile;//malloc (sizeof(sSpikeInFile));
					spikePortrait->electrode = 1;
					spikePortrait->channel = EldeChannel;
					filePortraitStack.push_back(spikePortrait);

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

					
						float *mNewYVal;
						mNewYVal = (float*) malloc (NUM_SAMPLES * NUM_CHANNELS * sizeof(float));

						// conversion of values, and also reorganize matrix (Ch,S)->(S,Ch)
						for (int i = 0;i < (NUM_SAMPLES); i++)
						{
							*(mNewYVal + i) = (float) (10*( *((short*) (dp + SU_SPIKE_OFFSET + i*2 ) ) ))/4096;
							*(mNewYVal + i + 1*NUM_SAMPLES) = *(mNewYVal + i + 2*NUM_SAMPLES) = *(mNewYVal + i + 3*NUM_SAMPLES) = *(mNewYVal + i);
						}
						NewVerx->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
						m_VerxStack.push_back(NewVerx);

						spikeInf = (sSpike*) new sSpike;//malloc (sizeof(sSpike));
						spikeInf->TimeStamp = NewVerx->GetTimeStamp();
						spikeInf->cluster = NewVerx->GetClust();
						spikeInf->origCluster = NewVerx->GetOrigClust();
						if ( NewVerx->GetOrigClust() > m_NumOriginalCl )
							m_NumOriginalCl = NewVerx->GetOrigClust();
						spikeInf->type = 1; //SPIKE_IN_MEMORY;
						spikeInf->offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;
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
					sSpikeInFile *spikePortrait;
					spikePortrait = (sSpikeInFile*) new sSpikeInFile;//malloc (sizeof(sSpikeInFile));
					spikePortrait->electrode = 2;
					spikePortrait->channel = EldeChannel;
					filePortraitStack.push_back(spikePortrait);

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
					sSpikeInFile *spikePortrait;
					spikePortrait = (sSpikeInFile*) new sSpikeInFile;//malloc (sizeof(sSpikeInFile));
					spikePortrait->electrode = 4;
					spikePortrait->channel = EldeChannel;
					filePortraitStack.push_back(spikePortrait);

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
					
						float *mNewYVal;
						mNewYVal = (float*) malloc (NUM_SAMPLES * NUM_CHANNELS * sizeof(float));

						// conversion of values, and also reorganize matrix (Ch,S)->(S,Ch)
						for (int i = 0;i < (NUM_SAMPLES); i++)
							for (int j = 0; j < NUM_CHANNELS; j++)
							{
								*(mNewYVal + i + j*NUM_SAMPLES) = (float) (10*( *((short*) (dp + SU_SPIKE_OFFSET + i*NUM_CHANNELS*2 + j*2) ) ))/4096;
							}
						NewVerx->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
						m_VerxStack.push_back(NewVerx);

						spikeInf = (sSpike*) new sSpike;// malloc (sizeof(sSpike));
						spikeInf->TimeStamp = NewVerx->GetTimeStamp();
						spikeInf->cluster = NewVerx->GetClust();
						spikeInf->origCluster = NewVerx->GetOrigClust();
						if ( NewVerx->GetOrigClust() > m_NumOriginalCl )
							m_NumOriginalCl = NewVerx->GetOrigClust();
						spikeInf->type = 1; //SPIKE_IN_MEMORY;
						spikeInf->offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;
						spikeInf->fileParams.clear();
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
			free(m_u);
			return 2;
		}

	}while (current_byte < nRead);

	free(m_u);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
long CVerxStack::GetBPFDataPtr(CFile *file)
{
	//Searching of index of binary data
	int		m_j;
	int		FoundEnd;
	int		nRead;
	char	*buffer;
	long	m_i;
	char	pomStr[40];
								
	buffer =(char*) malloc(8192);
	
	nRead = file->Read(buffer,8192);


	m_i=0;
	FoundEnd=0;
	sprintf(pomStr,"%%%%END_HEADER");
	while (m_i<nRead&&!FoundEnd)
	{
		m_j=0;
		while ((*(buffer+m_i+m_j)==pomStr[m_j])&&(m_j<12))
			m_j+=1;	
		
		if (m_j==12)
			FoundEnd=1;
		m_i+=1;
	}


	if (!FoundEnd)
	{
		SetEmpty();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		free(m_FileName);
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
	sSpike *spike;
	MY_SPIKE_STACK::iterator indSpike = m_SpikesStack.begin();
	int i=0;
	while ( indSpike != m_SpikesStack.end() )
	{
		spike = ((sSpike*) *indSpike);

		spike->fileParams.push_back( (float) 0 );
		spike->fileParams.push_back( (float) 0 );
		spike->fileParams.push_back( (float) 0 );
		spike->fileParams.push_back( (float) 0 );

		indSpike++;
		i++;
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
			sSpike *spike;
			sSpikeInFile *fp;
			MY_FILE_PORTRAIT_STACK::iterator indFP = filePortraitStack.begin();
			MY_SPIKE_STACK::iterator indSpike = m_SpikesStack.begin();
			while ( fileState != EOF && indFP != filePortraitStack.end() )
			{
				fp = ((sSpikeInFile*) *indFP);
				spike = ((sSpike*) *indSpike);
				switch ( fp->electrode )
				{
				case 1:
					fileState = fscanf(file,"%f",&data[0]);
					if ( m_ElectrodeType == 1 && fp->channel == m_ElectrodeChannel )
					{
						spike->fileParams.push_back(data[0]);
						spike->fileParams.push_back(data[0]);
						spike->fileParams.push_back(data[0]);
						spike->fileParams.push_back(data[0]);

						indSpike++;
					}
					break;
				case 2:
					fileState = fscanf(file,"%f %f", &data[0], &data[1]);
					if ( m_ElectrodeType == 2 && fp->channel == m_ElectrodeChannel )
					{
						spike->fileParams.push_back(data[0]);
						spike->fileParams.push_back(data[1]);
						spike->fileParams.push_back(data[0]);
						spike->fileParams.push_back(data[1]);

						indSpike++;
					}
					break;
				case 4:
					fileState = fscanf(file,"%f %f %f %f", &data[0], &data[1], &data[2], &data[3]);
					if ( m_ElectrodeType == 3 && fp->channel == m_ElectrodeChannel )
					{
						spike->fileParams.push_back(data[0]);
						spike->fileParams.push_back(data[1]);
						spike->fileParams.push_back(data[2]);
						spike->fileParams.push_back(data[3]);
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
		(*indSpike)->fileParams.erase( (*indSpike)->fileParams.begin() + iNum );
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

	char *fileMask = (char*) malloc (wcpFileNameMask.GetLength()+1);
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
	free( fileMask );


	// Read parameter name of each file
	char *buffer = (char*) malloc(1024);
	MY_WCP_FILES_STACK::iterator indFiles;
	for (indFiles = wcpFilesStack.begin(); indFiles != wcpFilesStack.end(); indFiles++)
	{
		CFile file;
		if ( file.Open( (LPCTSTR) ((sWcpFiles)*indFiles).fileName,CFile::modeRead) )
		{	// file is successfully opened
			int nRead = file.Read(buffer,1024);
			if ( nRead == 0 )
			{
				file.Close();
			}
			else
			{	// header is successfully read

				char	*pdest, *formatInf;
				int		formatInfStart = 0;
				int		bpfEnd = 0;
	
				formatInf = strstr(buffer, "WCLUST_PARAMETER_FILE");
				if (formatInf != NULL)
				{
					formatInfStart = formatInf - buffer + 1;
				
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

	free(buffer);
	
//	FILE *f=fopen((LPCTSTR) ((sWcpFiles) (wcpFilesStack.begin())).fileName,"w+");
//	FILE *f=fopen("a.wcp","w+");
//	fprintf(f,"%%%%BEGIN_HEADER WCLUST_PARAMETER_FILE\n%%ParameterName.0 ( SpikeAsc )\n%%AmountElectrodes.0 (1)\n%%%%END_HEADER WCLUST_PARAMETER_FILE\n");
//	for (int i=0; i<15947; i++)
//		fprintf(f,"%d\n",i);
//	fclose(f);
}

void CVerxStack::AddFakeSpike( int TimeStamp )
{
	sSpike *spikeInf;
	spikeInf = (sSpike*) new sSpike;//malloc (sizeof(sSpike));
	spikeInf->TimeStamp = TimeStamp;
	spikeInf->cluster = 0;
	spikeInf->origCluster = 0;
	spikeInf->fileParams.clear();
	spikeInf->type = 1; //SPIKE_IN_MEMORY;

	// Storing data to container
	CVertex *NewVerx;
	NewVerx=new CVertex(m_PalStack);
	NewVerx->SetOrigCl( 0 );
	NewVerx->SetTimeStamp( TimeStamp ); // 1 ~ [100us]
	NewVerx->AddPnt( 0 );  //empty cluster's set
								
	m_NumVerx++;

	// Convert to real value
	float *mNewYVal;
	float *m_u = (float*) malloc(NUM_SAMPLES * sizeof(float));

	mNewYVal = (float*) malloc (NUM_SAMPLES * NUM_CHANNELS * sizeof(float));
	for (int m_l=0;m_l<(NUM_CHANNELS*NUM_SAMPLES);m_l++)
	{
		*(mNewYVal+m_l) = (float) 0;
	}
	NewVerx->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);

	NewVerx->AddClust(0);
	m_VerxStack.push_back(NewVerx);

	spikeInf->offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;

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
	{	
		SAMPLE_FREQ = GetNumberRoundBr(pdest);
	}

	// EEG_SAMPLING_FREQUENCY
	pdest = strstr(formatInf,EEG_SAMPLING_FREQUENCY);
	if ( pdest == NULL )
	{	}
	else
	{	
		EEG_SAMPLE_FREQ = GetNumberRoundBr(pdest);
	}
	
	return 0;
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
				NUM_SAMPLES = GetNumberBr(pdest);
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
	
	// Find END_HEADER
	pdest = strstr(buffer, HEADER_END_LABEL);
	if ( pdest == NULL )
		return 0;
	bpfEnd = pdest - buffer + 1 + 13;
	return bpfEnd;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::LoadBPF(CFile *file, char *buffer, int nRead, char *errChar, long *errOffset)
{
	FileType = 1;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	buffer[8191] = '\0';
	NUM_CHANNELS=4; 
	NUM_SAMPLES=64;
	EEG_SAMPLE_FREQ = 2000;
	SAMPLE_FREQ=32000; 

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
		free(m_FileName);
		return 2;
	}

	free(m_x);
	m_x=(float*)malloc(NUM_SAMPLES*sizeof(float));
	for (int i=0;i<NUM_SAMPLES;i++)
		m_x[i] = (float) i/SAMPLE_FREQ;


	// Main reading of data
	char	m_Identif;
	char	ElChannel;
	short	*m_buffer;
	float	*m_u,*m_dY;
	CVertex *NewVerx;
		
	// inspection in file
	// Number of spikes in each channel
	int *ChannelNo;
	char max_elde = 3;
	ChannelNo = (int*) malloc(max_elde * 256 * sizeof(int));
	for (i=0; i<max_elde; i++)
		for (int j=0; j<256; j++)
			ChannelNo[i*256+j] = 0;

	// for making scope in SYNC 
	const	ResolutionHistogram = 10; 
	int		*histogram;
	char	*SyncValues;
	int		NthToHistogram = 0;
	int		TimeStamp;
	char	isLoaded;	// when typical character found ( Y, S, U, T, A, R, E,....)
	UINT	bLoaded;		// amount of loaded bytes

	// prepare tables
	SyncValues = (char*) malloc(2*1000);
	histogram = (int*) malloc(256*sizeof(int));
	for (i = 0; i<256; i++)
		histogram[i] = 0;
	
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
		// EEG
		if ( m_Identif == bpf.EEG_TYPE )
		{
			isLoaded = 1;
			*errChar = 'E'; *errOffset = m_i;
			dLastType = 'E'; dLastPos = m_i;
			
			m_i += bpf.EEG_SIZE;
		}

		// KEY EVENT
		if ( m_Identif == bpf.KEY_EVENT_TYPE )
		{
			isLoaded = 1;
			*errChar = 'K'; *errOffset = m_i;
			dLastType = 'K'; dLastPos = m_i;
			
			m_i += bpf.KEY_EVENT_SIZE;
		}

		// SYNC
		if ( m_Identif == bpf.SYNC_TYPE )
		{	
			isLoaded = 1;
			*errChar = 'Y'; *errOffset = m_i;
			dLastType = 'Y'; dLastPos = m_i;

			if (NthToHistogram == ResolutionHistogram)
			{
				
				bLoaded = file->Read(&TimeStamp,4);
				if ( bLoaded == 4 )
				{
					bLoaded = file->Read(SyncValues,2000);	// 2000 - SyncData
					if ( bLoaded == 2000 )
					{
						for (i = 0; i<1000; i++)		// 1000	- SyncData
							histogram[SyncValues[2*i+1]+127]++; // (char)floor(SyncValues[i]/256)
						NthToHistogram = 0;
					}
				}
			}
			NthToHistogram++;
			m_i += bpf.SYNC_SIZE;
		}
		// SINGLE
		if ( m_Identif == bpf.SINGLE_TYPE )
		{	
			isLoaded = 1;
			*errChar = 'U'; *errOffset = m_i;
			dLastType = 'U'; dLastPos = m_i;
		
			
			file->Seek(5,CFile::current);
			bLoaded = file->Read(&ElChannel,1);
			ChannelNo[0*256 + ElChannel] += 1;

			m_i += bpf.SINGLE_SIZE;
		}
		// STEREO
		if ( m_Identif == bpf.STEREO_TYPE )
		{	
			isLoaded = 1;
			*errChar = 'S'; *errOffset = m_i;
			dLastType = 'S'; dLastPos = m_i;
		
			
			file->Seek(5,CFile::current);
			file->Read(&ElChannel,1);
			ChannelNo[1*256 + ElChannel] += 1;

			m_i += bpf.STEREO_SIZE;
		}
		// TETRODE		
		if ( m_Identif == bpf.TETRODE_TYPE )
		{	
			isLoaded = 1;
			*errChar = 'T'; *errOffset = m_i;
			dLastType = 'T'; dLastPos = m_i;
			
			
			file->Seek(4,CFile::current);
			file->Read(&ElChannel,1);
			ChannelNo[2*256 + ElChannel] += 1;

			m_i += bpf.TETRODE_SIZE;
		}
		// ARENA
		if ( m_Identif == bpf.ARENA_POSITION_TYPE )
		{
			isLoaded = 1;
			*errChar = 'A'; *errOffset = m_i;
			dLastType = 'A'; dLastPos = m_i;
		

			InBpfExist |= 1;
			m_i += bpf.ARENA_POSITION_SIZE;
		}
		// ROOM		
		if ( m_Identif == bpf.ROOM_POSITION_TYPE )
		{	
			isLoaded = 1;
			*errChar = 'R'; *errOffset = m_i;
			dLastType = 'R'; dLastPos = m_i;
			

			InBpfExist |= 2;
			m_i += bpf.ROOM_POSITION_SIZE;
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
				free(m_FileName);
				free(histogram);
				free(SyncValues);
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

	free(histogram);
	free(SyncValues);
	// Set the middle 
	m_MainSyncStack.SetMiddle( (HpeakI + (histMaxI-HpeakI)/2 - 127)*256 );
	
	// Check if there are electrode data in BPF 	
	i = 0;
	int total_channel_data = max_elde * 256;	
	while ( i < total_channel_data && ChannelNo[i] == 0)
	{
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
		SelTTDial.DoModal();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		// Get values
		m_ElectrodeChannel = SelTTDial.EldeCh;
		m_ElectrodeType = SelTTDial.EldeType;
		AmountDataFromFile = SelTTDial.GetAmount();//SelTTDial.Amount;
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

	free(ChannelNo);
		
	m_MainEEGStack.SetParams( (bpf.EEG_SIZE - 5) / 2000, 1000);
	m_MainEEGStack.SetFrequency( EEG_SAMPLE_FREQ );

	m_i = startPos;

	m_buffer = (short*) malloc(NUM_CHANNELS * NUM_SAMPLES * sizeof(short));
	m_u = (float*) malloc(NUM_SAMPLES * sizeof(float));
	m_dY = (float*) malloc(NUM_SAMPLES * sizeof(float));

	// Preparing indices for loading
	srand( (unsigned)time( NULL ) );
	float NumbLoaded = 0;
	float Increase = (float) (100 / (float) AmountDataFromFile); // perc.
	int PlusRand = (int) floor(Increase) - 1;
	float IndexNext = Increase;
	int Index = 0;
	int IndexSpike = (int) ( (float) PlusRand*rand()/32768);;
	char isSpike = 0;

	sSpike *spikeInf;
	
	while ( m_i < dFileLength )
	{
		isLoaded = 0;
		
		file->Seek(m_i,0);
		file->Read(&m_Identif,1);
			
		if ( m_Identif == bpf.ARENA_POSITION_TYPE ) 
		{	// Arena
			isLoaded = 1;
			m_MainSyncStack.LoadArena(file);
			m_i += bpf.ARENA_POSITION_SIZE;
		}

		if ( m_Identif == bpf.ROOM_POSITION_TYPE ) 
		{	// Room
			isLoaded = 1;
			m_MainSyncStack.LoadRoom(file);
			m_i += bpf.ROOM_POSITION_SIZE;
		}
		
		if ( m_Identif == bpf.EEG_TYPE ) 
		{	// EEG
			isLoaded = 1;
			m_MainEEGStack.LoadEEG(file);
			m_i += bpf.EEG_SIZE;
		}
			
		if ( m_Identif == bpf.SYNC_TYPE ) 
		{	//Synchronize

		
			isLoaded = 1;
			m_i += bpf.SYNC_SIZE;
			m_MainSyncStack.LoadSync(file);
			

		}

		
		if ( m_Identif == bpf.KEY_EVENT_TYPE )
		{	// KEY EVENT
			isLoaded = 1;
			
			m_i += bpf.KEY_EVENT_SIZE;
		}

		if ( m_Identif == bpf.SINGLE_TYPE ) 
		{	// SINGLE
			isLoaded = 1;
			file->Read(&TimeStamp,4);
			file->Read(&ElChannel,1);

			// Add file portrait
			sSpikeInFile* spikePortrait = new sSpikeInFile;//malloc (sizeof(sSpikeInFile));
			spikePortrait->electrode = 1;
			spikePortrait->channel = ElChannel;
			filePortraitStack.push_back(spikePortrait);

			m_i += bpf.SINGLE_SIZE;
		}

		if ( m_Identif == bpf.STEREO_TYPE ) 
		{	// STEREO
			isLoaded = 1;
			file->Read(&TimeStamp,4);
			file->Read(&ElChannel,1);
			
			// Add file portrait
			sSpikeInFile* spikePortrait = new sSpikeInFile;//malloc (sizeof(sSpikeInFile));
			spikePortrait->electrode = 2;
			spikePortrait->channel = ElChannel;
			filePortraitStack.push_back(spikePortrait);

			m_i += bpf.STEREO_SIZE;
		}


		if ( m_Identif == bpf.TETRODE_TYPE ) 
		{	// TETRODE
			char OrigCl;
			int TimeStamp;

			isLoaded = 1;
			file->Read(&TimeStamp,4);
			file->Read(&ElChannel,1);
			file->Read(&OrigCl,1);
			
			// Add file portrait
			sSpikeInFile* spikePortrait = new sSpikeInFile;//malloc (sizeof(sSpikeInFile));
			spikePortrait->electrode = 4;
			spikePortrait->channel = ElChannel;
			filePortraitStack.push_back(spikePortrait);

			// we need this channel
			if ( ElChannel == m_ElectrodeChannel )
			{
				spikeInf = (sSpike*) new sSpike;//malloc (sizeof(sSpike));
				spikeInf->TimeStamp = TimeStamp;
				spikeInf->offset = m_i;
				spikeInf->type = 0; //SPIKE_ON_DISK; // on disk
				spikeInf->cluster = OrigCl;
				spikeInf->origCluster = OrigCl;
				spikeInf->fileParams.clear();
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
												
					file->Read(m_buffer,NUM_CHANNELS*NUM_SAMPLES*sizeof(short));
					m_NumVerx++;
	
					// Convert to real value
					float* mNewYVal = (float*) malloc (NUM_SAMPLES * NUM_CHANNELS * sizeof(float));
					for (int m_l=0;m_l<(NUM_CHANNELS*NUM_SAMPLES);m_l++)
					{
						mNewYVal[m_l] = (float) (10*m_buffer[m_l])/SHRT_MAX;
					}
					NewVerx->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
				
					NewVerx->AddClust(0);
					m_VerxStack.push_back(NewVerx);

					spikeInf->type = 1; //SPIKE_IN_MEMORY;
					spikeInf->offset = (m_VerxStack.end() - m_VerxStack.begin()) - 1;
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
	
		if ( isLoaded == 0 )
		{
			if(m_i+1==dFileLength){
				m_i+=1;
			}
			else{
				free(m_buffer);
				free(m_u);
				free(m_dY);
				file->Close();
				SetEmpty();
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

				free(m_FileName);
				return 2;
			}
		}
	} //while not eof
			
	if ( m_FakeSpikes )
	{		
		// Adding 2 spikes, 
		AddFakeSpike( m_MainEEGStack.GetFirstTimeStamp() );
		AddFakeSpike( m_MainEEGStack.GetLastTimeStamp() + 5000);
	}

	if ( InBpfExist == 0 )
		m_MainSyncStack.CreateSync();

	free(m_buffer);
	free(m_u);
	free(m_dY);
	file->Close();

	return 0;
			
}
/////////////////////////////////////////////////////////////////////////////
int CVerxStack::LoadVerx( char *errChar, long *errOffset)
{
	m_MainSyncStack.SetFileType(0);
	whichDraw = 1;
    static char BASED_CODE szFilter[] = "Brain potential files (*.bpf, *.uff)|*.bpf;*.uff|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "bpf", "*.bpf;*.uff",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if (dlg.DoModal()==IDOK)
	{
		CFile file;
		if (!file.Open(dlg.GetPathName(),CFile::modeRead))
		{
			*errChar = 'o';
			*errOffset = 0;

			//CWnd::MessageBox(TEXT("Error: Open brain potential file"),NULL,MB_OK|MB_ICONERROR);
			return 2;
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
			
			m_FileNameWithoutExt = dlg.GetPathName();
			if ( dlg.GetFileExt().GetLength() > 0 )
			{
				m_FileNameWithoutExt.Delete( dlg.GetPathName().GetLength() - dlg.GetFileExt().GetLength() - 1, dlg.GetFileExt().GetLength()+1);
			}

			// Set TMP file name
			m_FileName = (char*) malloc( dlg.GetPathName().GetLength() + 1);
			m_TmpName = (char*) malloc( dlg.GetPathName().GetLength() + 1);
			for (int i=0; i<dlg.GetPathName().GetLength(); i++)
			{
				m_FileName[i] = dlg.GetPathName().GetAt(i);
				m_TmpName[i] = dlg.GetPathName().GetAt(i);
			}
			m_FileName[i] = 0;
			m_TmpName[i-3] = 84;//T
			m_TmpName[i-2] = 77;//M
			m_TmpName[i-1] = 80;//P
			m_TmpName[i] = 0;

			// Remove old data from memory
			SetEmpty();

			// Creating name of WCP files (WClust Parameter)
			wcpFileNameMask = "*.wcp";
			int length = dlg.GetPathName().GetLength();
			if (dlg.GetFileExt().GetLength() > 0)
				length -= dlg.GetFileExt().GetLength() + 1;
			for (i = 0; i < length; i++)
			{
				wcpFileNameMask.Insert(i, dlg.GetPathName().GetAt(i));
			}

			wcpPath.Empty();
			for (i = 0; i < (dlg.GetPathName().GetLength() - dlg.GetFileName().GetLength() ); i++)
			{
				wcpPath.Insert(i, dlg.GetPathName().GetAt(i));
			}

			// Finding WCP files
			CheckWcpFiles();

			// Loading first bytes from file
			char *buffer;
			buffer = (char*) malloc(8192);
			if ( buffer == NULL )
			{	//CWnd::MessageBox(TEXT("Error: Out of memory"),NULL,MB_OK|MB_ICONERROR);
				free(m_FileName);
				FileType = 0;
				file.Close();
				return 1;
			}

			int nRead=file.Read(buffer,8192);
			if ( nRead == 0 )
			{
				free(buffer);
				file.Close();
				FileType = 0;
				return 2;
			}

			int errorNO;
			if ( buffer[0] == HEADER_BYTE_0 && buffer[1] == HEADER_BYTE_1 )
			{	// UFF file
	
				// Prepare memory for file
				free(buffer);
				buffer = (char*) malloc(file.GetLength());
				if (buffer==NULL)
				{	
					free(m_FileName);
					FileType = 0;
					file.Close();
					return 1;
				}
				// Loading file to memory
				file.Seek(0,CFile::begin);
				int nRead=file.Read(buffer,file.GetLength());
				if ( nRead == 0 )
				{
					free(buffer);
					file.Close();
					SetEmpty();
					FileType = 0;
					return 2;
				}

				// get data from buffer
				errorNO = LoadUFF(&file, buffer, nRead);
				if (errorNO != 0)
				{
					free(buffer);
					FileType = 0;
					file.Close();
					return errorNO;
				}
				free(buffer);
				file.Close();

			}
			else{
				// BPF file
				errorNO = LoadBPF(&file, buffer, nRead, errChar, errOffset);
				free(buffer);
				if (errorNO != 0)
				{
					FileType = 0;
					return errorNO;
				}
			}
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		} //Loading file
		
		if (m_NumVerx == 0)
		{
			AreData=0;
		}
		else
		{
			m_NumVerxAll = m_SpikesStack.end() - m_SpikesStack.begin();
			m_MainSyncStack.SetFileType(FileType);
			CalcAfterLoad();
		}
		return 0;
	}
	return 3;
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
		((CVertex*)*Index)->DrawSpikes(pDC, m_x,SAMPLE_FREQ, NUM_SAMPLES, DrawWin, whichChanel, whichClust, whichDraw);
	}
	
	int Numb=0;
	float *average, y, yPrev;
	average = (float*) malloc(DrawWin.Width()*sizeof(float));
	for (int zeroI=0;zeroI<DrawWin.Width();zeroI++)
		*(average+zeroI) = 0;
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		((CVertex*)*Index)->CalcAvgSpike(m_x, SAMPLE_FREQ, NUM_SAMPLES, DrawWin,whichChanel, whichClust, average, &Numb, whichDraw);
	}
	
	if (Numb > 0)
	{
		for (int i = 0; i<DrawWin.Width(); i++)
		{
			*(average+i) /= Numb;
		}

		CPen newPen(PS_SOLID,1,RGB(0,0,0));
		CPen *pOldPen=pDC->SelectObject(&newPen);

		yPrev = DrawWin.bottom - (*(average+0))*DrawWin.Height()/20 - DrawWin.Height()/2;
		for (i=DrawWin.left+1;i<DrawWin.right;i++)
		{
			y = DrawWin.bottom - *(average+i-DrawWin.left)*DrawWin.Height()/20 - DrawWin.Height()/2;
			pDC->MoveTo(i,yPrev);
			pDC->LineTo(i+1,y);
			yPrev=y;
		}
		pDC->SelectObject(pOldPen);
	}
	free(average);
}
/////////////////////////////////////////////////////////////////////////////
void CVerxStack::DrawSpikes(CDC *pDC,CRect DrawWin,int whichChanel,int Start,int Stop)
{	// Spikes in play window after resizing
	MY_STACK::iterator Index;	
	for (Index=m_VerxStack.begin()+Start;Index!=m_VerxStack.begin()+Stop;Index++)
	{
		((CVertex*)*Index)->DrawSpikes(pDC, m_x, SAMPLE_FREQ, NUM_SAMPLES, DrawWin, whichChanel);
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
		if ( ((CVertex*)*Index)->DrawSpikes(pDC, m_x, SAMPLE_FREQ, NUM_SAMPLES, DrawWin, whichChanel) )
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
		if ( startTS > ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
		{
			minT = indexSpike;
			increment = (maxT - minT)/2;
			indexSpike += increment;
		}
		else{
			if ( startTS < ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
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
		if ( startTS > ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
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
	if ( ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp >= startTS )
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

		

		while ( ! fine && ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp < startTS + timeWindow )
		{
			char draw = (whichDraw == 0) ? clView[((sSpike*) *(m_SpikesStack.begin() + indexSpike))->cluster] & 1 : clView[((sSpike*) *(m_SpikesStack.begin() + indexSpike))->origCluster] & 1;
			if ( draw )
			{
				x1 = drawWin.left + ratioTS * (((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp - startTS);
				x2 = drawWin.left + ratioTS * (((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp - startTS + NUM_SAMPLES*10000/SAMPLE_FREQ);
//char a[120];
//sprintf(a,"%x",params);
//pDC->TextOut(10,25,a);

				int	yWin = 0;
				if ( (params & 3) == 0 )
				{	// RASTER
					COLORREF color = ( whichDraw == 0 ) ? ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->cluster : ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->origCluster;
				
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
					if ( ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->type & 1 )
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
								((CVertex*) *(m_VerxStack.begin() + ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->offset))->DrawTimeSpikes(pDC, m_x, SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
								yWin++;
							}
						}
					}	
					else{ // Spike on disk -> RASTER
						COLORREF color = ( whichDraw == 0 ) ? ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->cluster : ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->origCluster;
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
					if ( ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->type & 1 )
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
								((CVertex*) *(m_VerxStack.begin() + ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->offset))->DrawTimeSpikes(pDC, m_x, SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
								yWin++;
							}
						}
					}	
					else{ // Spike on disk -> SPIKE
						if ( file == NULL )
						{
							COLORREF color = ( whichDraw == 0 ) ? ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->cluster : ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->origCluster;
							CPen newPen(PS_SOLID,1, m_PalStack->GetSColor( color ));
							CPen *pOldPen = pDC->SelectObject(&newPen);
							for ( int nextCh = 0; nextCh < toDraw; nextCh++ )
								{
								pDC->MoveTo(x1,drawWin.top + nextCh * ySize1Spike + ySize1Spike / 10);
								pDC->LineTo(x1,drawWin.top + ( nextCh + 1 ) * ySize1Spike);
							}
							pDC->SelectObject(pOldPen);
						}
						else {
							CVertex *verx;
							verx = (CVertex*) new CVertex(m_PalStack);
							LoadBPFElde(verx, file, ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->offset);
						
							CRect spikeWin;
							for ( int nextCh = 0; nextCh < NUM_CHANNELS; nextCh++ )
							{
								if ( params & ( bite << nextCh ) )
								{
									spikeWin.left = x1;
									spikeWin.right = x2;
									spikeWin.top = drawWin.top + yWin * ySize1Spike;
									spikeWin.bottom = drawWin.top + ( yWin + 1 ) * ySize1Spike;
									((CVertex*) verx)->DrawTimeSpikes(pDC, m_x, SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
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
		if ( startTS > ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
		{
			minT = indexSpike;
			increment = (maxT - minT)/2;
			indexSpike += increment;
		}
		else{
			if ( startTS < ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
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
		if ( startTS > ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp )
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
	if ( ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp >= startTS )
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

		

		while ( ! fine && ((sSpike*) *(m_SpikesStack.begin() + indexSpike))->TimeStamp < startTS + timeWindow )
		{
			sSpike* pSpike = (sSpike*) *(m_SpikesStack.begin()+indexSpike);
			//if not in memory, then skip it
			if(!(pSpike->type & 1))
			{
				indexSpike++;
				if (indexSpike >= m_SpikesStack.size())
				{
					indexSpike--;
					fine = 1;
				}
				continue;
			}
			CVertex* verx = (CVertex*) *(m_VerxStack.begin() + pSpike->offset);
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
			}

			if ( draw )
			{
				x1 = drawWin.left + ratioTS * (pSpike->TimeStamp - startTS);
				x2 = drawWin.left + ratioTS * (pSpike->TimeStamp - startTS + NUM_SAMPLES*10000/SAMPLE_FREQ);

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
							verx->DrawTimeSpikes(pDC, m_x, SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
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
							verx->DrawTimeSpikes(pDC, m_x, SAMPLE_FREQ, NUM_SAMPLES, spikeWin, drawWin, nextCh , whichDraw);
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
		(*indSp)->fileParams.clear();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::SetEmpty()
{
	// Remove vectors
	MY_STACK::iterator Index;
	CVertex *verx;
	for (Index=m_VerxStack.begin();Index!=m_VerxStack.end();Index++)
	{
		verx = (CVertex*)*Index;
		delete verx;
	}
	m_VerxStack.clear();
	
	InBpfExist = 0;
	LoadedAdd = 0;
	m_MainSyncStack.SetEmpty();
	m_MainEEGStack.SetEmpty();

	// Remove axes names
	CString *myTxt;
	while (m_AxesStack.size()>0)
	{
		myTxt = *(m_AxesStack.end()-1);
		delete myTxt;
		m_AxesStack.pop_back();
	}
	m_AxesStack.clear();
	
	// Remove spikes indices...
	MY_SPIKE_STACK::iterator indSp;
	for (indSp = m_SpikesStack.begin(); indSp != m_SpikesStack.end(); indSp++)
	{
		free (*indSp);
	}
	m_SpikesStack.clear();

	// Remove spikes indices...
	MY_FILE_PORTRAIT_STACK::iterator indFP;
	for (indFP = filePortraitStack.begin(); indFP != filePortraitStack.end(); indFP++)
	{
		free (*indFP);
	}
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
	forStoreBPF_buffer = (short*) malloc(NUM_CHANNELS * NUM_SAMPLES * sizeof(short));
	if ( forStoreBPF_buffer == NULL )
	{
		return 2;
	}

	forStoreBPF_u = (float*) malloc(NUM_SAMPLES * sizeof(float));
	if ( forStoreBPF_buffer == NULL )
	{
		free(forStoreBPF_buffer);
		return 2;
	}

	if (!forStoreBPF_file.Open(m_FileName, CFile::modeReadWrite))
	{
		free(forStoreBPF_buffer);
		free(forStoreBPF_u);
		return 1;
	}

	forStoreBPF_IndexFile = GetBPFDataPtr( &forStoreBPF_file );
	if (forStoreBPF_IndexFile == 0)
	{
		free(forStoreBPF_buffer);
		free(forStoreBPF_u);
		forStoreBPF_file.Close();
		return 3;
	}
	
	forStoreBPF_IndexVx = m_VerxStack.begin();
	forStoreBPF_ArenaIndex = 0;
	forStoreBPF_RoomIndex = 0;
	forStoreBPF_SpikeIndex = -1;
	forStoreBPF_Spike = *m_SpikesStack.begin();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CVerxStack::FinishStoreBPF()
{
	free( forStoreBPF_buffer );
	free( forStoreBPF_u );
	forStoreBPF_file.Close();
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::LoadBPFElde(CVertex *pnt, CFile *file, int offset)
{
	int		TimeStamp;
	char	Channel;
	char	OrigCl;
	short	*m_buffer;
	float	*m_u;

	m_buffer = (short*) malloc(NUM_CHANNELS * NUM_SAMPLES * sizeof(short));
	m_u = (float*) malloc(NUM_SAMPLES * sizeof(float));

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
		file->Read(m_buffer,NUM_CHANNELS*NUM_SAMPLES*sizeof(short));
	}
				
	// Convert to real value
	float *mNewYVal;
	mNewYVal = (float*) malloc (NUM_SAMPLES * NUM_CHANNELS * sizeof(float));
	for (int m_l=0;m_l<(NUM_CHANNELS*NUM_SAMPLES);m_l++)
	{
		*(mNewYVal+m_l) = (float) (10*( *(m_buffer + m_l) ))/SHRT_MAX;
	}
	pnt->SetYValues(mNewYVal, m_x, m_u, NUM_CHANNELS, NUM_SAMPLES);
	pnt->AddClust(0);
	CalcOneSpike(pnt);
	free( m_u );
	free( m_buffer );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::StoreBPFElde()
{
	char DiskrUnit;

	if (m_ElectrodeType == 1)
	{ // Single
		forStoreBPF_file.Seek(forStoreBPF_IndexFile + 6,0);
		DiskrUnit = LookupVClust(forStoreBPF_SpikeIndex);
		forStoreBPF_file.Write(&DiskrUnit,1);
		forStoreBPF_IndexFile += bpf.SINGLE_SIZE;
	}
	
	if (m_ElectrodeType == 2)
	{ //Stereode
		forStoreBPF_file.Seek(forStoreBPF_IndexFile + 6,0);
		DiskrUnit = LookupVClust(forStoreBPF_SpikeIndex);
		forStoreBPF_file.Write(&DiskrUnit,1);
		forStoreBPF_IndexFile += bpf.STEREO_SIZE;
	}
	
	if (m_ElectrodeType == 3)
	{ // Tetrode
		forStoreBPF_file.Seek(forStoreBPF_IndexFile + 6,0);
		DiskrUnit = LookupVClust(forStoreBPF_SpikeIndex);
		forStoreBPF_file.Write(&DiskrUnit,1);
		forStoreBPF_IndexFile += bpf.TETRODE_SIZE;
	}
	
	delete forStoreBPF_NonClVx;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CVerxStack::StoreBPF(char *State)
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
			forStoreBPF_file.Read(&TimeStamp,4);
			forStoreBPF_file.Read(&Channel,1);
			if (Channel == m_ElectrodeChannel)
			{
				forStoreBPF_SpikeIndex++;
				forStoreBPF_Spike = * ( m_SpikesStack.begin() + forStoreBPF_SpikeIndex );

				int TS;
				if ( forStoreBPF_IndexVx == m_VerxStack.end() )
				{
					verx = (CVertex*) *forStoreBPF_IndexVx;
					TS = verx->GetTimeStamp();
				}
				else 
					TS = 0;
				if ( TimeStamp == TS )
				{
					DiskrUnit = LookupVClust(forStoreBPF_SpikeIndex);
					forStoreBPF_file.Write(&DiskrUnit,1);
					forStoreBPF_IndexVx ++;
					
					IndexFile += bpf.TETRODE_SIZE;
					forStoreBPF_IndexFile = IndexFile;
				}
				else{
					forStoreBPF_file.Read(&OrigCl,1);
					// Storing data to container
					forStoreBPF_NonClVx = new CVertex(m_PalStack);
					forStoreBPF_NonClVx->SetOrigCl(OrigCl);
					forStoreBPF_NonClVx->SetTimeStamp(float( TimeStamp));
					forStoreBPF_NonClVx->AddPnt( 0 );  //empty cluster's set
												
					forStoreBPF_file.Read(forStoreBPF_buffer,NUM_CHANNELS*NUM_SAMPLES*sizeof(short));
						
					// Convert to real value
					float* mNewYVal = (float*) malloc (NUM_SAMPLES * NUM_CHANNELS * sizeof(float));
					for (int m_l=0;m_l<(NUM_CHANNELS*NUM_SAMPLES);m_l++)
					{
						mNewYVal[m_l] = (float) (10 * forStoreBPF_buffer[m_l])/SHRT_MAX;
					}
					forStoreBPF_NonClVx->SetYValues(mNewYVal, m_x, forStoreBPF_u, NUM_CHANNELS, NUM_SAMPLES);
					forStoreBPF_NonClVx->AddClust(0);
					CalcOneSpike(forStoreBPF_NonClVx);
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

	buffer = (char*) malloc(file->GetLength());
	if (buffer==NULL)
	{	
		return 1;
	}

	// Loading file to memory
	int nRead=file->Read(buffer,file->GetLength());
	if ( nRead == 0 )
	{
		free(buffer);
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

/////////////////////////////////////////////////////////////////////////////
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

	fprintf(file,"%%%%END_HEADER\n");
}

