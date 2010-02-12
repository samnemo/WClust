// $Id: AviReader.cpp,v 1.3 2008/04/24 23:41:24 samn Exp $ 
#include "stdafx.h"
#include "AviReader.h"
#include "Log.h"

bool AVIReader::bInitLib = false;
int AVIReader::iReaders = 0;

AVIReader::AVIReader()
{
	iReaders++;
	if(!bInitLib)
	{
		AVIFileInit();// Opens The AVIFile Library
		bInitLib = true;
	}

	m_iCurrFrame = 0;
	m_iLastFrame = 0;
	m_iFirstFrame = 0;
	m_iHeight = 0;
	m_iWidth = 0;
	m_dMPF = 0.0;
	m_pAVIStream = 0;
	m_pAVIFile = 0;
	m_pGetFrame = 0;
	m_bOpen = false;
}

AVIReader::~AVIReader()
{
	Close();
	iReaders--;
	if(iReaders==0)
	{
		AVIFileExit();	// Release The File
	}
}

bool AVIReader::IsOpen()
{
	return m_bOpen;
}

bool AVIReader::Open(const char* fname)
{
	m_bOpen = false;
	int res = 0;

	// Opens The AVI Stream
	res = AVIStreamOpenFromFile(&m_pAVIStream, fname, streamtypeVIDEO, 0, OF_READ, NULL);
	if(res!=AVIERR_OK)
    {	//an error occurs
		Close();
		return false;
	}

	// Reads Information About The Stream Into psi
	AVIStreamInfo(m_pAVIStream, &m_oStreamInfo, sizeof(m_oStreamInfo));

	// Width Is Right Side Of Frame Minus Left
	m_iWidth=m_oStreamInfo.rcFrame.right-m_oStreamInfo.rcFrame.left;
	// Height Is Bottom Of Frame Minus Top
	m_iHeight=m_oStreamInfo.rcFrame.bottom-m_oStreamInfo.rcFrame.top;

	// The First Frame Of The Stream
	m_iFirstFrame=AVIStreamStart(m_pAVIStream);
    if(m_iFirstFrame==-1)
    {	//Error getting the frame inside the stream
		Close();
		return false;
	}

	// The Last Frame Of The Stream
	m_iLastFrame=AVIStreamLength(m_pAVIStream);
	if(m_iLastFrame == -1)
	{
		Close();
		return false;
	}

	// Calculate Rough Milliseconds Per Frame
	m_dMPF=AVIStreamSampleToTime(m_pAVIStream,m_iLastFrame)/static_cast<double>(m_iLastFrame);
	//m_dMPF = 1000.0 / (m_oStreamInfo.dwRate / (double)(m_oStreamInfo.dwScale?m_oStreamInfo.dwScale:1.0));

	try
	{
		// Create The PGETFRAME	Using Our Request Mode
		m_pGetFrame=AVIStreamGetFrameOpen(m_pAVIStream, NULL);
		if (m_pGetFrame==NULL)
		{	// An Error Occurred Opening The Frame
			//MessageBox (HWND_DESKTOP, "Failed To Open The AVI Frame", "Error", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}
	}
	catch(...)
	{
		Write2Log("AVIReader::Open: Exception !!");
		return false;
	}
	m_bOpen = true;
	return true;
}

bool AVIReader::Close()
{	
	if(m_pAVIFile)
		AVIFileRelease(m_pAVIFile);
	// Deallocates The GetFrame Resources
	if(m_pGetFrame)
		AVIStreamGetFrameClose(m_pGetFrame);
	// Release The Stream
	if(m_pAVIStream)
		AVIStreamRelease(m_pAVIStream);

	m_iCurrFrame = 0;
	m_iFirstFrame = 0;
	m_iLastFrame = 0;
	m_iHeight = 0;
	m_iWidth = 0;
	m_dMPF = 0.0;

	m_pAVIStream = 0;
	m_pGetFrame = 0;

	m_bOpen = false;
	
	return true;
}

BOOL CreateFromPackedDIBPointer(LPBYTE pDIB, int iFrame)
{
    ASSERT(pDIB!=NULL);

    //Creates a full-color (no palette) DIB from a pointer to a

    //full-color memory DIB
    //get the BitmapInfoHeader

    BITMAPINFOHEADER bih;
    RtlMoveMemory(&bih.biSize, pDIB, sizeof(BITMAPINFOHEADER));

    //now get the bitmap bits

    if (bih.biSizeImage < 1)
    {
        return FALSE;
    }

    BYTE* Bits=new BYTE;

    RtlMoveMemory(Bits, pDIB + sizeof(BITMAPINFOHEADER), bih.biSizeImage);

    //and BitmapInfo variable-length UDT

    BYTE memBitmapInfo[40];
    RtlMoveMemory(memBitmapInfo, &bih, sizeof(bih));

    BITMAPFILEHEADER bfh;
    bfh.bfType=19778;    //BM header

    bfh.bfSize=55 + bih.biSizeImage;
    bfh.bfReserved1=0;
    bfh.bfReserved2=0;
    bfh.bfOffBits=sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER); //54

    
    CString FileName;
    FileName.Format("Frame-%05d.bmp", iFrame);
    
    FILE* fp=fopen(FileName, "wb");
    if (fp!=NULL)
    {
        fwrite(&bfh, sizeof(bfh), 1, fp);
        fwrite(&memBitmapInfo, sizeof(memBitmapInfo), 1, fp);
        fwrite(Bits, bih.biSizeImage, 1, fp);
        fclose(fp);
    }
    else
    {
        TRACE0(_T("Error writing the bitmap file"));
        return FALSE;
    }

    delete [] Bits;
    return TRUE;
}

LPBITMAPINFOHEADER AVIReader::GetFrame(int iFrame)
{
	if(iFrame < m_iFirstFrame || iFrame >= m_iLastFrame)
		return 0;
	
	// Grab Data From The AVI Stream
	return (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_pGetFrame, iFrame);
}

LPBITMAPINFOHEADER AVIReader::GetCurrentFrame()
{
	return GetFrame(m_iCurrFrame);
}

int AVIReader::FirstFrame()
{
	return m_iFirstFrame;
}

int AVIReader::LastFrame()
{
	return m_iLastFrame;
}

int AVIReader::CurrentFrame()
{
	return m_iCurrFrame;
}

bool AVIReader::GoToFrame(int iFrame)
{
	if(iFrame < m_iFirstFrame || iFrame > m_iLastFrame)
		return false;
	
	m_iCurrFrame = iFrame;

	return true;
}

bool AVIReader::FreeFrame(byte*& pFrame)
{
	if(!pFrame)
		return false;
	delete [] pFrame;
	pFrame = 0;
	return true;
}

int AVIReader::Width()
{
	return m_iWidth;
}

int AVIReader::Height()
{
	return m_iHeight;
}

double AVIReader::MSPerFrame()
{
	return m_dMPF;
}

