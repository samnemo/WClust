// WClustView.cpp : implementation of the CWClustView class
//

#include "stdafx.h"
#include "WClust.h"

#include "WClustDoc.h"
#include "WClustView.h"
#include "EditDlg.h"
#include "MyObj.h"
#include "SetDial.h"
#include "Sync.h"
#include "TimeEv.h"
#include "DlgPositionReg.h"
#include "DlgWcpNotFound.h"
#include "DlgFileInfo.h"
#include "Hist.h"
#include <math.h>
#include <process.h>
#include <memory>
#include <float.h>
#include ".\wclustview.h"
#include "APClustDialog.h"
#include "ShowXYSpikeDialog.h"
#include "KMDialog.h"
#include "EClustDlg.h"
#include "UPDialog.h"
#include "RateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDT_AUTO_STORE_CL 100
/////////////////////////////////////////////////////////////////////////////
// CWClustView

IMPLEMENT_DYNCREATE(CWClustView, CFormView)

BEGIN_MESSAGE_MAP(CWClustView, CFormView)
	//{{AFX_MSG_MAP(CWClustView)
	ON_COMMAND(ID_FILE_OPENBPFILE, OnFileOpenbpfile)
	ON_COMMAND(ID_EDITDLG, OnEditdlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDC_NEWCLUST, OnNewclust)
	ON_COMMAND(ID_FILE_SAVECLUSTERS, OnFileSaveclusters)
	ON_COMMAND(ID_FILE_LOADCLUSTERS, OnFileLoadclusters)
	ON_BN_CLICKED(IDC_BUT_SET, OnButSet)
	ON_COMMAND(ID_FILE_STORETOBPFILE, OnFileStoretobpfile)
	ON_UPDATE_COMMAND_UI(ID_FILE_STORETOBPFILE, OnUpdateFileStoretobpfile)
	ON_UPDATE_COMMAND_UI(ID_MODE_SWAPPOLARITY,OnUpdateModeSwappolarity)
	ON_COMMAND(ID_ENVIRONMENT_LOAD, OnEnvironmentLoad)
	ON_COMMAND(ID_ENVIRONMENT_SAVE, OnEnvironmentSave)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_CBN_SELCHANGE(IDC_COMBOCLUST, OnSelchangeComboclust)
	ON_BN_CLICKED(IDC_HIDE, OnHide)
	ON_BN_CLICKED(IDC_SHOW, OnShow)
	ON_COMMAND(ID_LOAD_ARENA, OnLoadArena)
	ON_COMMAND(ID_LOAD_ROOM, OnLoadRoom)
	ON_UPDATE_COMMAND_UI(ID_LOAD_ARENA, OnUpdateLoadArena)
	ON_UPDATE_COMMAND_UI(ID_LOAD_ROOM, OnUpdateLoadRoom)
	ON_COMMAND(ID_MODE_SWAPPOLARITY, OnModeSwappolarity)
	ON_COMMAND(ID_POS_REG_WIN, OnPosRegWin)
	ON_UPDATE_COMMAND_UI(ID_POS_REG_WIN, OnUpdatePosRegWin)
	ON_COMMAND(ID_TIME_EV_WIN, OnTimeEvWin)
	ON_UPDATE_COMMAND_UI(ID_TIME_EV_WIN, OnUpdateTimeEvWin)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_CMB_ORIG_NEW1, OnSelchangeCmbOrigNew1)
	ON_COMMAND(ID_TOOLS_INFO, OnToolsInfo)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_INFO, OnUpdateToolsInfo)
	ON_COMMAND(ID_TOOLS_EXPORT, OnToolsExport)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EXPORT, OnUpdateToolsExport)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_COMMAND(ID_CLUSTER_INFOCLUST, OnClusterInfoclust)
	ON_COMMAND(ID_CLUSTER_K, OnClusterK)
	ON_COMMAND(ID_CLUSTER_RESISTORINFOCLUST, OnClusterResistorinfoclust)
	ON_COMMAND(ID_CLUSTER_APCLUST, OnClusterApclust)
	ON_COMMAND(ID_CLUSTER_KLUSTAKWIKCLUST, OnClusterKlustakwikclust)
	ON_COMMAND(ID_CLUSTER_USERSPIKEX, OnClusterUserspikex)
	ON_COMMAND(ID_CLUSTER_USERSPIKEXY, OnClusterUserspikexy)
	ON_UPDATE_COMMAND_UI(ID_CLUSTER_USE_SPIKEXY, OnUpdateClusterUseSpikexy)
	ON_COMMAND(ID_CLUSTER_USE_SPIKEXY, OnClusterUseSpikexy)
	ON_COMMAND(ID_CLUSTER_ENTROPYINFOCLUST, OnClusterEntropyinfoclust)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWClustView construction/destruction

CWClustView::CWClustView()
	: CFormView(CWClustView::IDD),
	  m_pRDlg(0)
{
	//{{AFX_DATA_INIT(CWClustView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_Redraw=1;
	
	m_MiniView[0].m_Focus=1;

	m_MiniView[0].m_AxesX=1;
	m_MiniView[0].m_AxesY=2;
	m_MiniView[1].m_AxesX=1;
	m_MiniView[1].m_AxesY=3;
	m_MiniView[2].m_AxesX=1;
	m_MiniView[2].m_AxesY=4;
	m_MiniView[3].m_AxesX=2;
	m_MiniView[3].m_AxesY=3;
	m_MiniView[4].m_AxesX=2;
	m_MiniView[4].m_AxesY=4;
	m_MiniView[5].m_AxesX=3;
	m_MiniView[5].m_AxesY=4;

	MIN_WIN_X = 200;
	MIN_WIN_Y = 200;
	m_FirstEv=1;
	m_LastEv=2;
	m_PercEv=100;
	m_SpikeEdSize = 0;
	
/*	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_wndComboClust.SetPalette(&pDoc->m_MainPalette);*/
}

CWClustView::~CWClustView()
{
	if(m_pRDlg) delete m_pRDlg;
}

void CWClustView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWClustView)
	DDX_Control(pDX, IDC_CMB_ORIG_NEW1, m_wndCmbOrigNew);
	DDX_Control(pDX, IDC_COMBOCLUST, m_wndComboClust);
	DDX_Control(pDX, IDC_SHOW, m_wndShow);
	DDX_Control(pDX, IDC_HIDE, m_wndHide);
	//}}AFX_DATA_MAP
}

BOOL CWClustView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CWClustView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}

/////////////////////////////////////////////////////////////////////////////
// CWClustView printing

BOOL CWClustView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWClustView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWClustView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CWClustView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CWClustView diagnostics

#ifdef _DEBUG
void CWClustView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWClustView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CWClustDoc* CWClustView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWClustDoc)));
	return (CWClustDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWClustView message handlers
void CWClustView::OnFileOpenbpfile() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	int index;
	long errOffset;
	char errChar;

	m_Redraw = 0;

	int ErrorNumb=pDoc->m_MainDataStack.LoadVerx( &errChar, &errOffset);
	m_Redraw = 1;
	
	switch (ErrorNumb)
	{
	case 0:
		if (!pDoc->m_MainDataStack.AreData)
		{
			char pomT[80];
			sprintf(pomT,"No data loaded - single el. and stereode is not implemented yet.");
			CWnd::MessageBox(pomT,NULL,MB_ICONWARNING||MB_OK);
		}
		else
		{
			if ( pDoc->m_MainDataStack.GetFakeSpikes() )
				CWnd::MessageBox(TEXT("Warning: File does not contain electrode data.\n2 virtual spikes were added to allow working with the file."),NULL,MB_OK|MB_ICONWARNING);
			pDoc->m_MainDataStack.CheckWcpFiles();
			CDlgWcpNotFound dlg;
			dlg.m_MainClusters = &pDoc->m_MainClusters;
			dlg.m_MainDataStack = &pDoc->m_MainDataStack;
			bool result = dlg.CheckWcp();

			if ( result == false )
			{
				dlg.DoModal();
				pDoc->m_MainDataStack.CheckWcpFiles();
				bool result = dlg.CheckWcp();
			}

			MY_WCP_FILES_STACK::iterator indWcp;
			for ( indWcp = pDoc->m_MainClusters.usedExternWCP.begin(); indWcp != pDoc->m_MainClusters.usedExternWCP.end(); indWcp++ )
			{
				if ( (*indWcp).fileName.Compare( "-" ) == 0)
				{	// parameter is not found
					pDoc->m_MainDataStack.PasteFileParamAsZeros();
				}
				else 
				{	// parameter is found
					pDoc->m_MainDataStack.PasteFileParameter( &(*indWcp) );
				}
			}

			pDoc->m_MainClusters.RemParamZeros(&pDoc->m_MainDataStack);

//			pDoc->m_MainClusters.CalcParam(&pDoc->m_MainDataStack);
//			pDoc->m_MainDataStack.CalcMinMax();
//			pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);
			
			
			
			m_FirstEv = 1;
			m_LastEv = pDoc->m_MainDataStack.GetNumVerx();
			m_PercEv = (int) 100*2000/m_LastEv;
			if (m_PercEv>100)
				m_PercEv=100;
			if (m_PercEv==0)
				m_PercEv=1;

			m_FirstEdEv = m_FirstEv;
			m_LastEdEv = m_LastEv;
			m_PercEdEv = m_PercEv;
		
			m_FirstSp = m_FirstEv;
			m_LastSp = m_LastEv;
			m_PercSp = (int) 100*400/m_LastEv;
			if (m_PercSp == 0)
				m_PercSp = 1;
			if (m_PercSp>100)
				m_PercSp=100;

			m_FirstEdSp = m_FirstEv;
			m_LastEdSp = m_LastEv;
			m_PercEdSp = m_PercEv;
		
			pDoc->m_MainDataStack.CalcViewVx(m_FirstEv,m_LastEv,m_PercEv);

			if (0 && pDoc->m_MainClusters.GetSwap())
			{	//swap turned off!!!
				//bad - hard coded 8
				//CVertex::Swap caused crash when reduced # of params from 17 to 12
				//couldn't find bug for hours :( :( :(
				//
				pDoc->m_MainClusters.ClearSwap();
				pDoc->m_MainClusters.SwapPolarity(&pDoc->m_MainDataStack);
			}
			else{
				pDoc->m_MainClusters.CalcParam(&pDoc->m_MainDataStack);
				pDoc->m_MainDataStack.CheckNoise();
				pDoc->m_MainDataStack.CalcMinMax();
				pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);
			}

			for (index=0; index<6; index++)
			{
				if ( m_MiniView[index].m_AxesX > pDoc->m_MainDataStack.Dimension )
					m_MiniView[index].m_AxesX = pDoc->m_MainDataStack.Dimension;
	
				if ( m_MiniView[index].m_AxesY > pDoc->m_MainDataStack.Dimension )
					m_MiniView[index].m_AxesY = pDoc->m_MainDataStack.Dimension;

				if ( m_MiniView[index].m_AxesX == m_MiniView[index].m_AxesY)
				{
					if (m_MiniView[index].m_AxesX>1)
						m_MiniView[index].m_AxesX -=1;
					else
						m_MiniView[index].m_AxesX +=1;
				}
			}
		}
		m_wndCmbOrigNew.SetCurSel(pDoc->m_MainDataStack.whichDraw);

		break;
	case 1:
		CWnd::MessageBox(TEXT("Error: Out of memory"),NULL,MB_OK|MB_ICONERROR);
		break;
	case 2:
		char errText[200];
		sprintf(errText,"Error: Open brain potential file\nType: %c    File offset: %d (0x%x)",errChar,errOffset,errOffset);
		CWnd::MessageBox(errText,NULL,MB_OK|MB_ICONERROR);
		break;
	}
	Invalidate();
// TODO	
//	pDoc->m_MainDataStack.m_MainSyncStack.Save();

//	char aaa[20];
//	sprintf(aaa,"%d",pDoc->m_MainDataStack.bpf.EEG_TYPE);
//	CWnd::MessageBox(aaa);
}

