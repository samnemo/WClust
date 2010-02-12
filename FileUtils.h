// $Id: FileUtils.h,v 1.4 2009/06/24 18:01:15 samn Exp $ 
#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>

std::string GetFileExt(std::string& strPath);
CString GetFileExt(CString& strPath);
std::string GetFileName(std::string& strPath,bool bGetExt);
CString GetFileName(CString& strPath,bool bGetExt);
CString GetFilePath(CString& strPath,bool bGetExt);
CString GetDir(CString& strFilePath);
bool GetLines(FILE* fp,std::vector<std::string>& vstr);
bool GetLines(char* fname,std::vector<std::string>& vstr);
bool FileExists(const char* fname);
long FileSize(const char* fname);

//path is directory, ext is file type extension, vfiles will have full path of each file found
//returns true on success
bool GetFilesInDir(std::string& path,std::string& ext,std::vector<std::string>& vfiles);

#endif
