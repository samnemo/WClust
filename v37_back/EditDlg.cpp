// EditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "EditDlg.h"
#include "SetDial.h"
#include "DialList.h"
#include "TimeEv.h"
#include "Hist.h"
#include ".\editdlg.h"
//#include "ChooseClDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog
#define	IDT_INVALIDATE_CHOOSE	110

CEditDlg::CEditDlg(CWnd* pParent)
	: cdxCSizingDialog(CEditDlg::IDD, pParent),
	  m_bNoiseMode(false),
	  m_pRDlg(0)
{
	//{{AFX_DATA_INIT(CEditDlg)
	//}}AFX_DATA_INIT
}

void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	cdxCSizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDlg)
	DDX_Control(pDX, IDC_CMB_ORIG_NEW, m_wndCmbOrigNew);
	DDX_Control(pDX, IDC_EXPAND, m_wndExpand);
	DDX_Control(pDX, IDC_TIME_EV, m_wndTimeEv);
	DDX_Control(pDX, IDC_BUT_UNDO, m_wndUndo);
	DDX_Control(pDX, IDC_COMBOCLUST, m_wndComboClust);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_INITVIEW, m_wndInitView);
	DDX_Control(pDX, IDC_CHECKFIT, m_wndCheckFit);
	DDX_Control(pDX, IDC_SET_SPIKES, m_wndSetSpikes);
	DDX_Control(pDX, IDC_SPIN_X, m_wndSpinX);
	DDX_Control(pDX, IDC_FIT_VIEW, m_wndFitView);
	DDX_Control(pDX, IDC_SPIN_SPIKES, m_wndSpinSpikes);
	DDX_Control(pDX, IDC_SETTINGS, m_wndSettings);
	DDX_Control(pDX, IDC_DRAW_SPIKES, m_wndDrawSpikes);
	DDX_Control(pDX, IDC_COMBOPEN, m_wndComboPen);
	DDX_Control(pDX, IDC_COPYCLUSTER, m_wndCopyCluster);
	DDX_Control(pDX, IDC_ZOOMIN, m_wndZoomIn);
	DDX_Control(pDX, IDC_SPIN_Y, m_wndSpinY);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_REMOVEPOLY, m_wndRemovePoly);
	DDX_Control(pDX, IDC_REMALLPOLY, m_wndRemAllPoly);
	DDX_Control(pDX, IDC_NEWPOLY, m_wndNewPoly);
	DDX_Control(pDX, IDC_HIDEPOLY, m_wndHidePoly);
	DDX_Control(pDX, IDC_DRAWPOLY, m_wndDrawPoly);
	DDX_Control(pDX, IDC_MAKE_NOISE_BTN, m_NoiseButton);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON_RATE, m_BtnRate);
}


