// $Id: Log.h,v 1.7 2009/01/28 21:40:57 samn Exp $ 
#ifndef LOG_H
#define LOG_H

#include <vector>
#include "A2D.h"

using namespace std;

bool Write2Log(const char* cstr,...);
bool Write2LogPlain(const char* cstr,...);
bool WriteVec2Log(vector<int>& v);
bool WriteVec2Log(vector<float>& v);
bool WriteVec2Log(vector<double>& v);
bool WriteMat2Log(vector<vector<float> >& m);
bool WriteMat2Log(vector<vector<double> >& m);
bool WriteMat2Log(A2D<float>& m);
bool WriteMat2Log(A2D<int>& m);
string GetDateTime(time_t t);

//stupid log file class
struct LogF
{
	FILE* m_fp;
	LogF()
		:m_fp(0)
	{}
	FILE* Open()
	{
		m_fp=fopen("wclust_.log","a+");
		return m_fp;
	}
	void Close()
	{	if(m_fp)
		{	fclose(m_fp);
			m_fp=0;
		}
	}
	~LogF()
	{	Close();
	}
};

#endif