void CWClustView::OnDraw(CDC* pDC) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);


	const int FontSizeX = 14;
	const int FontSizeY =4;

/////
	int storePos = m_wndComboClust.GetCurSel();

	CBoundary *Bndr;
	MY_STACK::iterator index;
	index=(pDoc->m_MainClusters.m_BndrStack.begin());
	char pomt[20];
	m_wndComboClust.ResetContent();
	m_wndComboClust.SetPalette(&pDoc->m_MainPalette);
	m_wndComboClust.AddString("Noise");
	m_wndComboClust.AddString("Non clustered");
	
	m_wndCmbOrigNew.ResetContent();
	m_wndCmbOrigNew.AddString(TEXT("Your new cl."));
	m_wndCmbOrigNew.AddString(TEXT("Original cl."));
	m_wndCmbOrigNew.AddString(TEXT("Kmeans cl."));
	m_wndCmbOrigNew.AddString(TEXT("Info cl."));
	m_wndCmbOrigNew.AddString(TEXT("AP cl."));
	m_wndCmbOrigNew.AddString(TEXT("KK cl."));
	m_wndCmbOrigNew.SetCurSel(pDoc->m_MainDataStack.whichDraw);

	int ik=0,ii=0,iw=pDoc->m_MainDataStack.whichDraw;
	switch(iw)
	{
	case CLUST_KM:
	case CLUST_AP:
	case CLUST_INFO:
	case CLUST_KK:
		for(ii=1;ii<=pDoc->m_MainClusters.m_pNumClusts[iw];ii++)
		{
			sprintf(pomt,"cluster %d",ii);
			m_wndComboClust.AddString(pomt);
		}
		if (1 || pDoc->m_MainClusters.m_pNumClusts[iw]>0)
		{
			if (storePos < 0)
				storePos = 1;
			if (storePos<m_wndComboClust.GetCount())
				m_wndComboClust.SetCurSel(storePos);
			else 
			{
				m_wndComboClust.SetCurSel(m_wndComboClust.GetCount());
				storePos = m_wndComboClust.GetCount();
			}
			
			if (storePos == 0)
				storePos = 255;
			else 
				storePos -= 1;
		}
		break;
	case CLUST_ORIG:
		for(ii=1;ii<=pDoc->m_MainDataStack.m_NumOriginalCl;ii++)
		{
			sprintf(pomt,"cluster %d",ii);
			m_wndComboClust.AddString(pomt);
		}
		if(pDoc->m_MainDataStack.m_NumOriginalCl>0)
		{
			if (storePos < 0)
				storePos = 1;
			if (storePos<m_wndComboClust.GetCount())
				m_wndComboClust.SetCurSel(storePos);
			else 
			{
				m_wndComboClust.SetCurSel(m_wndComboClust.GetCount());
				storePos = m_wndComboClust.GetCount();
			}
			
			if (storePos == 0)
				storePos = 255;
			else 
				storePos -= 1;
		}
		break;
	case CLUST_USER:
	default:
		while ( index!=pDoc->m_MainClusters.m_BndrStack.end() )
		{
			Bndr=(CBoundary*) *index;
			if (Bndr->GetNumb()!=0)
			{
				sprintf(pomt,"cluster %d",Bndr->GetNumb());
				m_wndComboClust.AddString(pomt);
			}
			index++;
		}
		if (pDoc->m_MainClusters.m_Count>0)
		{
			if (storePos < 0)
				storePos = 1;
			if (storePos<m_wndComboClust.GetCount())
				m_wndComboClust.SetCurSel(storePos);
			else 
			{
				m_wndComboClust.SetCurSel(m_wndComboClust.GetCount());
				storePos = m_wndComboClust.GetCount();
			}
			
			if (storePos == 0)
				storePos = 255;
			else 
				storePos -= 1;
		}
		break;
	}
	if (storePos>=0)
	{
		int CanShow=0,CanHide=0;
		for (int i1=0; i1<6;i1++)
		{
			if (m_MiniView[i1].m_ClustToView[storePos])
				CanHide=1;
			else CanShow=1;
		}
		if (CanHide)
			m_wndHide.EnableWindow(true);
		else m_wndHide.EnableWindow(false);
		m_wndHide.Invalidate();

		if (CanShow)
			m_wndShow.EnableWindow(true);
		else m_wndShow.EnableWindow(false);
		m_wndShow.Invalidate();
	}
/////

	//CString pomT("WClust - ");
	if (pDoc->m_MainDataStack.AreData)
	{
		char strTitle[1024];
		sprintf(strTitle,"WClust - Tetrode%d of %s",pDoc->m_MainDataStack.m_ElectrodeChannel,pDoc->m_MainDataStack.m_FileName);
		CWnd::GetOwner()->SetWindowText(strTitle);
	}
	else
		CWnd::GetOwner()->SetWindowText("WClust - ");
	//CWnd::GetOwner()->SetWindowText(pomT);
	
//	HICON pom1 = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	CWnd::SetIcon(pom1,true);

	CRect rect;
	GetClientRect(rect);
	pDC->SelectStockObject(GRAY_BRUSH);

	// First & last event writing
	CFont newFontEv;
	newFontEv.CreateFont(FontSizeX,FontSizeY,0,0,0,FALSE,
			 			0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						DEFAULT_PITCH,
						"Times New Roman");
	CFont *pOldFontEv = pDC->SelectObject(&newFontEv); 
	char pom[30];
	sprintf(pom,"First event: %d",m_FirstEv);
	pDC->TextOut(10,6,pom);		
	sprintf(pom,"Last event: %d (%d)",m_LastEv,pDoc->m_MainDataStack.GetNumVerx());
	pDC->TextOut(10,24,pom);		
	sprintf(pom,"%% event: %d",m_PercEv);
	pDC->TextOut(10,44,pom);		
	
	pDC->SelectObject(pOldFontEv);

	if ((rect.bottom>MIN_WIN_Y)&&(rect.right>MIN_WIN_X))
	{
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(rect.right,rect.bottom);
		pDC->SetViewportExt(rect.right,-rect.bottom);
		pDC->SetViewportOrg(0,rect.bottom);

		CFloatRect flRect(-10.0,10.0,10.0,-10.0);
		rect.OffsetRect(0,-rect.bottom);
		rect.bottom=-rect.bottom;
		rect.top=-rect.top;
		m_ViewRect=rect;

		pDC->SelectStockObject(WHITE_BRUSH);
		rect.top-=80;		
		rect.left+=10;

		int j;
		for (int i=0;i<3;i++)
		{
			for (j=0;j<2;j++)
			{	m_MiniView[i*2+j].m_WindowArea.top
					= rect.top-j*(rect.top-rect.bottom)/2;
				m_MiniView[i*2+j].m_WindowArea.bottom
					= rect.top-(j+1)*(rect.top-rect.bottom)/2+10;
				m_MiniView[i*2+j].m_WindowArea.left
					= rect.left+(i)*(rect.right-rect.left)/3;
				m_MiniView[i*2+j].m_WindowArea.right
					= rect.left+(i+1)*(rect.right-rect.left)/3-10;
				pDC->Rectangle(m_MiniView[i*2+j].m_WindowArea);
				pDC->SetROP2(R2_NOT);
				if (m_MiniView[i*2+j].m_Focus)
				{
					pDC->MoveTo(m_MiniView[i*2+j].m_WindowArea.left-1,m_MiniView[i*2+j].m_WindowArea.bottom);
					pDC->LineTo(m_MiniView[i*2+j].m_WindowArea.right,m_MiniView[i*2+j].m_WindowArea.bottom);
					pDC->LineTo(m_MiniView[i*2+j].m_WindowArea.right,m_MiniView[i*2+j].m_WindowArea.top+1);
					pDC->LineTo(m_MiniView[i*2+j].m_WindowArea.left-1,m_MiniView[i*2+j].m_WindowArea.top+1);
					pDC->LineTo(m_MiniView[i*2+j].m_WindowArea.left-1,m_MiniView[i*2+j].m_WindowArea.bottom);
				}
				pDC->SetROP2(R2_COPYPEN);

				// Setting window for drawing in each window
				m_MiniView[i*2+j].m_DrawingArea=m_MiniView[i*2+j].m_WindowArea;
				m_MiniView[i*2+j].m_DrawingArea.left += FontSizeX+2;
				m_MiniView[i*2+j].m_DrawingArea.right -= 1;
				m_MiniView[i*2+j].m_DrawingArea.top -=1;
				m_MiniView[i*2+j].m_DrawingArea.bottom += FontSizeX+2;

				// Main drawing of vectors and clusters
				if (m_Redraw)
				{
					switch(pDoc->m_MainDataStack.whichDraw)
					{
					case CLUST_USER:
						pDoc->m_MainClusters.Draw(pDC,m_MiniView[i*2+j].m_DrawingArea,&m_MiniView[i*2+j].m_DataArea,CPoint(m_MiniView[i*2+j].m_AxesX,m_MiniView[i*2+j].m_AxesY),m_MiniView[i*2+j].m_ClustToView,0);
						pDoc->m_MainDataStack.Draw(pDC,m_MiniView[i*2+j].m_DrawingArea,&m_MiniView[i*2+j].m_DataArea,CPoint(m_MiniView[i*2+j].m_AxesX,m_MiniView[i*2+j].m_AxesY),m_MiniView[i*2+j].m_ClustToView,m_MiniView[i*2+j].m_PointsSize);
						break;
					case CLUST_ORIG:
						pDoc->m_MainDataStack.DrawOrig(pDC,m_MiniView[i*2+j].m_DrawingArea,&m_MiniView[i*2+j].m_DataArea,CPoint(m_MiniView[i*2+j].m_AxesX,m_MiniView[i*2+j].m_AxesY),m_MiniView[i*2+j].m_ClustToView,m_MiniView[i*2+j].m_PointsSize);
						break;
					case CLUST_KM:
					case CLUST_INFO:
					case CLUST_AP:
					case CLUST_KK:
					default:
						pDoc->m_MainDataStack.DrawAutoC(pDC,m_MiniView[i*2+j].m_DrawingArea,&m_MiniView[i*2+j].m_DataArea,CPoint(m_MiniView[i*2+j].m_AxesX,m_MiniView[i*2+j].m_AxesY),m_MiniView[i*2+j].m_ClustToView,m_MiniView[i*2+j].m_PointsSize,pDoc->m_MainDataStack.whichDraw);
						break;
					}
				}

				if (!pDoc->m_MainDataStack.IsEmpty())
				{
					int oldBkColor = pDC->SetBkColor(RGB(255,255,255));
					pDC->SelectStockObject(BLACK_BRUSH);
					char AxValue[8];

					// Text of Axes X
					CFont newFontX;
					CString* AxXName = pDoc->m_MainDataStack.GetAxesName(m_MiniView[i*2+j].m_AxesX);
					newFontX.CreateFont(FontSizeX,FontSizeY,00,0,0,FALSE,
							0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
							DEFAULT_PITCH,
							"Times New Roman");
					CFont *pOldFontX = pDC->SelectObject(&newFontX);
					pDC->TextOut(m_MiniView[i*2+j].m_DrawingArea.left+m_MiniView[i*2+j].m_DrawingArea.Width()/2-AxXName->GetLength()*FontSizeY,m_MiniView[i*2+j].m_WindowArea.bottom+FontSizeX+1,*AxXName);
					// Min X
					sprintf(AxValue,"%2.2f",m_MiniView[i*2+j].m_DataArea.left);
					pDC->TextOut(m_MiniView[i*2+j].m_DrawingArea.left+1,m_MiniView[i*2+j].m_WindowArea.bottom+FontSizeX+1,AxValue);
					// Max X
					sprintf(AxValue,"%2.2f",m_MiniView[i*2+j].m_DataArea.right);
					pDC->TextOut(m_MiniView[i*2+j].m_DrawingArea.right-FontSizeY*8,m_MiniView[i*2+j].m_WindowArea.bottom+FontSizeX+1,AxValue);
					pDC->SelectObject(pOldFontX);																				

					// Text of Axes Y
					CFont newFontY;
					CString* AxYName = pDoc->m_MainDataStack.GetAxesName(m_MiniView[i*2+j].m_AxesY);
					newFontY.CreateFont(FontSizeX,FontSizeY,-900,0,0,FALSE,
							0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
							DEFAULT_PITCH,
							"Times New Roman");
					CFont *pOldFontY = pDC->SelectObject(&newFontY); 
					pDC->TextOut(m_MiniView[i*2+j].m_WindowArea.left+1,m_MiniView[i*2+j].m_DrawingArea.bottom-m_MiniView[2*i+j].m_DrawingArea.Height()/2-AxYName->GetLength()*(FontSizeY-1),*AxYName);
					// Min Y
					sprintf(AxValue,"%2.2f",m_MiniView[i*2+j].m_DataArea.bottom);
					pDC->TextOut(m_MiniView[i*2+j].m_WindowArea.left+1,m_MiniView[i*2+j].m_DrawingArea.bottom-1,AxValue);
					// Max Y
					sprintf(AxValue,"%2.2f",m_MiniView[i*2+j].m_DataArea.top);
					pDC->TextOut(m_MiniView[i*2+j].m_WindowArea.left+1,m_MiniView[i*2+j].m_DrawingArea.top-8*(FontSizeY-1),AxValue);
					pDC->SelectObject(pOldFontY);

					// Line of axes X	
					pDC->MoveTo(m_MiniView[i*2+j].m_DrawingArea.left-4,m_MiniView[i*2+j].m_DrawingArea.bottom-1);
					pDC->LineTo(m_MiniView[i*2+j].m_DrawingArea.right,m_MiniView[i*2+j].m_DrawingArea.bottom-1);
					// Line of Axes Y
					pDC->MoveTo(m_MiniView[i*2+j].m_DrawingArea.left-1,m_MiniView[i*2+j].m_DrawingArea.bottom-4);
					pDC->LineTo(m_MiniView[i*2+j].m_DrawingArea.left-1,m_MiniView[i*2+j].m_DrawingArea.top);
					
					pDC->SelectStockObject(WHITE_BRUSH);
					pDC->SetBkColor(oldBkColor);
				}
			}
		}	
	} //Drawing