BEGIN_MESSAGE_MAP(CEditDlg, cdxCSizingDialog)
	ON_MESSAGE(WM_GOODBYE, OnGoodbye)
	//{{AFX_MSG_MAP(CEditDlg)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_NEWPOLY, OnNewpoly)
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_ZOOMIN, OnZoomin)
	ON_CBN_SELCHANGE(IDC_COMBOCLUST, OnSelchangeComboclust)
	ON_BN_CLICKED(IDC_HIDEPOLY, OnHidepoly)
	ON_BN_CLICKED(IDC_REMOVEPOLY, OnRemovepoly)
	ON_BN_CLICKED(IDC_REMALLPOLY, OnRemallpoly)
	ON_BN_CLICKED(IDC_DRAWPOLY, OnDrawpoly)
	ON_BN_CLICKED(IDC_COPYCLUSTER, OnCopycluster)
	ON_CBN_SELCHANGE(IDC_COMBOPEN, OnSelchangeCombopen)
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_BN_CLICKED(IDC_FIT_VIEW, OnFitView)
	ON_BN_CLICKED(IDC_DRAW_SPIKES, OnDrawSpikes)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_SET_SPIKES, OnSetSpikes)
	ON_BN_CLICKED(IDC_INITVIEW, OnInitview)
	ON_BN_CLICKED(IDC_LIST, OnList)
	ON_BN_CLICKED(IDC_BUT_UNDO, OnButUndo)
	ON_BN_CLICKED(IDC_TIME_EV, OnTimeEv)
	ON_BN_CLICKED(IDC_EXPAND, OnExpand)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_CMB_ORIG_NEW, OnSelchangeCmbOrigNew)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MAKE_NOISE_BTN, OnBnClickedMakeNoiseBtn)
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, OnDeltaposSpinY)
	ON_BN_CLICKED(IDC_BUTTON_RATE, OnBnClickedButtonRate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDlg message handlers

BOOL CEditDlg::OnInitDialog() 
{
	cdxCSizingDialog::OnInitDialog();
	
	RestoreWindowPosition(_T("Main\\Window")); 

	AddSzControl( m_wndComboClust,mdRepos,mdRepos); 
	AddSzControl( m_wndComboPen,mdRepos,mdRepos); 
	AddSzControl( m_wndDrawPoly, mdRepos,mdRepos);
	AddSzControl( m_wndHidePoly, mdRepos,mdRepos);
	AddSzControl( m_wndNewPoly, mdRepos,mdRepos);
	AddSzControl( m_NoiseButton, mdRepos,mdRepos);
	AddSzControl( m_wndOK, mdRepos,mdRepos);
	AddSzControl( m_BtnRate, mdRepos,mdRepos);
	AddSzControl( m_wndRemAllPoly, mdRepos,mdRepos);
	AddSzControl( m_wndRemovePoly, mdRepos,mdRepos);
	AddSzControl( m_wndCmbOrigNew, mdRepos,mdRepos);
	AddSzControl( m_wndCopyCluster, mdRepos,mdRepos);
	AddSzControl( m_wndDrawSpikes, mdRepos,mdRepos);
	AddSzControl( m_wndSettings, mdRepos,mdRepos);
	AddSzControl( m_wndSetSpikes, mdRepos,mdRepos);
	AddSzControl( m_wndFitView, mdRepos,mdRepos);
	AddSzControl( m_wndZoomIn, mdRepos,mdRepos);
	AddSzControl( m_wndCheckFit, mdRepos,mdRepos);
	AddSzControl( m_wndInitView, mdRepos,mdRepos);
	AddSzControl( m_wndSpinSpikes, mdRepos,mdRepos);
	AddSzControl( m_wndList, mdRepos,mdRepos);
	AddSzControl( m_wndUndo, mdRepos,mdRepos);
	AddSzControl( m_wndTimeEv, mdRepos,mdRepos);
	AddSzControl( m_wndExpand, mdRepos,mdRepos);

	AddSzControl( m_wndSpinX, mdRelative,mdRepos);
	AddSzControl( m_wndSpinY, mdNone,mdRelative);

	m_wndCmbOrigNew.AddString(TEXT("Your new cl."));
	m_wndCmbOrigNew.AddString(TEXT("Original cl."));
	m_wndCmbOrigNew.AddString(TEXT("Kmeans cl."));
	m_wndCmbOrigNew.AddString(TEXT("Info cl."));
	m_wndCmbOrigNew.AddString(TEXT("AP cl."));
	m_wndCmbOrigNew.AddString(TEXT("KK cl."));

	HeightSpike = 60;
	m_StopSpikes = 1;
	m_wndComboClust.SetPalette(m_MainPalette);
	m_HCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_bNoiseMode = false;

	char strTitle[1024];
	sprintf(strTitle,"Edit Clusters of Tetrode%d of %s",m_MainDataStack->m_ElectrodeChannel,m_MainDataStack->m_FileName);
	SetWindowText(strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CountDrawClusts(CCluster& C,CVerxStack& D)
{
	switch(D.whichDraw)
	{
	case CLUST_USER:
		return C.GetCount()+1;		 
	case CLUST_ORIG:
		return D.m_NumOriginalCl+2;
	default:
		return C.m_pNumClusts[D.whichDraw]+2;
	}
}

int CountClusts(CCluster& C,CVerxStack& D)
{
	switch(D.whichDraw)
	{
	case CLUST_USER:
		return C.GetCount()-1;
	case CLUST_ORIG:
		return D.m_NumOriginalCl;
	default:
		return C.m_pNumClusts[D.whichDraw];
	}
}

void CEditDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call cdxCSizingDialog::OnPaint() for painting messages
	char pomStr[60];

	m_wndSpinX.SetPos(m_MyView->m_AxesX);
	m_wndSpinY.SetPos(m_MyView->m_AxesY);
	
	GetClientRect(m_wndRect);
	m_DrawingRect=m_wndRect;
	m_DrawingRect.left+=10;
	m_DrawingRect.top+=10;
	m_DrawingRect.right-=190;
	m_DrawingRect.bottom-=10;
	
	m_rect=m_DrawingRect;
	m_rect.top+=10;
	m_rect.left+=20;
	m_rect.right-=10;
	m_rect.bottom-=20;

	// Drawing frame
	dc.Rectangle(m_DrawingRect);
	

	//Drawing spikes window
	m_DrawingSpikesRect.left = m_wndRect.right - 183; //180
	m_DrawingSpikesRect.right = m_wndRect.right - 11; //10
	m_DrawingSpikesRect.top = m_wndRect.top + 10;
	m_DrawingSpikesRect.bottom = m_wndRect.bottom - 300; //300
	dc.Rectangle(m_DrawingSpikesRect);


	// Font for View Params
	CFont newFontVP;
	newFontVP.CreateFont(14,6,00,0,0,FALSE,
			0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
			DEFAULT_PITCH,
			"Times New Roman");
	CFont *pOldFontVP = dc.SelectObject(&newFontVP);
	int oldBkMode = dc.SetBkMode(TRANSPARENT);

	// Show View parameters
	sprintf(pomStr,"%d %% of  (%d .. %d)", m_PercSp,m_FirstSp,m_LastSp);
	dc.TextOut(m_DrawingSpikesRect.left+1,m_DrawingSpikesRect.bottom+25,pomStr);
	sprintf(pomStr,"max. %d",m_MainDataStack->m_NumVerx);
	dc.TextOut(m_DrawingSpikesRect.left+10,m_DrawingSpikesRect.bottom+37,pomStr);
	
	sprintf(pomStr,"%d %% of  (%d .. %d)", m_PercEdEv,m_FirstEdEv,m_LastEdEv);
	dc.TextOut(m_DrawingSpikesRect.left+1,m_DrawingSpikesRect.bottom+67,pomStr);
	sprintf(pomStr,"max. %d",m_MainDataStack->m_NumVerx);
	dc.TextOut(m_DrawingSpikesRect.left+10,m_DrawingSpikesRect.bottom+79,pomStr);


	// Drawing lines around params
	dc.MoveTo(m_DrawingSpikesRect.left,m_DrawingSpikesRect.bottom+26);
	dc.LineTo(m_DrawingSpikesRect.left,m_DrawingSpikesRect.bottom+106);
	dc.LineTo(m_DrawingSpikesRect.right,m_DrawingSpikesRect.bottom+106);
	dc.LineTo(m_DrawingSpikesRect.right,m_DrawingSpikesRect.bottom+26);
	dc.LineTo(m_DrawingSpikesRect.left,m_DrawingSpikesRect.bottom+26);
	dc.MoveTo(m_DrawingSpikesRect.left,m_DrawingSpikesRect.bottom+65);
	dc.LineTo(m_DrawingSpikesRect.right,m_DrawingSpikesRect.bottom+65);
	
	// Restoring font View Params
	dc.SelectObject(pOldFontVP);
	dc.SetBkMode(oldBkMode);
	// Change background color to white
	int oldBkColor = dc.SetBkColor(RGB(255,255,255));
	
	// Font for Axes Y
	CFont newFontY;
	newFontY.CreateFont(18,6,900,0,0,FALSE,
			0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
			DEFAULT_PITCH,
			"Times New Roman");
	CFont *pOldFontY = dc.SelectObject(&newFontY);
	
	// Axes Y
	dc.MoveTo(m_rect.left-1,m_rect.top);
	dc.LineTo(m_rect.left-1,m_rect.bottom+4);
	CString *nameY;
	nameY = (CString*) *(m_MainDataStack->m_AxesStack.begin()+m_MyView->m_AxesY-1);
	dc.TextOut(m_DrawingRect.left+1,m_rect.bottom+(m_rect.top-m_rect.bottom)/2-51,*nameY);
	
	// Min Y
	sprintf(pomStr,"%2.2f",m_MyView->m_DataArea.bottom+m_MyView->m_DataArea.GetSizeY()/12);
	dc.TextOut(m_DrawingRect.left+1,m_rect.bottom-m_rect.Height()/12+17,pomStr);
	//dc.TextOut(m_DrawingRect.left+1,m_rect.bottom-m_rect.Height()/12+12,pomStr);
	dc.MoveTo(m_rect.left-1,m_rect.bottom-m_rect.Height()/12);
	dc.LineTo(m_rect.left-5,m_rect.bottom-m_rect.Height()/12);
	
	// Max Y
	sprintf(pomStr,"%2.2f",m_MyView->m_DataArea.top-m_MyView->m_DataArea.GetSizeY()/12);
	dc.TextOut(m_DrawingRect.left+1,m_rect.bottom-m_rect.Height()*11/12+17,pomStr);
	//dc.TextOut(m_DrawingRect.left+1,m_rect.bottom-m_rect.Height()*11/12+12,pomStr);
	dc.MoveTo(m_rect.left-1,m_rect.top+m_rect.Height()/12);
	dc.LineTo(m_rect.left-5,m_rect.top+m_rect.Height()/12);

	//Restoring font Axes Y
	dc.SelectObject(pOldFontY);

	// Font for Axes X
	CFont newFontX;
	newFontX.CreateFont(18,6,00,0,0,FALSE,
			0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
			DEFAULT_PITCH,
			"Times New Roman");
	CFont *pOldFontX = dc.SelectObject(&newFontX);
	
	// Axes X
	dc.MoveTo(m_rect.left-4,m_rect.bottom+1);
	dc.LineTo(m_rect.right+1,m_rect.bottom+1);
	CString *nameX;
	nameX = (CString*) *(m_MainDataStack->m_AxesStack.begin()+m_MyView->m_AxesX-1);
	dc.TextOut(m_rect.left+(m_rect.right-m_rect.left)/2+15,m_rect.bottom+2,*nameX);

	// Min X
	sprintf(pomStr,"%2.2f",m_MyView->m_DataArea.left+m_MyView->m_DataArea.GetSizeX()/12);
	dc.TextOut(m_rect.left+m_rect.Width()/12-17,m_rect.bottom+2,pomStr);
	//dc.TextOut(m_rect.left+m_rect.Width()/12-12,m_rect.bottom+2,pomStr);
	dc.MoveTo(m_rect.left+m_rect.Width()/12,m_rect.bottom+1);
	dc.LineTo(m_rect.left+m_rect.Width()/12,m_rect.bottom+5);
	
	// Max X
	sprintf(pomStr,"%2.2f",m_MyView->m_DataArea.right-m_MyView->m_DataArea.GetSizeX()/12);
	//dc.TextOut(m_rect.left+m_rect.Width()*11/12-17,m_rect.bottom+2,pomStr);
	dc.TextOut(m_rect.left+m_rect.Width()*11/12-12,m_rect.bottom+2,pomStr);
	dc.MoveTo(m_rect.right-m_rect.Width()/12,m_rect.bottom+1);
	dc.LineTo(m_rect.right-m_rect.Width()/12,m_rect.bottom+5);

	// Restoring font Axes X
	dc.SelectObject(pOldFontX);
	
	// Axes X = 0 & Y = 0
	CPen axisZeroPen(PS_SOLID,1, RGB(200,200,200));
	CPen *pOldAxisZeroPen = dc.SelectObject(&axisZeroPen);

	if ( m_MyView->m_DataArea.left * m_MyView->m_DataArea.right < 0 )
	{
		dc.MoveTo(m_rect.left - m_MyView->m_DataArea.left * m_rect.Width() / m_MyView->m_DataArea.GetSizeX(), m_rect.bottom-1);
		dc.LineTo(m_rect.left - m_MyView->m_DataArea.left * m_rect.Width() / m_MyView->m_DataArea.GetSizeX(), m_rect.top);
	}
	if ( m_MyView->m_DataArea.top * m_MyView->m_DataArea.bottom < 0 )
	{
		dc.MoveTo(m_rect.left + 1, m_rect.bottom + m_MyView->m_DataArea.bottom * m_rect.Height() / m_MyView->m_DataArea.GetSizeY());
		dc.LineTo(m_rect.right - 1, m_rect.bottom + m_MyView->m_DataArea.bottom * m_rect.Height() / m_MyView->m_DataArea.GetSizeY());
	}
	dc.SelectObject(pOldAxisZeroPen);
	
	// Main drawing of vectors & clusters
	switch(m_MainDataStack->whichDraw)
	{
	case CLUST_USER:
		m_MainDataStack->Draw(&dc,m_rect,&m_MyView->m_DataArea,CPoint(m_MyView->m_AxesX,m_MyView->m_AxesY),m_MyView->m_ClustToView,m_MyView->m_PointsSize);
		m_MainClusters->Draw(&dc,m_rect,&m_MyView->m_DataArea,CPoint(m_MyView->m_AxesX,m_MyView->m_AxesY),m_MyView->m_ClustToView,1);
		break;
	case CLUST_ORIG:
		m_MainDataStack->DrawOrig(&dc,m_rect,&m_MyView->m_DataArea,CPoint(m_MyView->m_AxesX,m_MyView->m_AxesY),m_MyView->m_ClustToView,m_MyView->m_PointsSize);
		break;
	case CLUST_KM:
	case CLUST_INFO:
	case CLUST_AP:
	case CLUST_KK:
	default:
		m_MainDataStack->DrawAutoC(&dc,m_rect,&m_MyView->m_DataArea,CPoint(m_MyView->m_AxesX,m_MyView->m_AxesY),m_MyView->m_ClustToView,m_MyView->m_PointsSize,m_MainDataStack->whichDraw);
		break;
	}

	dc.SetBkColor(oldBkColor);

	// Preparing values for drawing spikes
	m_StopSpikes = m_DrawingSpikesRect.Height()/HeightSpike;
	int iDrawClusts = CountDrawClusts(*m_MainClusters,*m_MainDataStack);
	int hiRng = iDrawClusts-m_StopSpikes+1;
	if (hiRng <= 0)
		hiRng=1;
	if (m_StartSpikes>hiRng)
		m_StartSpikes=hiRng;
	m_wndSpinSpikes.SetRange(hiRng-1,0);
	m_wndSpinSpikes.Invalidate();

	// Draw arrow up
	CPen newPenB(PS_DASH,2,RGB(0,0,0));
	CPen *pOldPenB=dc.SelectObject(&newPenB);
	if (m_StartSpikes>0)
	{
		dc.MoveTo(m_DrawingSpikesRect.right-7,m_DrawingSpikesRect.top+12);
		dc.LineTo(m_DrawingSpikesRect.right-7,m_DrawingSpikesRect.top+2);
		dc.LineTo(m_DrawingSpikesRect.right-12,m_DrawingSpikesRect.top+7);
		dc.MoveTo(m_DrawingSpikesRect.right-7,m_DrawingSpikesRect.top+2);
		dc.LineTo(m_DrawingSpikesRect.right-2,m_DrawingSpikesRect.top+7);
	}
	// Draw arrow down
	if (m_StartSpikes+m_StopSpikes < CountDrawClusts(*m_MainClusters,*m_MainDataStack))
	{
		dc.MoveTo(m_DrawingSpikesRect.right-7,m_DrawingSpikesRect.bottom-12);
		dc.LineTo(m_DrawingSpikesRect.right-7,m_DrawingSpikesRect.bottom-2);
		dc.LineTo(m_DrawingSpikesRect.right-12,m_DrawingSpikesRect.bottom-7);
		dc.MoveTo(m_DrawingSpikesRect.right-7,m_DrawingSpikesRect.bottom-2);
		dc.LineTo(m_DrawingSpikesRect.right-2,m_DrawingSpikesRect.bottom-7);
	}
	dc.SelectObject(pOldPenB);

	// Drawing spikes
	if (m_DrawSpikes)
	{
		m_MainClusters->CalcClusterInfo(*m_MainDataStack,false);

		CFont newFontSpikes;
		newFontSpikes.CreateFont(14,6,0,0,0,FALSE,
				0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
				DEFAULT_PITCH,
				"Times New Roman");

		m_DrawSpikes=0;
		CRect MiniWin;
		int widthText = 10;

		int iLim = m_StartSpikes+m_StopSpikes;

		for (int i=m_StartSpikes;i<iLim;i++)
		{
			MiniWin.top = m_DrawingSpikesRect.top +	(i-m_StartSpikes)*HeightSpike;
			MiniWin.bottom = m_DrawingSpikesRect.top +	(i-m_StartSpikes+1)*HeightSpike;

			for (int j=0; j<4; j++)
			{
				MiniWin.left = m_DrawingSpikesRect.left + j * (m_DrawingSpikesRect.Width())/4+1;
				MiniWin.right = m_DrawingSpikesRect.left + (j+1) * (m_DrawingSpikesRect.Width())/4-1;
				m_MainDataStack->DrawSpikes( &dc , MiniWin , j , i - 1 );
			}

			//draw cluster number
			if(i-1<=CountClusts(*m_MainClusters,*m_MainDataStack))
			{
				char myText[256];
				sprintf( myText, "%d", i-1);
				if(i-1<0) 
					dc.SetTextColor(RGB(204,204,204));
				else
					dc.SetTextColor(m_MainPalette->GetSColor(i-1));
				
				dc.TextOut(m_DrawingSpikesRect.left+1,MiniWin.top+1,myText);

				if(i-1>0 && i-1<m_MainClusters->m_vInfo[m_MainDataStack->whichDraw].size())
				{
					CFont *pOldFontVP = dc.SelectObject(&newFontSpikes);
					sprintf(myText , "IN=%.2f UDN=%.2f", 
						m_MainClusters->m_vInfo[m_MainDataStack->whichDraw][i-1][1],
						m_MainClusters->m_vInfo[m_MainDataStack->whichDraw][i-1][3]);

					dc.TextOut(m_DrawingSpikesRect.left+1,MiniWin.bottom-15,myText);
					dc.SelectObject(pOldFontVP);
				}				
			}
		}
	}
}


void CEditDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_SPIN_X:
		if (m_MyView->m_AxesX != nPos)
		{
			if (nPos == m_MyView->m_AxesY)
			{
				if (nPos > m_MyView->m_AxesX)
				{ //up
					if (nPos+1 <= m_MainDataStack->Dimension)
						nPos++;
					else nPos--;
					m_MyView->m_AxesX = nPos;
				}
				if (nPos < m_MyView->m_AxesX)
				{ //down
					if (nPos-1 > 0)
						nPos--;
					else nPos++;
					m_MyView->m_AxesX = nPos;
				}
			}
			else m_MyView->m_AxesX = nPos;
			m_wndSpinX.SetPos(nPos);
			SetActiveButton();
			if (m_wndCheckFit.GetCheck())
				FittingView();
			InvalidateRect(m_DrawingRect,FALSE);
		}
		break;
	}
	cdxCSizingDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEditDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_SPIN_Y:
		if (m_MyView->m_AxesY != nPos)
		{
			if (nPos == m_MyView->m_AxesX)
			{
				if (nPos > m_MyView->m_AxesY)
				{ //up
					if (nPos+1 <= m_MainDataStack->Dimension)
						nPos++;
					else nPos--;
					m_MyView->m_AxesY = nPos;
				}
				if (nPos < m_MyView->m_AxesY)
				{ //down
					if (nPos-1 > 0)
						nPos--;
					else nPos++;
					m_MyView->m_AxesY = nPos;
				}
			}
			else m_MyView->m_AxesY = nPos;
			m_wndSpinY.SetPos(nPos);
			SetActiveButton();
			if (m_wndCheckFit.GetCheck())
				FittingView();
			InvalidateRect(m_DrawingRect,FALSE);
		}

		
		break;
	case IDC_SPIN_SPIKES:
		if (m_StartSpikes != nPos)
		{
			m_StartSpikes = nPos;
			m_DrawSpikes = 1;
			InvalidateRect(m_DrawingSpikesRect,FALSE);
		}
		break;
}
	cdxCSizingDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEditDlg::CheckComboClust()
{
	m_wndComboClust.ResetContent();

	// Filling combo of clusters
	MY_STACK::iterator index=(m_MainClusters->m_BndrStack.begin());	

	char pom[20];
	
	m_wndComboClust.AddString("noise");
	m_wndComboClust.AddString("non clustered");
	
	int ii=0,iw=0;
	switch(m_MainDataStack->whichDraw)
	{
	case CLUST_KM:
	case CLUST_INFO:
	case CLUST_AP:
	case CLUST_KK:
		iw=m_MainClusters->m_pNumClusts[m_MainDataStack->whichDraw];
		for(ii=1;ii<=iw;ii++)
		{
			sprintf(pom,"cluster %d",ii);
			m_wndComboClust.AddString(pom);
		}
		break;
	case CLUST_ORIG:
		iw=m_MainDataStack->m_NumOriginalCl;
		for(ii=1;ii<=iw;ii++)
		{
			sprintf(pom,"cluster %d",ii);
			m_wndComboClust.AddString(pom);
		}
		break;
	case CLUST_USER:
	default:
		while ( index!=m_MainClusters->m_BndrStack.end() )
		{
			CBoundary* Bndr=(CBoundary*) *index;
			if (Bndr->GetNumb()!=0)
			{
				sprintf(pom,"cluster %d",Bndr->GetNumb());
				m_wndComboClust.AddString(pom);
			}
			index++;
		}
		break;
	}
	m_wndComboClust.SetCurSel(1);
}


void CEditDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	cdxCSizingDialog::OnShowWindow(bShow, nStatus);

	m_State = 0;
	m_Dragging = 0;
	m_DrawSpikes = 0;
	m_StartSpikes = 0;
	m_colDlgState = 0;
	
	m_wndSpinX.SetRange(1,m_MainDataStack->Dimension);
	m_wndSpinY.SetRange(1,m_MainDataStack->Dimension);

	float x1,y1,x2,y2;
	x1=m_MyView->m_DataArea.GetLeft()-m_MyView->m_DataArea.GetSizeX()*0.1;
	x2=m_MyView->m_DataArea.GetRight()+m_MyView->m_DataArea.GetSizeX()*0.1;
	y1=m_MyView->m_DataArea.GetTop()+m_MyView->m_DataArea.GetSizeY()*0.1;
	y2=m_MyView->m_DataArea.GetBottom()-m_MyView->m_DataArea.GetSizeY()*0.1;
	m_MyView->m_DataArea.SetValue(x1,y1,x2,y2);

	// Filling combo of clusters
	CheckComboClust();

	m_wndComboPen.SetCurSel(m_MyView->m_PointsSize-1);

	m_MainDataStack->CalcViewSpike(m_FirstSp,m_LastSp,m_PercSp);
	m_MainDataStack->CalcViewVx(m_FirstEdEv,m_LastEdEv,m_PercEdEv);
	
	m_wndCmbOrigNew.SetCurSel( m_MainDataStack->whichDraw );

	SetActiveButton();

	HICON pom1 = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CWnd::SetIcon(pom1,true);
}


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void CEditDlg::OnNewpoly() 
{
	if(UserCheating()) return;

	m_MainDataStack->whichDraw = CLUST_USER;

	if (m_State==0)
	{
		m_State=1;
		m_First=1;
		m_Dragging=0;
		m_TwoLines=0;
		m_IsVisible=0;
	
		m_NewPoly = new CBoundary(m_MainPalette);
		m_NewProj = new CProjection(m_MainPalette);
	
		m_NewPoly->m_Numb = m_MainClusters->m_Count;
		m_NewPoly->m_bNoise = m_bNoiseMode;
		
		if (m_MyView->m_AxesX<m_MyView->m_AxesY)
		{	// pravotociva
			m_StoringMode=0; 
			m_NewProj->SetAxes(m_MyView->m_AxesX,m_MyView->m_AxesY);
		}
		else
		{	// levotociva
			m_StoringMode=1; 
			m_NewProj->SetAxes(m_MyView->m_AxesY,m_MyView->m_AxesX);
		}
	
		m_NewPoly->AddProj(m_NewProj);
		m_MainClusters->AddBndry(m_NewPoly);
		SetActiveButton();
	}
}



void CEditDlg::OnDrawpoly() 
{
	m_MainDataStack->whichDraw = CLUST_USER;

	MY_STACK::iterator Index,IndProj;
		
	if (m_State==0)
	{
		m_State=2;  // Drawing polygon of existing cluster
		m_First=1;
		m_Dragging=0;
		m_TwoLines=0;
		m_IsVisible=0;
		
		int Selected=m_wndComboClust.GetCurSel();
		Selected = (Selected == 0) ? 255 : (Selected - 1);		

		if (Selected > 0 && Selected < 255)
		{
			Index = (m_MainClusters->m_BndrStack.begin()+Selected);
			m_NewPoly = (CBoundary*) *Index;
			m_NewProj = new CProjection(m_MainPalette);

			if (m_MyView->m_AxesX<m_MyView->m_AxesY)
			{	// pravotociva
				m_StoringMode=0; 
				m_NewProj->SetAxes(m_MyView->m_AxesX,m_MyView->m_AxesY);
			}
			else
			{	// levotociva
				m_StoringMode=1; 
				m_NewProj->SetAxes(m_MyView->m_AxesY,m_MyView->m_AxesX);
			}
//			m_NewPoly->AddProj(m_NewProj);
			SetActiveButton(); InvalidateRect(m_DrawingRect,FALSE);
		}
	}
}


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

void CEditDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CClientDC dc(this); // device context for painting

	CPoint m_point,m_ClientPoint;
	// Set cross cursor in drawing area
	m_point.x=point.x;
	m_point.y=point.y;
	m_InDrawArea=0;
	if (m_point.x>=m_rect.left && m_point.x<=m_rect.right && m_point.y<=m_rect.bottom && m_point.y>=m_rect.top && m_State!=0)
	{
		::SetCursor(m_HCross);
		m_InDrawArea=1;
	}
	
	m_point.x=point.x;
	m_point.y=m_wndRect.bottom-point.y;

	// Count Client 
	m_ClientPoint=point;
	m_ClientPoint.x=point.x-m_rect.left;
	m_ClientPoint.y=m_rect.bottom-point.y;
	
	// Selecting view area (ZOOM)
	if (m_State==10) 
	{ 
		if (m_InDrawArea)
		{
			CRect iRect; 
			
			CWnd::GetWindowRect(iRect);
			iRect.left += m_rect.left + 4;
			iRect.right = iRect.left + m_rect.Width(); 
			iRect.bottom -= 33;
			iRect.top = iRect.bottom-m_rect.Height();
			::ClipCursor(iRect);
			SetCapture();
			
			m_Dragging=1;
			m_OldPoint=point;
			m_Origin=point;
			m_OrigClient=m_ClientPoint;
		}
	}
	
	// Draw after creating new polygon or existing poly in another project.
	if (m_State==1||m_State==2)  
	{
		// Drawing
		if (m_First)
		{	
			if (m_InDrawArea)	
			{
				//Mouse Cursor
				CRect iRect,jRect;
				CWnd::GetWindowRect(iRect);
				if (m_State==1 || (m_State==2 /*&& !m_IsCoorX && !m_IsCoorY*/)) //Create new
				{
					CRect iRect; 
			
					CWnd::GetWindowRect(iRect);
					iRect.left += m_rect.left + 4;
					iRect.right = iRect.left + m_rect.Width(); 
					iRect.bottom -= 33;
					iRect.top = iRect.bottom-m_rect.Height();
					::ClipCursor(iRect);
					SetCapture();

					m_Dragging=1;
					m_OldPoint=point;
					m_Origin=point;
					m_OrigClient=m_ClientPoint;
					m_TempPoint=point;
				}
			}
		}
		else 
		{	
			m_Dragging=1;
			if (m_TwoLines!=1)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_Origin);
				dc.LineTo(m_OldPoint);
				dc.MoveTo(m_Origin);
				dc.LineTo(point);
				m_OldPoint=point;
			}
			else
			{
				if (m_IsVisible)
				{
					dc.SetROP2(R2_NOT);
					dc.MoveTo(m_TempPoint);
					dc.LineTo(m_OldPoint);
					dc.LineTo(m_Origin);
					m_IsVisible=0;
				}
				CVect2D *m_v1,*m_v2,*m_v3;
				m_v1=(CVect2D*)*m_NewProj->m_Vect2DStack.begin();
				m_v2=(CVect2D*)*(m_NewProj->m_Vect2DStack.end()-1);
					
				float x,y;
				x=m_MyView->m_DataArea.GetLeft()+m_ClientPoint.x*m_MyView->m_DataArea.GetSizeX()/m_rect.Width();
				y=m_MyView->m_DataArea.GetBottom()+m_ClientPoint.y*m_MyView->m_DataArea.GetSizeY()/m_rect.Height();
				if (m_StoringMode)
					{ float z; z=x; x=y;y=z;}
			
				CPoint2D *m_p1,*m_p2;
				m_v3= new CVect2D();
				m_p1= (CPoint2D*)*m_NewProj->m_Pnt2DStack.begin();
				m_p2= (CPoint2D*)*(m_NewProj->m_Pnt2DStack.end()-1);
				m_v3->Make(m_p1,m_p2);

				delete m_v3;
				if (m_v1->IsIn(x,y)&&m_v2->IsIn(x,y)&&m_v3->IsIn(x,y))
				{
					m_IsVisible=1;
					m_OldPoint=point;
					dc.SetROP2(R2_NOT);
					dc.MoveTo(m_TempPoint);
					dc.LineTo(m_OldPoint);
					dc.LineTo(m_Origin);
				}
			}
		}
		
		// Storing first point of polygon
		if (m_NewProj->m_Count==0)
		{
			if (m_State==1 && m_InDrawArea || m_State==2 && m_InDrawArea /* && flag*/)
			{
				float x,y;
				x=m_MyView->m_DataArea.GetLeft()+m_ClientPoint.x*m_MyView->m_DataArea.GetSizeX()/m_rect.Width();
				y=m_MyView->m_DataArea.GetBottom()+m_ClientPoint.y*m_MyView->m_DataArea.GetSizeY()/m_rect.Height();

				if (m_StoringMode)
				{ 
					float z;
					z=x; x=y;y=z;
				}
				m_NewPoint2D = new CPoint2D(x,y);
				m_NewProj->AddPnt2D(m_NewPoint2D);	
			}
		};
	} // m_State==1 && 2

	cdxCSizingDialog::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

void CEditDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	CPoint m_point,m_ClientPoint;
	m_point.x=point.x;
	m_point.y=point.y;
	m_InDrawArea=0;

	m_ClientPoint=point;
	m_ClientPoint.x=point.x-m_rect.left;
	m_ClientPoint.y=m_rect.bottom-point.y;

	if (m_point.x>=m_rect.left && m_point.x<=m_rect.right && m_point.y<=m_rect.bottom && m_point.y>=m_rect.top && m_State!=0)
	{
		::SetCursor(m_HCross);
		m_InDrawArea=1;
	}
	
	// Selecting view area (ZOOM)
	if (m_State==10&&m_Dragging)
	{
		dc.SetROP2(R2_NOT);
		dc.MoveTo(m_Origin);
		dc.LineTo(m_Origin.x,m_OldPoint.y);
		dc.LineTo(m_OldPoint);
		dc.LineTo(m_OldPoint.x,m_Origin.y);
		dc.LineTo(m_Origin);
		
		m_OldPoint=point;
		dc.MoveTo(m_Origin);
		dc.LineTo(m_Origin.x,m_OldPoint.y);
		dc.LineTo(m_OldPoint);
		dc.LineTo(m_OldPoint.x,m_Origin.y);
		dc.LineTo(m_Origin);
				
		m_OldPoint=point;
	}

	// Draw after creating new polygon
	if (m_State==1 || m_State==2)
	{
		if (m_First&&m_Dragging)
		{
			m_First=0;
			dc.SetROP2(R2_NOT);
			dc.MoveTo(m_Origin);
			dc.LineTo(m_OldPoint);
			dc.MoveTo(m_Origin);
			dc.LineTo(point);
			m_OldPoint=point;
		}
		else
		{
			if (m_Dragging==1)
			{
				if (m_TwoLines!=1)
				{
					dc.SetROP2(R2_NOT);
					dc.MoveTo(m_Origin);
					dc.LineTo(m_OldPoint);
					dc.MoveTo(m_Origin);
					dc.LineTo(point);
					m_OldPoint=point;
				}
				else
				{
					if (m_IsVisible)
					{
						dc.SetROP2(R2_NOT);
						dc.MoveTo(m_TempPoint);
						dc.LineTo(m_OldPoint);
						dc.LineTo(m_Origin);
						m_IsVisible=0;
					}
					CVect2D *m_v1,*m_v2,*m_v3;
					m_v1=(CVect2D*)*m_NewProj->m_Vect2DStack.begin();
					m_v2=(CVect2D*)*(m_NewProj->m_Vect2DStack.end()-1);
					
					float x,y;
					x=m_MyView->m_DataArea.GetLeft()+m_ClientPoint.x*m_MyView->m_DataArea.GetSizeX()/m_rect.Width();
					y=m_MyView->m_DataArea.GetBottom()+m_ClientPoint.y*m_MyView->m_DataArea.GetSizeY()/m_rect.Height();
					if (m_StoringMode)
						{ float z; z=x; x=y;y=z;}
			
					CPoint2D *m_p1,*m_p2;
					m_v3= new CVect2D();
					m_p1= (CPoint2D*)*m_NewProj->m_Pnt2DStack.begin();
					m_p2= (CPoint2D*)*(m_NewProj->m_Pnt2DStack.end()-1);
					m_v3->Make(m_p1,m_p2);

	
					delete m_v3;
					if (m_v1->IsIn(x,y)&&m_v2->IsIn(x,y)&&m_v3->IsIn(x,y))
					{
						m_IsVisible=1;
						m_OldPoint=point;
						dc.SetROP2(R2_NOT);
						dc.MoveTo(m_TempPoint);
						dc.LineTo(m_OldPoint);
						dc.LineTo(m_Origin);
										
					}
				}
			}
		}
	}
	cdxCSizingDialog::OnMouseMove(nFlags, point);
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void CEditDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	CPoint m_point,m_ClientPoint;
	// Set cross cursor
	m_point.x=point.x;
	m_point.y=point.y;
	m_InDrawArea=0;
	if (m_point.x>=m_rect.left && m_point.x<=m_rect.right && m_point.y<=m_rect.bottom && m_point.y>=m_rect.top && m_State!=0)
	{
		::SetCursor(m_HCross);
		m_InDrawArea=1;
	}
	// Count Client 
	m_ClientPoint=point;
	m_ClientPoint.x=point.x-m_rect.left;
	m_ClientPoint.y=m_rect.bottom-point.y;
	
	// Selecting view area (ZOOM)
	if (m_State==10&&m_Dragging)
	{
		dc.SetROP2(R2_NOT);
		dc.MoveTo(m_Origin);
		dc.LineTo(m_Origin.x,m_OldPoint.y);
		dc.LineTo(m_OldPoint);
		dc.LineTo(m_OldPoint.x,m_Origin.y);
		dc.LineTo(m_Origin);

		m_Dragging=0;
		::ClipCursor(NULL);
		ReleaseCapture();
		m_State=0;
		if (point.x!=m_Origin.x&&point.y!=m_Origin.y)
		{
			float x1,y1,x2,y2;
			CFloatRect pomFRect;
			int pom;
			if (m_ClientPoint.x>m_OrigClient.x)
				{ pom=m_ClientPoint.x; m_ClientPoint.x=m_OrigClient.x; m_OrigClient.x=pom; }
			if (m_ClientPoint.y>m_OrigClient.y)
				{ pom=m_ClientPoint.y; m_ClientPoint.y=m_OrigClient.y; m_OrigClient.y=pom; }
			pomFRect.right=m_MyView->m_DataArea.GetLeft()+m_OrigClient.x*m_MyView->m_DataArea.GetSizeX()/m_rect.Width();
			pomFRect.left=m_MyView->m_DataArea.GetLeft()+m_ClientPoint.x*m_MyView->m_DataArea.GetSizeX()/m_rect.Width();
			pomFRect.top=m_MyView->m_DataArea.GetBottom()+m_OrigClient.y*m_MyView->m_DataArea.GetSizeY()/m_rect.Height();
			pomFRect.bottom=m_MyView->m_DataArea.GetBottom()+m_ClientPoint.y*m_MyView->m_DataArea.GetSizeY()/m_rect.Height();

			x1=pomFRect.GetLeft()-pomFRect.GetSizeX()*0.1;
			x2=pomFRect.GetRight()+pomFRect.GetSizeX()*0.1;
			y1=pomFRect.GetTop()+pomFRect.GetSizeY()*0.1;
			y2=pomFRect.GetBottom()-pomFRect.GetSizeY()*0.1;
		
			m_MyView->m_DataArea.SetValue(x1,y1,x2,y2);
			InvalidateRect(m_DrawingRect,FALSE);
		}
		else MessageBeep(0);

	}

	// Drawing polygon
	if (m_State==1 || m_State==2)
	{
		// Drawing
		if (m_Dragging)
		{
			if (m_IsVisible)
			{
				//m_First=0;
				if (point!=m_Origin)
				{
					dc.SetROP2(R2_NOT);
					dc.MoveTo(m_Origin);
					dc.LineTo(m_OldPoint);
					dc.SetROP2(R2_COPYPEN);
					dc.MoveTo(m_Origin);
					dc.LineTo(point);
	
					m_Origin=point;
					m_OldPoint=point;
					m_Dragging=0;
				}
			}
		}


		if (m_NewProj->m_Count>2)
		{
			if (m_IsVisible)//&&(point!=m_TempPoint)&&(point!=m_Origin))
			{
				float x,y;
				x=m_MyView->m_DataArea.GetLeft()+m_ClientPoint.x*m_MyView->m_DataArea.GetSizeX()/m_rect.Width();
				y=m_MyView->m_DataArea.GetBottom()+m_ClientPoint.y*m_MyView->m_DataArea.GetSizeY()/m_rect.Height();
				if (m_StoringMode)
					{ float z; z=x; x=y;y=z;}
			
				m_NewPoint2D = new CPoint2D(x,y);
				CPoint2D *pnt;
				CVect2D *vect;

				if (m_StoringBack)
				{
					pnt=(CPoint2D*)*(m_NewProj->m_Pnt2DStack.end()-1);
					m_NewProj->AddPnt2D(m_NewPoint2D);
					vect = new CVect2D();
					vect->Make(pnt,m_NewPoint2D);
					m_NewProj->AddVect2D(vect);
				}
				else 
				{
					pnt=(CPoint2D*)*(m_NewProj->m_Pnt2DStack.begin());
					m_NewProj->AddPnt2Dinv(m_NewPoint2D);
					vect = new CVect2D();
					vect->Make(m_NewPoint2D,pnt);
					m_NewProj->AddVect2Dinv(vect);

				}
			}else MessageBeep(0);
		}

		if (m_NewProj->m_Count==2)
		{	
			if ( (point != m_TempPoint) && (point!=m_Origin) )
			{
				float x,y,x1,y1,x2,y2;
				x=m_MyView->m_DataArea.GetLeft()+m_ClientPoint.x*m_MyView->m_DataArea.GetSizeX()/m_rect.Width();
				y=m_MyView->m_DataArea.GetBottom()+m_ClientPoint.y*m_MyView->m_DataArea.GetSizeY()/m_rect.Height();
				if (m_StoringMode)
					{ float z; z=x; x=y;y=z;}
			
				CPoint2D *m_pnt1,*m_pnt2;
				CVect2D *m_vect1,*m_vect2;
				m_vect1 = new CVect2D();
				m_vect2 = new CVect2D();
			
				m_pnt1 = (CPoint2D*) *m_NewProj->m_Pnt2DStack.begin();
				x1 = m_pnt1->m_X;
				y1 = m_pnt1->m_Y;
				m_pnt2 = (CPoint2D*) *(m_NewProj->m_Pnt2DStack.begin()+1);
				x2 = m_pnt2->m_X;
				y2 = m_pnt2->m_Y;
				m_vect1->Make(x1,y1,x2,y2);
			
				if (m_vect1->IsIn(x,y))
				{ 
					m_StoringBack=1;
					m_NewPoint2D = new CPoint2D(x,y);
					m_NewProj->AddPnt2D(m_NewPoint2D);
					
					m_NewProj->AddVect2D(m_vect1);
					m_vect2->Make(x2,y2,x,y);
					m_NewProj->AddVect2D(m_vect2);
				}
				else
				{
					m_StoringBack=0;
					m_NewProj->m_Pnt2DStack.clear();
					m_NewProj->m_Count=0;
					m_NewProj->AddPnt2D(m_pnt2);
					m_NewProj->AddPnt2D(m_pnt1);
	
					m_NewPoint2D = new CPoint2D(x,y);
					m_NewProj->AddPnt2Dinv(m_NewPoint2D);	
	
					m_vect1->Make(x,y,x2,y2);
					m_NewProj->AddVect2D(m_vect1);
					m_vect2->Make(x2,y2,x1,y1);
					m_NewProj->AddVect2D(m_vect2);
				}

				m_IsVisible=0;
				m_TwoLines=1;
				m_Origin=point;
				m_Dragging=0;
			}
			else MessageBeep(0);
		};

		if ( (m_NewProj->m_Count == 1) )
		{
//			m_First=0;
			if (point!=m_Origin && ((abs(point.x - m_Origin.x) > 2) || (abs(point.y - m_Origin.y) > 2)))
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_Origin);
				dc.LineTo(m_OldPoint);
				dc.SetROP2(R2_COPYPEN);
				dc.MoveTo(m_Origin);
				dc.LineTo(point);
	
				m_Origin=point;
				m_OldPoint=point;
				m_Dragging=0;

				float x,y;
				x=m_MyView->m_DataArea.GetLeft()+m_ClientPoint.x*m_MyView->m_DataArea.GetSizeX()/m_rect.Width();
				y=m_MyView->m_DataArea.GetBottom()+m_ClientPoint.y*m_MyView->m_DataArea.GetSizeY()/m_rect.Height();
				if (m_StoringMode)
					{ float z; z=x; x=y;y=z;}
				m_NewPoint2D = new CPoint2D(x,y);
				m_NewProj->AddPnt2D(m_NewPoint2D);	
			}else MessageBeep(0);
		};
	}
