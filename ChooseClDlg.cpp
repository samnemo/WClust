// ChooseClDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wclust.h"
#include "ChooseClDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	IDT_INVALIDATE_VIEW	115
/////////////////////////////////////////////////////////////////////////////
// CChooseClDlg dialog


CChooseClDlg::CChooseClDlg(CWnd* pParent /*=NULL*/,std::vector<unsigned char>& toView_)
	: cdxCSizingDialog(CChooseClDlg::IDD, pParent),
	  toView(toView_)
{
	//{{AFX_DATA_INIT(CChooseClDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChooseClDlg::DoDataExchange(CDataExchange* pDX)
{
	cdxCSizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseClDlg)
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_LIST_VERX, m_wndListVerx);
	DDX_Control(pDX, IDC_LIST_PNTS, m_wndListPnts);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseClDlg, cdxCSizingDialog)
	//{{AFX_MSG_MAP(CChooseClDlg)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PNTS, OnItemchangedListPnts)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VERX, OnItemchangedListVerx)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseClDlg message handlers

void CChooseClDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	cdxCSizingDialog::OnShowWindow(bShow, nStatus);
	
	HICON pom1 = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CWnd::SetIcon(pom1,true);

//	maxCluster = ( m_MainDataStack->m_NumOriginalCl > (m_MainClusters->m_Count - 1) ) ? m_MainDataStack->m_NumOriginalCl : ( m_MainClusters->m_Count - 1 );
	maxCluster = m_MainClusters->m_Count-1;
	FillImageList();
	FillClList(1);	// points
	FillClList(2);	// vectors
	listFilled = 1;
	SetTimer(IDT_INVALIDATE_VIEW,10,NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CChooseClDlg::FillClList(char select)
{
	CString		Str;
	CListCtrl	*list;
	bool		state;
	unsigned char times;
	

	switch (select)
	{
	case 1:	// POINTS
		list = & m_wndListPnts;
		times = 1;
		Str.Format("points of clusters");
		break;
	case 2:	// VECTORS
		list = & m_wndListVerx;
		times = 2;
		Str.Format("boundaries of clusters");
		break;
	}
	list->InsertColumn(0,Str,LVCFMT_LEFT,80);
	list->SetImageList(&m_imageList,LVSIL_SMALL);
	list->SetExtendedStyle( LVS_EX_CHECKBOXES  );

	list->InsertItem(0,Str,0);
	Str.Format("noise");
	list->SetItemText(0,0,Str);

	if ( (toView[255] & times) != 0)
		state = true;
	else
		state = false;
	list->SetCheck(0,state );
	
	list->InsertItem(1,Str,1);
	Str.Format("non cl");
	list->SetItemText(1,0,Str);

	if ( (toView[0] & times) != 0)
		state = true;
	else
		state = false;
	list->SetCheck(1,state );

	for (int i = 1; i <= maxCluster ; i++)
	{
		list->InsertItem(i+1,Str,i+1);
		Str.Format("cl %d",(i));
		list->SetItemText(i+1,0,Str);

		if ( (toView[i] & times) != 0)
			state = true;
		else
			state = false;
		list->SetCheck(i+1,state );
	}
}

/////////////////////////////////////////////////////////////////////////////
void CChooseClDlg::FillImageList()
{
	CBitmap bitmap;
	m_imageList.Create(16, 16, ILC_COLORDDB , 8, 255); // 32, 32 for large icons

		bitmap.LoadBitmap(IDB_CCCCCC);	// NOISE
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_909090);	// non cl
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_FF0000);	// cl 1
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_0000FF);	// cl 2
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_00FF00);	// cl 3
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_F000F0);	// cl 4
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_9000F0);	// cl 5
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_00F0F0);	// cl 6
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_000000);	// cl 7
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F09000);	// cl 8
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F00090);	// cl 9
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_900090);	// cl 10
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_9000F0);	// cl 11
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_00F090);	// cl 12
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_90F000);	// cl 13
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_009090);	// cl 14
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_909000);	// cl 15
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	
	bitmap.LoadBitmap(IDB_9090F0);	// cl 16
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_90F090);	// cl 17
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F09090);	// cl 18
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	
/*	bitmap.LoadBitmap(IDB_000000);	// NOISE
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	
	bitmap.LoadBitmap(IDB_909090);	// non cl
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_0000FF);	// cl 1
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_00FF00);	// cl 2
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_FF0000);	// cl 3
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_009090);	// cl 4
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_909000);	// cl 5
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_900090);	// cl 6
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_00F0F0);	// cl 7
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F0F000);	// cl 8
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F000F0);	// cl 9
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_0090F0);	// cl 10
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_9000F0);	// cl 11
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_00F090);	// cl 12
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_90F000);	// cl 13
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F00090);	// cl 14
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F09000);	// cl 15
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	
	bitmap.LoadBitmap(IDB_9090F0);	// cl 16
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_90F090);	// cl 17
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F09090);	// cl 18
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); */
}

