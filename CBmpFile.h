// $Id: CBmpFile.h,v 1.1 2008/03/29 03:13:43 samn Exp $ 
// CBmpFile is Copyright (c)1998 Jonathan Nix
// All Rights Reserved.

#pragma once

#include <Windows.h>
#include <mmsystem.h> // incase MFC

// This lib supports 24bpp and only 24bpp.
// You can convert to other formats using the GDI.

struct PIXEL
{
	BYTE red, green, blue;
};

typedef PIXEL *LPRGB;

class CBmpFile
{
public:
	BITMAPINFO* GetBitmapInfo();
	int GetFileSize();
	int GetBpp();
	int Width(); // Pixels...
	int Height();
	CBmpFile();
	CBmpFile(LPSTR szFilename);
	~CBmpFile();

	bool Load(LPSTR szFilename);
	bool Load(HANDLE hFile);
	bool Load(BITMAPINFOHEADER* p);

	bool Save(LPSTR szFilename);
	bool Save(HANDLE hFile);

	LPRGB GetRGB();
	bool IsValid() { return m_nIsValid==1; }
	DWORD GetNumPixels() { return m_nNumPixels; }

	BOOL Maskify(PIXEL rgbBkgColor);
	BOOL BlueScreen(PIXEL rgbSetTo, PIXEL rgbThreshold);

	HBITMAP CreateHandle(HDC hDC);

protected:
	int m_nNumPixels;
	void Init();
	void Destroy();

	int m_nIsValid;

	BITMAPFILEHEADER m_bmFileHeader;
	BITMAPINFOHEADER m_bmInfoHeader;
	LPRGB m_pRGB;

private:
};