////////////////////
/*
	if (pDoc->m_MainDataStack.AreData)
	{
		char aa[160];
		sprintf(aa,"%d\t %d\t %d\t %d\t%d\t",
			((sSpike*)*(pDoc->m_MainDataStack.m_SpikesStack.begin()+0))->offset,
			((sSpike*)*(pDoc->m_MainDataStack.m_SpikesStack.begin()+1))->offset,
			((sSpike*)*(pDoc->m_MainDataStack.m_SpikesStack.begin()+2))->offset,
			((sSpike*)*(pDoc->m_MainDataStack.m_SpikesStack.begin()+3))->offset,
			sizeof(sSpike)
			);
		CWnd::MessageBox(aa);
	}
*/
}

//allocate rating dialog if needed
void CWClustView::CheckRateDlg()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(!m_pRDlg)
		m_pRDlg = new RateDlg(this,&pDoc->m_MainDataStack,&pDoc->m_MainPalette,&pDoc->m_MainClusters);
}

void CWClustView::CallEditDlg()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc->m_MainDataStack.IsEmpty())
	{
		CEditDlg EditDlg;

		//make sure rating dialog pointer is allocated
		CheckRateDlg();

		EditDlg.m_pRDlg = m_pRDlg;

		EditDlg.m_MainClusters=&pDoc->m_MainClusters;
		EditDlg.m_MainDataStack=&pDoc->m_MainDataStack;
		EditDlg.m_MainPalette=&pDoc->m_MainPalette;
		int whichView;
		for (int i=0;i<6;i++)
			if (m_MiniView[i].m_Focus)
				whichView=i;
		EditDlg.m_MyView=&m_MiniView[whichView];
	
		m_Redraw=0;
		
		// view parameters
		EditDlg.m_FirstEdEv = m_FirstEdEv;
		EditDlg.m_LastEdEv = m_LastEdEv;
		EditDlg.m_PercEdEv = m_PercEdEv;

		EditDlg.m_FirstSp = m_FirstSp;
		EditDlg.m_LastSp = m_LastSp;
		EditDlg.m_PercSp = m_PercSp;

		EditDlg.m_FirstEdSp = m_FirstEdSp;
		EditDlg.m_LastEdSp = m_LastEdSp;
		EditDlg.m_PercEdSp = m_PercEdSp;
		EditDlg.m_SpikeEdSize = m_SpikeEdSize;

		// Run edit window
		if (EditDlg.DoModal()==IDOK)
		{
		}
		
		m_FirstEdEv = EditDlg.m_FirstEdEv;
		m_LastEdEv = EditDlg.m_LastEdEv;
		m_PercEdEv = EditDlg.m_PercEdEv;

		m_FirstSp = EditDlg.m_FirstSp;
		m_LastSp = EditDlg.m_LastSp;
		m_PercSp = EditDlg.m_PercSp;

		m_FirstEdSp = EditDlg.m_FirstEdSp;
		m_LastEdSp = EditDlg.m_LastEdSp;
		m_PercEdSp = EditDlg.m_PercEdSp;
		m_SpikeEdSize = EditDlg.m_SpikeEdSize;
		
		pDoc->m_MainDataStack.CalcViewVx(m_FirstEv,m_LastEv,m_PercEv);
		m_MiniView[whichView].DoSize();

		for (int j=0;j<6;j++)
		{
			if (m_MiniView[j].m_AxesY > pDoc->m_MainDataStack.Dimension)
				m_MiniView[j].m_AxesY = pDoc->m_MainDataStack.Dimension;
			if (m_MiniView[j].m_AxesX > pDoc->m_MainDataStack.Dimension)
				m_MiniView[j].m_AxesX = pDoc->m_MainDataStack.Dimension;
			if (m_MiniView[j].m_AxesX == m_MiniView[j].m_AxesY)
				m_MiniView[j].m_AxesX = m_MiniView[j].m_AxesY - 1;
		}

		m_Redraw=1; // Points can be visibile
		Invalidate();
	}
	else MessageBeep(0);
}

void CWClustView::OnEditdlg() 
{
	CallEditDlg();
}

void CWClustView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint m_point;
	m_point.x=point.x;
	m_point.y=m_ViewRect.top-point.y;
	CRect rect,clRect;
	rect=m_ViewRect;
	rect.top=m_ViewRect.bottom;
	rect.bottom=m_ViewRect.top;
	
	GetClientRect(clRect);

	int OldFocus=0;
	int IsFocus=0;
	int NewFocus=0;
	CClientDC dc(this);

	if (clRect.bottom>MIN_WIN_Y&&clRect.right>MIN_WIN_X)
	{
		for (int i=0;i<6;i++)
		{
			rect=m_MiniView[i].m_WindowArea;
			if (m_point.x>rect.left&&m_point.x<rect.right&&m_point.y<rect.top&&m_point.y>rect.bottom)
			{
				if (m_MiniView[i].m_Focus==0)
				{
					IsFocus=1;
					NewFocus=i+1;
					m_MiniView[i].m_Focus=1;
				}
			}
			else
			{
				if (m_MiniView[i].m_Focus==1)
				{
					m_MiniView[i].m_Focus=0;
					OldFocus=i+1;
				}
			}
		}
		if (!IsFocus)
			m_MiniView[OldFocus-1].m_Focus=1;
		if (IsFocus&&(OldFocus!=NewFocus))
		{
			dc.SetMapMode(MM_ANISOTROPIC);
			dc.SetWindowExt(clRect.right,clRect.bottom);
			dc.SetViewportExt(clRect.right,-clRect.bottom);
			dc.SetViewportOrg(0,clRect.bottom);
			
			dc.SetROP2(R2_NOT);
			i=OldFocus-1;
			dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
			dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
			dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
			dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
			i=NewFocus-1;
			dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
			dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
			dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
			dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
			dc.SetROP2(R2_COPYPEN);
		}
	}
	CFormView::OnLButtonDown(nFlags, point);
}



void CWClustView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPoint m_point;
	CRect rect;

	m_point.x=point.x;
	m_point.y=point.y;
	m_point.y=m_ViewRect.top-point.y;
	rect=m_ViewRect;
	rect.top=m_ViewRect.bottom;
	rect.bottom=m_ViewRect.top;

	int OldFocus=0;
	int IsFocus=0;

	CRect clRect;
	GetClientRect(clRect);
	if (clRect.bottom>MIN_WIN_Y&&clRect.right>MIN_WIN_X)
	{
		for (int i=0;i<6;i++)
		{
			rect=m_MiniView[i].m_WindowArea;
	
			if (m_point.x>rect.left&&m_point.x<rect.right&&m_point.y<rect.top&&m_point.y>rect.bottom)
			{
				IsFocus=i+1;
				m_MiniView[i].m_Focus=1;
			}
			else
			{
				if (m_MiniView[i].m_Focus==1)
					OldFocus=i;
				m_MiniView[i].m_Focus=0;
			}
		}

		if (IsFocus)
			CallEditDlg();
		else
			m_MiniView[OldFocus].m_Focus=1;
	}
	CFormView::OnLButtonDblClk(nFlags, point);
}

void CWClustView::OnNewclust() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_MainClusters.SetEmpty();
	CBoundary *Bnd;
	Bnd=new CBoundary(&pDoc->m_MainPalette); 
	pDoc->m_MainClusters.AddBndry(Bnd);
	pDoc->m_MainDataStack.ClearExternalParamData();
	
	pDoc->m_MainClusters.CalcParam(&pDoc->m_MainDataStack);
	pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);

	// Axis Adaptation
	for (int index=0; index<6; index++)
	{
		if ( m_MiniView[index].m_AxesX > pDoc->m_MainDataStack.Dimension )
			m_MiniView[index].m_AxesX = pDoc->m_MainDataStack.Dimension;
		if ( m_MiniView[index].m_AxesY > pDoc->m_MainDataStack.Dimension )
			m_MiniView[index].m_AxesY = pDoc->m_MainDataStack.Dimension;
		if (m_MiniView[index].m_AxesX == m_MiniView[index].m_AxesY)
		{
			if (m_MiniView[index].m_AxesX>1)
				m_MiniView[index].m_AxesX -=1;
			else
				m_MiniView[index].m_AxesX +=1;
		}
	}

	Invalidate();
}

void CWClustView::OnFileSaveclusters() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CheckRateDlg();
	
	if(!m_pRDlg || !m_pRDlg->FinishedRating())
	{
		MessageBox("You must rate all clusters before saving!","WClust",MB_ICONINFORMATION);
		return;
	}

	CString strRating;
	m_pRDlg->GetRatingString(strRating);
	
	int m_ErrorNumb;
	
	//get info gain for each cluster as string to be written to header
	pDoc->m_MainDataStack.whichDraw = CLUST_USER;	
	CString strInfo = GetClusterInfoString(pDoc->m_MainDataStack,pDoc->m_MainClusters);

	CString strExtra;
	strExtra += strRating;
	strExtra += strInfo;

	m_ErrorNumb=pDoc->m_MainClusters.ChooseFileAndStore(strExtra);

	switch (m_ErrorNumb)
	{
	case 0:
		Invalidate();
		break;
	case 1:
		CWnd::MessageBox(TEXT("Error: Out of memory"),NULL,MB_OK|MB_ICONERROR);
		break;
	case 2:
		CWnd::MessageBox(TEXT("Error: Save clusters' data"),NULL,MB_OK|MB_ICONERROR);
		break;
	}
}