/////////////////////////////////////////////////////////////////////////////
void CChooseClDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Do not call cdxCSizingDialog::OnPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////
void CChooseClDlg::OnItemchangedListPnts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (listFilled == 1)
	{
		int i = 0;
		for ( i = 0; i <= maxCluster + 1; i++ ) //+1 ~ noise
		{
			if ( m_wndListPnts.GetCheck(i) )
				toView[(i==0) ? 255 : (i-1)] |= 1;
			else
				toView[(i==0) ? 255 : (i-1)] &= 0xFE;
		}
		*flag |= 1;
	}

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
void CChooseClDlg::OnItemchangedListVerx(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (listFilled == 1)
	{	int i = 0;
		for ( i = 0; i <= maxCluster + 1; i++ )  //+1 ~ noise
		{
			if ( m_wndListVerx.GetCheck(i) )
				toView[(i==0) ? 255 : (i-1)] |= 2;
			else
				toView[(i==0) ? 255 : (i-1)] &= 0xFD;
		}	
		*flag |= 1;
	}

	*pResult = 0;
}



BOOL CChooseClDlg::OnInitDialog() 
{
	cdxCSizingDialog::OnInitDialog();
	
	RestoreWindowPosition(_T("Main\\ChooseWindow"));
	
	AddSzControl( m_wndListPnts, mdNone, mdResize); 
	AddSzControl( m_wndListVerx, mdNone, mdResize); 
	AddSzControl( m_wndOK, mdNone, mdRepos); 
	
	listFilled = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseClDlg::OnOK() 
{
	StoreWindowPosition(_T("Main\\ChooseWindow")); 
	
	if (m_parent != NULL)
	{
		UpdateData(TRUE);
		m_parent->PostMessage(WM_GOODBYE, IDOK);
	}
	else
	{
		cdxCSizingDialog::OnOK();
	}
}


BOOL CChooseClDlg::Create(CWnd *parent)
{
	m_parent = parent;
	return cdxCSizingDialog::Create(CChooseClDlg::IDD,parent);
}


void CChooseClDlg::OnTimer(UINT nIDEvent) 
{
	switch ( nIDEvent )
	{
	case IDT_INVALIDATE_VIEW:
		if ( (*flag & 2) != 0 )
		{
			listFilled = 0;
			m_imageList.DeleteImageList();
			m_wndListPnts.DeleteAllItems();
			m_wndListPnts.DeleteColumn(0);
			m_wndListVerx.DeleteAllItems();
			m_wndListVerx.DeleteColumn(0);
			
			maxCluster = m_MainClusters->m_Count-1;
			FillImageList();
			FillClList(1);	// points
			FillClList(2);	// vectors
			
			*flag &= 0xFD;
			listFilled = 1;
			Invalidate();
			break;
		}
	}
	


	cdxCSizingDialog::OnTimer(nIDEvent);
}
