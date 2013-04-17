// Sync.cpp: implementation of the CSync class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wclust.h"
#include "Sync.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CSync
//////////////////////////////////////////////////////////////////////

CSync::CSync(int TimeSt)
{
	TimeStamp = TimeSt;
}

CSync::~CSync()
{
}

//////////////////////////////////////////////////////////////////////
// CSyncBPF
//////////////////////////////////////////////////////////////////////

CSyncBPF::CSyncBPF(int TimeStmp) : CSync(TimeStmp) 
{
	ArenaX = 0;
	ArenaY = 0;
	ArenaZ = 0;
	ArenaAng = 0;
	RoomX = 0;
	RoomY = 0;
	RoomZ = 0;
	RoomAng = 0;
	RoomAngX = RoomAngY = ArenaAngX = ArenaAngY = 0;
	LoadedTimeStamp = 0;
}

CSyncBPF::~CSyncBPF()
{
}

//////////////////////////////////////////////////////////////////////
// CSyncUFF
//////////////////////////////////////////////////////////////////////

CSyncUFF::CSyncUFF(int TimeStmp) : CSync(TimeStmp) 
{
	GreenX = 0;
	GreenY = 0;
	RedX = 0;
	RedY= 0;
	Angel = 0;
}

CSyncUFF::~CSyncUFF()
{
}
