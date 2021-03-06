// $Id: VideoDlg.cpp,v 1.1 2008/03/29 03:12:37 samn Exp $ 
// VideoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "VideoDlg.h"
#include "Log.h"

const int IDT_VID_FRAME = 164;

// VideoDlg dialog

IMPLEMENT_DYNAMIC(VideoDlg, CDialog)
VideoDlg::VideoDlg(CWnd* pParent /*=NULL*/)
	: cdxCSizingDialog(VideoDlg::IDD, pParent)
{
	m_hdd=DrawDibOpen();
}

VideoDlg::~VideoDlg()
{
	m_oAVIReader.Close();
	DrawDibClose(m_hdd);
}

void VideoDlg::DoDataExchange(CDataExchange* pDX)
{
	cdxCSizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VideoDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VideoDlg, cdxCSizingDialog)
	//{{AFX_MSG_MAP(VideoDlg)
	ON_BN_CLICKED(IDC_BUTTON_PICK_VID, OnBnClickedButtonPickVid)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL VideoDlg::OnInitDialog()
{
	return TRUE;
}

bool SaveBMP(BITMAPINFOHEADER* pBMP, char* fname)
{
	BITMAPFILEHEADER bfh;
    bfh.bfType=19778;    //BM header

    //bfh.bfSize=55 + pBMP->biSizeImage;
	bfh.bfSize=sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + pBMP->biSizeImage;
    bfh.bfReserved1=0;
    bfh.bfReserved2=0;
    bfh.bfOffBits=sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER); //54
    
    FILE* fp=fopen(fname, "wb");
    if (fp!=NULL)
    {
        fwrite(&bfh, sizeof(bfh), 1, fp);
		fwrite(pBMP, sizeof(BITMAPINFOHEADER), 1, fp);
		byte* bits = (byte*)pBMP+pBMP->biSize+pBMP->biClrUsed*sizeof(RGBQUAD);
        fwrite(bits, pBMP->biSizeImage, 1, fp);
        fclose(fp);
    }
    else
    {
        Write2Log("Error writing the bitmap file");
        return false;
    }
	return true;
}

void VideoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	/*
	CRect rect;
	GetClientRect(&rect);
	CFont font;
    int nHeight = -((dc.GetDeviceCaps (LOGPIXELSY) * 20) / 72);

    font.CreateFont (nHeight, 0, 0, 0, FW_BOLD, TRUE, 0, 0,
        DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman");

    //dc.SetBkMode (TRANSPARENT);
    dc.SetBkColor(RGB(255,0,255));
    dc.SetTextColor (RGB (255, 255, 255));
	
    CFont* pOldFont = dc.SelectObject (&font);
    dc.DrawText ("Hello, MFC", -1, &rect, DT_SINGLELINE | DT_CENTER |
        DT_VCENTER);
    dc.SelectObject (pOldFont);*/


	Write2Log("Up to frame %d",m_oAVIReader.CurrentFrame());

	LPBITMAPINFOHEADER pBMP = m_oAVIReader.GetCurrentFrame();

	if(!pBMP)
		return;

	SaveBMP(pBMP,"test.bmp");

	byte* pdata=(byte*)pBMP+pBMP->biSize+pBMP->biClrUsed*sizeof(RGBQUAD);

	// Convert Data To Requested Bitmap Format & Draw IT!!!
	DrawDibDraw(m_hdd, dc.GetSafeHdc(), 32, 32, pBMP->biWidth, pBMP->biHeight, pBMP, pdata, 0, 0, m_oAVIReader.Width(), m_oAVIReader.Height(), 0);
}

void VideoDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	cdxCSizingDialog::OnShowWindow(bShow, nStatus);
}


// VideoDlg message handlers

void VideoDlg::OnBnClickedButtonPickVid()
{
	m_oAVIReader.Close(); //close before re-opening

	static char BASED_CODE szFilter[] = "AVI video files (*.avi)|*.avi|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "avi", "*.avi", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if(dlg.DoModal()!=IDOK)
		return;

	CString cstrAVIPath=dlg.GetPathName();
	CString strMsg;
	if(!m_oAVIReader.Open(cstrAVIPath))
	{	strMsg.Format("Couldn't open %s",cstrAVIPath);
		MessageBox(strMsg);
	}
	else
	{
		strMsg.Format("Opened %s w=%d h=%d firstf=%d lastf=%d",
			cstrAVIPath,
			m_oAVIReader.Width(),
			m_oAVIReader.Height(),
			m_oAVIReader.FirstFrame(),
			m_oAVIReader.LastFrame());
		MessageBox(strMsg);
	}
	m_oAVIReader.GoToFrame(m_oAVIReader.FirstFrame());
	if(!SetTimer(IDT_VID_FRAME,10,NULL))
	{
		Write2Log("Couldn't start timer!");
	}
	InvalidateRect(NULL,FALSE);
}

void VideoDlg::OnTimer(UINT nIDEvent) 
{
	InvalidateRect(NULL,FALSE);
	if(m_oAVIReader.CurrentFrame()+1<m_oAVIReader.LastFrame())
		m_oAVIReader.GoToFrame(m_oAVIReader.CurrentFrame()+1);
	else
		m_oAVIReader.GoToFrame(m_oAVIReader.FirstFrame());
	cdxCSizingDialog::OnTimer(nIDEvent);
}
