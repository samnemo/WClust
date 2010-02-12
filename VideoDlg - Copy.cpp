// VideoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "VideoDlg.h"
#include "Log.h"


// VideoDlg dialog

IMPLEMENT_DYNAMIC(VideoDlg, CDialog)
VideoDlg::VideoDlg(CWnd* pParent /*=NULL*/)
	: cdxCSizingDialog(VideoDlg::IDD, pParent)
{
}

VideoDlg::~VideoDlg()
{
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL VideoDlg::OnInitDialog()
{
	return TRUE;
}

void VideoDlg::OnPaint()
{
	//MessageBox("onpaint");

	CPaintDC dc(this); // device context for painting
	dc.SetBkColor(RGB(255,255,255));
	
	CBitmap oBitmap;

	CDC memDC;
	if(!memDC.CreateCompatibleDC(&dc))
	{
		Write2Log("Couldn't create memDC");
		retun;
	}
	
	//oBitmap.CreateCompatibleBitmap(&dc,m_oAVIReader.Width(),m_oAVIReader.Height());
	//memDC.SelectObject(&oBitmap);

	//HDRAWDIB hdd;						// Handle For Our Dib
	//HBITMAP hBitmap;					// Handle To A Device Dependant Bitmap
	//HDC hdc = CreateCompatibleDC(0);	// Creates A Compatible Device Context

	LPBITMAPINFOHEADER pBMP;
	//byte* pdata = new byte[3*m_oAVIReader.Width()*m_oAVIReader.Height()];

	pBMP = m_oAVIReader.GetCurrentFrame();

	if(!pBMP)
		return;

	//oBitmap.CreateBitmapIndirect(

	byte* pdata;
	HBITMAP hBitmap = CreateDIBSection (memDC.GetSafeHdc(), (BITMAPINFO*)(pBMP), DIB_RGB_COLORS, (void**)(&pdata), NULL, NULL);
	BITMAP bm; 
	GetObject(hBitmap,sizeof(bm),&bm);
	CBitmap* pCBitmap = memDC.SelectObject(&bm);
	//memDC.SelectObject
	//SelectObject (hdc, hBitmap);								// Select

	////////////////////////
	//CClientDC dc(this); // always and only attached to screen (frame buffer) bitmap
    CRect rect;
    GetClientRect(&rect);

    //bitmap.LoadBitmap(IDR_LOGOIMAGE); // load from "bmp" file identified in resource file
    //bitmap.GetObject (sizeof (BITMAP), &bm); // get information on this bitmap
    //CPoint size (bm.bmWidth, bm.bmHeight); // among which is width and height
    //pDC->DPtoLP (&size); // to be sure convert to logical units

    //CPoint org (0, 0);
    //pDC->DPtoLP (&org);

    //CDC dcMem; // memory device context for drawing on memory bitmap
    //dcMem.CreateCompatibleDC (&dc); // make sure it is compatible with hardware device context
	//dcMem.SelectObject(&bm);
    //dcMem.SelectObject (bitmap); // select bitmap into the memory device context
    //dcMem.SetMapMode (dc.GetMapMode ()); // get compatible modes

    //pDC->BitBlt (x, y, size.x, size.y, &dcMem, org.x, org.y, SRCCOPY); // transfer bitmap to screen
	/////////////////////////


	/*if(!oB.CreateBitmapIndirect(&bm))
	{
		Write2Log("Couldn't create bitmap for frame %d of %d",m_oAVIReader.CurrentFrame(),m_oAVIReader.LastFrame());
		return;
	}*/
	//oBMDC.SelectObject(&oB);
	if(!dc.BitBlt(0,0,m_oAVIReader.Width(),m_oAVIReader.Height(),&memDC,0,0,SRCCOPY))
	{
		Write2Log("Couldn't draw bitmap to window");
	}
	//dc.StretchBlt(25,25,
	//	m_oAVIReader.Width(),m_oAVIReader.Height(),
	//	&oBMDC,0,0,m_oAVIReader.Width(),m_oAVIReader.Height(),
	//	SRCCOPY);

	DeleteObject(hBitmap);
}

void VideoDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
}


// VideoDlg message handlers

void VideoDlg::OnBnClickedButtonPickVid()
{
	// TODO: Add your control notification handler code here
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

	int iFrame = m_oAVIReader.FirstFrame(), iMSPerFrame = m_oAVIReader.MSPerFrame();
	for(;iFrame<2; iFrame++) // m_oAVIReader.LastFrame();iFrame++)
	{
		m_oAVIReader.GoToFrame(iFrame);
		//Invalidate();
		InvalidateRect(NULL,FALSE);
		//OnPaint();
		Sleep(iMSPerFrame);
	}
	
	if(!m_oAVIReader.Close())
	{	MessageBox("Couldn't close vid!");
	}
}