//	char pom[20];
//	sprintf(pom,"%d %d",m_ClientPoint.x,m_ClientPoint.y);
//	CWnd::MessageBox(pom,NULL,MB_OK);
	

	cdxCSizingDialog::OnLButtonUp(nFlags, point);
}

void CEditDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_State==10)
	{
		m_State = 0;
	}

	if (m_State==1||m_State==2)
	{
		if (!m_Dragging)
		{
			if (m_NewProj->m_Count>2)
			{
				m_Dragging=0;
				::ClipCursor(NULL);
				::ReleaseCapture();

				CPoint2D *m_p1,*m_p2;
				CVect2D *m_v3;
				m_v3= new CVect2D();
				m_p1= (CPoint2D*)*m_NewProj->m_Pnt2DStack.begin();
				m_p2= (CPoint2D*)*(m_NewProj->m_Pnt2DStack.end()-1);
				
				m_MainClusters->WantStore();	
				
				if (m_StoringBack) { m_v3->Make(m_p2,m_p1); m_NewProj->AddVect2D(m_v3); }
				else { m_v3->Make(m_p2,m_p1); m_NewProj->AddVect2D(m_v3); }
				
				if (m_State==1)
				{
					CCmdTarget::BeginWaitCursor();

					m_MainClusters->MakeUNDO(m_NewPoly->GetNumb(),3);
					m_MainClusters->Clustering(m_MainDataStack,m_bNoiseMode);

					EndWaitCursor();				

					m_MainDataStack->whichDraw = 0;
					CheckComboClust();
					m_wndComboClust.SetCurSel(m_NewPoly->GetNumb()+1);
					flagChoose |= 2;
				}
				else if (m_State==2) 
				{
					int Selected = m_wndComboClust.GetCurSel();
					Selected = (Selected == 0) ? 255 : (Selected - 1);

					m_MainClusters->MakeUNDO(Selected,1);
					

					RemPolyFromActualProj(Selected);
					
					m_NewPoly->AddProj(m_NewProj);
					CCmdTarget::BeginWaitCursor();
					m_MainClusters->Clustering(m_MainDataStack);
					m_MainDataStack->whichDraw = 0;
					EndWaitCursor();
					flagChoose |= 2;
				}
				m_State=0;
			}
			else 
			{
				if (m_State==1)
				{
					m_NewPoly->SetEmpty();
					m_MainClusters->m_BndrStack.pop_back();
					m_MainClusters->m_Count-=1;
					delete m_NewPoly;
				}
				else if (m_State==2)
				{
					m_NewProj->SetEmpty();
//					m_NewPoly->m_ProjStack.pop_back();
//					m_NewPoly->m_Count-=1;
					delete m_NewProj;
				}
				m_Dragging=0;
				::ClipCursor(NULL);
				ReleaseCapture();
				m_State=0;
			}
			CheckComboClust();
			SetActiveButton(); InvalidateRect(m_rect,FALSE);
		}
		else
		{
			::ClipCursor(NULL);
			ReleaseCapture();			
		}
		
	}
	cdxCSizingDialog::OnRButtonDown(nFlags, point);
}

void CEditDlg::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_State==1)
	{
		m_MainClusters->RemLastBndry();
		m_NewPoly->SetEmpty();
		delete m_NewPoly;
		m_Dragging=0;
		::ClipCursor(NULL);
		::ReleaseCapture();
		m_State=0;
		MessageBeep(0);
	}
	else if (m_State==2)
	{
//		m_NewPoly->RemLastProj();
		m_NewProj->SetEmpty();
		delete m_NewProj;
		
		m_Dragging=0;
		::ClipCursor(NULL);
		::ReleaseCapture();
		m_State=0;
		MessageBeep(0);
	}

	SetActiveButton(); InvalidateRect(m_rect,FALSE);
					
	cdxCSizingDialog::OnRButtonDblClk(nFlags, point);
}

