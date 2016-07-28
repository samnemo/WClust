// $Id: EditSpikes.cpp,v 1.6 2008/08/11 20:56:30 samn Exp $ 
// EditSpikes.cpp : implementation file
//

/*

Version 123 changes:: Jaini Shah :: 2.19.16

In all of the below functions, replaced 63/32000 with CVertex::FREQ/CVertex::SAMPLES
OnPaint, OnLButtonDown, OnMouseMove, OnSelchangeComboprop, OnSelchangeCmbSelParam, 

Reason for change : Milenna had started recordings where Spike Sampling Freq was 48000 and 
Tetrode record format was 1440. 63/32000 was displaying maximum of 2ms of data. 
where as for Milenna's recording we needed 1440/48000 = 30ms of data display. So, now with 
CVertex::FREQ/CVertex::SAMPLES, it gets FREQ and SAMPLES values from header file. 
Vertex.cpp=>CVerxStack:GetBPFHeader_SetupInformation and 
Vertex.cpp=>CVerxStack::GetBPFHeader

*/

#include "stdafx.h"
#include "wclust.h"
#include "EditSpikes.h"
#include "SetDial.h"
#include "math.h"
#include "DlgAddWCP.h"
#include "StringUtils.h"
#include "WCMath.h"
#include ".\editspikes.h"
#include "vertex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int ID_TimerPlay = 100;

/////////////////////////////////////////////////////////////////////////////
// CEditSpikes dialog

CEditSpikes::CEditSpikes(CWnd* pParent /*=NULL*/)
	: cdxCSizingDialog(CEditSpikes::IDD, pParent)
{

	//{{AFX_DATA_INIT(CEditSpikes)
	//}}AFX_DATA_INIT

}


void CEditSpikes::DoDataExchange(CDataExchange* pDX)
{
	cdxCSizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditSpikes)
	DDX_Control(pDX, IDC_LIST_CLUST, m_wndListClust);
	DDX_Control(pDX, IDC_BCLEAR, m_wndClear);
	DDX_Control(pDX, IDC_ADD, m_wndAdd);
	DDX_Control(pDX, IDC_CMB_SEL_PARAM, m_wndCmbSelParam);
	DDX_Control(pDX, IDC_SET_SP, m_wndSetSp);
	DDX_Control(pDX, IDC_REMOVE, m_wndRemove);
	DDX_Control(pDX, IDC_CHECK_CH4, m_wndCheck4);
	DDX_Control(pDX, IDC_CHECK_CH3, m_wndCheck3);
	DDX_Control(pDX, IDC_CHECK_CH2, m_wndCheck2);
	DDX_Control(pDX, IDC_CHECK_CH1, m_wndCheck1);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_STOP, m_wndStop);
	DDX_Control(pDX, IDC_PLAY, m_wndPlay);
	DDX_Control(pDX, IDC_NEXT, m_wndNext);
	DDX_Control(pDX, IDC_COMBOPROP, m_wndComboProp);
	DDX_Control(pDX, IDC_EDIT_EXTRA_PARAM, m_wndExtraParm);
	DDX_Control(pDX, IDC_STATIC_EXTRA_PARAM, m_wndPrompt);
		
	//}}AFX_DATA_MAP
}
	

BEGIN_MESSAGE_MAP(CEditSpikes, cdxCSizingDialog)
	//{{AFX_MSG_MAP(CEditSpikes)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_CBN_SELCHANGE(IDC_COMBOPROP, OnSelchangeComboprop)
	ON_BN_CLICKED(IDC_CHECK_CH1, OnCheckCh1)
	ON_BN_CLICKED(IDC_CHECK_CH2, OnCheckCh2)
	ON_BN_CLICKED(IDC_CHECK_CH3, OnCheckCh3)
	ON_BN_CLICKED(IDC_CHECK_CH4, OnCheckCh4)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_SET_SP, OnSetSp)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_CBN_SELCHANGE(IDC_CMB_SEL_PARAM, OnSelchangeCmbSelParam)
	ON_BN_CLICKED(IDC_BCLEAR, OnBclear)
	ON_BN_CLICKED(IDC_EXT_PARAM, OnExtParam)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CLUST, OnItemchangedListClust)

	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDC_SelectAll, OnBnClickedButtonSelectall)
	//ON_BN_CLICKED(IDC_UnselectAll, OnBnClickedButtonUnselectall)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditSpikes message handlers

