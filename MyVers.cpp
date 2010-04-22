// $Id: MyVers.cpp,v 1.40 2009/06/24 18:07:20 samn Exp $ 
#include "stdafx.h"
#include "MyVers.h"

HRESULT LastError () {
   HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
   if (SUCCEEDED(hr)) 
      return E_FAIL;
   return hr;
}

// This little function emits an error message based on WIN32 error messages
void EmitErrorMsg (HRESULT hr) {
   char szMsg[1024];
   FormatMessage( 
      FORMAT_MESSAGE_FROM_SYSTEM, 
      NULL,
      hr,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      szMsg,
      1024,
      NULL 
      );
        printf("%s\n",szMsg);
}


// This function gets the file version info structure
HRESULT GetFileVersion (char *filename, VS_FIXEDFILEINFO *pvsf) {
   DWORD dwHandle;
   DWORD cchver = GetFileVersionInfoSize(filename,&dwHandle);
   if (cchver == 0)
      return LastError();
   char* pver = new char[cchver];
   BOOL bret = GetFileVersionInfo(filename,dwHandle,cchver,pver);
   if (!bret)
      return LastError();
   UINT uLen;
   void *pbuf;
   bret = VerQueryValue(pver,"\\",&pbuf,&uLen);
   if (!bret)
      return LastError();
   memcpy(pvsf,pbuf,sizeof(VS_FIXEDFILEINFO));
   delete[] pver;
   return S_OK;
} 

CString GetWClustVersionString()
{
	CString strV("2.0.0.110");
	return strV;
}