void CEditDlg::SetActiveButton()
{
	MY_STACK::iterator Index,IndProj;
	int Selected;
	int axX,axY;
	int nalezen=0;
	Selected = m_wndComboClust.GetCurSel();
	Selected = (Selected == 0) ? 255 : (Selected - 1);

	CBoundary *MyBoundary;
	CProjection *MyProj;

	m_wndCmbOrigNew.SetCurSel( m_MainDataStack->whichDraw );

	if (Selected>0 && Selected <255)
	{
		if (m_MyView->m_AxesX<m_MyView->m_AxesY)
		{	// pravotociva
			axX=m_MyView->m_AxesX;
			axY=m_MyView->m_AxesY;
		}
		else
		{	// levotociva
			axX=m_MyView->m_AxesY;
			axY=m_MyView->m_AxesX;
		}

		if((m_MainDataStack->whichDraw == CLUST_ORIG ||
		   m_MainDataStack->whichDraw == CLUST_USER) &&
		   Selected < m_MainClusters->m_BndrStack.size())
		{
			Index = (m_MainClusters->m_BndrStack.begin()+Selected);
			MyBoundary = (CBoundary*) *Index;
			IndProj = MyBoundary->m_ProjStack.begin();
			while (IndProj != MyBoundary->m_ProjStack.end() && ~nalezen)
			{
				MyProj = (CProjection*) *IndProj;
				if (MyProj->m_AxesX==axX && MyProj->m_AxesY==axY)
				{
					nalezen=1;
				}
				++IndProj;
			}
		}
		else nalezen = 1;

		if (nalezen)
		{	// Polygon is drawn in this projection
			if (m_MyView->m_ClustToView[Selected]>0)
			{
				SetDlgItemText(IDC_HIDEPOLY,TEXT("&Hide"));
			}
			else
			{
				SetDlgItemText(IDC_HIDEPOLY,TEXT("&Show"));
			}
	

			if (m_State==1 || m_State==2)
			{
				m_wndCopyCluster.EnableWindow(FALSE);	
				m_wndZoomIn.EnableWindow(FALSE);
				m_wndFitView.EnableWindow(FALSE);

				m_wndDrawPoly.EnableWindow(FALSE);
				m_wndRemovePoly.EnableWindow(FALSE);
				m_wndRemAllPoly.EnableWindow(FALSE);
				m_wndNewPoly.EnableWindow(FALSE);
				m_wndSpinX.EnableWindow(FALSE);
				m_wndSpinY.EnableWindow(FALSE);
				m_wndInitView.EnableWindow(FALSE);
				m_wndOK.EnableWindow(FALSE);
				m_wndCmbOrigNew.EnableWindow(FALSE);
				m_wndList.EnableWindow(FALSE);
			}
			else
			{
				m_wndCopyCluster.EnableWindow(TRUE);	
				m_wndZoomIn.EnableWindow(TRUE);
				m_wndFitView.EnableWindow(TRUE);

				m_wndDrawPoly.EnableWindow(TRUE); //FALSE
				m_wndRemovePoly.EnableWindow(TRUE);
				m_wndRemAllPoly.EnableWindow(TRUE);
				m_wndNewPoly.EnableWindow(TRUE);
				m_wndSpinX.EnableWindow(TRUE);
				m_wndSpinY.EnableWindow(TRUE);
				m_wndInitView.EnableWindow(TRUE);
				m_wndOK.EnableWindow(TRUE);
				m_wndCmbOrigNew.EnableWindow(TRUE);
				m_wndList.EnableWindow(TRUE);
			}
		}
		else
		{	// Polygon is not drawn in this projection, but in other is
	
			if (m_MyView->m_ClustToView[Selected]>0)
			{
				SetDlgItemText(IDC_HIDEPOLY,TEXT("&Hide"));
			}
			else
			{
				SetDlgItemText(IDC_HIDEPOLY,TEXT("&Show"));
			}
	

			if (m_State==1 || m_State==2)
			{
				m_wndCopyCluster.EnableWindow(FALSE);	
				m_wndZoomIn.EnableWindow(FALSE);
				m_wndFitView.EnableWindow(FALSE);

				m_wndDrawPoly.EnableWindow(FALSE);
				m_wndRemovePoly.EnableWindow(FALSE);
				m_wndRemAllPoly.EnableWindow(FALSE);
				m_wndNewPoly.EnableWindow(FALSE);
				m_wndSpinX.EnableWindow(FALSE);
				m_wndSpinY.EnableWindow(FALSE);
				m_wndInitView.EnableWindow(FALSE);
				m_wndOK.EnableWindow(FALSE);
				m_wndCmbOrigNew.EnableWindow(FALSE);
				m_wndList.EnableWindow(FALSE);
			}
			else
			{
				m_wndCopyCluster.EnableWindow(TRUE);	
				m_wndZoomIn.EnableWindow(TRUE);
				m_wndFitView.EnableWindow(TRUE);

				m_wndDrawPoly.EnableWindow(TRUE);
				m_wndRemovePoly.EnableWindow(FALSE);
				m_wndRemAllPoly.EnableWindow(TRUE);
				m_wndNewPoly.EnableWindow(TRUE);
				m_wndSpinX.EnableWindow(TRUE);
				m_wndSpinY.EnableWindow(TRUE);
				m_wndInitView.EnableWindow(TRUE);
				m_wndOK.EnableWindow(TRUE);
				m_wndCmbOrigNew.EnableWindow(TRUE);
				m_wndList.EnableWindow(TRUE);
			}
		}
		if (Selected==0)
		{
			m_wndDrawPoly.EnableWindow(FALSE);
			m_wndRemAllPoly.EnableWindow(FALSE);
		}
	}
	else
	{	
		if (Selected == 0)
		{
			// Polygon is not selected
			m_wndDrawPoly.EnableWindow(FALSE);
			m_wndRemovePoly.EnableWindow(FALSE);
			m_wndRemAllPoly.EnableWindow(FALSE);
			m_wndCopyCluster.EnableWindow(FALSE);
			if (m_MyView->m_ClustToView[Selected]>0)
			{
				SetDlgItemText(IDC_HIDEPOLY,TEXT("&Hide"));
			}
			else
			{
				SetDlgItemText(IDC_HIDEPOLY,TEXT("&Show"));
			}

			if (m_State==1 || m_State==2)
			{
				m_wndZoomIn.EnableWindow(FALSE);
				m_wndFitView.EnableWindow(FALSE);
				m_wndNewPoly.EnableWindow(FALSE);
				m_wndSpinX.EnableWindow(FALSE);
				m_wndSpinY.EnableWindow(FALSE);
				m_wndInitView.EnableWindow(FALSE);
				m_wndOK.EnableWindow(FALSE);
				m_wndCmbOrigNew.EnableWindow(FALSE);
				m_wndList.EnableWindow(FALSE);
			}
			else
			{
				m_wndZoomIn.EnableWindow(TRUE);
				m_wndFitView.EnableWindow(TRUE);
				m_wndNewPoly.EnableWindow(TRUE);
				m_wndSpinX.EnableWindow(TRUE);
				m_wndSpinY.EnableWindow(TRUE);
				m_wndInitView.EnableWindow(TRUE);
				m_wndOK.EnableWindow(TRUE);
				m_wndCmbOrigNew.EnableWindow(TRUE);
				m_wndList.EnableWindow(TRUE);
			}
		}
		else { // NOISE
			m_wndDrawPoly.EnableWindow(FALSE);
			m_wndRemovePoly.EnableWindow(FALSE);
			m_wndRemAllPoly.EnableWindow(FALSE);
			m_wndCopyCluster.EnableWindow(FALSE);
			if (m_MyView->m_ClustToView[Selected]>0)
			{
				SetDlgItemText(IDC_HIDEPOLY,TEXT("&Hide"));
			}
			else
			{
				SetDlgItemText(IDC_HIDEPOLY,TEXT("&Show"));
			}

			if (m_State==1 || m_State==2)
			{
				m_wndZoomIn.EnableWindow(FALSE);
				m_wndFitView.EnableWindow(FALSE);
				m_wndNewPoly.EnableWindow(FALSE);
				m_wndSpinX.EnableWindow(FALSE);
				m_wndSpinY.EnableWindow(FALSE);
				m_wndInitView.EnableWindow(FALSE);
				m_wndOK.EnableWindow(FALSE);
				m_wndCmbOrigNew.EnableWindow(FALSE);
				m_wndList.EnableWindow(FALSE);
			}
			else
			{
				m_wndZoomIn.EnableWindow(TRUE);
				m_wndFitView.EnableWindow(TRUE);
				m_wndNewPoly.EnableWindow(TRUE);
				m_wndSpinX.EnableWindow(TRUE);
				m_wndSpinY.EnableWindow(TRUE);
				m_wndInitView.EnableWindow(TRUE);
				m_wndOK.EnableWindow(TRUE);
				m_wndCmbOrigNew.EnableWindow(TRUE);
				m_wndList.EnableWindow(TRUE);
			}
		}
	}

	if (m_State==1 || m_State==2)
	{
		m_wndUndo.EnableWindow(FALSE);
	}
	else
	{
		if ( m_MainClusters->IsUNDO() )
		{
			m_wndUndo.EnableWindow(TRUE);
		}
		else
		{
			m_wndUndo.EnableWindow(FALSE);
		}
	}
}

void CEditDlg::OnZoomin() 
{
	if (m_State==0)
		m_State=10;
}


void CEditDlg::OnFitView() 
{
	FittingView();
	
	InvalidateRect(m_DrawingRect,FALSE);
}

void CEditDlg::OnSelchangeComboclust() 
{
	SetActiveButton(); //InvalidateRect(m_ColorRect);
}

void CEditDlg::OnHidepoly() 
{
	int Selected;
	Selected = m_wndComboClust.GetCurSel();
	Selected = (Selected == 0) ? 255 : (Selected - 1);
	if (Selected>=0)
	{
		if (m_MyView->m_ClustToView[Selected]>0)
		{
			m_MyView->m_ClustToView[Selected] = 0;
			SetDlgItemText(IDC_HIDEPOLY,TEXT("&Show"));
		}
		else
		{
			m_MyView->m_ClustToView[Selected] = 3;
			SetDlgItemText(IDC_HIDEPOLY,TEXT("&Hide"));
		}
	}
	//SetActiveButton(); 
	InvalidateRect(m_rect,FALSE);
}

void CEditDlg::RemPolyFromActualProj(int Selected)
{
	MY_STACK::iterator Index,IndProj;
	
	int axX,axY;
	int nalezen=0,counter=0;
	CBoundary *MyBoundary;
	CProjection *MyProj;

	if (Selected!=0)
	{
		if (m_MyView->m_AxesX<m_MyView->m_AxesY)
			{ axX=m_MyView->m_AxesX; axY=m_MyView->m_AxesY; }
		else { axX=m_MyView->m_AxesY; axY=m_MyView->m_AxesX; }
			Index = (m_MainClusters->m_BndrStack.begin()+abs(Selected));
		MyBoundary = (CBoundary*) *Index;
		IndProj = MyBoundary->m_ProjStack.begin();
		while ((IndProj!=MyBoundary->m_ProjStack.end()) && (!nalezen))
		{
			MyProj = (CProjection*) *IndProj;
			if (MyProj->m_AxesX==axX && MyProj->m_AxesY==axY)
			{
				nalezen=1;
			}
			++IndProj;
			counter++;
		}
		if (nalezen)
		{
			--IndProj;
			MyBoundary->m_ProjStack.erase(IndProj);
			MyBoundary->m_Count-=1;
			delete MyProj;
							
			m_MainClusters->Clustering(m_MainDataStack);
			m_MainClusters->WantStore();
			SetActiveButton(); InvalidateRect(m_rect,FALSE);
		}
	}
}

