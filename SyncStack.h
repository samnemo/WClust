// SyncStack.h: interface for the CSyncStack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNCSTACK_H__F68FB042_497E_48E8_AC58_C69232E44AB5__INCLUDED_)
#define AFX_SYNCSTACK_H__F68FB042_497E_48E8_AC58_C69232E44AB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyObj.h"
#include "Sync.h"
#include <deque>

using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////

typedef deque <CSync*> MY_SYNC_STACK;

////////////////////////////////////////////////////////////////////////////////////////////

class CSyncStack : public CMyObject  
{
	int		cIndex;
public:
	MY_SYNC_STACK SyncStack;
	MY_INT_STACK TimeIntStack;
	short	LastPrevious; 
	int		NoArena, NoRoom;
	short	middle;		// middle of sync voltages (for selecting log.H or log.L)
	int		Time;			// for loading
	int		NoTimeStamp;
	char	typeOfData;	// 0 - none, 1 - BPF, 2 - UFF
// TO REMOVE	- only for diagnostic
	int		noAdd;
	int		noEmpty;
	int		lastFrame;
// END TO REMOVE
public:
	CSyncStack();
	virtual ~CSyncStack();
	// universal methods	
	void	AddSync(CSync *toStore);
	int		GetTimeStamp(int position);
	void	SetEmpty();
	void	SetFileType( char typeOfFile ) { typeOfData = typeOfFile; }
	
	// BPF methods
	void	Clustering( int TS, unsigned char newCl, unsigned char newOrigCl );
	void	CreateSync();
	void	DrawArenaRoom(CDC *pDC, CRect drawWin, int firstTS, int timeWin, char params, unsigned char *clView, int whichDraw);
	void	FinishClustering();
	void	LoadSync(CFile *from);
	int		LoadArena(CFile *from, bool hasZ = false);	// from BPF
	int		LoadArenaFromDAT(FILE *from);
	int		LoadRoom(CFile *from, bool hasZ = false);	// from BPF
	int		LoadRoomFromDAT(FILE *from);
	int		GetLoadedTS(int position);
	void	NewPosFromUFF(int TS, unsigned char RedX, unsigned char RedY, unsigned char GreenX, unsigned char GreenY);
	void	OrganizeSync(char type, float param);
	void	PrepareClustering();
	void	Save(); // <<-- This is only for TESTING. In the final version this will NOT be used
	void	SaveArenaFirst(CFile *fptr);
	void	SaveRoomFirst(CFile *fptr);
	void	SetMiddle(short middle_in) { middle = middle_in; }
	
	// UFF methods

};

#endif // !defined(AFX_SYNCSTACK_H__F68FB042_497E_48E8_AC58_C69232E44AB5__INCLUDED_)
