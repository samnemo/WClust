// Sync.h: interface for the CSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNC_H__FFDB7FDD_4B19_4EE4_9D90_56D044987A2C__INCLUDED_)
#define AFX_SYNC_H__FFDB7FDD_4B19_4EE4_9D90_56D044987A2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyObj.h"

class CSync : public CMyObject  
{
	unsigned char	origCluster;	// clustered by origCluster (from BPF/UFF)
	unsigned char	cluster;
	int		TimeStamp;
public:
	CSync(int TimeSt);
	virtual	~CSync();
	void	ChangeTimeStamp(int newTimeStamp) { TimeStamp = newTimeStamp;} ;
	unsigned char	GetCluster() { return cluster; };
	unsigned char	GetOrigCluster() { return origCluster; };
	int		GetTimeStamp() { return TimeStamp; };
	void	SetCluster(unsigned char newCl) { cluster = newCl; };
	void	SetOrigCluster(unsigned char newOrigCl) { origCluster = newOrigCl; };
	void	SetTimeStamp(int newTS) { TimeStamp = newTS; };
	//virtual unsigned char GetRoomX(){ return -1;}
	//virtual unsigned char GetRoomX(){ return -1;}
};

class CSyncBPF : public CSync
{
	int				LoadedTimeStamp;
	unsigned char	ArenaX,ArenaY,ArenaZ;
	unsigned char	RoomX,RoomY,RoomZ;
	short			RoomAng;
	short			ArenaAng;
public:	
	CSyncBPF(int TimeStmp);
	virtual ~CSyncBPF(); 

	int				GetLoadedTS(){ return LoadedTimeStamp; };
	short			GetArenaAng(){ return ArenaAng; };
	unsigned char	GetArenaX(){ return ArenaX; };
	unsigned char	GetArenaY(){ return ArenaY; };
	unsigned char	GetArenaZ(){ return ArenaZ; };
	short			GetRoomAng(){ return RoomAng; };
	unsigned char	GetRoomX(){ return RoomX; };
	unsigned char	GetRoomY(){ return RoomY; };
	unsigned char	GetRoomZ(){ return RoomZ; };
	void			SetArena( unsigned char X, unsigned char Y, unsigned char Z, short Ang){ ArenaX=X; ArenaY=Y; ArenaZ=Z; ArenaAng=Ang;};
	void			SetLoadedTS(int TS){ LoadedTimeStamp = TS; }; // from *.Dat
	void			SetRoom( unsigned char X, unsigned char Y, unsigned char Z, short Ang){ RoomX=X; RoomY=Y; RoomZ=Z; RoomAng=Ang;};
};

class CSyncUFF : public CSync
{
	unsigned char	RedX,RedY;
	unsigned char	GreenX,GreenY;
	short			Angel;
public:	
	CSyncUFF(int TimeStmp);
	virtual ~CSyncUFF();

	unsigned char	GetGreenX(){ return GreenX; };
	unsigned char	GetGreenY(){ return GreenY; };
	unsigned char	GetRedX(){ return RedX; };
	unsigned char	GetRedY(){ return RedY; };
	
	void	SetGreen( unsigned char X, unsigned char Y){ GreenX = X; GreenY = Y; };
	void	SetRed( unsigned char X, unsigned char Y){ RedX = X; RedY = Y; };
};



#endif // !defined(AFX_SYNC_H__FFDB7FDD_4B19_4EE4_9D90_56D044987A2C__INCLUDED_)
