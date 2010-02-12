// $Id: EEGStack.h,v 1.3 2008/04/01 06:07:41 samn Exp $ 
// EEGStack.h: interface for the CEEGStack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EEGSTACK_H__5B949062_2476_4433_BA7A_18491BA2F3F1__INCLUDED_)
#define AFX_EEGSTACK_H__5B949062_2476_4433_BA7A_18491BA2F3F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////
#include "MyObj.h"
#include "eeg.h"
#include <deque>
#include <vector>
#include <string>
/////////////////////////////
using namespace std;

typedef deque <CEEG*> MY_EEG_STACK;

/////////////////////////////
class CEEGStack : public CMyObject  
{
	MY_EEG_STACK	eegStack;
	char			NUM_CHANNELS;	
	short			NUM_SAMPLES;
	int				FREQUENCY;
	vector<string>  m_vNames;
public:
	CEEGStack();
	virtual ~CEEGStack();
	void	AddEEG(CEEG *toStore);
	void	DrawEEG( CDC *pDC, CRect drawWin, int startTS, int timeWindow, unsigned int params , vector<double>& vGainList);
	char	LoadEEG(CFile *file);
	char	GetChannels() { return NUM_CHANNELS; };
	int		GetFirstTimeStamp();
	int		GetLastTimeStamp();
	void	SetEmpty();
	void	SetParams(char numChannels, short numSamples);
	void	SetFrequency( int intFreq ) { FREQUENCY = intFreq; };
	int		GetFrequency() { return FREQUENCY; }

};

#endif // !defined(AFX_EEGSTACK_H__5B949062_2476_4433_BA7A_18491BA2F3F1__INCLUDED_)