void CWClustView::OnFileLoadclusters() 
{
	m_Redraw = 0;
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CFile file;
	int errorNumb;
	errorNumb = pDoc->m_MainClusters.OpenFile(&file,&pDoc->m_MainDataStack);
	if (errorNumb) 
	{
		// File is not opened
		char errorText[200];
		switch (errorNumb)
		{
		case 2:
			sprintf(errorText,"File was corrupted");
			break;
		case 3:
			sprintf(errorText,"Memory problem - cannot create a buffer for loading the cluster file");
			break;
		case 4:
			sprintf(errorText,"File cannot be opened");
			break;
		}
		if ( errorNumb != 1 ) // 1 is not cancel
		{
			CWnd::MessageBox(errorText, NULL,MB_OK|MB_ICONERROR);
			pDoc->m_MainClusters.Clear();
			pDoc->m_MainDataStack.ClearHighDim();
			pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);
		}
	}
	else 
	{
		if (pDoc->m_MainDataStack.AreData)
		{	// BPF/UFF is loaded
			pDoc->m_MainDataStack.CheckWcpFiles();
			CDlgWcpNotFound dlg;
			dlg.m_MainClusters = &pDoc->m_MainClusters;
			dlg.m_MainDataStack = &pDoc->m_MainDataStack;
			bool result = dlg.CheckWcp();

			if ( result == false )
			{
				dlg.DoModal();
				pDoc->m_MainDataStack.CheckWcpFiles();
				bool result = dlg.CheckWcp();
			}

			MY_WCP_FILES_STACK::iterator indWcp;
			for ( indWcp = pDoc->m_MainClusters.usedExternWCP.begin(); indWcp != pDoc->m_MainClusters.usedExternWCP.end(); indWcp++ )
			{
				if ( (*indWcp).fileName.Compare( "-" ) == 0)
				{	// parameter is not found
					pDoc->m_MainDataStack.PasteFileParamAsZeros();
				}
				else 
				{	// parameter is found
					pDoc->m_MainDataStack.PasteFileParameter( &(*indWcp) );
				}
			}
		}
		errorNumb = pDoc->m_MainClusters.LoadData( &file, &pDoc->m_MainDataStack );
		if ( errorNumb != 0 )
		{
			// File is not opened
			char errorText[200];
			switch (errorNumb)
			{
			case 2:
				sprintf(errorText,"File was corrupted");
				break;
			case 3:
				sprintf(errorText,"Memory problem - cannot create a buffer for loading the cluster file");
				break;
			case 4:
				sprintf(errorText,"File cannot be opened");
				break;
			}
			if ( errorNumb != 1 ) // 1 is not cancel
			{
				CWnd::MessageBox(errorText, NULL,MB_OK|MB_ICONERROR);
				pDoc->m_MainClusters.Clear();
				pDoc->m_MainDataStack.ClearHighDim();
				pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);
			}
		}
		else {

			pDoc->m_MainClusters.RemParamZeros(&pDoc->m_MainDataStack);

			pDoc->m_MainClusters.CalcParam(&pDoc->m_MainDataStack);
			pDoc->m_MainDataStack.CalcMinMax();
//			pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);
//			pDoc->m_MainClusters.RemParamZeros(&pDoc->m_MainDataStack);


			pDoc->m_MainDataStack.whichDraw = 0;
		
			pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);

			// Axis Adaptation
			for (int index=0; index<6; index++)
			{
				if ( m_MiniView[index].m_AxesX > pDoc->m_MainDataStack.Dimension )
					m_MiniView[index].m_AxesX = pDoc->m_MainDataStack.Dimension;
				if ( m_MiniView[index].m_AxesY > pDoc->m_MainDataStack.Dimension )
					m_MiniView[index].m_AxesY = pDoc->m_MainDataStack.Dimension;
				if (m_MiniView[index].m_AxesX == m_MiniView[index].m_AxesY)
				{
					if (m_MiniView[index].m_AxesX>1)
						m_MiniView[index].m_AxesX -=1;
					else
						m_MiniView[index].m_AxesX +=1;
				}
			}
		}
	}

	Invalidate();
	m_Redraw = 1;
}

void CWClustView::UpdateControlsFromDoc()
{
	
	
	UpdateData(FALSE);
}

void CWClustView::UpdateDocFromControls()
{
	CWClustDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);
	UpdateData(TRUE);
}


void CWClustView::OnButSet() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSetDial SetDial;

	SetDial.m_EdFirstEv = m_FirstEv;
	SetDial.m_EdLastEv = m_LastEv;
	SetDial.m_EdPrecent = m_PercEv;
	SetDial.m_ClMode = pDoc->m_MainClusters.m_ClMode;
	SetDial.m_Type = 2;
	if (SetDial.DoModal()==IDOK)
	{
		m_FirstEv = SetDial.m_EdFirstEv;
		if (m_FirstEv>pDoc->m_MainDataStack.GetNumVerx()-1)
			m_FirstEv=pDoc->m_MainDataStack.GetNumVerx()-1;
		if (m_FirstEv<1)
			m_FirstEv=1;
		
		m_LastEv = SetDial.m_EdLastEv;
		if (m_LastEv>pDoc->m_MainDataStack.GetNumVerx() || m_LastEv==0)
			m_LastEv=pDoc->m_MainDataStack.GetNumVerx();
		if (m_LastEv<=m_FirstEv)
			m_LastEv=m_FirstEv+1;

		m_PercEv = SetDial.m_EdPrecent;
		if (m_PercEv<1)
			m_PercEv=1;
		if (m_PercEv>100)
			m_PercEv=100;
		if (SetDial.m_ClMode != pDoc->m_MainClusters.m_ClMode)
		{ 
			pDoc->m_MainClusters.m_ClMode = SetDial.m_ClMode;
			pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);
		}
	}
	pDoc->m_MainDataStack.CalcViewVx(m_FirstEv,m_LastEv,m_PercEv);
	Invalidate();
}


void sscanfLine(char *from, char *readStr, long *pos)
{
	int	i = 0;

	while (from[i + *pos] != 0x0D && from[i + 1 + *pos] != 0x0A && from[i + *pos] != 0x0A)
	{
		readStr[i] = from[i + *pos];
		i++;
	}

	
	if ( from[i + *pos] == 0x0A )
	{
		readStr[i] = 0x0A;
		i++;
		*pos += i;
	}
	else
	{
		readStr[i] = 0x0A;
		readStr[i+1] = '\0';
		i++;i++;
		*pos += i;
	}
}