BOOL CEditSpikes::OnInitDialog()
{
	cdxCSizingDialog::OnInitDialog();
	
	RestoreWindowPosition(_T("Main\\EdSpWin")); 

	AddSzControl( m_wndComboProp, mdNone,mdRepos);
	AddSzControl( m_wndPlay, mdNone,mdRepos);
	AddSzControl( m_wndStop, mdNone,mdRepos);
	AddSzControl( m_wndNext, mdNone,mdRepos);
	AddSzControl( m_wndClear, mdNone,mdRepos);
	AddSzControl( m_wndCheck1, mdNone,mdRepos);
	AddSzControl( m_wndCheck2, mdNone,mdRepos);
	AddSzControl( m_wndCheck3, mdNone,mdRepos);
	AddSzControl( m_wndCheck4, mdNone,mdRepos);
	AddSzControl( m_wndCmbSelParam, mdNone,mdRepos);
	AddSzControl( m_wndAdd, mdNone,mdRepos);
	AddSzControl( m_wndPrompt, mdNone, mdRepos);
	AddSzControl( m_wndExtraParm, mdNone, mdRepos);
	AddSzControl( m_wndRemove, mdNone,mdRepos);
	AddSzControl( m_wndSetSp, mdRepos,mdRepos);
	AddSzControl( m_wndOK, mdRepos,mdRepos);
	AddSzControl( m_wndListClust, mdRepos,mdResize);
	
	m_HCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_HSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_LoadAct = 0;
	CLUST_SEL = 1;

	m_wndExtraParm.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEditSpikes::OnOK() 
{
	StoreWindowPosition(_T("Main\\EdSpWin")); 	
	cdxCSizingDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Do not call cdxCSizingDialog::OnPaint() for painting messages

	GetClientRect(m_MainRect);

	m_ClearRect = m_MainRect;
	m_ClearRect.left += 7;
	m_ClearRect.right -= 127;//7;
	m_ClearRect.top += 12;
	m_ClearRect.bottom -= 78;
	if ( (m_ClearRect.Width()-2*m_size) < 40)
		m_size = (m_ClearRect.Width()-40)/2;
			
	
	m_DrawingRect=m_MainRect;
	m_DrawingRect.left += 11+m_size;
	m_DrawingRect.top += 13;
	m_DrawingRect.right -= 131+m_size;//11+m_size;
	m_DrawingRect.bottom -= 79;
	
	m_sizeLeft=m_DrawingRect;
	m_sizeLeft.left -= 4;
	m_sizeLeft.right = m_sizeLeft.left+3;
	m_sizeLeft.top -= 1;
	m_sizeLeft.bottom += 1;

	m_sizeRight = m_DrawingRect;
	m_sizeRight.right += 4;
	m_sizeRight.left = m_sizeRight.right-3;
	m_sizeRight.top -= 1;
	m_sizeRight.bottom += 1;

	// When changing size of drawing window	
	if (m_MovingMode == 20 || m_MovingMode ==21)
	{
		dc.MoveTo(m_sizeLeft.right,m_sizeLeft.top);
		dc.LineTo(m_sizeRight.left,m_sizeRight.top);
		
		dc.MoveTo(m_sizeLeft.right,m_sizeLeft.bottom-1);
		dc.LineTo(m_sizeRight.left,m_sizeRight.bottom-1);
		
		dc.Rectangle(m_sizeLeft);
		dc.Rectangle(m_sizeRight);
	}
	
	if (m_MovingMode == 0)
	{
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
		char pomStr[60];
		sprintf(pomStr,"%d %% of  (%d .. %d)", m_PercEdSp,m_FirstEdSp,m_LastEdSp);
		dc.TextOut(10+120,m_DrawingRect.bottom+59,pomStr);
	
		// Restoring font View Params
		dc.SelectObject(pOldFontVP);
		dc.SetBkMode(oldBkMode);

		int oldBkColor = dc.SetBkColor(RGB(255,255,255));

		int mDrw[4], iI=0;
		if (m_wndCheck1.GetCheck()) 
			{ mDrw[iI] = 0; iI++; }
		if (m_wndCheck2.GetCheck()) 
			{ mDrw[iI] = 1; iI++; }
		if (m_wndCheck3.GetCheck()) 
			{ mDrw[iI] = 2; iI++; }
		if (m_wndCheck4.GetCheck()) 
			{ mDrw[iI] = 3; iI++; }
	
		m_DrawingMode = iI;

		if (!m_DrawOnlyOne)
		{
			dc.Rectangle(m_DrawingRect.left-1,m_DrawingRect.top-1,m_DrawingRect.right+1,m_DrawingRect.bottom+1);
			dc.Rectangle(m_sizeLeft);
			dc.Rectangle(m_sizeRight);
			CRect SpikeRect;
			for (iI=0; iI<m_DrawingMode; iI++)
			{
				SpikeRect = m_DrawingRect;
				SpikeRect.top = m_DrawingRect.top + iI*(m_DrawingRect.Height()/m_DrawingMode) + 1;
				SpikeRect.bottom = m_DrawingRect.top + (iI+1)*(m_DrawingRect.Height()/m_DrawingMode) -1;
				m_MainDataStack->DrawSpikes(&dc,SpikeRect,mDrw[iI],0,m_ActualEdSp);
	
				char sTxt[20];
				sprintf(sTxt,"Ch. %d",mDrw[iI]+1);
				dc.TextOut(SpikeRect.left+5,SpikeRect.top+5,sTxt);
			}
		}
		else
		{	// adding only one spike into drawn picture
			m_DrawOnlyOne=0;

			if (m_IsLine1 == 2) 
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
				m_IsLine1 = 1;
			}

			if (m_wndCmbSelParam.GetCurSel()==2 && m_IsLine2 == 2)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
				m_IsLine2 = 1;
			}


			int Last;
			CRect SpikeRect;
			for (iI=0; iI<m_DrawingMode; iI++)
			{
				SpikeRect = m_DrawingRect;
				SpikeRect.top = m_DrawingRect.top + iI*(m_DrawingRect.Height()/m_DrawingMode) + 1;
				SpikeRect.bottom = m_DrawingRect.top + (iI+1)*(m_DrawingRect.Height()/m_DrawingMode) -1;
				m_IsEnd = m_MainDataStack->DrawSpikes(&dc,SpikeRect,mDrw[iI],m_ActualEdSp,1,&Last);
			}
			m_ActualEdSp += Last ;
		}
		if (m_IsLine1)
		{
			dc.SetROP2(R2_NOT);
			dc.MoveTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
			dc.LineTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
			dc.SetROP2(R2_COPYPEN);
			m_IsLine1 = 2;
		}
		if (m_wndCmbSelParam.GetCurSel()==2 && m_IsLine2)
		{
			dc.SetROP2(R2_NOT);
			dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
			dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
			dc.SetROP2(R2_COPYPEN);
			m_IsLine2 = 2;
		}
		dc.SetBkColor(oldBkColor);
	}
	if (m_LoadAct)
	{
		m_LoadAct = 0;
		m_ActualEdSp = m_RestoreAct;
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	cdxCSizingDialog::OnShowWindow(bShow, nStatus);

	m_IsEnd = 0;
	m_DrawOnlyOne = 0;
	m_ActualEdSp = 0;
	m_IsPlay = 0;
	m_IsStopped = 0;

	if (m_MainDataStack->whichDraw == CLUST_ORIG)
		maxCluster =  m_MainDataStack->m_NumOriginalCl;
	else if(m_MainDataStack->whichDraw == CLUST_USER)
		maxCluster = m_MainClusters->m_Count - 1;
	else
		maxCluster =  m_MainClusters->m_pNumClusts[m_MainDataStack->whichDraw];

	clView = vector<unsigned char>(256);
	std::fill(clView.begin(),clView.end(),(unsigned char)0xFF);

	FillImageList();
	FillClList();

	m_wndCheck1.SetCheck(1);
	m_wndCheck2.SetCheck(1);
	m_wndCheck3.SetCheck(1);
	m_wndCheck4.SetCheck(1);
	
	m_DrawingMode = 4;
	
	m_Line1Pos = (float) 0.0010;
	m_Line2Pos = (float) 0.0015;
	m_IsLine1 = 1;
	m_IsLine2 = 0;
	
	m_MovingMode = 0;

	//Fill sel. param. combo
	m_wndCmbSelParam.AddString("V(t)");
	m_wndCmbSelParam.AddString("dV(t)/dt");
	m_wndCmbSelParam.AddString("Energy(t1..t2)");
	m_wndCmbSelParam.AddString("from file...");
	m_wndCmbSelParam.AddString("Wavelet");
	//m_wndCmbSelParam.AddString("PCA");
	//m_wndCmbSelParam.AddString("HQPCAEnergy");
	m_wndCmbSelParam.SetCurSel(0);

	// Fill cluster's combo
/*	CBoundary *Bndr;
	MY_STACK::iterator index;
	index=(m_MainClusters->m_BndrStack.begin());
	char pom[40];
	m_wndComboSpClust.AddString("Noise");
	m_wndComboSpClust.AddString("All");
	while ( index!=m_MainClusters->m_BndrStack.end() )
	{
		Bndr=(CBoundary*) *index;
		if (Bndr->GetNumb()!=0)
		{
			sprintf(pom,"cluster %d",Bndr->GetNumb());
			m_wndComboSpClust.AddString(pom);
		}
		index++;
	}

	m_wndComboSpClust.SetCurSel(1);
*/
	m_MainDataStack->CalcViewSpike(m_FirstEdSp,m_LastEdSp,m_PercEdSp);

	FillCmbProp();
	
	HICON pom1 = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CWnd::SetIcon(pom1,true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::FillCmbProp()
{
	m_wndComboProp.ResetContent();

	MY_STACK& oParmStack = m_MainClusters->m_ParamDimStack;
	MY_STACK::iterator IT = oParmStack.begin();
	while(IT<oParmStack.end())
	{
		CParamDim* pAct = (CParamDim*) *IT;
		char pom[40];
		if (pAct->GetType() == 2)
			sprintf(pom,"Y(t) t=%1.3fms",pAct->GetParam(1)*1000);
		else if (pAct->GetType() == 3)
			sprintf(pom,"dY(t)/dt t=%1.3fms",pAct->GetParam(1)*1000);
		else if (pAct->GetType() == 4)
			sprintf(pom,"E (%1.3f - %1.3f)ms",pAct->GetParam(1)*1000,pAct->GetParam(2)*1000);
		else if (pAct->GetType() == 5)
			sprintf(pom,"%s", (LPCTSTR) (m_MainClusters->usedExternWCP.begin() + pAct->GetParam(1))->paramName );
		else if (pAct->GetType() == 6)
			sprintf(pom,"Wavelet %g", pAct->m_Par2);
		else if (pAct->GetType() == 7)
			sprintf(pom,"HQPC1Energy");
		m_wndComboProp.AddString(pom);
		if(pAct->GetType() == 6)
			IT += pAct->m_Par2;
		else if(pAct->GetType() == 7)
			IT += 8;
		else
			IT += 4;
	}
	
	m_wndComboProp.SetCurSel(m_wndComboProp.GetCount()-1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::EnableButton()
{
	m_wndCheck1.EnableWindow();
	m_wndCheck2.EnableWindow();
	m_wndCheck3.EnableWindow();
	m_wndCheck4.EnableWindow();
	m_wndComboProp.EnableWindow();
	m_wndOK.EnableWindow();
	m_wndAdd.EnableWindow();
	m_wndCmbSelParam.EnableWindow();
	m_wndRemove.EnableWindow();
	m_wndNext.EnableWindow();
	m_wndSetSp.EnableWindow();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::DisableButton()
{
	m_wndCheck1.EnableWindow(false);
	m_wndCheck2.EnableWindow(false);
	m_wndCheck3.EnableWindow(false);
	m_wndCheck4.EnableWindow(false);
	m_wndComboProp.EnableWindow(false);
	m_wndOK.EnableWindow(false);
	m_wndAdd.EnableWindow(false);
	m_wndCmbSelParam.EnableWindow(false);
	m_wndRemove.EnableWindow(false);
	m_wndNext.EnableWindow(false);
	m_wndSetSp.EnableWindow(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnNext() 
{
	if (!m_IsEnd)
	{
//		m_ActualEdSp++;
		m_DrawOnlyOne=1;
		InvalidateRect(m_DrawingRect,FALSE);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnPlay() 
{
	if (!m_IsPlay)
	{   // Play is pressed
		if (!m_IsEnd)
		{	// Playing mode from pause mode
			if (m_IsStopped)
			{
				m_IsEnd = 0;
				m_ActualEdSp = 0;
				InvalidateRect(m_DrawingRect);
				m_IsStopped = 0;
			}
			SetDlgItemText(IDC_PLAY,TEXT("&Pause"));
			SetTimer(ID_TimerPlay,100,NULL);
			m_IsPlay = 1;
			DisableButton();
		}
		else
		{
			m_IsEnd = 0;
			m_ActualEdSp = 0;
			InvalidateRect(m_DrawingRect);
			m_IsStopped = 0;
			SetDlgItemText(IDC_PLAY,TEXT("&Pause"));
			SetTimer(ID_TimerPlay,100,NULL);
			m_IsPlay = 1;
			DisableButton();
		}
	}
	else 
	{   // Pause is pressed
		SetDlgItemText(IDC_PLAY,TEXT("&Play")); 
		KillTimer(ID_TimerPlay);
		EnableButton();
		m_IsPlay = 0;
	}
}

//////////////////////////////////////////////////////////
void CEditSpikes::OnStop() 
{
	SetDlgItemText(IDC_PLAY,TEXT("&Play"));
	KillTimer(ID_TimerPlay);	
	m_IsPlay = 0;
	m_IsStopped = 1; 
	EnableButton();
}

//////////////////////////////////////////////////////////
void CEditSpikes::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
	case ID_TimerPlay:
		if (!m_IsEnd)
		{
//			m_ActualEdSp++;
			m_DrawOnlyOne=1;
			InvalidateRect(m_DrawingRect,FALSE);
		}
		else
		{
			KillTimer(ID_TimerPlay);	
			m_IsPlay = 0;
			m_IsStopped = 1; 
			SetDlgItemText(IDC_PLAY,TEXT("&Play"));
			EnableButton();
		}
		break;
	}
	cdxCSizingDialog::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////
void CEditSpikes::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CClientDC dc(this); // device context for painting

	// Changing window size - LEFT
	if (m_sizeLeft.PtInRect(point) && !m_IsPlay)
	{
		::SetCursor(m_HSize);
		m_MovingMode = 20;
		SetCapture();
		InvalidateRect(m_ClearRect);		
	}

	// Changing window size - RIGHT
	if (m_sizeRight.PtInRect(point) && !m_IsPlay)
	{
		::SetCursor(m_HSize);
		m_MovingMode = 21;
		SetCapture();
		InvalidateRect(m_ClearRect);		
	}

	if (m_DrawingRect.PtInRect(point))
	{
		::SetCursor(m_HCross);

		float NewPos = ((float)(point.x-m_DrawingRect.left)/m_DrawingRect.Width())*CVertex::SAMPLES/CVertex::FREQ;
		if (m_wndCmbSelParam.GetCurSel()==2)
		{
			float p1,p2;
			p1 = ((NewPos-m_Line1Pos)>=0) ? (NewPos-m_Line1Pos) : (NewPos-m_Line1Pos)*(-1);
			p2 = ((NewPos-m_Line2Pos)>=0) ? (NewPos-m_Line2Pos) : (NewPos-m_Line2Pos)*(-1);
			
			if ( p1 <= p2 )
				m_MovingMode = 1;
			else
				m_MovingMode = 2;
		}
		else m_MovingMode = 1;
		
	
		CRect iRect;
		CWnd::GetWindowRect(iRect);
		iRect.left += m_DrawingRect.left + 4;
		iRect.top += m_DrawingRect.top + 23;
		iRect.right = iRect.left + m_DrawingRect.Width(); 
		iRect.bottom = iRect.top + m_DrawingRect.Height();
		::ClipCursor(iRect);
		SetCapture();
		
		if (m_MovingMode == 1)
		{
			if (m_IsLine1 == 2)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
			}

			m_Line1Pos = NewPos;//((float)(point.x-m_DrawingRect.left)/m_DrawingRect.Width())*SAMPLES/FREQ;

			if (m_IsLine1)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
			}
		}

		if (m_MovingMode == 2)
		{
			if (m_IsLine2 == 2)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
			}

			m_Line2Pos = NewPos;//((float)(point.x-m_DrawingRect.left)/m_DrawingRect.Width())*NUM_SAMPLES/FREQ;

			if (m_IsLine1)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
			}
		}

	}

	cdxCSizingDialog::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////
void CEditSpikes::OnMouseMove(UINT nFlags, CPoint point) 
{
	CClientDC dc(this); // device context for painting

	if (m_MovingMode == 20)
	{
		::SetCursor(m_HSize);
		if (point.x >= m_ClearRect.left && (point.x - m_ClearRect.left) < (m_ClearRect.Width()-40)/2)
		{
			m_size = point.x - m_ClearRect.left;
		}
		
		InvalidateRect(m_ClearRect);		
	}
	if (m_MovingMode == 21)
	{
		::SetCursor(m_HSize);
		if (point.x <= m_ClearRect.right && (m_ClearRect.right - point.x) < (m_ClearRect.Width()-40)/2)
		{
			m_size = m_ClearRect.right - point.x;
		}
		
		InvalidateRect(m_ClearRect);		
	}

	if (m_MovingMode == 0 && (m_sizeLeft.PtInRect(point) || m_sizeRight.PtInRect(point)))
	{
		::SetCursor(m_HSize);
	}

	if (m_DrawingRect.PtInRect(point) && m_MovingMode != 20 && m_MovingMode != 21)
	{
		::SetCursor(m_HCross);

		if (m_MovingMode == 1)
		{
			if (m_IsLine1 == 2)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
			}

			m_Line1Pos = ((float)(point.x-m_DrawingRect.left)/m_DrawingRect.Width())*CVertex::SAMPLES/CVertex::FREQ;
	
			if (m_IsLine1)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
			}
		}

		if (m_MovingMode == 2)
		{
			if (m_IsLine2 == 2)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
			}

			m_Line2Pos = ((float)(point.x-m_DrawingRect.left)/m_DrawingRect.Width())*CVertex::SAMPLES/CVertex::FREQ;
	
			if (m_IsLine2)
			{
				dc.SetROP2(R2_NOT);
				dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
				dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
				dc.SetROP2(R2_COPYPEN);
			}
		}
	}
	cdxCSizingDialog::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////
void CEditSpikes::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CClientDC dc(this); // device context for painting

	if (m_MovingMode == 20 || m_MovingMode == 21)
	{
		::SetCursor(m_HSize);
		m_MovingMode = 0;
		ReleaseCapture();
		InvalidateRect(m_ClearRect);
	}
	
	if (m_DrawingRect.PtInRect(point) && m_MovingMode != 20 && m_MovingMode != 21)
	{
		::SetCursor(m_HCross);
		if (m_MovingMode)
		{
			::ClipCursor(NULL);
			ReleaseCapture();
			m_MovingMode = 0;
		}
	}
	
	cdxCSizingDialog::OnLButtonUp(nFlags, point);
}


//////////////////////////////////////////////////////////
void CEditSpikes::OnSelchangeComboprop() 
{

	CParamDim *ParDim;
	ParDim = (CParamDim*) *(m_MainClusters->m_ParamDimStack.begin() + m_wndComboProp.GetCurSel()*4);
	if (ParDim->GetType() == 2 || ParDim->GetType() == 3)
	{
		CClientDC dc(this); // device context for painting

		if (m_IsLine1 == 2)
		{
			dc.SetROP2(R2_NOT);
			dc.MoveTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
			dc.LineTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
			dc.SetROP2(R2_COPYPEN);
		}

		m_Line1Pos = (float) ParDim->GetParam(1);
	
		if (m_IsLine1)
		{
			dc.SetROP2(R2_NOT);
			dc.MoveTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
			dc.LineTo(m_DrawingRect.left + (m_Line1Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
			dc.SetROP2(R2_COPYPEN);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnCheckCh1() 
{
	if (!m_wndCheck1.GetCheck() && !m_wndCheck2.GetCheck() && !m_wndCheck3.GetCheck() && !m_wndCheck4.GetCheck())
	{
		m_wndCheck1.SetCheck(1);
	}
	InvalidateRect(m_DrawingRect,false);		
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnCheckCh2() 
{
	if (!m_wndCheck1.GetCheck() && !m_wndCheck2.GetCheck() && !m_wndCheck3.GetCheck() && !m_wndCheck4.GetCheck())
	{
		m_wndCheck2.SetCheck(1);
	}
	InvalidateRect(m_DrawingRect,false);		
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnCheckCh3() 
{
	if (!m_wndCheck1.GetCheck() && !m_wndCheck2.GetCheck() && !m_wndCheck3.GetCheck() && !m_wndCheck4.GetCheck())
	{
		m_wndCheck3.SetCheck(1);
	}
	InvalidateRect(m_DrawingRect,false);		
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnCheckCh4() 
{
	if (!m_wndCheck1.GetCheck() && !m_wndCheck2.GetCheck() && !m_wndCheck3.GetCheck() && !m_wndCheck4.GetCheck())
	{
		m_wndCheck4.SetCheck(1);
	}
	InvalidateRect(m_DrawingRect,false);		
}

pair<int,int> CEditSpikes::GetSelectedDimInfo()
{
	int isz = m_wndComboProp.GetCount() , iCurDim = 0 , i = 0 , iCurSel = m_wndComboProp.GetCurSel(), iDims = 0;
	CString str;
	for(;i<isz;i++)
	{	
		m_wndComboProp.GetLBText(i,str);
		if(str.Find("Wavelet")!=-1)
		{
			char cStr[1024];
			sprintf(cStr,"%s",str);
			vector<string> vstr;
			Split(cStr,string(" "),vstr);
			iDims = atoi(vstr[1].c_str());
		}
		else if(str.Find("HQPC")!=-1)
		{
			iDims = 8;
		}
		else
		{
			iDims = 4;
		}

		if( i >= iCurSel ) break;

		iCurDim += iDims;
	}	
	return pair<int,int>(iCurDim,iDims);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnRemove() 
{
	m_wndRemove.EnableWindow(FALSE);	//don't let multiple button presses @ once

	if(m_wndComboProp.GetCurSel()<0)	//make sure valid selection
	{	m_wndRemove.EnableWindow(TRUE);
		return;
	}

	//offset into m_paramDimStack - X 4 since 1 dim for each wire
	CString strParm;
	pair<int,int> DimInf = GetSelectedDimInfo();
	const int iNum = DimInf.first; // 4*m_wndComboProp.GetCurSel(); 
	int iDimRem = DimInf.second;

	MY_STACK::iterator indexPD;
	CCluster& oC = *m_MainClusters;
	int i=0;
	//readjust dimensions that are AFTER the one being removed so they 'point' to the right
	//dimension number in the data stack
	for(indexPD = oC.m_ParamDimStack.begin();indexPD!=oC.m_ParamDimStack.end();indexPD++,i++)
	{	if(i>=iNum+iDimRem)
		{	CParamDim* parDim = (CParamDim*) *indexPD;
			parDim->m_iVDim -= iDimRem;
		}
	}
	//remove the previously added dimensions from data stack
	for(i=0;i<iDimRem;i++)
	{	//remove vertex dimension + corresponding text axes
		MY_STACK::iterator indexPD = m_MainClusters->m_ParamDimStack.begin() + iNum;
		CParamDim* parDim = (CParamDim*) *indexPD;
		m_MainDataStack->RemoveADim(  parDim->m_iVDim );
	}

	//remove once for each wire
	for(i=0;i<iDimRem;i++)
		oC.RemParam(m_MainDataStack,iNum);
	
	// NEW
	//m_MainClusters->CalcParam(m_MainDataStack); // why recalc all params if removing one???????????
	m_MainDataStack->CalcMinMax();
	m_MainClusters->Clustering(m_MainDataStack);
	// end NEW

	m_MainClusters->ClearUNDO();
	FillCmbProp();	

	m_wndRemove.EnableWindow(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnSetSp() 
{
	CSetDial SetDial;

	SetDial.m_EdFirstEv = m_FirstEdSp;
	SetDial.m_EdLastEv = m_LastEdSp;
	SetDial.m_EdPrecent = m_PercEdSp;
	SetDial.m_Type = 1;
	SetDial.m_ClMode = m_MainClusters->m_ClMode;

	if (SetDial.DoModal()==IDOK)
	{
		m_FirstEdSp = SetDial.m_EdFirstEv;
		if (m_FirstEdSp>m_MainDataStack->GetNumVerx()-1)
			m_FirstEdSp=m_MainDataStack->GetNumVerx()-1;
		if (m_FirstEdSp<1)
			m_FirstEdSp=1;
		
		m_LastEdSp = SetDial.m_EdLastEv;
		if (m_LastEdSp>m_MainDataStack->GetNumVerx() || m_LastEdSp==0)
			m_LastEdSp=m_MainDataStack->GetNumVerx();
		if (m_LastEdSp<=m_FirstEdSp)
			m_LastEdSp=m_FirstEdSp+1;

		m_PercEdSp = SetDial.m_EdPrecent;
		if (m_PercEdSp<1)
			m_PercEdSp=1;
		if (m_PercEdSp>100)
			m_PercEdSp=100;

		if (SetDial.m_ClMode != m_MainClusters->m_ClMode)
		{ 
			m_MainClusters->m_ClMode = SetDial.m_ClMode;
			m_MainClusters->Clustering(m_MainDataStack);
		}
	}

	CalcWhatShow();

	InvalidateRect(CRect(m_DrawingRect.left+123,m_DrawingRect.bottom+48,m_DrawingRect.left+400,m_DrawingRect.bottom+63));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnAdd() 
{
	m_wndAdd.EnableWindow(FALSE);
	
	int Select = m_wndCmbSelParam.GetCurSel();
	if(Select < 0 || Select > 6)
	{
		m_wndAdd.EnableWindow(TRUE);
		return;
	}

	TopDimPopPush oT(m_MainDataStack); //pop off x,y,time so can add new dimensions before them
									   //CCluster::AddParam will add the dimension to m_MainDataStack

	if (Select == 0) // V(t)
	{	// 2, t, ch
		int i = 0;
		for(i=0;i<4;i++)
			m_MainClusters->AddParam(new CParamDim(2,(float) m_Line1Pos,(float) i),m_MainDataStack,true);	
		FillCmbProp();
	}
	else if (Select == 1) // dV(t)/dt
	{	// 3, t, ch
		int i = 0;
		for(i=0;i<4;i++)
			m_MainClusters->AddParam(new CParamDim(3,(float) m_Line1Pos,(float) i),m_MainDataStack,true);	
		FillCmbProp();
	}
	else if (Select == 2) // Energy
	{	// 4, t1, t2, ch
		float p1,p2;
		if (m_Line1Pos > m_Line2Pos)
		{
			p1 = m_Line2Pos;
			p2 = m_Line1Pos;
		}
		if (m_Line1Pos < m_Line2Pos)
		{
			p1 = m_Line1Pos;
			p2 = m_Line2Pos;
		}

		if (m_Line1Pos != m_Line2Pos)
		{	int i = 0;
			for(i=0;i<4;i++)
				m_MainClusters->AddParam(new CParamDim(4,(float) p1, (float) p2,(float) i),m_MainDataStack,true);	
			FillCmbProp();
		}
		else 
			CWnd::MessageBox(TEXT("Range is zero"),NULL,MB_ICONERROR|MB_OK);
	}
	else if (Select == 3) // External parameter from FILE
	{	// 5, param_no, ch, index_in_spike
		CParamDim *param;
		CDlgAddWCP addWcpDlg;
		addWcpDlg.m_MainClusters = m_MainClusters;
		addWcpDlg.m_MainDataStack = m_MainDataStack;
		if (addWcpDlg.DoModal() == IDOK)
		{
			MY_WCP_FILES_STACK::iterator indNewParam;
			for (indNewParam = addWcpDlg.newParamsStack.begin(); indNewParam != addWcpDlg.newParamsStack.end(); indNewParam++)
			{
				indNewParam->flag = m_MainClusters->usedExternWCP.size();
				m_MainClusters->usedExternWCP.push_back( *indNewParam );
				m_MainDataStack->PasteFileParameter( &(*indNewParam) );

				int i = 0;
				for(i=0;i<4;i++)
					m_MainClusters->AddParam(new CParamDim(5,(float) m_MainClusters->usedExternWCP.size() - 1 ,(float) i, (float) ((m_MainClusters->usedExternWCP.size() - 1) * 4 + i)),
											 m_MainDataStack,true);
			}			
			FillCmbProp();
		}
	}
	else if (Select == 4) // Haar Wavelet Coefficients
	{	CString str;
		m_wndExtraParm.GetWindowText(str);
		int iTot = 24 , iMaxC = 24;
		if(!str.GetLength() || (iTot=atoi(str)) < 1)
			MessageBox("You must enter # of wavelet coefficients to store and then press Add param.","WClust - error",MB_ICONERROR); 
		else if( iTot > (iMaxC=CeilPowOf2( ((CVertex*) *m_MainDataStack->m_VerxStack.begin())->VYSize())) )
		{	str.Format("# of wavelet coefficients must be <= %d",iMaxC);
			MessageBox(str,"WClust - error",MB_ICONINFORMATION); 
		}
		else
		{	int i = 0;
			for(i=0;i<iTot;i++)                     // W        Cat       Tot        ID
				m_MainClusters->AddParam(new CParamDim(6,(float) 1, (float) iTot,(float) i),m_MainDataStack,true);	
			FillCmbProp();
		}
	}
	else if (Select == 5) // upsampled peak aligned PC1 , energy for each wire
	{	int i = 0;
		for(i=0;i<8;i++)                     // H        Cat       Tot        ID
			m_MainClusters->AddParam(new CParamDim(7,(float) 0, (float) 8,(float) i),m_MainDataStack,true);	
		FillCmbProp();
	}
	m_wndAdd.EnableWindow(TRUE);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnSelchangeCmbSelParam() 
{
	CClientDC dc(this);
	
	int Selected = m_wndCmbSelParam.GetCurSel();

	if (Selected == 0)
	{
		if (m_IsLine2)
		{
			dc.SetROP2(R2_NOT);
			dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
			dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
			dc.SetROP2(R2_COPYPEN);
		}
		m_IsLine2 = 0;
	}
	else if (Selected == 1)
	{
		if (m_IsLine2)
		{
			dc.SetROP2(R2_NOT);
			dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
			dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
			dc.SetROP2(R2_COPYPEN);
		}
		m_IsLine2 = 0;
	}
	else if (Selected == 2)
	{
		if (!m_IsLine2)
		{
			dc.SetROP2(R2_NOT);
			dc.MoveTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.top+1);
			dc.LineTo(m_DrawingRect.left + (m_Line2Pos/((float)CVertex::SAMPLES/CVertex::FREQ))*m_DrawingRect.Width(),m_DrawingRect.bottom-1);
			dc.SetROP2(R2_COPYPEN);
		}
		m_IsLine2 = 2;
	}
	else if (Selected == 4)
	{
		m_wndPrompt.SetWindowText("Enter # W. coeffs>");
		m_wndExtraParm.EnableWindow(TRUE);
	}
	if(Selected != 4) { m_wndPrompt.SetWindowText(""); m_wndExtraParm.EnableWindow(FALSE); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnBclear() 
{
	m_RestoreAct = m_ActualEdSp;
	m_LoadAct = 1;
	m_ActualEdSp = 0;
	InvalidateRect(m_DrawingRect);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::OnExtParam() 
{
	// TODO: Add your control notification handler code here
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::FillImageList()
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEditSpikes::FillClList()
{
	CString Str;
	CListCtrl *list;
	
	list = & m_wndListClust;
	Str.Format("Spikes cl.");

	list->InsertColumn(0,Str,LVCFMT_LEFT,65);
	list->SetImageList(&m_imageList,LVSIL_SMALL);
	
	list->InsertItem(0,Str,0);
	Str.Format("noise");
	list->SetItemText(0,0,Str);
	list->SetCheck(0, (clView[255] && CLUST_SEL) ? true : false);
		
	list->InsertItem(1,Str,1);
	Str.Format("non cl");
	list->SetItemText(1,0,Str);
	list->SetCheck(1, (clView[0] && CLUST_SEL) ? true : false);
	
	for (int i = 1; i <= maxCluster ; i++)
	{
		list->InsertItem(i+1,Str,i+1);
		Str.Format("cl %d",i);
		list->SetItemText(i+1,0,Str);
		list->SetCheck(i+1,(clView[i] && CLUST_SEL) ? true : false );		
	}
	list->SetExtendedStyle( LVS_EX_CHECKBOXES  );
}

/*void CEditSpikes::OnBnClickedButtonSelectall()
{
			CListCtrl *list;
	
			list = & m_wndListClust;
			list->SetCheck(0, true);
			list->SetCheck(1, true);

			for (int i = 1; i <= maxCluster ; i++)
				{	
					list->SetCheck(i+1, true);		
				}
	
	// TODO: Add your control notification handler code here
}

void CEditSpikes::OnBnClickedButtonUnselectall()
{
  
	CListCtrl *list;
	
	list = & m_wndListClust;
	list->SetCheck(0, false);
	list->SetCheck(1, false);

	for (int i = 1; i <= maxCluster ; i++)
	{
		list->SetCheck(i+1, false);		
	}

	// TODO: Add your control notification handler code here
}
*/
void CEditSpikes::OnItemchangedListClust(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	CalcWhatShow();
	
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CEditSpikes::CalcWhatShow()
{
	unsigned char Selected;
	for (unsigned char i = 0; i <= maxCluster + 1; i++ )
	{
		if ( i == 0 )
			Selected = 255;
		else
			Selected = i - 1;
		if ( m_wndListClust.GetCheck(i) )
			clView[ Selected ] |= CLUST_SEL;
		else
			clView[ Selected ] &= (0xFF - CLUST_SEL);
	}

	m_MainDataStack->CalcViewSpike(m_FirstEdSp, m_LastEdSp, m_PercEdSp, &clView[0], CLUST_SEL);
}
