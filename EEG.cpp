// $Id: EEG.cpp,v 1.2 2008/03/26 03:56:47 samn Exp $ 
// EEG.cpp: implementation of the CEEG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wclust.h"
#include "EEG.h"
# include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEEG::CEEG()
{

}

CEEG::~CEEG()
{
	
}

//////////////////////////////////////////////////////////////////////
void CEEG::SetEmpty()
{
	eeg.clear();
}

//////////////////////////////////////////////////////////////////////
void CEEG::DrawEEG(CDC *pDC, CRect eegWin, CRect drawWin, char channel, short NUM_SAMPLES, double dScale)
{
	unsigned long color = RGB(0,0,0);
	CPen newPen(PS_SOLID,1,color);
	CPen *pOldPen=pDC->SelectObject(&newPen);

	char fine = 0;
	int y, yPrev;
	float fy;
	float t=0;
	float tStep=(float) (NUM_SAMPLES - 1) / eegWin.Width();

	if ( eegWin.right > drawWin.right )
	{
		eegWin.right = drawWin.right;
	}

	if ( eegWin.left < drawWin.left )
	{
		t = tStep * ( drawWin.left - eegWin.left );
		eegWin.left = drawWin.left;
	}
	
	if ( (int)floor(t)>999 )
		fine = 1;
	else{
		fy = dScale*eeg[(int)floor(t) + channel * NUM_SAMPLES]; 
		yPrev = eegWin.bottom - fy*eegWin.Height()/20 - eegWin.Height()/2;

		int i = eegWin.left + 1; 

		while (i <= eegWin.right && !fine)
		{
			fy = dScale*eeg[(int)floor(t) + channel * NUM_SAMPLES]; 
			y = eegWin.bottom - fy*eegWin.Height()/20 - eegWin.Height()/2;
		
			pDC->MoveTo(i-1,yPrev);
			pDC->LineTo(i,y);
			
			yPrev=y;

			t += tStep;
			if ( (int)floor(t)>999 )
				fine = 1;
			i++;
		}
	}
	pDC->SelectObject(pOldPen);
}