void CWClustView::OnFileStoretobpfile() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	
	// where store
	char pomStr[120];
	sprintf(pomStr,"Overwrite %s (Yes) or create new BP file (No)?",pDoc->m_MainDataStack.m_FileName);
	int result = CWnd::MessageBox(pomStr,NULL,MB_YESNOCANCEL);

	char *newFileName, *newTmpName; 

	CFile file, fileNew;
	
	if ( result == IDYES || result == IDNO )
	{

		if ( result == IDNO )
		{	// Create a copy
			CFileDialog *dlg;
			if ( pDoc->m_MainDataStack.GetFileType() == 1)
			{
				static char BASED_CODE szFilter[] = "Brain potential files (*.bpf)|*.bpf|All Files (*.*)|*.*||";
				dlg = (CFileDialog*) new CFileDialog(FALSE, "bpf", "*.bpf",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
			}
			if ( pDoc->m_MainDataStack.GetFileType() == 2)
			{
				static char BASED_CODE szFilter[] = "Brain potential files (*.uff)|*.uff|All Files (*.*)|*.*||";
				dlg = (CFileDialog*) new CFileDialog(FALSE, "uff", "*.uff",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
			}

			if (dlg->DoModal()==IDOK)
			{	
				// convert file name 
				newFileName = (char*) malloc( dlg->GetPathName().GetLength() + 1);
				newTmpName = (char*) malloc( dlg->GetPathName().GetLength() + 1);
				for (int i=0; i<dlg->GetPathName().GetLength(); i++)
				{
					newFileName[i] = dlg->GetPathName().GetAt(i);
					newTmpName[i] = dlg->GetPathName().GetAt(i);
				}
				newFileName[i] = 0;
				newTmpName[i-3] = 't';
				newTmpName[i-2] = 'm';
				newTmpName[i-1] = 'p';
				newTmpName[i] = 0;
				
				// test if it is not the same file
				int testI=0;
				while ( pDoc->m_MainDataStack.m_FileName[testI] == newFileName[testI] && pDoc->m_MainDataStack.m_FileName[testI] != 0 && newFileName[testI] != 0)
				{
					testI++;
				}

				if ( pDoc->m_MainDataStack.m_FileName[testI] == 0 && newFileName[testI] == 0 )
				{
					CWnd::MessageBox(TEXT("Error: You cannot do a copy of file with the same name"),NULL,MB_OK|MB_ICONERROR);
					free(newFileName);
					free(newTmpName);
					delete(dlg);
					return;
				}
				
				// open file to store
				if (!fileNew.Open(dlg->GetPathName(),CFile::modeCreate|CFile::modeWrite))
				{
					CWnd::MessageBox(TEXT("Error: Create new file"),NULL,MB_OK|MB_ICONERROR);
					free(newFileName);
					free(newTmpName);
					delete(dlg);
					return;
				}
				else
				{
					if (!file.Open(pDoc->m_MainDataStack.m_FileName,CFile::modeRead))
					{
						CWnd::MessageBox(TEXT("Error: Open original file"),NULL,MB_OK|MB_ICONERROR);
						free(newFileName);
						free(newTmpName);
						delete(dlg);
						return;	
					}
					
				}
				delete(dlg);
				
			}
			else {
				delete(dlg);
				return;
			}
			
		} //NO
		else if ( result == IDYES )
		{	// Rewrite existing file
			
			if ( pDoc->m_MainDataStack.GetFileType() == 1)
			{	// BPF
				if (!fileNew.Open(pDoc->m_MainDataStack.m_TmpName,CFile::modeCreate|CFile::modeWrite))
				{
					CWnd::MessageBox(TEXT("Error: Create tmp file"),NULL,MB_OK|MB_ICONERROR);
					return;
				}
				else
				{
					newFileName = pDoc->m_MainDataStack.m_TmpName;
					if (!file.Open(pDoc->m_MainDataStack.m_FileName,CFile::modeRead))
					{
						CWnd::MessageBox(TEXT("Error: Open original file"),NULL,MB_OK|MB_ICONERROR);
						return;	
					}
				}
			}

		}//YES
	
		/////////////////////////////////////// create a copy
				
		// first copy of header
	
		if ( pDoc->m_MainDataStack.GetFileType() == 1)
		{	// BPF

			char* buffer = new char[16000];
			int nRead;
			if (buffer==NULL)
			{	
				CWnd::MessageBox(TEXT("Error: Out of memory"),NULL,MB_OK|MB_ICONERROR);
				return;
			}
			else	
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
				
				fileNew.Close();
				FILE *strFile;
				strFile = fopen(newFileName,"w+");

				char loadText[160];

				char endSetup = 0;
				char endRecordFormat = 0;
				char endHeader = 0;

				char endSetupStr[30];
				char endRecordFormatStr[32];
				char endHeaderStr[20];

				int endSetupLen = 23;
				int endRecordFormatLen = 31;
				int endHeaderLen = 12;

				sprintf(endSetupStr,"%%%%END SETUP_INFORMATION"); //23
				sprintf(endRecordFormatStr,"%%%%END RECORD_FORMAT_INFORMATION");//31
				sprintf(endHeaderStr,"%%%%END_HEADER");//12

		
				long m_i = 0, m_oldI;
				int m_j;
				nRead=file.Read(buffer,16000);
				char whatToStore = pDoc->m_MainDataStack.LoadedAdd & 0x3;
				if ( whatToStore > 0 )
				{
					if ( whatToStore > 0 ) // room || both
					{
						do
						{
							m_oldI = m_i;
							sscanfLine(buffer,loadText,&m_i);
		
							m_j=0;
							while (( endSetupStr[m_j] == loadText[m_j]) && (m_j < endSetupLen))
								m_j++;
							if (m_j == endSetupLen)
							{
								endSetup = 1;
							}
							if ( !endSetup )
							{
								fprintf(strFile,"%s",loadText);
							}
						}while (!endSetup);
						if ( whatToStore & 1 )
							fprintf(strFile,"%%ArenaPositions.0 (1)\n");
						if ( whatToStore & 2 )
							fprintf(strFile,"%%RoomPositions.0 (1)\n");
						fprintf(strFile,"%s",loadText);
	
						do
						{
							m_oldI = m_i;
							sscanfLine(buffer,loadText,&m_i);
		
							m_j=0;
							while (( endRecordFormatStr[m_j] == loadText[m_j]) && (m_j < endRecordFormatLen))
								m_j++;
							if (m_j == endRecordFormatLen)
							{
								endRecordFormat = 1;
							}
							if ( !endRecordFormat )
							{
								fprintf(strFile,"%s",loadText);
							}
						}while (!endRecordFormat);
						if ( whatToStore & 1 )
						{
							fprintf(strFile,"%%ArenaPositionIdentifier.1 ('A')\n");
							fprintf(strFile,"%%ArenaPositionRecordFormat.9 Identifier.1 100usTimestamp.4 ArenaX1.1 ");
							fprintf(strFile,"ArenaY1.1 ArenaAng.2\n");
						}

					
					
						if ( whatToStore & 2 )
						{
							fprintf(strFile,"%%RoomPositionIdentifier.1 ('R')\n");
							fprintf(strFile,"%%RoomPositionRecordFormat.9 Identifier.1 100usTimestamp.4 RoomX1.1 ");
							fprintf(strFile,"RoomY1.1 RoomAng.2\n");
						}
						fprintf(strFile,"%s",loadText);
					}	
				}

				do
				{
					sscanfLine(buffer,loadText,&m_i);
	
					m_j=0;
					while (( endHeaderStr[m_j] == loadText[m_j]) && (m_j < endHeaderLen))
						m_j++;
					if (m_j == endHeaderLen)
					{
						endHeader = 1;
					}
					if ( !endHeader )
					{
						fprintf(strFile,"%s",loadText);
					}
				}while (!endHeader);
				fprintf(strFile,"%s",loadText);

				fclose(strFile);			

				fileNew.Open(newFileName,CFile::modeWrite);
				fileNew.Seek(0,CFile::end);

				file.Seek(m_i,CFile::begin);
					
				if ( pDoc->m_MainDataStack.FileType == 1 && pDoc->m_MainDataStack.GetDFileOK() == 0 )
				{	// repairing file - don't copy last error Record
					int readBytes;
					char *buffer;
					buffer =(char*) malloc(65536);
					
					readBytes = file.Read(buffer,65536);
					m_i += readBytes;
					if ( m_i > pDoc->m_MainDataStack.GetDLastPos() )
						readBytes -= m_i - pDoc->m_MainDataStack.GetDLastPos();
					
					// main copy loop
					while (readBytes == 65536)
					{
						fileNew.Write(buffer,readBytes);
						readBytes = file.Read(buffer,65536);
						
						m_i += readBytes;
						if ( m_i > pDoc->m_MainDataStack.GetDLastPos() )
							readBytes -= m_i - pDoc->m_MainDataStack.GetDLastPos();
					
					}
					fileNew.Write(buffer,readBytes);

				}
				else
				{	// BPF is OK or UFF
					char *buffer = new char[65536];
					int readBytes;
					readBytes = file.Read(buffer,65536);
					while (readBytes == 65536)
					{
						fileNew.Write(buffer,readBytes);
						readBytes = file.Read(buffer,65536);
					}
					fileNew.Write(buffer,readBytes);
					delete [] buffer;//needs to be deleted!!
				}

				delete [] buffer;//buffer declared at this scope needs to be deleted too!!
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			}	

			if ( result == IDYES)
			{ // copy back and delete tmp

				fileNew.Close();
				file.Close();
				CFile::Remove(pDoc->m_MainDataStack.m_FileName);
				CFile::Rename(pDoc->m_MainDataStack.m_TmpName,pDoc->m_MainDataStack.m_FileName);
			}
			if ( result == IDNO )
			{
				free(pDoc->m_MainDataStack.m_FileName);
				pDoc->m_MainDataStack.m_FileName = newFileName;
				free(pDoc->m_MainDataStack.m_TmpName);
				pDoc->m_MainDataStack.m_TmpName = newTmpName;
				fileNew.Close();
				file.Close();
			}
			
		}

		if ( pDoc->m_MainDataStack.GetFileType() == 2)
		{	// UFF
			if ( result == IDYES)
			{ // copy back and delete tmp
				fileNew.Close();
				file.Close();
			}
			if ( result == IDNO )
			{

				fileNew.Seek(0,CFile::end);
				file.Seek(0,CFile::begin);
				
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

				char* buffer = new char[65536];
				int readBytes;

				readBytes = file.Read(buffer,65536);
				while (readBytes == 65536)
				{
					fileNew.Write(buffer,readBytes);
					readBytes = file.Read(buffer,65536);
				}
				fileNew.Write(buffer,readBytes);

				free(pDoc->m_MainDataStack.m_FileName);
				pDoc->m_MainDataStack.m_FileName = newFileName;
				free(pDoc->m_MainDataStack.m_TmpName);
				pDoc->m_MainDataStack.m_TmpName = newTmpName;
				fileNew.Close();
				file.Close();
				delete [] buffer;
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			}
		}

	
		///////////////////// end create the copy

		if (pDoc->m_MainDataStack.GetFileType() == 1)
		{	// store BPF
			MY_STACK::iterator IndexVertex;
			
			char	State;
			int		error = pDoc->m_MainDataStack.PrepareStoreBPF();
			int flag = 5;
			
			if (error == 0)
			{
				while (flag == 5)
				{
					flag = pDoc->m_MainDataStack.StoreBPF(&State);
					if (flag == 5)
					{
						if (pDoc->m_MainClusters.GetSwap())
						{
							pDoc->m_MainDataStack.forStoreBPF_NonClVx->Swap(pDoc->m_MainDataStack.NUM_CHANNELS,pDoc->m_MainDataStack.NUM_SAMPLES);
						}
						else{
						}
						pDoc->m_MainClusters.CalcParamOneSpike( &pDoc->m_MainDataStack, pDoc->m_MainDataStack.forStoreBPF_NonClVx, pDoc->m_MainDataStack.forStoreBPF_Spike );
						pDoc->m_MainClusters.Clustering( pDoc->m_MainDataStack.forStoreBPF_NonClVx );
						pDoc->m_MainDataStack.StoreBPFElde();
					}
				}

				// if arena or room data are not included in BPF
				pDoc->m_MainDataStack.StoreBPFAddition();
				pDoc->m_MainDataStack.FinishStoreBPF();
				MessageBeep(0);
			}			
			
			if (error == 1)
			{}
			
			if (error == 2)
			{}
			
			if (error == 3)
			{}
		
		}
		if (pDoc->m_MainDataStack.GetFileType() == 2)
		{	// store UFF
			int ErrorNumb = pDoc->m_MainDataStack.StoreClMarks();
			switch (ErrorNumb)
			{
			case 0:
				if (result == IDYES)
					CWnd::MessageBox("File is updated");
				else
					CWnd::MessageBox("Data are stored");
				break;
			}
		}

	} //YES & NO
}

void CWClustView::OnUpdateFileStoretobpfile(CCmdUI* pCmdUI) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc->m_MainDataStack.IsEmpty())
	{
		pCmdUI->Enable(true);
	}
	else
	{
		pCmdUI->Enable(false);
	}
}

void CWClustView::OnEnvironmentLoad() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
    static char BASED_CODE szFilter[] = "Environment files (*.env)|*.env|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, "env", "*.env",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if (dlg.DoModal()==IDOK)
	{
		CFile file;
		if (!file.Open(dlg.GetPathName(),CFile::modeRead))
		{
			CWnd::MessageBox(TEXT("Error: Open enviromnent data file"),NULL,MB_OK|MB_ICONERROR);
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	
			char *buffer, pomStr[20];
			int nRead, errorNo=0;
			buffer = (char*) malloc(8192);
			if (buffer==NULL)
				CWnd::MessageBox(TEXT("Error: Out of memory"),NULL,MB_OK|MB_ICONERROR);
			else
			{
				//Searching of index of binary data
				nRead=file.Read(buffer,8192);
				long m_i;
				int FoundEnd;
				int m_j;
								
				m_i=0;
				FoundEnd=0;
				sprintf(pomStr,"%%%%END_HEADER");
				while (m_i<(nRead-14)&&!FoundEnd)
				{
					m_j=0;
					while ((*(buffer+m_i+m_j)==pomStr[m_j])&&(m_j<12))
						m_j+=1;	
					if (m_j==12)
					{
						FoundEnd=1;
					}
					m_i+=1;
				}
				free(buffer);
				
				if (FoundEnd)
				{
					// Set file pointer to the first data identifier
					m_i+=13;
					file.Seek(m_i,0);

					int index=0,maxIndex;

					char m_Identif; 
					file.Read(&m_Identif,1);
				
					if (m_Identif=='V')
					{	
						file.Read(&m_Identif,1);
						maxIndex = m_Identif;
						if (maxIndex = 6)
						{
							while ( index<maxIndex && errorNo==0 )
							{
								file.Read(&m_Identif,1);
								if (m_Identif=='W')
								{
									file.Read(&m_Identif,1);
									if (m_Identif=='X')
									{
										file.Read(&m_MiniView[index].m_AxesX,sizeof(m_MiniView[index].m_AxesX));
										if (pDoc->m_MainDataStack.AreData)
											if ( m_MiniView[index].m_AxesX > pDoc->m_MainDataStack.Dimension )
												m_MiniView[index].m_AxesX = pDoc->m_MainDataStack.Dimension;
											
									}
									else errorNo=1;
	
									file.Read(&m_Identif,1);
									if (m_Identif=='Y' && errorNo == 0)
									{
										file.Read(&m_MiniView[index].m_AxesY,sizeof(m_MiniView[index].m_AxesY));
		
										if (pDoc->m_MainDataStack.AreData)
											if ( m_MiniView[index].m_AxesY > pDoc->m_MainDataStack.Dimension )
												m_MiniView[index].m_AxesY = pDoc->m_MainDataStack.Dimension;

									}
									else errorNo=1;

									if (m_MiniView[index].m_AxesX == m_MiniView[index].m_AxesY)
									{
										if (m_MiniView[index].m_AxesX>1)
											m_MiniView[index].m_AxesX -=1;
										else
											m_MiniView[index].m_AxesX +=1;
									}


									file.Read(&m_Identif,1);
									if (m_Identif=='S' && errorNo == 0)
									{
										file.Read(&m_MiniView[index].m_PointsSize,sizeof(m_MiniView[index].m_PointsSize));
									}
									else errorNo=1;

									file.Read(&m_Identif,1);
									if (m_Identif=='D' && errorNo == 0)
									{
										file.Read(&m_MiniView[index].m_DataArea.left,   sizeof(m_MiniView[index].m_DataArea.left));
										file.Read(&m_MiniView[index].m_DataArea.right,  sizeof(m_MiniView[index].m_DataArea.right));
										file.Read(&m_MiniView[index].m_DataArea.bottom, sizeof(m_MiniView[index].m_DataArea.bottom));
										file.Read(&m_MiniView[index].m_DataArea.top,    sizeof(m_MiniView[index].m_DataArea.top));
									}
									else errorNo=1;

									file.Read(&m_Identif,1);
									if (m_Identif=='C' && errorNo == 0)
									{
										file.Read(m_MiniView[index].m_ClustToView,sizeof(unsigned char)*100);
									}
									else errorNo=1;

									file.Read(&m_Identif,1);
									if (m_Identif=='M' && errorNo == 0)
									{
										file.Read(&pDoc->m_MainClusters.m_ClMode,sizeof(pDoc->m_MainClusters.m_ClMode));
									}
							
								}
								else errorNo=1;
								index ++;
							}
						
						}
						else errorNo = 1;
					}
					else errorNo = 1;
				}
				else errorNo = 1;

			}
			if (errorNo == 1)
				CWnd::MessageBox(TEXT("Error: File was corrupted"),NULL,MB_OK|MB_ICONERROR);
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			file.Close();
		}
	}
	pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);
	Invalidate();
}