void CEditDlg::OnRemovepoly() 
{
	int Selected;
	Selected = m_wndComboClust.GetCurSel();
	Selected = (Selected == 0) ? 255 : (Selected - 1);
	
	char pom[80];
	sprintf(pom,"Do you really want to remove boundary of cluster %d from this projection?",Selected);
	if (CWnd::MessageBox(pom,NULL,MB_OKCANCEL|MB_ICONSTOP)==IDOK)
	{
		m_MainClusters->MakeUNDO(Selected,1);
		RemPolyFromActualProj(Selected);
		m_MainClusters->WantStore();
	}
}

void CEditDlg::OnRemallpoly() 
{	
	int Selected = m_wndComboClust.GetCurSel();
	Selected = (Selected == 0) ? 255 : (Selected - 1);

	char pom[80];
	sprintf(pom,"Do you really want to remove cluster %d from all projection?",Selected);
	if (CWnd::MessageBox(pom,NULL,MB_OKCANCEL|MB_ICONSTOP)==IDOK)
	{
		if (Selected>0 && Selected < 255)
		{
			m_MainClusters->MakeUNDO(Selected,2);
			
			CBoundary *Bndr =(CBoundary*) *(m_MainClusters->m_BndrStack.begin()+Selected);
			m_MainClusters->m_BndrStack.erase((m_MainClusters->m_BndrStack.begin()+Selected));
			m_MainClusters->m_Count-=1;
			delete Bndr;
			int i=Selected;
			MY_STACK::iterator Index;
			for (Index=(m_MainClusters->m_BndrStack.begin()+Selected);Index!=m_MainClusters->m_BndrStack.end();++Index)
			{
				CBoundary *MyBoundary = (CBoundary*)*Index;
				MyBoundary->m_Numb=i;
				i++;
				Selected++;
			}
//			m_wndComboClust.DeleteString(Selected+1);
			m_MainDataStack->whichDraw = CLUST_USER;
			CheckComboClust();
			m_MainClusters->Clustering(m_MainDataStack);
			m_MainClusters->WantStore();

			m_wndComboClust.SetCurSel(0);
			SetActiveButton(); InvalidateRect(m_rect,FALSE); 
			flagChoose |= 2;
		}
	}
}

void CEditDlg::OnCopycluster() 
{
	MY_STACK::iterator IndBndr, IndProj, IndPnt2D, IndVect2D;

	CBoundary *mainBndr,*newBndr;
	CProjection *newProj;
	CPoint2D *newPnt2D;
	CVect2D *newVect2D;

	int Selected=m_wndComboClust.GetCurSel();
	Selected = (Selected == 0) ? 255 : (Selected - 1);


	if (Selected > 0 && Selected < 255)
	{
		IndBndr = (m_MainClusters->m_BndrStack.begin()+Selected);
		mainBndr = (CBoundary*) *IndBndr;

		newBndr = new CBoundary(m_MainPalette);
		newBndr->m_Numb = m_MainClusters->m_Count;
		m_MainClusters->AddBndry(newBndr);

//		char pom[20];
//		sprintf(pom,"cluster %d",newBndr->m_Numb);
//		m_wndComboClust.AddString(pom);
		m_MainDataStack->whichDraw = CLUST_USER;
		CheckComboClust();

		for (IndProj=mainBndr->m_ProjStack.begin();IndProj!=mainBndr->m_ProjStack.end();IndProj++)
		{

			newProj = new CProjection(m_MainPalette);
			newProj->m_AxesX = ((CProjection*) *IndProj)->m_AxesX;
			newProj->m_AxesY = ((CProjection*) *IndProj)->m_AxesY;
			newBndr->AddProj(newProj);

			for (IndPnt2D=((CProjection*)*IndProj)->m_Pnt2DStack.begin();IndPnt2D!=((CProjection*)*IndProj)->m_Pnt2DStack.end();IndPnt2D++)
			{
				newPnt2D = new CPoint2D();
				newPnt2D->m_X = ((CPoint2D*) *IndPnt2D)->m_X;
				newPnt2D->m_Y = ((CPoint2D*) *IndPnt2D)->m_Y;
				newProj->AddPnt2D(newPnt2D);
			}
			
			for (IndVect2D=((CProjection*)*IndProj)->m_Vect2DStack.begin();IndVect2D!=((CProjection*)*IndProj)->m_Vect2DStack.end();IndVect2D++)
			{
				newVect2D = new CVect2D();
				newVect2D->m_X = ((CVect2D*) *IndVect2D)->m_X;
				newVect2D->m_Y = ((CVect2D*) *IndVect2D)->m_Y;
				newVect2D->m_b = ((CVect2D*) *IndVect2D)->m_b;
				newProj->AddVect2D(newVect2D);
			}
		}
		m_MainClusters->Clustering(m_MainDataStack);
		SetActiveButton(); InvalidateRect(m_rect,FALSE);
		char pomStr[80];
		sprintf(pomStr,"Cluster %d was copied to cluster %d",mainBndr->m_Numb,newBndr->m_Numb);
		m_MainClusters->MakeUNDO( newBndr->m_Numb, 3);
		SetActiveButton();
		flagChoose |= 2;
		CWnd::MessageBox(pomStr,NULL,MB_OK);
	}
}

void CEditDlg::OnOK() 
{
	StoreWindowPosition(_T("Main\\Window")); 	
	cdxCSizingDialog::OnOK();
}

void CEditDlg::OnSelchangeCombopen() 
{
	m_MyView->m_PointsSize = m_wndComboPen.GetCurSel()+1;
	InvalidateRect(m_rect,FALSE);
}

void CEditDlg::OnSettings() 
{
	CSetDial SetDial;

	SetDial.m_EdFirstEv = m_FirstEdEv;
	SetDial.m_EdLastEv = m_LastEdEv;
	SetDial.m_EdPrecent = m_PercEdEv;
	SetDial.m_Type = 2;
	SetDial.m_ClMode = m_MainClusters->m_ClMode;

	if (SetDial.DoModal()==IDOK)
	{
		m_FirstEdEv = SetDial.m_EdFirstEv;
		if (m_FirstEdEv>m_MainDataStack->GetNumVerx()-1)
			m_FirstEdEv=m_MainDataStack->GetNumVerx()-1;
		if (m_FirstEdEv<1)
			m_FirstEdEv=1;
		
		m_LastEdEv = SetDial.m_EdLastEv;
		if (m_LastEdEv>m_MainDataStack->GetNumVerx() || m_LastEdEv==0)
			m_LastEdEv=m_MainDataStack->GetNumVerx();
		if (m_LastEdEv<=m_FirstEdEv)
			m_LastEdEv=m_FirstEdEv+1;

		m_PercEdEv = SetDial.m_EdPrecent;
		if (m_PercEdEv<1)
			m_PercEdEv=1;
		if (m_PercEdEv>100)
			m_PercEdEv=100;

		if (SetDial.m_ClMode != m_MainClusters->m_ClMode)
		{ 
			m_MainClusters->m_ClMode = SetDial.m_ClMode;
			m_MainClusters->Clustering(m_MainDataStack);
		}
	}
	m_MainDataStack->CalcViewVx(m_FirstEdEv,m_LastEdEv,m_PercEdEv);
	InvalidateRect(m_DrawingRect,false);
	InvalidateRect(CRect(m_DrawingSpikesRect.left+1,m_DrawingSpikesRect.bottom+26,m_DrawingSpikesRect.right-1,m_DrawingSpikesRect.bottom+105));
}

void CEditDlg::OnDrawSpikes() 
{
	m_DrawSpikes=1;
	InvalidateRect(m_DrawingSpikesRect,FALSE);
}

void CEditDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_DrawingSpikesRect.PtInRect(point))
	{
		CEditSpikes ViewSpikes;
		
		ViewSpikes.m_MainDataStack = m_MainDataStack;
		ViewSpikes.m_MainPalette = m_MainPalette;
		ViewSpikes.m_MainClusters = m_MainClusters;
		ViewSpikes.m_FirstEdSp = m_FirstEdSp;
		ViewSpikes.m_LastEdSp = m_LastEdSp;
		ViewSpikes.m_PercEdSp = m_PercEdSp;
		ViewSpikes.m_size = m_SpikeEdSize;

		if (ViewSpikes.DoModal()==IDOK)
		{
		}
		m_FirstEdSp = ViewSpikes.m_FirstEdSp;
		m_LastEdSp = ViewSpikes.m_LastEdSp;
		m_PercEdSp = ViewSpikes.m_PercEdSp;
		m_SpikeEdSize = ViewSpikes.m_size;
		
		m_wndSpinX.SetRange(1,m_MainDataStack->Dimension);
		m_wndSpinY.SetRange(1,m_MainDataStack->Dimension);
		if (m_MyView->m_AxesY > m_MainDataStack->Dimension)
			m_MyView->m_AxesY = m_MainDataStack->Dimension;
		if (m_MyView->m_AxesX > m_MainDataStack->Dimension)
			m_MyView->m_AxesX = m_MainDataStack->Dimension;
		if (m_MyView->m_AxesX == m_MyView->m_AxesY)
			m_MyView->m_AxesX = m_MyView->m_AxesY - 1;
		m_wndSpinX.SetPos(m_MyView->m_AxesX);
		m_wndSpinY.SetPos(m_MyView->m_AxesY);

		m_MainDataStack->CalcViewSpike(m_FirstSp,m_LastSp,m_PercSp);

		SetActiveButton();
		InvalidateRect(m_DrawingRect,false);
	}
// WITH THIS ERROR. WHY?
//	cdxCSizingDialog::OnLButtonDblClk(nFlags, point);
//MessageBeep(0);::Sleep(1000);
}

