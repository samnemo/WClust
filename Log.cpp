// $Id: Log.cpp,v 1.7 2008/08/11 20:49:36 samn Exp $ 
#include "stdafx.h"
#include <time.h>
#include <string>
#include <io.h>
#include "Log.h"
#include "MyVers.h"

using namespace std;

string GetDateTime(time_t t)
{
  struct tm	*date;		/* Date/time value */
  static time_t	last_time = -1;	/* Last time value */
  static char	s[1024];	/* Date/time string */
  static const char * const months[12] =
		{		/* Months */
		  "Jan",
		  "Feb",
		  "Mar",
		  "Apr",
		  "May",
		  "Jun",
		  "Jul",
		  "Aug",
		  "Sep",
		  "Oct",
		  "Nov",
		  "Dec"
		};


  if (t != last_time)
  {
    last_time = t;
  
    date = localtime(&t);

    sprintf(s,  "%02d/%s/%04d:%02d:%02d:%02d",
	     date->tm_mday, months[date->tm_mon], 1900 + date->tm_year,
	     date->tm_hour, date->tm_min, date->tm_sec);
  }

  return string(s);
}

bool Write2LogPlain(const char* cstr,...)
{
	try
	{
		static CString strVers = GetWClustVersionString();
		const char* fname = "wclust_.log";
		FILE* fp = fopen(fname,"a+");
		if(!fp)
		{
#ifdef _DEBUG
			MessageBox(0,"Couldn't write to wclust_.log!!!","WClust - error!",MB_ICONERROR);
			int iRet = _access("wclust_.log",02);
			if(iRet==0) MessageBox(0,"Have access to write","Wclust",MB_ICONINFORMATION);
			else  MessageBox(0,"Don't have access to write wclust_.log ! ! !","Wclust",MB_ICONERROR);
#endif
			return false;
		}

		va_list ap;

		static char line[32768]={0};

		va_start(ap, cstr);
		vsprintf(line, cstr, ap);
		va_end(ap);

		fprintf(fp,"%s",line);

		fclose(fp);

		return true;
	}
	catch(...)
	{	
#ifdef _DEBUG
		MessageBox(0,"Exception in Write2Log!!!","Wclust - error",MB_ICONERROR);
#endif
		return false;
	}
}

bool Write2Log(const char* cstr,...)
{
	try
	{
		static CString strVers = GetWClustVersionString();
		const char* fname = "wclust_.log";
		FILE* fp = fopen(fname,"a+");
		if(!fp)
		{
#ifdef _DEBUG
			MessageBox(0,"Couldn't write to wclust_.log!!!","WClust - error!",MB_ICONERROR);
			int iRet = _access("wclust_.log",02);
			if(iRet==0) MessageBox(0,"Have access to write","Wclust",MB_ICONINFORMATION);
			else  MessageBox(0,"Don't have access to write wclust_.log ! ! !","Wclust",MB_ICONERROR);
#endif
			return false;
		}

		va_list ap;

		static char line[32768]={0};

		va_start(ap, cstr);
		vsprintf(line, cstr, ap);
		va_end(ap);

		fprintf(fp,"[%s V%s]: %s\n",GetDateTime(time(0)).c_str(),strVers,line);

		fclose(fp);

		return true;
	}
	catch(...)
	{	
#ifdef _DEBUG		
		MessageBox(0,"Exception in Write2Log!!!","Wclust - error",MB_ICONERROR);
#endif
		return false;
	}
}

bool WriteVec2Log(vector<float>& v)
{
	try
	{
		CString str("\n"),tmp;
		int iSz = v.size() , i;
		tmp.Format("vsz=%d\n",iSz);
		str+=tmp;
		for(i=0;i<iSz;i++)
		{
			tmp.Format("%.4f\t",v[i]);
			str += tmp;
		}
		return Write2Log(str);
	}
	catch(...)
	{
		return false;
	}
}

bool WriteVec2Log(vector<double>& v)
{
	try
	{
		std::string str("\n");
		CString tmp;
		int iSz = v.size() , i;
		tmp.Format("vsz=%d\n",iSz);
		str+=tmp;
		Write2Log(str.c_str());

		const char* fname = "wclust_.log";
		FILE* fp = fopen(fname,"a+");
		if(!fp)
			return false;

		for(i=0;i<iSz;i++)
			fprintf(fp,"%.4f\t",v[i]);
		fprintf(fp,"\n");

		fclose(fp);
		
		return true;
	}
	catch(...)
	{	try{Write2Log("Exception in WriteVec2Log!");}catch(...){}
		return false;
	}
}

bool WriteMat2Log(vector<vector<float> >& m)
{
	try
	{
		CString str("\n"),tmp;
		int iRows = m.size(), iCols = m[0].size() , i, j;
		tmp.Format("%d X %d\n",iRows,iCols);
		str+=tmp;
		for(i=0;i<iRows;i++)
		{	
			for(j=0;j<iCols;j++)
			{
				tmp.Format("%.4f\t",m[i][j]);
				str += tmp;
			}
			str += "\n";
		}
		return Write2Log(str);
	}
	catch(...)
	{
		return false;
	}
}

bool WriteMat2Log(A2D<float>& m)
{
	try
	{
		CString str("\n"),tmp;
		int iRows = m.Rows(), iCols = m.Cols() , i, j;
		tmp.Format("%d X %d\n",iRows,iCols);
		str+=tmp;
		for(i=0;i<iRows;i++)
		{	
			for(j=0;j<iCols;j++)
			{
				tmp.Format("%.4f\t",m[i][j]);
				str += tmp;
			}
			str += "\n";
		}
		return Write2Log(str);
	}
	catch(...)
	{
		return false;
	}
}

bool WriteMat2Log(A2D<int>& m)
{
	try
	{
		CString str("\n"),tmp;
		int iRows = m.Rows(), iCols = m.Cols() , i, j;
		tmp.Format("%d X %d\n",iRows,iCols);
		str+=tmp;
		for(i=0;i<iRows;i++)
		{	
			for(j=0;j<iCols;j++)
			{
				tmp.Format("%d\t",m[i][j]);
				str += tmp;
			}
			str += "\n";
		}
		return Write2Log(str);
	}
	catch(...)
	{
		return false;
	}
}


bool WriteMat2Log(vector<vector<double> >& m)
{
	try
	{
		CString str("\n"),tmp;
		int iRows = m.size(), iCols = m[0].size() , i, j;
		tmp.Format("%d X %d\n",iRows,iCols);
		str+=tmp;
		for(i=0;i<iRows;i++)
		{	
			for(j=0;j<iCols;j++)
			{
				tmp.Format("%.4f\t",m[i][j]);
				str += tmp;
			}
			str += "\n";
		}
		return Write2Log(str);
	}
	catch(...)
	{
		return false;
	}
}

bool WriteVec2Log(vector<int>& v)
{
	try
	{
		std::string str("\n");
		CString tmp;
		int iSz = v.size() , i;
		tmp.Format("vsz=%d\n",iSz);
		str+=tmp;
		Write2Log(str.c_str());

		const char* fname = "wclust_.log";
		FILE* fp = fopen(fname,"a+");
		if(!fp)
			return false;

		for(i=0;i<iSz;i++)
			fprintf(fp,"%d\t",v[i]);
		fprintf(fp,"\n");

		fclose(fp);
		
		return true;
	}
	catch(...)
	{	try{Write2Log("Exception in WriteVec2Log!");}catch(...){}
		return false;
	}
}