void CWClustView::OnEnvironmentSave() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    static char BASED_CODE szFilter[] = "Environment files (*.env)|*.env|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, "env", "*.env",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if (dlg.DoModal()==IDOK)
	{
		CFile file;
		if (!file.Open(dlg.GetPathName(),CFile::modeCreate | CFile::modeWrite))
		{
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
			// Store header
			char pomStr[80];
			sprintf(pomStr,"%%%%BEGIN_HEADER\n");
			file.Write(pomStr,15);
			sprintf(pomStr,"%%%%END_HEADER\t\t");
			file.Write(pomStr,14);
			sprintf(pomStr,"V%c",6);
			file.Write(pomStr,2);
			for (int i=0; i<6; i++)
			{
				file.Write("W",1);
				file.Write("X",1);
				file.Write(&m_MiniView[i].m_AxesX,sizeof(m_MiniView[i].m_AxesX));
				file.Write("Y",1);
				file.Write(&m_MiniView[i].m_AxesY,sizeof(m_MiniView[i].m_AxesY));
				file.Write("S",1);
				file.Write(&m_MiniView[i].m_PointsSize,sizeof(m_MiniView[i].m_PointsSize));
				file.Write("D",1);
				file.Write(&m_MiniView[i].m_DataArea.left,  sizeof(m_MiniView[i].m_DataArea.left));
				file.Write(&m_MiniView[i].m_DataArea.right, sizeof(m_MiniView[i].m_DataArea.right));
				file.Write(&m_MiniView[i].m_DataArea.bottom,sizeof(m_MiniView[i].m_DataArea.bottom));
				file.Write(&m_MiniView[i].m_DataArea.top,   sizeof(m_MiniView[i].m_DataArea.top));
				file.Write("C",1);
				file.Write(m_MiniView[i].m_ClustToView, sizeof(unsigned char)*100);
				file.Write("M",1);
				file.Write(&pDoc->m_MainClusters.m_ClMode, sizeof(pDoc->m_MainClusters.m_ClMode));
			}
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			file.Close();
		}		
	}
	
}

void CWClustView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CClientDC dc(this);
	int iOldF,iNewF;
	switch (nChar)
	{
	case VK_HOME:
		
		for (int i=0; i<3; i++)
			if (m_MiniView[i].m_Focus)
			{
				m_MiniView[i].m_Focus = 0;
				m_MiniView[i+3].m_Focus = 0;
				iOldF = i;
				iNewF = i+3;
			}
		break;
	}
	dc.SetROP2(R2_NOT);
	dc.MoveTo(m_MiniView[iOldF].m_WindowArea.left-1,m_MiniView[iOldF].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[iOldF].m_WindowArea.right,m_MiniView[iOldF].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[iOldF].m_WindowArea.right,m_MiniView[iOldF].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[iOldF].m_WindowArea.left-1,m_MiniView[iOldF].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[iOldF].m_WindowArea.left-1,m_MiniView[iOldF].m_WindowArea.bottom);
	
	dc.SetROP2(R2_COPYPEN);
	dc.MoveTo(m_MiniView[iNewF].m_WindowArea.left-1,m_MiniView[iNewF].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[iNewF].m_WindowArea.right,m_MiniView[iNewF].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[iNewF].m_WindowArea.right,m_MiniView[iNewF].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[iNewF].m_WindowArea.left-1,m_MiniView[iNewF].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[iNewF].m_WindowArea.left-1,m_MiniView[iNewF].m_WindowArea.bottom);
	
	
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CWClustView::OnButton1() 
{	// 1
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int x = 5,y = 4;

	CRect clRect;

	int OldFocus=0;
	int IsFocus=1;

	for (int i=0;i<6;i++)
	{
		if (m_MiniView[i].m_Focus)
		{
			OldFocus = i;
			m_MiniView[i].m_Focus = 0;
		}
		if (i == 0)
			m_MiniView[i].m_Focus = 1;
	}

	CClientDC dc(this);
	GetClientRect(clRect);

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(clRect.right,clRect.bottom);
	dc.SetViewportExt(clRect.right,-clRect.bottom);
	dc.SetViewportOrg(0,clRect.bottom);
			
	dc.SetROP2(R2_NOT);
	i=OldFocus;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	i=0;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	dc.SetROP2(R2_COPYPEN);

	CallEditDlg();
}

void CWClustView::OnButton3() 
{	// 2
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect clRect;

	int OldFocus=0;
	int IsFocus=1;

	for (int i=0;i<6;i++)
	{
		if (m_MiniView[i].m_Focus)
		{
			OldFocus = i;
			m_MiniView[i].m_Focus = 0;
		}
		if (i == 1)
			m_MiniView[i].m_Focus = 1;
	}

	CClientDC dc(this);
	GetClientRect(clRect);

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(clRect.right,clRect.bottom);
	dc.SetViewportExt(clRect.right,-clRect.bottom);
	dc.SetViewportOrg(0,clRect.bottom);
			
	dc.SetROP2(R2_NOT);
	i=OldFocus;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	i=1;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	dc.SetROP2(R2_COPYPEN);

	CallEditDlg();

}

void CWClustView::OnButton4() 
{
	// 3	
		CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect clRect;

	int OldFocus=0;
	int IsFocus=1;

	for (int i=0;i<6;i++)
	{
		if (m_MiniView[i].m_Focus)
		{
			OldFocus = i;
			m_MiniView[i].m_Focus = 0;
		}
		if (i == 2)
			m_MiniView[i].m_Focus = 1;
	}

	CClientDC dc(this);
	GetClientRect(clRect);

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(clRect.right,clRect.bottom);
	dc.SetViewportExt(clRect.right,-clRect.bottom);
	dc.SetViewportOrg(0,clRect.bottom);
			
	dc.SetROP2(R2_NOT);
	i=OldFocus;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	i=2;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	dc.SetROP2(R2_COPYPEN);

	CallEditDlg();

}

void CWClustView::OnButton5() 
{
	// 4
		CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect clRect;

	int OldFocus=0;
	int IsFocus=1;

	for (int i=0;i<6;i++)
	{
		if (m_MiniView[i].m_Focus)
		{
			OldFocus = i;
			m_MiniView[i].m_Focus = 0;
		}
		if (i == 3)
			m_MiniView[i].m_Focus = 1;
	}

	CClientDC dc(this);
	GetClientRect(clRect);

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(clRect.right,clRect.bottom);
	dc.SetViewportExt(clRect.right,-clRect.bottom);
	dc.SetViewportOrg(0,clRect.bottom);
			
	dc.SetROP2(R2_NOT);
	i=OldFocus;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	i=3;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	dc.SetROP2(R2_COPYPEN);

	CallEditDlg();

}

void CWClustView::OnButton6() 
{
	//	5
		CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect clRect;

	int OldFocus=0;
	int IsFocus=1;

	for (int i=0;i<6;i++)
	{
		if (m_MiniView[i].m_Focus)
		{
			OldFocus = i;
			m_MiniView[i].m_Focus = 0;
		}
		if (i == 4)
			m_MiniView[i].m_Focus = 1;
	}

	CClientDC dc(this);
	GetClientRect(clRect);

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(clRect.right,clRect.bottom);
	dc.SetViewportExt(clRect.right,-clRect.bottom);
	dc.SetViewportOrg(0,clRect.bottom);
			
	dc.SetROP2(R2_NOT);
	i=OldFocus;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	i=4;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	dc.SetROP2(R2_COPYPEN);

	CallEditDlg();

}

void CWClustView::OnButton7() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect clRect;

	int OldFocus=0;
	int IsFocus=1;

	for (int i=0;i<6;i++)
	{
		if (m_MiniView[i].m_Focus)
		{
			OldFocus = i;
			m_MiniView[i].m_Focus = 0;
		}
		if (i == 5)
			m_MiniView[i].m_Focus = 1;
	}

	CClientDC dc(this);
	GetClientRect(clRect);

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(clRect.right,clRect.bottom);
	dc.SetViewportExt(clRect.right,-clRect.bottom);
	dc.SetViewportOrg(0,clRect.bottom);
			
	dc.SetROP2(R2_NOT);
	i=OldFocus;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	i=5;
	dc.MoveTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.bottom);
	dc.LineTo(m_MiniView[i].m_WindowArea.right,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.top+1);
	dc.LineTo(m_MiniView[i].m_WindowArea.left-1,m_MiniView[i].m_WindowArea.bottom);
			
	dc.SetROP2(R2_COPYPEN);

	CallEditDlg();
	
}

void CWClustView::OnSelchangeComboclust() 
{
	InvalidateRect(m_ColorRect,false);
}

void CWClustView::OnHide() 
{
	int Selected = m_wndComboClust.GetCurSel();
	if (Selected == 0)
		Selected = 255;
	else 
		Selected -= 1;

	for (int i=0;i<6;i++)
	{
		m_MiniView[i].m_ClustToView[Selected] = 0;
	}
	CRect DrawRect = m_ViewRect;
	DrawRect.bottom = 80;
	Invalidate();
}

void CWClustView::OnShow() 
{
	int Selected = m_wndComboClust.GetCurSel();
	if (Selected == 0)
		Selected = 255;
	else 
		Selected -= 1;

	for (int i=0;i<6;i++)
	{
		m_MiniView[i].m_ClustToView[Selected] = 3;
	}	

	CRect DrawRect = m_ViewRect;
	DrawRect.bottom = 80;
	Invalidate();
}

void CWClustView::OnLoadArena() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    static char BASED_CODE szFilter[] = "Arena data files (*.dat)|*.dat|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "dat", "*.dat",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if (dlg.DoModal()==IDOK)
	{
		FILE *fptr;
		fptr = fopen(dlg.GetPathName(),"r");
		if ( fptr == NULL )
		{
			CWnd::MessageBox(TEXT("Error: Cannot open arena data file"),NULL,MB_ICONWARNING||MB_OK);
		}
		else
		{
			int error = pDoc->m_MainDataStack.m_MainSyncStack.LoadArenaFromDAT(fptr);
			fclose(fptr);
			if ( error == 0 )
				pDoc->m_MainDataStack.LoadedAdd |= 1;
			if ( error == 2 )
				CWnd::MessageBox(TEXT("Error: Structure of arena file"),NULL,MB_ICONWARNING||MB_OK);
		}
	}
// TODO diagnostic
//char a[2000];
//sprintf(a,"no time stamp (from sync): %d \nEmpty frames in Arena.DAT (and added as 0,0 pos.) : %d \nlast frame loaded(check with last FrameCount in Arena.DAT): %d \n2*last frame no(in DAT): %d \nArena loaded from DAT: %d \n",
//		pDoc->m_MainDataStack.m_MainSyncStack.NoTimeStamp,
//		pDoc->m_MainDataStack.m_MainSyncStack.noEmpty,
//		pDoc->m_MainDataStack.m_MainSyncStack.lastFrame,
//		2 * pDoc->m_MainDataStack.m_MainSyncStack.lastFrame,
//		pDoc->m_MainDataStack.m_MainSyncStack.noAdd);
//CWnd::MessageBox(a);
}

void CWClustView::OnLoadRoom() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    static char BASED_CODE szFilter[] = "Room data files (*.dat)|*.dat|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "dat", "*.dat",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if (dlg.DoModal()==IDOK)
	{
		FILE *fptr;
		fptr = fopen(dlg.GetPathName(),"r");
		if ( fptr == NULL )
		{
			CWnd::MessageBox(TEXT("Error: Cannot open arena data file"),NULL,MB_ICONWARNING||MB_OK);
		}
		else
		{
			int error = pDoc->m_MainDataStack.m_MainSyncStack.LoadRoomFromDAT(fptr);
			fclose(fptr);
			if ( error == 0 )
				pDoc->m_MainDataStack.LoadedAdd |= 2;
			if ( error == 2 )
				CWnd::MessageBox(TEXT("Error: Structure of position file"),NULL,MB_ICONWARNING||MB_OK);
		}
	}
}

