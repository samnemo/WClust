// $Id: EEG.h,v 1.2 2008/03/26 03:59:08 samn Exp $ 
// EEG.h: interface for the CEEG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EEG_H__9CF28DBA_6483_4144_84AC_14E04C044911__INCLUDED_)
#define AFX_EEG_H__9CF28DBA_6483_4144_84AC_14E04C044911__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyObj.h"

class CEEG : public CMyObject  
{
	int timeStamp;
	char flag;	
public:
	vector<float> eeg;
public:
	CEEG();
	virtual ~CEEG();
	void	DrawEEG(CDC *pDC, CRect eegWin, CRect drawWin, char channel, short NUM_SAMPLES, double dScale);
	int		GetTimeStamp() { return timeStamp; };
//	void	SetEEG(vector<float>& newEEG) { eeg = newEEG; };
	
	void    SetEEG(vector<short>& buffer)
	{
		int iBufSz = buffer.size(), i = 0;
		eeg.resize(iBufSz);
		for (i = 0; i < iBufSz; i++ )
			eeg[i] = (float) (10*buffer[i])/SHRT_MAX;
	}
	
	void	SetEmpty();
	void	SetTimeStamp(int TS) { timeStamp = TS;};
};

#endif // !defined(AFX_EEG_H__9CF28DBA_6483_4144_84AC_14E04C044911__INCLUDED_)
