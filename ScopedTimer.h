// $Id: ScopedTimer.h,v 1.4 2008/07/24 18:47:51 samn Exp $ 
#pragma once

#include <windows.h>
#include <string>

#include "Log.h"

struct ScopedTimer
{
	DWORD S,E;
	std::string msg;

	ScopedTimer(char* c)
	{
		if(c) msg = c;
		S = GetTickCount();
	}

	~ScopedTimer()
	{
		E = GetTickCount();
		
		char msg2[1024];
		
		if(msg.length())
			sprintf(msg2,"%s time = %d",msg.c_str(),E-S);
		else
			sprintf(msg2,"time = %d",E-S);

		Write2Log(msg2);		
	}
};

struct MTimer
{
	int iCount;
	std::string msg;
	DWORD totalT;
	MTimer(char* c)
	{	iCount=0;
		if(c) msg=c;
		totalT=0;
	}
	void Count(DWORD T)
	{	iCount ++;
		totalT += T;
	}
	~MTimer()
	{
		if(iCount)
			Write2Log("%s MTimer calls=%d totalT=%d time/call=%g",msg.c_str(),iCount,totalT,(double)totalT/(double)iCount);
		else
			Write2Log("%s MTimer calls=0 totalT=0");
	}
};

struct TimerInc
{
	MTimer& _t;
	DWORD S;
	TimerInc(MTimer& t)
		:_t(t),
		S(GetTickCount())
	{
	}
	~TimerInc()
	{
		_t.Count(GetTickCount()-S);
	}
};