void CWClustView::OnUpdateLoadArena(CCmdUI* pCmdUI) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->m_MainDataStack.IsEmpty() || pDoc->m_MainDataStack.InBpfExist & 1 || pDoc->m_MainDataStack.LoadedAdd & 1)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}

void CWClustView::OnUpdateLoadRoom(CCmdUI* pCmdUI) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->m_MainDataStack.IsEmpty() || pDoc->m_MainDataStack.InBpfExist & 2 || pDoc->m_MainDataStack.LoadedAdd & 2)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}

void CWClustView::OnModeSwappolarity() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->m_MainClusters.SwapPolarity(&pDoc->m_MainDataStack);	

	Invalidate();
}

void CWClustView::OnUpdateModeSwappolarity(CCmdUI* pCmdUI) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(pDoc->m_MainClusters.m_Swap)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CWClustView::OnPosRegWin() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CDlgPositionReg dlg;
	dlg.m_MainDataStack = &pDoc->m_MainDataStack;
	dlg.DoModal();
}

void CWClustView::OnUpdatePosRegWin(CCmdUI* pCmdUI) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->m_MainDataStack.IsEmpty() || pDoc->m_MainDataStack.GetFileType() == 2 || !(pDoc->m_MainDataStack.InBpfExist & 3) &&  !(pDoc->m_MainDataStack.LoadedAdd & 3) )
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}

void CWClustView::OnTimeEvWin() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	CTimeEv dlg;

	dlg.m_MainDataStack = &pDoc->m_MainDataStack;
	dlg.m_MainClusters = &pDoc->m_MainClusters;
	dlg.m_MainPalette = &pDoc->m_MainPalette;

	dlg.DoModal();

}

void CWClustView::OnUpdateTimeEvWin(CCmdUI* pCmdUI) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if ( pDoc->m_MainDataStack.IsEmpty() )
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}

void CWClustView::OnTimer(UINT nIDEvent) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	switch(nIDEvent)
	{
	case IDT_AUTO_STORE_CL:
		{
			char fileNameTmpCl[100];
			::GetDateFormat(LOCALE_NEUTRAL, 0, NULL, "'tmp'yyyy'-'MM'-'dd'", fileNameTmpCl,100);
			::GetTimeFormat(LOCALE_NEUTRAL, 0, NULL, "'_'HH'-'mm'.cl'", &fileNameTmpCl[13],77);

			//get info gain for each cluster as string to be written to header
			pDoc->m_MainDataStack.whichDraw = CLUST_USER;
			CString strInfo = GetClusterInfoString(pDoc->m_MainDataStack,pDoc->m_MainClusters);
			pDoc->m_MainClusters.AutoSave(fileNameTmpCl,strInfo);
		}
		break;
	}
	CFormView::OnTimer(nIDEvent);
}

void CWClustView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFormView::OnShowWindow(bShow, nStatus);


	AutoSaveClMinutes = 10;
	SetTimer(IDT_AUTO_STORE_CL, AutoSaveClMinutes*60*1000,NULL);
}


void CWClustView::OnSelchangeCmbOrigNew1() 
{
	CWClustDoc* pDoc = GetDocument();
	if ( pDoc->m_MainDataStack.whichDraw != m_wndCmbOrigNew.GetCurSel() )
	{
		pDoc->m_MainDataStack.whichDraw = m_wndCmbOrigNew.GetCurSel();
		Invalidate();
	}	
	
}

void CWClustView::OnToolsInfo() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	CDlgFileInfo dlg;
	dlg.m_MainClusters = &pDoc->m_MainClusters;
	dlg.m_MainDataStack = &pDoc->m_MainDataStack;
	dlg.DoModal();
}

void CWClustView::OnUpdateToolsInfo(CCmdUI* pCmdUI) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if ( pDoc->m_MainDataStack.IsEmpty() )
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}


void CWClustView::OnToolsExport() 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	FILE *fileTXT;
	
	CString fileTXTName = pDoc->m_MainDataStack.m_FileNameWithoutExt;
	char dateAndTime[30];
	::GetDateFormat(LOCALE_NEUTRAL, 0, NULL, "'_'yyyy'-'MM'-'dd'", dateAndTime,30);
	::GetTimeFormat(LOCALE_NEUTRAL, 0, NULL, "'_'HH'-'mm", &dateAndTime[11],17);
	dateAndTime[17] = 0;
	fileTXTName.Insert(fileTXTName.GetLength(), dateAndTime);

	fileTXTName.Insert(fileTXTName.GetLength(),".txt");
	fileTXT = fopen((LPCTSTR) fileTXTName,"w");
	if ( fileTXT != NULL )
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
			
		CFile fileBPF;
		fileBPF.Open(pDoc->m_MainDataStack.m_FileName,CFile::modeRead);

		//get info gain for each cluster as string to be written to header
		pDoc->m_MainDataStack.whichDraw = CLUST_USER;
		CString strInfo = GetClusterInfoString(pDoc->m_MainDataStack,pDoc->m_MainClusters);
		pDoc->m_MainDataStack.CreateHeaderForExport(fileTXT,strInfo);
		
		CVertex *verx;
		MY_SPIKE_STACK::iterator indexSp;

		int iV = 0;//index into reassignment vector, if it is needed at all...
	
		for (indexSp = pDoc->m_MainDataStack.m_SpikesStack.begin(); indexSp != pDoc->m_MainDataStack.m_SpikesStack.end(); indexSp++, iV++)
		{
			if ( (*indexSp)->type == 1 ) // spike is in memory
			{
				verx = (CVertex*)*(pDoc->m_MainDataStack.m_VerxStack.begin() + (*indexSp)->offset);
				int iC = pDoc->m_MainDataStack.LookupVClust(iV);
				verx->ExportValues(fileTXT,&iC);
			}
			else	// spike is on disk
			{
				verx = (CVertex*) new CVertex(&pDoc->m_MainPalette);
				pDoc->m_MainDataStack.LoadBPFElde(verx, &fileBPF, (*indexSp)->offset );
			
				if (pDoc->m_MainClusters.GetSwap())
				{
					verx->Swap(pDoc->m_MainDataStack.NUM_CHANNELS, pDoc->m_MainDataStack.NUM_SAMPLES);
				}
				pDoc->m_MainClusters.CalcParamOneSpike( &pDoc->m_MainDataStack, verx, *indexSp );
				verx->CheckNoise();
				pDoc->m_MainClusters.Clustering( verx );
				int iC = pDoc->m_MainDataStack.LookupVClust(iV);
				verx->ExportValues(fileTXT,&iC);

				delete verx;
			}
		}	
		fclose(fileTXT);
		fileBPF.Close();
		
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				
		char mess[500];
		sprintf(mess,"Data were exported to '%s' file.",(LPCTSTR) fileTXTName);
		MessageBox(mess);
		
	}
	else
	{
		char mess[500];
		sprintf(mess,"Error: cannot create '%s' file for your data.",(LPCTSTR) fileTXTName);
		MessageBox(mess,NULL,MB_ICONERROR);
	}
}

void CWClustView::OnUpdateToolsExport(CCmdUI* pCmdUI) 
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if ( pDoc->m_MainDataStack.IsEmpty() )
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}	
}

void CWClustView::OnBnClickedOk()
{
	//MessageBox("HI","HI",0);
	// TODO: Add your control notification handler code here
}
#include "DlgClustNum.h"
//int CWClustView::gNumClusts = 4;
int gNumClusts = 4;

struct InfoClustParams
{
	CWClustDoc* pDoc;
	int iClust;
	bool bExclusive; //if true do exclusive info clust, otherwise ud info clust
};

bool InfoClustThread(const CUPDUPDATA* pUp)
{
	InfoClustParams* pParams = (InfoClustParams*) pUp->GetAppData();

	int iClust = pParams->iClust;
	CWClustDoc* pDoc = pParams->pDoc;
	bool bExclusive = pParams->bExclusive;

	char msg[1024];

	if(bExclusive)
	{
		sprintf(msg,"Performing exclusive info-max clustering with %d clusters",iClust);
		pUp->SetProgress(msg,0);
		pDoc->m_MainClusters.ExclusiveInfoClust(pDoc->m_MainDataStack,iClust,pUp);
	}
	else
	{
		sprintf(msg,"Performing UD info-max clustering with %d clusters",iClust);
		pUp->SetProgress(msg,0);
		pDoc->m_MainClusters.ResistorInfoClust(pDoc->m_MainDataStack,iClust,pUp);
	}

	return true;
}

void CWClustView::OnClusterInfoclust()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	gNumClusts = 4;

	CDlgClustNum oDlg;
	INT_PTR ir = oDlg.DoModal();

	if(ir == IDCANCEL) return;

	if(gNumClusts<2)
	{
		MessageBox("Must use at least 2 clusters!!","Invalid # of clusters!!!",MB_ICONERROR);
		return;
	}

	//params for InfoClustThread
	InfoClustParams oParams;
	oParams.iClust = gNumClusts;
	oParams.bExclusive = true;
	oParams.pDoc = pDoc;

	//create progress monitor and thread for entropy reduction clustering
	CUPDialog o(this->GetSafeHwnd(),InfoClustThread,&oParams,"WClust - Exclusive info-max clustering",true);
	if(o.DoModal()==IDOK)
	{	//finished clustering
		pDoc->m_MainDataStack.whichDraw = CLUST_INFO;
		MessageBox("Finished ExclusiveInfoClust","WClust",MB_ICONINFORMATION);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		Invalidate();
	}
	else
	{	//clustering cancelled by user
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));;
		MessageBox("Cancelled exclusive info-max clustering","WClust",MB_ICONINFORMATION);
		pDoc->m_MainClusters.m_pNumClusts[CLUST_INFO] = 0;
	}
}

//params for k-means/medians clustering thread
struct KMeansParams
{
	CWClustDoc* pDoc;
	int iMinClust;
	int iMaxClust;
	int iIter;
	int iDBIter;
	bool bNormalize;
	bool bUseMedians;
};

//k-means/medians clustering thread
bool KMeansClustThread(const CUPDUPDATA* pUp)
{
	KMeansParams* pParams = (KMeansParams*) pUp->GetAppData();
	
	int iMinClust = pParams->iMinClust, iMaxClust = pParams->iMaxClust,
		iIter = pParams->iIter, iDBIter = pParams->iDBIter;
	
	CWClustDoc* pDoc = pParams->pDoc;
	
	bool bNormalize = pParams->bNormalize, bUseMedians = pParams->bUseMedians;

	pDoc->m_MainClusters.KmeansClust(pDoc->m_MainDataStack,iMinClust,iMaxClust,iIter,iDBIter,bNormalize,bUseMedians,pUp);

	return true;
}