void CEditDlg::OnSetSpikes() 
{
	CSetDial SetDial;

	SetDial.m_EdFirstEv = m_FirstSp;
	SetDial.m_EdLastEv = m_LastSp;
	SetDial.m_EdPrecent = m_PercSp;
	SetDial.m_Type = 1;
	SetDial.m_ClMode = m_MainClusters->m_ClMode;

	if (SetDial.DoModal()==IDOK)
	{
		m_FirstSp = SetDial.m_EdFirstEv;
		if (m_FirstSp>m_MainDataStack->GetNumVerx()-1)
			m_FirstSp=m_MainDataStack->GetNumVerx()-1;
		if (m_FirstSp<1)
			m_FirstSp=1;
		
		m_LastSp = SetDial.m_EdLastEv;
		if (m_LastSp>m_MainDataStack->GetNumVerx() || m_LastSp==0)
			m_LastSp=m_MainDataStack->GetNumVerx();
		if (m_LastSp<=m_FirstSp)
			m_LastSp=m_FirstSp+1;

		m_PercSp = SetDial.m_EdPrecent;
		if (m_PercSp<1)
			m_PercSp=1;
		if (m_PercSp>100)
			m_PercSp=100;

		if (SetDial.m_ClMode != m_MainClusters->m_ClMode)
		{ 
			m_MainClusters->m_ClMode = SetDial.m_ClMode;
			m_MainClusters->Clustering(m_MainDataStack);
			InvalidateRect(m_DrawingRect,false);
		}
	}
	m_MainDataStack->CalcViewSpike(m_FirstSp,m_LastSp,m_PercSp);
	InvalidateRect(CRect(m_DrawingSpikesRect.left+1,m_DrawingSpikesRect.bottom+26,m_DrawingSpikesRect.right-1,m_DrawingSpikesRect.bottom+105));
}

void CEditDlg::FittingView()
{
	const int iX = m_MyView->m_AxesX;
	const int iY = m_MyView->m_AxesY;

	const CString& oStrX = *m_MainDataStack->m_AxesStack[iX-1];
	const CString& oStrY = *m_MainDataStack->m_AxesStack[iY-1];

	float fFctrX = 9.5 , fFctrY = 9.5;

	if(-1!=oStrX.Find("Room") || -1!=oStrX.Find("Arena"))
		fFctrX = 1.0;

	if(-1!=oStrY.Find("Room") || -1!=oStrY.Find("Arena"))
		fFctrY = 1.0;

	CFloatRect& DataArea = m_MyView->m_DataArea;

	DataArea.left   = m_MainDataStack->GetMin(iX);
	DataArea.right  = m_MainDataStack->GetMax(iX);
	DataArea.bottom = m_MainDataStack->GetMin(iY);
	DataArea.top    = m_MainDataStack->GetMax(iY);

	float fXShift = DataArea.GetSizeX() / fFctrX;
	float fYShift = DataArea.GetSizeY() / fFctrY;
	
	DataArea.left   -= fXShift;
	DataArea.right  += fXShift;
	DataArea.bottom -= fYShift;
	DataArea.top    += fYShift;	
}

void CEditDlg::OnInitview() 
{
	m_MyView->m_DataArea.left   = m_MyView->m_DefaultDataArea.left;	
	m_MyView->m_DataArea.right  = m_MyView->m_DefaultDataArea.right;	
	m_MyView->m_DataArea.top    = m_MyView->m_DefaultDataArea.top;
	m_MyView->m_DataArea.bottom = m_MyView->m_DefaultDataArea.bottom;	

	float x1,y1,x2,y2;
	x1=m_MyView->m_DataArea.GetLeft()-m_MyView->m_DataArea.GetSizeX()*0.1;
	x2=m_MyView->m_DataArea.GetRight()+m_MyView->m_DataArea.GetSizeX()*0.1;
	y1=m_MyView->m_DataArea.GetTop()+m_MyView->m_DataArea.GetSizeY()*0.1;
	y2=m_MyView->m_DataArea.GetBottom()-m_MyView->m_DataArea.GetSizeY()*0.1;
	m_MyView->m_DataArea.SetValue(x1,y1,x2,y2);

	InvalidateRect(m_DrawingRect,FALSE);
}

void CEditDlg::OnSelchangeCmbOrigNew() 
{
	m_MainDataStack->whichDraw = m_wndCmbOrigNew.GetCurSel();	
	CheckComboClust();
	InvalidateRect(m_DrawingRect,FALSE);
	SetActiveButton();
}

void CEditDlg::OnList() 
{
	CDialList dl;
	dl.m_MainClusters = m_MainClusters;
	dl.m_MainDataStack = m_MainDataStack;
	if (dl.DoModal()==IDOK)
	{
		if (dl.AxX>0 && dl.AxY>0)
		{
			m_MyView->m_AxesX = dl.AxX;
			m_MyView->m_AxesY = dl.AxY;
			InvalidateRect(m_DrawingRect,FALSE);
			m_wndComboClust.SetCurSel(dl.Cl+1);
			SetActiveButton();
		}
		if (m_wndCheckFit.GetCheck())
		{
			m_MyView->m_DataArea.left = m_MainDataStack->GetMin((int)m_MyView->m_AxesX);
			m_MyView->m_DataArea.right = m_MainDataStack->GetMax((int)m_MyView->m_AxesX);
			m_MyView->m_DataArea.bottom = m_MainDataStack->GetMin(m_MyView->m_AxesY);
			m_MyView->m_DataArea.top = m_MainDataStack->GetMax(m_MyView->m_AxesY);
	
			m_MyView->m_DataArea.left -= m_MyView->m_DataArea.GetSizeX()/9.5;
			m_MyView->m_DataArea.right += m_MyView->m_DataArea.GetSizeX()/9.5;
			m_MyView->m_DataArea.bottom -= m_MyView->m_DataArea.GetSizeY()/9.5;
			m_MyView->m_DataArea.top += m_MyView->m_DataArea.GetSizeY()/9.5;
	
			InvalidateRect(m_DrawingRect,FALSE);
		}
	}
}

void CEditDlg::OnButUndo() 
{
	m_MainDataStack->whichDraw = CLUST_USER;

	m_MainClusters->UNDO();
	m_MainClusters->Clustering(m_MainDataStack);
	
	// Filling combo of clusters
	int oldSel = m_wndComboClust.GetCurSel();
	
	CheckComboClust();

	m_wndComboClust.SetCurSel(oldSel);
	
	InvalidateRect(m_DrawingRect,false);
	flagChoose |= 2;
	SetActiveButton();
}

void CEditDlg::OnTimeEv() 
{
	CTimeEv dlg;

	dlg.m_MainDataStack = m_MainDataStack;
	dlg.m_MainClusters = m_MainClusters;
	dlg.m_MainPalette = m_MainPalette;

	dlg.DoModal();
}


void CEditDlg::OnExpand() 
{
	flagChoose = 0;

//	CChooseClDlg dlg;
//	dlg.flag = &flagChoose;
//	dlg.toView = m_MyView->m_ClustToView;
//	dlg.m_MainClusters = m_MainClusters;
//	dlg.m_MainDataStack = m_MainDataStack;

//	SetTimer(IDT_INVALIDATE_CHOOSE,10,NULL);
//	dlg.DoModal();
//	KillTimer(IDT_INVALIDATE_CHOOSE);

//	if (m_colDlg->GetSafeHwnd() == 0)
	if ( m_colDlgState == 0 )
	{
		
		m_colDlg = new CChooseClDlg;
		m_colDlg->Create(this); // displays the dialog window

		m_colDlg->flag = &flagChoose;
		m_colDlg->toView = m_MyView->m_ClustToView;
		m_colDlg->m_MainDataStack = m_MainDataStack;
		m_colDlg->m_MainClusters = m_MainClusters;
		m_colDlg->ShowWindow(SW_SHOW);
		SetTimer(IDT_INVALIDATE_CHOOSE,10,NULL);
		m_colDlgState = 1;
//		m_wndExpand.SetState(1);
	}
/*	else
	{
		m_colDlgState = 0;
		m_colDlg->DestroyWindow();
		delete m_colDlg;
		KillTimer(IDT_INVALIDATE_CHOOSE);
		m_wndExpand.SetState(0);
	}
	
*/
	SetActiveButton();
}

void CEditDlg::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
	case IDT_INVALIDATE_CHOOSE:
		if ((flagChoose & 1)== 1)
		{
			flagChoose &= 0xFE;
			InvalidateRect(m_rect,FALSE);
		}
		break;
	}	

	cdxCSizingDialog::OnTimer(nIDEvent);
}


void CEditDlg::OnClose() 
{
	if (m_colDlgState != 0)
	{
		m_colDlg->DestroyWindow();
		m_colDlgState = 0;
		delete m_colDlg;
	}
	cdxCSizingDialog::OnClose();
}

LRESULT CEditDlg::OnGoodbye(WPARAM wParam, LPARAM lParam)
{
	// message received in response to modeless dialog OK
	//  and Cancel buttons
	m_colDlg->DestroyWindow();
	delete m_colDlg;
	m_colDlgState = 0;	
	m_wndExpand.SetState(0);
	return 0L;
}

bool CEditDlg::UserCheating()
{
	CString* nameY = (CString*) *(m_MainDataStack->m_AxesStack.begin()+m_MyView->m_AxesY-1);
	CString* nameX = (CString*) *(m_MainDataStack->m_AxesStack.begin()+m_MyView->m_AxesX-1);
	if(!nameY->Compare("RoomX") || !nameY->Compare("RoomY") ||
	   !nameX->Compare("RoomX") || !nameX->Compare("RoomY"))
	{
		char msg[1024]={0} , sUser[1024]={0};
		DWORD dLen = 1024;
		if(!GetUserName(sUser,&dLen)) sprintf(sUser,"...");
		sprintf(msg,"Nice try %s...No cheating allowed!! Can't use position for clustering!!",sUser);
		MessageBox(msg,"WClust - cheater detected",MB_ICONERROR);
		return true;
	}
	return false;
}

void CEditDlg::OnBnClickedMakeNoiseBtn()
{
	if(m_bNoiseMode)
	{
		m_bNoiseMode = false;
		SetDlgItemText(IDC_MAKE_NOISE_BTN,TEXT("Noise Off"));
	}
	else
	{
		m_bNoiseMode = true;
		SetDlgItemText(IDC_MAKE_NOISE_BTN,TEXT("Noise On"));
	}
}

void CEditDlg::OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CEditDlg::OnBnClickedButtonRate()
{
	// TODO: Add your control notification handler code here
	if(!m_pRDlg)
	{
		MessageBox("Error: no rating dialog available!!","WClust - error!",MB_ICONERROR);
		return;
	}

	m_wndCmbOrigNew.SetCurSel(0);

	m_MainDataStack->whichDraw = CLUST_USER;

	CheckComboClust();

	InvalidateRect(m_DrawingRect,FALSE);

	m_pRDlg->DoModal();
}
