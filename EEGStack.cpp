// $Id: EEGStack.cpp,v 1.6 2008/04/09 04:57:26 samn Exp $ 
// EEGStack.cpp: implementation of the CEEGStack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wclust.h"
#include "EEGStack.h"
#include "Hist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEEGStack::CEEGStack()
{
	NUM_CHANNELS = 0;
	NUM_SAMPLES = 0;
	FREQUENCY = 0;
}

CEEGStack::~CEEGStack()
{
	SetEmpty();
}

//////////////////////////////////////////////////////////////////////
void CEEGStack::AddEEG(CEEG *toStore)
{
	eegStack.push_back(toStore);
}

//////////////////////////////////////////////////////////////////////
void CEEGStack::SetParams(char numChannels, short numSamples)
{
	NUM_CHANNELS = numChannels;
	NUM_SAMPLES = numSamples;
}

//////////////////////////////////////////////////////////////////////
void CEEGStack::SetEmpty()
{
	MY_EEG_STACK::iterator index;
	CEEG *eeg;
	for ( index = eegStack.begin(); index != eegStack.end(); index++)
	{
		eeg = (CEEG*) *index;
		eeg->SetEmpty();
		delete eeg;
	}
	NUM_CHANNELS = 0;
	NUM_SAMPLES = 0;
	eegStack.clear();
}

//////////////////////////////////////////////////////////////////////
char CEEGStack::LoadEEG(CFile *file)
{
	const int iBufferSz = NUM_CHANNELS * NUM_SAMPLES;
	int TS;
	vector<short> buffer(iBufferSz);
	file->Read(&TS,4);
	file->Read(&buffer[0], iBufferSz * sizeof(short) );
		
	CEEG* newEEG = new CEEG();
	newEEG->SetTimeStamp(TS);
	newEEG->SetMyPalette(m_PalStack);

	newEEG->SetEEG(buffer);
	AddEEG(newEEG);

	return 0;
}

//////////////////////////////////////////////////////////////////////
void CEEGStack::DrawEEG( CDC *pDC, CRect drawWin, int startTS, int timeWindow, unsigned int params, vector<double>& vGainList)
{
	timeWindow *= 10;
	int minT,maxT;	
	int indexEEG = eegStack.size() / 2 ; // start at middle of stack
	int increment = indexEEG;
	minT = 0; maxT = eegStack.size() - 1;
	char finishedFind = 0;
	while (increment > 0) // find index into eegStack that corresponds to startTS timestamp 
	{					  // this is done with binary search
		if ( startTS > ((CEEG*) *(eegStack.begin() + indexEEG))->GetTimeStamp() )
		{
			minT = indexEEG;
			increment = (maxT - minT)/2;
			indexEEG += increment;
		}
		else{
			if ( startTS < ((CEEG*) *(eegStack.begin() + indexEEG))->GetTimeStamp() )
			{
				maxT = indexEEG;
				increment = (maxT - minT)/2;
				indexEEG -= increment;
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
		indexEEG = minT;
		if ( startTS > ((CEEG*) *(eegStack.begin() + indexEEG))->GetTimeStamp() )
		{
			indexEEG = maxT;
		}
		else indexEEG = minT;
	}

	// Draw
	int x1,x2;
	char fine = 0;
	float ratioTS = (float)drawWin.Width() / (float)timeWindow;
	int indexFirst = indexEEG;
	if (indexEEG >= eegStack.size()) //??
	{
		indexEEG = eegStack.size();
		fine = 1;
	}
//	if ( ((CEEG*) *(eegStack.begin() + indexEEG))->GetTimeStamp() >= startTS )
	{ 
		unsigned char	toDraw = 0;
		unsigned char	bite = 4;
		for ( int i = 0; i < NUM_CHANNELS; i++ )
		{
			if ( params & ( bite << i ) )
				toDraw++;
		}
		int	ySize1EEG = drawWin.Height() / toDraw;

		if ( indexEEG > 0 )
		{
			indexEEG--;
		}
		
		double dGainSum = 0.0;
		vector<int> vHeights(NUM_CHANNELS);
		for(i=0;i<NUM_CHANNELS;i++)
		{	if( !(params & ( bite << i ))) // if channel shouldn't be drawn
				continue;
			dGainSum += vGainList[i];
		}
		for(i=0;i<NUM_CHANNELS;i++)
			vHeights[i]=(vGainList[i]/dGainSum)*drawWin.Height();

		while ( ! fine && ((CEEG*) *(eegStack.begin() + indexEEG))->GetTimeStamp() < startTS + timeWindow)
		{	// draw from startTS until startTS+timeWindow units
			{
				x1 = drawWin.left + ratioTS * (((CEEG*) *(eegStack.begin() + indexEEG))->GetTimeStamp() - startTS);
				x2 = drawWin.left + ratioTS * (((CEEG*) *(eegStack.begin() + indexEEG))->GetTimeStamp() - startTS + NUM_SAMPLES*10000/FREQUENCY); //350 ~ 4000

				int	yWin = 0, yPos = 0;
				CRect eegWin;
				
				char iCurCh = 0;
				for (  iCurCh = 0; iCurCh < NUM_CHANNELS; iCurCh++ )	//draw portion of each eeg channel
				{	if ( params & ( bite << iCurCh ) )
					{
						eegWin.left = x1;
						eegWin.right = x2;
						eegWin.top = drawWin.top + yPos;
						eegWin.bottom = drawWin.top + yPos + vHeights[iCurCh];
						((CEEG*) *(eegStack.begin() + indexEEG))->DrawEEG(pDC,eegWin,drawWin,iCurCh,NUM_SAMPLES,vGainList[iCurCh]);
						yWin++;
						yPos += vHeights[iCurCh];
					}
				}
			}
			indexEEG++;
			if (indexEEG >= eegStack.size())
			{
				indexEEG = 0;//--;
				fine = 1;
			}
		}
	}
//	char a[200];
//	sprintf(a,"%d  %d " ,startTS, startTS + timeWindow);
//	pDC->TextOut(10,30,a);
}

//////////////////////////////////////////////////////////////////////
int	CEEGStack::GetFirstTimeStamp()
{
	if ( *eegStack.begin() != NULL )
		return ((CEEG*) *eegStack.begin())->GetTimeStamp();
	return 0;
}

//////////////////////////////////////////////////////////////////////
int	CEEGStack::GetLastTimeStamp()
{
	if ( *(eegStack.end()-1) != NULL )
		return ((CEEG*) *(eegStack.end()-1))->GetTimeStamp();
	return 0;
}