void CWClustView::OnClusterK()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	gNumClusts = 4;

	KMDialog oDlg;

	INT_PTR ir = oDlg.DoModal();

	if(ir == IDCANCEL) return;

	int iMinClust = oDlg.MinClust();
	int iMaxClust = oDlg.MaxClust();
	int iIter = oDlg.Iters();
	int iDBIter = oDlg.DBIters();
	bool bNormalize = oDlg.Normalize();
	bool bUseMedians = oDlg.Medians();	

	if(iMinClust < 2 || iMaxClust < 2)
	{
		MessageBox("Must use at least 2 clusters!!","WClust - ERROR: Invalid # of clusters!!!",MB_ICONERROR);
		return;
	}
	else if(iIter < 1 || (iMinClust!=iMaxClust && iDBIter<1))
	{
		MessageBox("Must use positive # of iterations!!","WClust - ERROR: Invalid # of iterations!!!",MB_ICONERROR);
		return;
	}

	//set up params for k-means clustering thread
	KMeansParams oParams;
	oParams.iMinClust = iMinClust;
	oParams.iMaxClust = iMaxClust;
	oParams.iIter = iIter;
	oParams.iDBIter = iDBIter;
	oParams.bNormalize = bNormalize;
	oParams.bUseMedians = bUseMedians;
	oParams.pDoc = pDoc;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//create progress monitor and thread for entropy reduction clustering
	CUPDialog o(this->GetSafeHwnd(),KMeansClustThread,&oParams,"WClust - K-means/medians clustering",true);
	if(o.DoModal()==IDOK)
	{	//finished clustering
		pDoc->m_MainDataStack.whichDraw = CLUST_KM;

		if(bUseMedians)
			MessageBox("Finished K-medians Clustering","WClust - finished K-medians",0);
		else
			MessageBox("Finished K-means Clustering","WClust - finished K-means",0);

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		Invalidate();
	}
	else
	{	//clustering cancelled by user
		pDoc->m_MainClusters.m_pNumClusts[CLUST_KM] = 0;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));;
		if(bUseMedians)
			MessageBox("Cancelled K-medians clustering","WClust",MB_ICONINFORMATION);
		else
			MessageBox("Cancelled K-means clustering","WClust",MB_ICONINFORMATION);
	}
}

//does UD distance clustering
void CWClustView::OnClusterResistorinfoclust()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	gNumClusts = 4;

	CDlgClustNum oDlg;
	INT_PTR ir = oDlg.DoModal();

	if(ir == IDCANCEL) return;

	if(gNumClusts<2)
	{
		MessageBox("Must use at least 2 clusters!!","Invalid # of clusters!!!",MB_ICONERROR);
		return;
	}

	//params for InfoClustThread
	InfoClustParams oParams;
	oParams.iClust = gNumClusts;
	oParams.bExclusive = false;
	oParams.pDoc = pDoc;

	//create progress monitor and thread for entropy reduction clustering
	CUPDialog o(this->GetSafeHwnd(),InfoClustThread,&oParams,"WClust - UD info-max clustering",true);
	if(o.DoModal()==IDOK)
	{	//finished clustering
		pDoc->m_MainDataStack.whichDraw = CLUST_INFO;
		MessageBox("Finished UDInfoClust","WClust",MB_ICONINFORMATION);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		Invalidate();
	}
	else
	{	//clustering cancelled by user
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));;
		MessageBox("Cancelled UD info-max clustering","WClust",MB_ICONINFORMATION);
		pDoc->m_MainClusters.m_pNumClusts[CLUST_INFO] = 0;
	}
}

struct APClustParams
{
	CWClustDoc* pDoc;
	int iIters;
	int iConvIters;
	double dDamp;
	double dPref;
};

bool APClustThread(const CUPDUPDATA* pUp)
{
	APClustParams* pParams = (APClustParams*) pUp->GetAppData();

	CWClustDoc* pDoc = pParams->pDoc;
	int iIters = pParams->iIters;
	int iConvIters = pParams->iConvIters;
	double dDamp = pParams->dDamp;
	double dPref = pParams->dPref;

	pDoc->m_MainClusters.APClust(pDoc->m_MainDataStack,iIters,iConvIters,dDamp,dPref,pUp);

	return true;
}


void CWClustView::OnClusterApclust()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CAPClustDialog oD;
	INT_PTR ir = oD.DoModal();

	if(ir == IDCANCEL) return;

	int iIters = oD.GetNumIters();
	int iConvIters = oD.GetConvIters();
	double dDamp = oD.GetDamp();
	double dPref = oD.GetPref();

	if(iIters < 1)
	{
		MessageBox("Must have positive # of iterations!","WClust - error!",MB_ICONERROR);
		return;
	}

	if(iConvIters < 1)
	{
		MessageBox("Must have positive # of converge iterations!","WClust - error!",MB_ICONERROR);
		return;
	}

	if(dDamp < 0.5 || dDamp >= 1.0)
	{
		MessageBox("Damp factor must be >= 0.5 and < 1.0!","WClust - error!",MB_ICONERROR);
		return;
	}

	if(dPref<=0.0)
	{
		MessageBox("Preference factor must be positive!","WClust - error!",MB_ICONERROR);
		return;
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//params for APClustThread
	APClustParams oParams;
	oParams.iIters = iIters;
	oParams.iConvIters = iConvIters;
	oParams.dDamp = dDamp;
	oParams.dPref = dPref;
	oParams.pDoc = pDoc;

	//create progress monitor and thread for entropy reduction clustering
	CUPDialog o(this->GetSafeHwnd(),APClustThread,&oParams,"WClust - Affinity Propagation",true);
	if(o.DoModal()==IDOK)
	{	//finished clustering
		MessageBox("Finished APClust","WClust",MB_ICONINFORMATION);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		pDoc->m_MainDataStack.whichDraw = CLUST_AP;
		Invalidate();
	}
	else
	{
		MessageBox("Cancelled APClust","WClust",MB_ICONINFORMATION);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		pDoc->m_MainClusters.m_pNumClusts[CLUST_AP]=0;
	}
}

struct KKParams
{
	CWClustDoc* pDoc;
};

bool KKClustThread(const CUPDUPDATA* pUp)
{
	KKParams* pParams = (KKParams*) pUp->GetAppData();

	CWClustDoc* pDoc = pParams->pDoc;

	return pDoc->m_MainClusters.KlustaKwikClust(pDoc->m_MainDataStack,pUp);
}

void CWClustView::OnClusterKlustakwikclust()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//params for KKClustThread
	KKParams oParams;
	oParams.pDoc = pDoc;

	//create progress monitor and thread for klustakwik reduction clustering
	CUPDialog o(this->GetSafeHwnd(),KKClustThread,&oParams,"WClust - KlustaKwik clustering",true);
	if(o.DoModal()==IDOK)
	{	//finished clustering
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		pDoc->m_MainDataStack.whichDraw = CLUST_KK;
		Invalidate();
	}
	else
	{
		MessageBox("Couldn't run KlustaKwikClust!!","WClust",MB_ICONERROR);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		pDoc->m_MainClusters.m_pNumClusts[CLUST_KK]=0;
	}
}

void CWClustView::OnClusterUserspikex()
{
	// TODO: Add your command handler code here
}

void CWClustView::OnClusterUserspikexy()
{
	// TODO: Add your command handler code here
}

void CWClustView::OnUpdateClusterUseSpikexy(CCmdUI *pCmdUI)
{
	/*
	pCmdUI->Enable();

	CMenu* pMenu = GetMenu();

	pMenu->AssertValid();


	pCmdUI->DoUpdate(

	if(m_bClustSpikeXY)
	{
		pCmdUI->SetCheck(0);
		m_bClustSpikeXY=false;
	}
	else
	{
		pCmdUI->SetCheck(1);
		m_bClustSpikeXY=true;
	}
	*/
}

void CWClustView::OnClusterUseSpikexy()
{
	ShowXYSpikeDialog oD;
	if(IDOK==oD.DoModal())
	{
		CWClustDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->m_MainDataStack.m_iPosCollapse = oD.CollapseFactor();
		pDoc->m_MainDataStack.SetShowXYSpikes(oD.GetShowXYSpikes());
	}	
}

//params for entropy reduction clustering thread
struct EntropyParams
{
	CWClustDoc* pDoc;
	int iMinClust;
	int iMaxClust;
	int iIters;
	int iClustIters;
	vector<double>* pEntropy;
};

//entropy reduction clustering thread entry point
bool EntropyClustThread(const CUPDUPDATA* pCUPDUPData)
{
	EntropyParams* pParams = (EntropyParams*) pCUPDUPData->GetAppData();

	int iMinClust = pParams->iMinClust;
	int iMaxClust = pParams->iMaxClust;
	int iIters = pParams->iIters;
	int iClustIters = pParams->iClustIters;
	CWClustDoc* pDoc = pParams->pDoc;
	vector<double>* pEntropy = pParams->pEntropy;

	int iClusts = iMinClust;

	if(iMinClust == iMaxClust)
	{
		*pEntropy = pDoc->m_MainClusters.EntropyInfoClust(pDoc->m_MainDataStack,iClusts,iIters,pCUPDUPData);
		pCUPDUPData->SetProgress("Finishing EntropyInfoClust",100);
	}
	else
	{
		int iC;
		double dAvgEntrop = DBL_MAX;
		iClusts = iMinClust;
		for(iC=iMinClust;iC<=iMaxClust && !pCUPDUPData->ShouldTerminate();iC++)
		{
			*pEntropy = pDoc->m_MainClusters.EntropyInfoClust(pDoc->m_MainDataStack,iC,iClustIters,pCUPDUPData);
			double dTmp = Sum(*pEntropy) / iC;
			if(dTmp < dAvgEntrop)
			{
				dAvgEntrop = dTmp;
				iClusts = iC;
			}
		}
		if(!pCUPDUPData->ShouldTerminate())
		{
			char msg[1024];
			sprintf(msg,"Avg entropy for %d clusters was %f, using %d clusters for full %d iterations.",
				iClusts,dAvgEntrop,iClusts,iIters);
			pCUPDUPData->SetProgress(msg);
			Sleep(3000);//let user read
			*pEntropy = pDoc->m_MainClusters.EntropyInfoClust(pDoc->m_MainDataStack,iClusts,iIters,pCUPDUPData);
			pCUPDUPData->SetProgress("Finishing EntropyInfoClust",100);
		}
	}

	return true;
}

void CWClustView::OnClusterEntropyinfoclust()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	//get clustering parameters from user via dialog
	EClustDlg oDlg;
	INT_PTR ir = oDlg.DoModal();

	if(ir == IDCANCEL) return;

	int iMinClust = oDlg.MinClust(), iMaxClust = oDlg.MaxClust(),
		iIters = oDlg.Iters(), iClustIters = oDlg.ClustIters();

	if(iMinClust < 2 || iMaxClust < 2)
	{
		MessageBox("Must use at least 2 clusters!!","Invalid # of clusters!!!",MB_ICONERROR);
		return;
	}
	else if(iMinClust>iMaxClust)
	{
		MessageBox("MaxClust must be >= MinClust!!","Invalid # of clusters!!!",MB_ICONERROR);
		return;
	}
	else if(iIters < 1 || (iMinClust!=iMaxClust && iClustIters < 1))
	{
		MessageBox("Number of iterations must be > 0!!","Invalid # of iterations!!!",MB_ICONERROR);
		return;
	}

	vector<double> vcEntropy;

	//set up params for entropy clustering thread
	EntropyParams oParams;
	oParams.iIters = iIters;
	oParams.iMinClust = iMinClust;
	oParams.iMaxClust = iMaxClust;
	oParams.iClustIters = iClustIters;
	oParams.pDoc = pDoc;
	oParams.pEntropy = &vcEntropy;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//create progress monitor and thread for entropy reduction clustering
	CUPDialog o(this->GetSafeHwnd(),EntropyClustThread,&oParams,"WClust - Entropy reduction clustering",true);
	if(o.DoModal()==IDOK)
	{	//finished clustering
		pDoc->m_MainDataStack.whichDraw = CLUST_INFO;

		MessageBox("Finished EntropyInfoClust","WClust",MB_ICONINFORMATION);

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		Invalidate();

		char msg[1024];
		sprintf(msg,"Total entropy = %f, avg entropy = %f, num clusters = %d",Sum(vcEntropy),Sum(vcEntropy)/((double)vcEntropy.size()-1),vcEntropy.size()-1);
		MessageBox(msg,"WClust",MB_ICONINFORMATION);
	}
	else
	{	//clustering cancelled by user
		pDoc->m_MainClusters.m_pNumClusts[CLUST_INFO] = 0;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));;
		MessageBox("Cancelled EntropyInfoClust","WClust",MB_ICONINFORMATION);
	}
}
