// $Id: FileUtils.cpp,v 1.4 2009/06/24 18:02:38 samn Exp $ 
#include "stdafx.h"
#include "FileUtils.h"
#include "StringUtils.h"
#include <stdio.h>
#include <io.h>

using namespace std;

std::string GetFileExt(std::string& strPath)
{
	char drive[_MAX_DRIVE], 
		 dir[_MAX_DIR],
		 fname[_MAX_FNAME],
		 ext[_MAX_EXT];
	_splitpath(strPath.c_str(),drive,dir,fname,ext);
	std::string strExt = ext;
	return strExt;
}

CString GetFileExt(CString& strPath)
{
	char drive[_MAX_DRIVE], 
		 dir[_MAX_DIR],
		 fname[_MAX_FNAME],
		 ext[_MAX_EXT];
	_splitpath(strPath.GetBuffer(),drive,dir,fname,ext);
	CString strExt(ext);
	return strExt;
}

std::string GetFileName(std::string& strPath,bool bGetExt)
{
	char drive[_MAX_DRIVE], 
		 dir[_MAX_DIR],
		 fname[_MAX_FNAME],
		 ext[_MAX_EXT];
	_splitpath(strPath.c_str(),drive,dir,fname,ext);
	std::string strFname(fname);
	if(bGetExt)
	{	std::string strExt(ext);
		strFname += ext;
	}
	return strFname;
}

CString GetFileName(CString& strPath,bool bGetExt)
{
	char drive[_MAX_DRIVE], 
		 dir[_MAX_DIR],
		 fname[_MAX_FNAME],
		 ext[_MAX_EXT];
	_splitpath(strPath.GetBuffer(),drive,dir,fname,ext);
	CString strFname(fname);
	if(bGetExt)
	{	CString strExt(ext);
		strFname += ext;
	}
	return strFname;
}

CString GetDir(CString& strFilePath)
{
	char drive[_MAX_DRIVE], 
		 dir[_MAX_DIR],
		 fname[_MAX_FNAME],
		 ext[_MAX_EXT];
	_splitpath(strFilePath.GetBuffer(),drive,dir,fname,ext);
	CString strDir;
	strDir.Format("%s%s",drive,dir);
	return strDir;
}

CString GetFilePath(CString& strPath,bool bGetExt)
{
	char drive[_MAX_DRIVE], 
		 dir[_MAX_DIR],
		 fname[_MAX_FNAME],
		 ext[_MAX_EXT];
	_splitpath(strPath.GetBuffer(),drive,dir,fname,ext);
	CString strFname(fname);
	if(bGetExt)
		strFname.Format("%s%s%s%s",drive,dir,fname,ext);
	else
		strFname.Format("%s%s%s",drive,dir,fname);
	return strFname;
}

bool GetLines(FILE* fp,std::vector<std::string>& vstr)
{
	if(!fp) return false;
	char line[1024];
	while(!feof(fp))
	{
		line[0]=0;
		fgets(line,1024,fp);
		Chop(line); //get rid of newlines
		if(line[0])
			vstr.push_back(line);
	}
	return true;
}

bool GetLines(char* fname,vector<string>& vstr)
{
	FILE* fp = fopen(fname,"r");
	if(!fp) return false;
	bool b = GetLines(fp,vstr);
	fclose(fp);
	return b;
}

bool FileExists(const char* fname)
{
	FILE* fp = fopen(fname,"r");
	if(fp)
	{
		fclose(fp);
		return true;
	}
	return false;
}

long FileSize(const char* fname)
{
	FILE* fp = fopen(fname,"rb");
	if(!fp) return 0;
	long sz;
	fseek(fp,0,SEEK_END);
	sz = ftell(fp);
	fclose(fp);
	return sz;
}

//path is directory, ext is file type extension, vfiles will have full path of each file found
//returns true on success
bool GetFilesInDir(std::string& path,std::string& ext,std::vector<std::string>& vfiles)
{
	vfiles.clear();	
	_finddata_t c_file;
	long hFile;	
	std::string srch(path);
	if(srch[srch.size()-1]!='\\')
		srch+="\\";
	srch+="*";
	if(ext[0]!='.')
		srch+=".";
	srch+=ext;	
	if ( (hFile = _findfirst(srch.c_str(), &c_file)) == -1L )
		return false;
	else
	{	do
		{
			if(!strcmp(c_file.name,".") && !strcmp(c_file.name,".."))
				vfiles.push_back(std::string(c_file.name));
		} while ( _findnext(hFile, &c_file) == 0 );
		_findclose(hFile);
	}
	return vfiles.size()!=0;
}
