// $Id: WClustView.cpp,v 1.40 2010/01/14 18:36:32 samn Exp $ 
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
#include "DlgAutoDeNoise.h"
#include "DlgSampBatchF.h"
#include "Log.h"
#include "DlgKLDOpt.h"
#include "DlgFlClust.h"
#include "DlgExpNotes.h"
#include "DlgKlustaKwik.h"

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
	ON_COMMAND(ID_TOOLS_AUTOFINDNOISE, OnToolsAutoFindNoise)
	ON_COMMAND(ID_BATCH_QUALITYRUN, OnBatchQualityrun)
	ON_COMMAND(ID_BATCH_SAMPLEBATCHFILE, OnBatchSamplebatchfile)
	ON_COMMAND(ID_CLUSTER_SETKLDOPTIONS, OnClusterSetkldoptions)
	ON_COMMAND(ID_TOOLS_EXPORTSPIKES, OnToolsExportspikes)
	ON_COMMAND(ID_CLUSTER_CALCCLUSTERQUALITY, OnClusterCalcclusterquality)
	ON_COMMAND(ID_TOOLS_EXPORTHQPC1, OnToolsExporthqpc1)
	ON_COMMAND(ID_TOOLS_IMPORTSPIKES, OnToolsImportspikes)
	ON_COMMAND(ID_BATCH_TIMESTAMPADJUST, OnBatchTimeStampAdjust)
	ON_COMMAND(ID_CLUSTER_FLAMECLUSTERING, OnClusterFlameclustering)
	ON_COMMAND(ID_CLUSTER_SOMCLUSTERING, OnClusterSomclustering)
	ON_COMMAND(ID_BATCH_AUTOCLUSTERING, OnBatchAutoclustering)
	ON_COMMAND(ID_TOOLS_EXPORTCLUSTERBOUNDS, OnToolsExportclusterbounds)
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

	m_iBatchMode = BATCH_OFF;
	
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
bool CWClustView::DoLoadBPF()
{
	CWClustDoc* pDoc = GetDocument();
	if(!m_iBatchMode) ASSERT_VALID(pDoc);

	CCluster& oC = pDoc->m_MainClusters;
	CVerxStack& oS = pDoc->m_MainDataStack;
	
	int index;
	long errOffset;
	char errChar;

	m_Redraw = 0;

	/////////////////////////////////////////
	/////////////////////////////////////////
	//should this be here?
	oC.Clear();
	oS.ClearHighDim();
	/////////////////////////////////////////
	/////////////////////////////////////////

	if(m_iBatchMode)m_oQB.bReadyToRun=false;

	int ErrorNumb=oS.LoadVerx(&errChar,&errOffset,m_iBatchMode?&m_oQB:0);
	m_Redraw = 1;
	
	switch (ErrorNumb)
	{
	case 0:
		if (!oS.AreData)
		{
			char pomT[80];
			sprintf(pomT,"No data loaded - single el. and stereode is not implemented yet.");
			CWnd::MessageBox(pomT,NULL,MB_ICONWARNING||MB_OK);
		}
		else
		{
			if ( oS.GetFakeSpikes() )
				CWnd::MessageBox(TEXT("Warning: File does not contain electrode data.\n2 virtual spikes were added to allow working with the file."),NULL,MB_OK|MB_ICONWARNING);
			oS.CheckWcpFiles();
			CDlgWcpNotFound dlg;
			dlg.m_MainClusters = &oC;
			dlg.m_MainDataStack = &oS;
			bool result = dlg.CheckWcp();

			if ( result == false )
			{
				dlg.DoModal();
				oS.CheckWcpFiles();
				bool result = dlg.CheckWcp();
			}

			MY_WCP_FILES_STACK::iterator indWcp;
			for ( indWcp = oC.usedExternWCP.begin(); indWcp != oC.usedExternWCP.end(); indWcp++ )
			{
				if ( (*indWcp).fileName.Compare( "-" ) == 0)
				{	// parameter is not found
					oS.PasteFileParamAsZeros();
				}
				else 
				{	// parameter is found
					oS.PasteFileParameter( &(*indWcp) );
				}
			}

			oC.RemParamZeros(&pDoc->m_MainDataStack);

//			pDoc->m_MainClusters.CalcParam(&pDoc->m_MainDataStack);
//			pDoc->m_MainDataStack.CalcMinMax();
//			pDoc->m_MainClusters.Clustering(&pDoc->m_MainDataStack);
									
			m_FirstEv = 1;
			m_LastEv = oS.GetNumVerx();
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
		
			oS.CalcViewVx(m_FirstEv,m_LastEv,m_PercEv);

			if (0 && oC.GetSwap())
			{	//swap turned off!!!
				//bad - hard coded 8
				//CVertex::Swap caused crash when reduced # of params from 17 to 12
				//couldn't find bug for hours :( :( :(
				//
				oC.ClearSwap();
				oC.SwapPolarity(&oS);
			}
			else
			{				
				oC.CalcParam(&oS);
				if(oS.GetAutoCheckNoise()) oS.CheckNoise();
				oS.CalcMinMax();
				oC.Clustering(&oS);
				
				//copy pca parameters to cluster
				oC.m_bUsePCA = oS.m_bUsePCA;
				oC.m_bCombinePCA = oS.m_bCombinePCA;
				oC.m_iNumPCsToStore = oS.m_iNumPCsToStore;
				oC.m_iPCAStartDim = oS.m_iPCAStartDim;
				oC.m_iPCAEndDim = oS.m_iPCAEndDim;
			}

			for (index=0; index<6; index++)
			{
				if ( m_MiniView[index].m_AxesX > oS.Dimension )
					m_MiniView[index].m_AxesX = oS.Dimension;
	
				if ( m_MiniView[index].m_AxesY > oS.Dimension )
					m_MiniView[index].m_AxesY = oS.Dimension;

				if ( m_MiniView[index].m_AxesX == m_MiniView[index].m_AxesY)
				{
					if (m_MiniView[index].m_AxesX>1)
						m_MiniView[index].m_AxesX -=1;
					else
						m_MiniView[index].m_AxesX +=1;
				}
			}
		}
		m_wndCmbOrigNew.SetCurSel(oS.whichDraw);

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
	
	Write2Log("Opened BPF file %s : channel=%d verx=%d verxall=%d prct=%.2f",
		oS.m_FileName,oS.m_ElectrodeChannel,oS.m_NumVerx,oS.m_NumVerxAll,
		oS.m_NumVerxAll ? 100.0*(double)oS.m_NumVerx/oS.m_NumVerxAll : 0.0);
	if(m_iBatchMode)m_oQB.bReadyToRun=true;
	return true;
}

// CWClustView message handlers
void CWClustView::OnFileOpenbpfile() 
{
	if(DoLoadBPF())
	{		
		CWClustDoc* pDoc = GetDocument();
		//CheckExpNotes(pDoc->m_MainDataStack.m_FileName.GetBuffer());
	}
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
	m_wndCmbOrigNew.AddString(TEXT("FLAME cl."));
	m_wndCmbOrigNew.SetCurSel(pDoc->m_MainDataStack.whichDraw);

	int ik=0,ii=0,iw=pDoc->m_MainDataStack.whichDraw;
	switch(iw)
	{
	case CLUST_KM:
	case CLUST_AP:
	case CLUST_INFO:
	case CLUST_KK:
	case CLUST_FL:
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
						pDoc->m_MainClusters.Draw(pDC,m_MiniView[i*2+j].m_DrawingArea,&m_MiniView[i*2+j].m_DataArea,CPoint(m_MiniView[i*2+j].m_AxesX,m_MiniView[i*2+j].m_AxesY),&m_MiniView[i*2+j].m_ClustToView[0],0);
						pDoc->m_MainDataStack.Draw(pDC,m_MiniView[i*2+j].m_DrawingArea,&m_MiniView[i*2+j].m_DataArea,CPoint(m_MiniView[i*2+j].m_AxesX,m_MiniView[i*2+j].m_AxesY),&m_MiniView[i*2+j].m_ClustToView[0],m_MiniView[i*2+j].m_PointsSize);
						break;
					case CLUST_ORIG:
						pDoc->m_MainDataStack.DrawOrig(pDC,m_MiniView[i*2+j].m_DrawingArea,&m_MiniView[i*2+j].m_DataArea,CPoint(m_MiniView[i*2+j].m_AxesX,m_MiniView[i*2+j].m_AxesY),&m_MiniView[i*2+j].m_ClustToView[0],m_MiniView[i*2+j].m_PointsSize);
						break;
					case CLUST_KM:
					case CLUST_INFO:
					case CLUST_AP:
					case CLUST_KK:
					case CLUST_FL:
					default:
						pDoc->m_MainDataStack.DrawAutoC(pDC,m_MiniView[i*2+j].m_DrawingArea,&m_MiniView[i*2+j].m_DataArea,CPoint(m_MiniView[i*2+j].m_AxesX,m_MiniView[i*2+j].m_AxesY),&m_MiniView[i*2+j].m_ClustToView[0],m_MiniView[i*2+j].m_PointsSize,pDoc->m_MainDataStack.whichDraw);
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

bool CWClustView::GetRatingString(CString& strRating)
{
	if(m_iBatchMode)
		strRating = ::GetRatingString(m_oQB.vRatings);
	else
	{
		CheckRateDlg();
		if(!m_pRDlg || !m_pRDlg->FinishedRating())
			return false;
		m_pRDlg->GetRatingString(strRating);
	}
	return true;
}

void CWClustView::OnFileSaveclusters() 
{
	CWClustDoc* pDoc = GetDocument();
	if(!m_iBatchMode) ASSERT_VALID(pDoc);

	CString strRating;	
	if(!GetRatingString(strRating))
	{
		MessageBox("You must rate all clusters before saving!","WClust",MB_ICONINFORMATION);
		return;
	}
	
	int m_ErrorNumb;
	
	//get info gain for each cluster as string to be written to header
	if(m_iBatchMode==BATCH_QUAL_ORIG)
		pDoc->m_MainDataStack.whichDraw = CLUST_ORIG;
	else
		pDoc->m_MainDataStack.whichDraw = CLUST_USER;
	CString strInfo = GetClusterInfoString(pDoc->m_MainDataStack,pDoc->m_MainClusters,GetSafeHwnd(),pDoc->m_MainDataStack.whichDraw);

	CString strExtra;
	strExtra += strRating;
	strExtra += strInfo;

	if(m_iBatchMode==BATCH_OFF)
	{
		m_ErrorNumb=pDoc->m_MainClusters.ChooseFileAndStore(strExtra);
	}
	else if(m_iBatchMode!=BATCH_TSA)
	{
		CString strTmp(m_oQB.outCL.c_str());
		m_ErrorNumb=pDoc->m_MainClusters.Store(strTmp,strExtra);
	}

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

void UpdatePCA(CCluster& oC,CVerxStack& oS)
{
	if(oC.m_bUsePCA)
	{
		if(oS.m_bUsePCA)
		{
			if(oS.m_bCombinePCA != oC.m_bCombinePCA ||
				oS.m_iNumPCsToStore != oC.m_iNumPCsToStore)
			{
				oS.RemovePCAAxes();
				oS.RemoveVertexPCA();
				oS.m_bCombinePCA = oC.m_bCombinePCA;
				oS.m_iNumPCsToStore = oC.m_iNumPCsToStore;
				oS.m_iPCAStartDim = oC.m_iPCAStartDim;
				if(oS.m_bCombinePCA)
					oS.m_iPCAEndDim = oS.m_iPCAStartDim+oS.m_iNumPCsToStore-1;
				else
					oS.m_iPCAEndDim = oS.m_iPCAStartDim+(oS.NUM_CHANNELS*oS.m_iNumPCsToStore)-1;
				TopDimPopPush oT(&oS);
				oS.DoVertexPCA();
				oS.AddPCAAxes();
				oS.BASE_DIMENSION += oS.NumPCADims();//so they dont get popped off in clearhighdim
			}
		}
		else
		{
			oS.m_bUsePCA = true;
			oS.m_bCombinePCA = oC.m_bCombinePCA;
			oS.m_iNumPCsToStore = oC.m_iNumPCsToStore;
			oS.m_iPCAStartDim = oC.m_iPCAStartDim;
			if(oS.m_bCombinePCA)
				oS.m_iPCAEndDim = oS.m_iPCAStartDim+oS.m_iNumPCsToStore-1;
			else
				oS.m_iPCAEndDim = oS.m_iPCAStartDim+(oS.NUM_CHANNELS*oS.m_iNumPCsToStore)-1;
			TopDimPopPush oT(&oS);
			oS.DoVertexPCA();
			oS.AddPCAAxes();
			oS.BASE_DIMENSION += oS.NumPCADims();//so they dont get popped off in clearhighdim
		}
	}
	else if(oS.m_bUsePCA)
	{	//once computed, may as well keep!! also avoids problem of
		//readjusting boundary IDs of dimensions that came AFTER
		//PCA dimensions
		//MessageBox(0,"removing vertex pca","removing vertex pca",0);
		//oS.m_bUsePCA = false;
		//oS.RemovePCAAxes();
		//oS.RemoveVertexPCA();
	}
}

void CWClustView::OnFileLoadclusters() 
{
	m_Redraw = 0;
	CWClustDoc* pDoc = GetDocument();
	if(!m_iBatchMode) ASSERT_VALID(pDoc);

	CCluster& oC = pDoc->m_MainClusters;
	CVerxStack& oS = pDoc->m_MainDataStack;

	CFile file;
	int errorNumb = oC.OpenFile(&file,&oS,m_iBatchMode?&m_oQB:0);
	CString cstrFile = file.GetFilePath();
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
			if(!m_iBatchMode)
				CWnd::MessageBox(errorText, NULL,MB_OK|MB_ICONERROR);			
			oC.Clear();
			oS.ClearHighDim();
			oC.Clustering(&oS);

			if(m_iBatchMode)m_oQB.bReadyToRun=false;
			Write2Log("Couldn't open cl file %s : error = %s",cstrFile,errorText);
		}
	}
	else 
	{
		if (oS.AreData)
		{	// BPF/UFF is loaded
			oS.CheckWcpFiles();
			CDlgWcpNotFound dlg;
			dlg.m_MainClusters = &oC;
			dlg.m_MainDataStack = &oS;
			bool result = dlg.CheckWcp();

			if ( result == false )
			{
				dlg.DoModal();
				oS.CheckWcpFiles();
				bool result = dlg.CheckWcp();
			}
			//check external params
			MY_WCP_FILES_STACK::iterator indWcp;
			for ( indWcp = oC.usedExternWCP.begin(); indWcp != oC.usedExternWCP.end(); indWcp++ )
			{
				if ( (*indWcp).fileName.Compare( "-" ) == 0)
				{	// parameter is not found
					oS.PasteFileParamAsZeros();
				}
				else 
				{	// parameter is found
					oS.PasteFileParameter( &(*indWcp) );
				}
			}
		}
		//do actual loading of cluster data from file
		errorNumb = oC.LoadData( &file, &oS );
		if ( errorNumb != 0 )
		{	// error in loading data
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
				oC.Clear();
				oS.ClearHighDim();
				oC.Clustering(&oS);

				if(m_iBatchMode)m_oQB.bReadyToRun=false;
				Write2Log("Couldn't open cl file %s : error = %s",cstrFile,errorText);
			}
		}
		else //success
		{
			oC.RemParamZeros(&oS);

			UpdatePCA(oC,oS);

			//calc new params loaded from .cl file
			oC.CalcParam(&oS);
			//calc new min,max of each dimension
			oS.CalcMinMax();
			//set drawing/calculation mode to CLUST_USER
			oS.whichDraw = CLUST_USER;
			//cluster vertices based on boundaries/projections just loaded
			oC.Clustering(&oS);

			// Axis Adaptation - make sure view axes are all within bounds of
			// data stack dimensions
			for (int index=0; index<6; index++)
			{
				if ( m_MiniView[index].m_AxesX > oS.Dimension )
					m_MiniView[index].m_AxesX = oS.Dimension;
				if ( m_MiniView[index].m_AxesY > oS.Dimension )
					m_MiniView[index].m_AxesY = oS.Dimension;
				if (m_MiniView[index].m_AxesX == m_MiniView[index].m_AxesY)
				{
					if (m_MiniView[index].m_AxesX>1)
						m_MiniView[index].m_AxesX -=1;
					else
						m_MiniView[index].m_AxesX +=1;
				}
			}

			Write2Log("Opened cl file %s",cstrFile);
		}
	}

	//force redraw
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
	if(!m_iBatchMode) ASSERT_VALID(pDoc);

	CVerxStack& oS = pDoc->m_MainDataStack;
	CCluster& oC = pDoc->m_MainClusters;

	if(!m_iBatchMode && oS.whichDraw != CLUST_USER)
	{	CString str;
		str.Format("Would you like to save %s clusters?",CStrClustNames[oS.whichDraw]);
		if(IDNO==MessageBox(str,"WClust",MB_YESNO|MB_ICONQUESTION))
			return;
	}
	
	// where store
	char pomStr[120];
	int result = 0;

	if(!m_iBatchMode)
	{
		sprintf(pomStr,"Overwrite %s (Yes) or create new BP file (No)?",oS.m_FileName);
		result = CWnd::MessageBox(pomStr,NULL,MB_YESNOCANCEL);

		if(result==IDCANCEL)
			return;
	}
	else result = IDYES;

	CString newFileName,newTmpName;

	CFile file, fileNew;
	
	if ( result == IDNO )
	{	// Create a copy
		auto_ptr<CFileDialog> dlg;
		if ( oS.GetFileType() == 1)
		{
			static char BASED_CODE szFilter[] = "Brain potential files (*.bpf)|*.bpf|All Files (*.*)|*.*||";
			dlg = auto_ptr<CFileDialog>(new CFileDialog(FALSE, "bpf", "*.bpf",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter));
		}
		if ( oS.GetFileType() == 2)
		{
			static char BASED_CODE szFilter[] = "Brain potential files (*.uff)|*.uff|All Files (*.*)|*.*||";
			dlg = auto_ptr<CFileDialog>(new CFileDialog(FALSE, "uff", "*.uff",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter));
		}

		if(dlg->DoModal()!=IDOK)
			return;

		// convert file name 
		newFileName = dlg->GetPathName();
		newTmpName = dlg->GetPathName();

		int i = dlg->GetPathName().GetLength();
		newTmpName.SetAt(i-3,'t');
		newTmpName.SetAt(i-2,'m');
		newTmpName.SetAt(i-1,'p');
		
		// test if it is not the same file
		if(!strcmp(oS.m_FileName,newFileName))
		{
			CWnd::MessageBox(TEXT("Error: You cannot do a copy of file with the same name"),NULL,MB_OK|MB_ICONERROR);
			return;
		}
		
		// open file to store
		if (!fileNew.Open(dlg->GetPathName(),CFile::modeCreate|CFile::modeWrite))
		{
			CWnd::MessageBox(TEXT("Error: Create new file"),NULL,MB_OK|MB_ICONERROR);
			return;
		}
		else
		{
			if (!file.Open(oS.m_FileName,CFile::modeRead))
			{
				CWnd::MessageBox(TEXT("Error: Open original file"),NULL,MB_OK|MB_ICONERROR);
				return;	
			}				
		}
	} //end NO
	else if ( result == IDYES )
	{	// Rewrite existing file
		
		if ( oS.GetFileType() == 1)
		{	// BPF
			if (!fileNew.Open(oS.m_TmpName,CFile::modeCreate|CFile::modeWrite))
			{
				CWnd::MessageBox(TEXT("Error: Create tmp file"),NULL,MB_OK|MB_ICONERROR);
				return;
			}
			else
			{
				newFileName = oS.m_TmpName;
				if (!file.Open(oS.m_FileName,CFile::modeRead))
				{
					CWnd::MessageBox(TEXT("Error: Open original file"),NULL,MB_OK|MB_ICONERROR);
					return;	
				}
			}
		}
	}//end YES

	/////////////////////////////////////// create a copy
			
	// first copy of header

	if ( oS.GetFileType() == 1)
	{	// BPF
		vector<char> buffer(16000);
		int nRead;
		if (!buffer.size())
		{	
			CWnd::MessageBox(TEXT("Error: Out of memory"),NULL,MB_OK|MB_ICONERROR);
			return;
		}
		else	
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
			
			fileNew.Close();
			
			FILE* strFile = fopen(newFileName,"w+");
			if(!strFile)
			{
				CWnd::MessageBox("Couldn't open file!","WClust",MB_OK);
				return;
			}

			char loadText[8392];
			char endSetup = 0, endRecordFormat = 0, endHeader = 0;
			char endSetupStr[512], endRecordFormatStr[512], endHeaderStr[512];
			int endSetupLen = 23, endRecordFormatLen = 31, endHeaderLen = 12;

			sprintf(endSetupStr,"%%%%END SETUP_INFORMATION"); //23
			sprintf(endRecordFormatStr,"%%%%END RECORD_FORMAT_INFORMATION");//31
			sprintf(endHeaderStr,"%%%%END_HEADER");//12
	
			long m_i = 0, m_oldI;
			int m_j;
			nRead=file.Read(&buffer[0],buffer.size());
			char whatToStore = oS.LoadedAdd & 0x3;
			if ( whatToStore > 0 )
			{
				if ( whatToStore > 0 ) // room || both
				{
					do
					{
						m_oldI = m_i;
						sscanfLine(&buffer[0],loadText,&m_i);
	
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
						sscanfLine(&buffer[0],loadText,&m_i);
	
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
						fprintf(strFile,"%%ArenaPositionRecordFormat.14 Identifier.1 100usTimestamp.4 ArenaX1.1 ");
						fprintf(strFile,"ArenaY1.1 ArenaZ1.1 ArenaAng.2 ArenaAngX.2 ArenaAngY.2\n");
					}				
				
					if ( whatToStore & 2 )
					{
						fprintf(strFile,"%%RoomPositionIdentifier.1 ('R')\n");
						fprintf(strFile,"%%RoomPositionRecordFormat.14 Identifier.1 100usTimestamp.4 RoomX1.1 ");
						fprintf(strFile,"RoomY1.1 RoomZ1.1 RoomAng.2 RoomAngX.2 RoomAngY.2\n");
					}
					fprintf(strFile,"%s",loadText);
				}	
			}
			do
			{
				sscanfLine(&buffer[0],loadText,&m_i);
				m_j=0;
				while (( endHeaderStr[m_j] == loadText[m_j]) && (m_j < endHeaderLen)) m_j++;
				if (m_j == endHeaderLen) endHeader = 1;
				if ( !endHeader ) fprintf(strFile,"%s",loadText);
			}while (!endHeader);
			fprintf(strFile,"%s",loadText);

			fclose(strFile);			

			fileNew.Open(newFileName.GetBuffer(),CFile::modeWrite);
			fileNew.Seek(0,CFile::end);

			file.Seek(m_i,CFile::begin);
				
			if ( oS.FileType == 1 && oS.GetDFileOK() == 0 )
			{	// repairing file - don't copy last error Record
				vector<char> buffer(65536);
				
				int readBytes = file.Read(&buffer[0],65536);
				m_i += readBytes;
				if ( m_i > oS.GetDLastPos() )
					readBytes -= m_i - oS.GetDLastPos();
				
				// main copy loop
				while (readBytes == 65536)
				{
					fileNew.Write(&buffer[0],readBytes);
					readBytes = file.Read(&buffer[0],65536);
					
					m_i += readBytes;
					if ( m_i > oS.GetDLastPos() )
						readBytes -= m_i - oS.GetDLastPos();					
				}
				fileNew.Write(&buffer[0],readBytes);
			}
			else
			{	// BPF is OK or UFF
				vector<char> buffer(65536);
				int readBytes = file.Read(&buffer[0],65536);
				while (readBytes == 65536)
				{
					fileNew.Write(&buffer[0],readBytes);
					readBytes = file.Read(&buffer[0],65536);
				}
				fileNew.Write(&buffer[0],readBytes);
			}
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}	

		if ( result == IDYES)
		{ // copy back and delete tmp

			fileNew.Close();
			file.Close();
			CFile::Remove(oS.m_FileName);
			CFile::Rename(oS.m_TmpName,oS.m_FileName);
		}
		else if ( result == IDNO )
		{
			oS.m_FileName = newFileName;			
			oS.m_TmpName = newTmpName;
			fileNew.Close();
			file.Close();
		}		
	}

	if ( oS.GetFileType() == 2)
	{	// UFF
		if ( result == IDYES)
		{ // copy back and delete tmp
			fileNew.Close();
			file.Close();
		}
		else if ( result == IDNO )
		{
			fileNew.Seek(0,CFile::end);
			file.Seek(0,CFile::begin);
			
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

			vector<char> buffer(65536);

			int readBytes = file.Read(&buffer[0],65536);
			while (readBytes == 65536)
			{
				fileNew.Write(&buffer[0],readBytes);
				readBytes = file.Read(&buffer[0],65536);
			}
			fileNew.Write(&buffer[0],readBytes);

			oS.m_FileName = newFileName;
			oS.m_TmpName = newTmpName;
			fileNew.Close();
			file.Close();
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
	}
	///////////////////// end create the copy

	if (oS.GetFileType() == 1)
	{	// store BPF
		MY_STACK::iterator IndexVertex;
		
		char	State;
		int		error = oS.PrepareStoreBPF();
		int flag = 5;
		
		if (error == 0)
		{
			while (flag == 5)
			{
				flag=oS.StoreBPF(&State,m_iBatchMode==BATCH_TSA);
				//,m_iBatchMode?(oS.m_SpikesStack.begin()+oS.GetForStoreBPFSpikeIndex())->TimeStamp:-1);
				if (flag == 5) // more spikes to write
				{
					if (oC.GetSwap())
						oS.forStoreBPF_NonClVx->Swap(oS.NUM_CHANNELS,oS.NUM_SAMPLES);
					//get spike's params and cluster it
					oC.CalcParamOneSpike( &oS, oS.forStoreBPF_NonClVx, oS.forStoreBPF_Spike );					
					oC.Clustering( oS.forStoreBPF_NonClVx );
					//store the cluster
					(oS.m_SpikesStack.begin()+oS.GetForStoreBPFSpikeIndex())->cluster=oS.forStoreBPF_NonClVx->GetClust();
					//write the cluster to file
					oS.StoreBPFElde();
				}
			}
			// if arena or room data are not included in BPF
			oS.StoreBPFAddition();
			oS.FinishStoreBPF();
			MessageBeep(0);
		}			
	}
	else if (oS.GetFileType() == 2)
	{	// store UFF
		int ErrorNumb = oS.StoreClMarks();
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
	
			char pomStr[20];
			int nRead, errorNo=0;
			char* buffer = new char[8192];
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
				delete [] buffer;
				
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
										file.Read(&m_MiniView[index].m_ClustToView[0],sizeof(unsigned char)*100);
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
				file.Write(&m_MiniView[i].m_ClustToView[0], sizeof(unsigned char)*100);
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
			else if ( error == 2 )
				CWnd::MessageBox(TEXT("Error: Structure of arena file"),NULL,MB_ICONWARNING||MB_OK);
			else if ( error == 3 )
				CWnd::MessageBox(TEXT("Error: insufficient transitions in sync file. Can't load arena positions."),NULL,MB_ICONERROR);
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
			CWnd::MessageBox(TEXT("Error: Cannot open room data file"),NULL,MB_ICONWARNING||MB_OK);
		}
		else
		{
			int error = pDoc->m_MainDataStack.m_MainSyncStack.LoadRoomFromDAT(fptr);
			fclose(fptr);
			if ( error == 0 )
				pDoc->m_MainDataStack.LoadedAdd |= 2;
			else if ( error == 2 )
				CWnd::MessageBox(TEXT("Error: Structure of position file"),NULL,MB_ICONWARNING||MB_OK);
			else if ( error == 3 )
				CWnd::MessageBox(TEXT("Error: insufficient transitions in sync file. Can't load room positions."),NULL,MB_ICONERROR);
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

	static CDlgPositionReg dlg;
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

	vector<string> vEEGNames = pDoc->m_MainDataStack.GetEEGNames();
	
	CTimeEv dlg(0,&vEEGNames,&pDoc->m_MainDataStack,&pDoc->m_MainPalette,&pDoc->m_MainClusters);

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
	if(!m_iBatchMode)
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

				//don't get info gain for each cluster - only when user explicitly saves .cl file
				pDoc->m_MainDataStack.whichDraw = CLUST_USER;
				CString strInfo(""); 
				pDoc->m_MainClusters.AutoSave(fileNameTmpCl,strInfo);
			}
			break;
		}
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

	CVerxStack& oS = pDoc->m_MainDataStack;
	if(oS.whichDraw != CLUST_USER)
	{	CString str;
		str.Format("Would you like to export %s clusters?",CStrClustNames[oS.whichDraw]);
		if(IDNO==MessageBox(str,"WClust",MB_YESNO|MB_ICONQUESTION))
			return;
	}
	
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
		//pDoc->m_MainDataStack.whichDraw = CLUST_USER;
		CString strInfo = GetClusterInfoString(oS,pDoc->m_MainClusters,GetSafeHwnd(),oS.whichDraw);
		oS.CreateHeaderForExport(fileTXT,strInfo);
		
		MY_SPIKE_STACK::iterator indexSp;

		int iV = 0;//index into reassignment vector, if it is needed at all...
	
		for (indexSp = oS.m_SpikesStack.begin(); indexSp != oS.m_SpikesStack.end(); indexSp++, iV++)
		{
			if ( indexSp->type == 1 ) // spike is in memory
			{
				CVertex* verx = (CVertex*)*(oS.m_VerxStack.begin() + indexSp->offset);
				int iC = oS.LookupVClust(iV,oS.whichDraw);
				verx->ExportValues(fileTXT,&iC);
			}
#if 0
			else	// spike is on disk
			{
				CCluster& oC = pDoc->m_MainClusters;
				
				CVertex* verx = (CVertex*) new CVertex(&pDoc->m_MainPalette);
				oS.LoadBPFElde(verx, &fileBPF, indexSp->offset );
			
				if (oC.GetSwap())
				{
					verx->Swap(oS.NUM_CHANNELS, oS.NUM_SAMPLES);
				}
				if(oS.m_bUsePCA) oS.AppendPCAProjections(verx);
				oC.CalcParamOneSpike( &oS, verx, &*indexSp );
				if(oS.GetAutoCheckNoise())
				{	//make sure spike on disk is checked for noise
					verx->CheckNoise(oS.NUM_CHANNELS,oS.NUM_SAMPLES);
				}
				oC.Clustering( verx );
				int iC = oS.LookupVClust(iV,oS.whichDraw);
				verx->ExportValues(fileTXT,&iC);

				delete verx;
			}
#endif
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
		//pDoc->m_MainClusters.ExclusiveInfoClust(pDoc->m_MainDataStack,iClust,pUp);
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

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-clustering!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}
	
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
		//MessageBox("Finished ExclusiveInfoClust","WClust",MB_ICONINFORMATION);
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

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-clustering!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}
	
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

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-clustering!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}
	
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

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-clustering!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}

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

struct FLParams
{
	CWClustDoc* pDoc;
	int DistT;
	int Steps;
	float Eps;
	float KNNPrct;
	float OutTH;
};

bool FLClustThread(const CUPDUPDATA* pUp)
{
	FLParams* pParams = (FLParams*) pUp->GetAppData();
	CWClustDoc* pDoc = pParams->pDoc;
	return pDoc->m_MainClusters.FLAMEClust(pDoc->m_MainDataStack,pParams->DistT,pParams->Steps,pParams->Eps,pParams->KNNPrct,pParams->OutTH,pUp);
}

void CWClustView::OnClusterFlameclustering()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-clustering!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}

	CDlgFLClust oDlg; oDlg.m_cmbDistT.SetPalette(&pDoc->m_MainPalette);
	if(oDlg.DoModal()!=IDOK)
		return;

	//params for KKClustThread
	FLParams oParams;
	oParams.pDoc = pDoc;

	oParams.Steps = oDlg.m_iIter;
	if(oParams.Steps < 1)
	{
		CWnd::MessageBox("Invalid # of steps specified -- must be > 0!","WClust - error",MB_ICONERROR);
		return;
	}
	oParams.Eps = oDlg.m_fEps;
	if(oParams.Eps <= 0.0)
	{
		CWnd::MessageBox("Invalid epsilon -- must be > 0.0!","WClust - error",MB_ICONERROR);
		return;
	}
	oParams.KNNPrct = oDlg.m_fKNNPrct;
	if(oParams.KNNPrct <= 0.0 || oParams.KNNPrct > 1.0)
	{
		CWnd::MessageBox("Invalid KNNPrct specified -- must be > 0.0 and <= 1.0!","WClust - error",MB_ICONERROR);
		return;
	}
	oParams.OutTH = oDlg.m_fOutTH;
	if(oParams.OutTH <= 0.0)
	{
		CWnd::MessageBox("Invalid Outlier Threshold specified -- must be > 0.0!","WClst - error",MB_ICONERROR);
		return;
	}
	oParams.DistT = oDlg.m_iDistT;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//create progress monitor and thread for klustakwik reduction clustering
	CUPDialog o(this->GetSafeHwnd(),FLClustThread,&oParams,"WClust - F.L.A.M.E. Clustering",true);
	if(o.DoModal()==IDOK)
	{	//finished clustering
		pDoc->m_MainDataStack.whichDraw = CLUST_FL;
		Invalidate();
	}
	else
	{
		MessageBox("Couldn't run F.L.A.M.E. Clustering!!","WClust",MB_ICONERROR);
		pDoc->m_MainClusters.m_pNumClusts[CLUST_FL]=0;
	}
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}


struct KKParams
{
	CWClustDoc* pDoc;
	int iMinClusts;
	int iMaxClusts;
	int iStartIters;
	int iMaxPossClusts;
};

bool KKClustThread(const CUPDUPDATA* pUp)
{
	KKParams* pParams = (KKParams*) pUp->GetAppData();

	CWClustDoc* pDoc = pParams->pDoc;

	return pDoc->m_MainClusters.KlustaKwikClust(pDoc->m_MainDataStack,pParams->iMinClusts,pParams->iMaxClusts,pParams->iStartIters,pParams->iMaxPossClusts,pUp);
}

void CWClustView::OnClusterKlustakwikclust()
{
	char pomT[80];

	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-clustering!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}

	DlgKlustaKwik oDlg;
	if(oDlg.DoModal()!=IDOK)
		return;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//params for KKClustThread
	KKParams oParams;
	oParams.pDoc = pDoc;
	oParams.iMinClusts = oDlg.m_iMinClusts;
	oParams.iMaxClusts = oDlg.m_iMaxClusts;
	oParams.iStartIters = oDlg.m_iStartIters;
	oParams.iMaxPossClusts = oDlg.m_iMaxPossClusts;

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
	vector<hprob>* pEntropy;
	double dErrPrct;
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
	vector<hprob>* pEntropy = pParams->pEntropy;
	CVerxStack& oS = pDoc->m_MainDataStack;
	CCluster& oC = pDoc->m_MainClusters;
	double dErrPrct = pParams->dErrPrct;

	int iClusts = iMinClust;

	if(iMinClust == iMaxClust)
	{
		*pEntropy = pDoc->m_MainClusters.EntropyInfoClust(pDoc->m_MainDataStack,iClusts,iIters,pCUPDUPData,dErrPrct);
		 ///**pEntropy =*/ pDoc->m_MainClusters.EntropyInfoClustKDTree(pDoc->m_MainDataStack,iClusts,iIters,pCUPDUPData);
		pCUPDUPData->SetProgress("Finishing EntropyInfoClust",100);
	}
	else
	{
		CQOpts opts , tmpopts = oC.m_oCQO;
		oC.m_oCQO = opts; //set cluster quality options to default (inter,bg kld , in 8 dims with 2D slices to pick dims)
		int iC;
		double dAvgKLDBG = -1. , dAvgKLDInter = -1.;
		iClusts = iMinClust;
		for(iC=iMinClust;iC<=iMaxClust && !pCUPDUPData->ShouldTerminate();iC++)
		{
			*pEntropy = pDoc->m_MainClusters.EntropyInfoClust(pDoc->m_MainDataStack,iC,iClustIters,pCUPDUPData,dErrPrct);
			
			Write2Log("eval info gain for %d clusters",iC);
			oS.whichDraw = CLUST_INFO;
			oC.CalcClusterInfo(oS,true);
			double dTmpAvgBG = 0. , dTmpAvgInter = 0.; int k;
			for(k=1;k<=iC;k++)
			{
				dTmpAvgBG += oC.m_vInfo[CLUST_INFO][k].m_fBGInfoGain;
				dTmpAvgInter += oC.m_vInfo[CLUST_INFO][k].m_fInterClustGain;
			}
			dTmpAvgBG /= iC;
			dTmpAvgInter /= iC;
			Write2Log("%d clusters. avg bgkld=%g. avg interkld=%g.",iC,dTmpAvgBG,dTmpAvgInter);
			if(dTmpAvgBG > dAvgKLDBG && dTmpAvgInter > dAvgKLDInter)
			{
				dAvgKLDBG = dTmpAvgBG; 
				dAvgKLDInter = dTmpAvgInter;
				iClusts = iC;
			}
		}
		oC.m_oCQO = tmpopts; //restore options as set by user
		if(!pCUPDUPData->ShouldTerminate())
		{
			char msg[1024];
			sprintf(msg,"Using %d clusters: avg bgkld=%g , avginterkld=%g",iClusts,dAvgKLDBG,dAvgKLDInter);
			pCUPDUPData->SetProgress(msg);
			Write2Log(msg);
			Sleep(3000);//let user read
			*pEntropy = pDoc->m_MainClusters.EntropyInfoClust(pDoc->m_MainDataStack,iClusts,iIters,pCUPDUPData,dErrPrct);
			pCUPDUPData->SetProgress("Finishing EntropyInfoClust",100);
		}
	}

	return true;
}

void CWClustView::OnClusterEntropyinfoclust()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-clustering!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}
	
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

	double dErrPrct = oDlg.ErrPrct();

	vector<hprob> vcEntropy;

	//set up params for entropy clustering thread
	EntropyParams oParams;
	oParams.iIters = iIters;
	oParams.iMinClust = iMinClust;
	oParams.iMaxClust = iMaxClust;
	oParams.iClustIters = iClustIters;
	oParams.pDoc = pDoc;
	oParams.pEntropy = &vcEntropy;
	oParams.dErrPrct = dErrPrct;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//create progress monitor and thread for entropy reduction clustering
	CUPDialog o(this->GetSafeHwnd(),EntropyClustThread,&oParams,"WClust - Entropy reduction clustering",true);
	if(o.DoModal()==IDOK)
	{	//finished clustering
		pDoc->m_MainDataStack.whichDraw = CLUST_INFO;

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		Invalidate();

		char msg[1024];
		sprintf(msg,"Finished EntropyInfoClust: Total entropy = %f, avg entropy = %f, num clusters = %d",Sum(vcEntropy),Sum(vcEntropy)/((double)vcEntropy.size()-1),vcEntropy.size()-1);
		MessageBox(msg,"WClust",MB_ICONINFORMATION);
	}
	else
	{	//clustering cancelled by user
		pDoc->m_MainClusters.m_pNumClusts[CLUST_INFO] = 0;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));;
		MessageBox("Cancelled EntropyInfoClust","WClust",MB_ICONINFORMATION);
	}
}

void CWClustView::OnToolsAutoFindNoise()
{
	// TODO: Add your command handler code here
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-find-noise!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}

	CDlgAutoDeNoise oD(0,&pDoc->m_MainDataStack);
	oD.DoModal();
}

void CWClustView::OnBatchQualityrun()
{
	// TODO: Add your command handler code here
	m_iBatchMode = BATCH_QUAL_USER;
	
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	char strMsg[1024];

	pDoc->m_MainDataStack;
	pDoc->m_MainClusters;

	CVerxStack oS;
	CCluster oC;

	static char BASED_CODE szFilter[] = "Batch text file (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "txt", "*.txt",OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()!=IDOK)
	{
		m_iBatchMode = BATCH_OFF;
		return;
	}

	CString batFileName = dlg.GetPathName();
	vector<string> vstr;
	if(!GetLines(batFileName.GetBuffer(),vstr))
	{
		sprintf(strMsg,"Couldn't open %s batch file for reading!",batFileName);
		MessageBox(strMsg,"WClust",MB_ICONERROR);
		m_iBatchMode = BATCH_OFF;
		return;
	}
	vector<BatchRec> vqb;
	if(!ParseQBatchLines(vstr,vqb,m_iBatchMode))
	{
		m_iBatchMode=BATCH_OFF;
		MessageBox("Invalid batch file format!","WClust - error!",MB_ICONERROR);
		return;
	}

	Write2Log("Starting measure cluster quality batch run file %s",batFileName.GetBuffer());

	if(!RunQBatch(vqb))
		MessageBox("Couldn't run cluster quality measure batch!","WClust",MB_ICONERROR);

	m_iBatchMode = BATCH_OFF;
}

struct BParams
{
	CWClustView* pClustView;
	vector<BatchRec>* pvb;
	string strOutLog;
};

bool QBThread(const CUPDUPDATA* pUp)
{
	BParams* pParams = (BParams*) pUp->GetAppData();

	CWClustView* pClustView = pParams->pClustView;
	vector<BatchRec>& vqb = *pParams->pvb;

	char msg[1024]={0};
	sprintf(msg,"Running measure cluster quality batch.");

	pUp->SetProgress(msg,0);

	CWClustDoc* pDoc = pClustView->GetDocument();	

	int iB = 0 , iSz = vqb.size();
	for(iB=0;iB<iSz && !pUp->ShouldTerminate();iB++)
	{
		sprintf(msg,"Running measure cluster quality batch job %d of %d",iB+1,iSz);
		
		try
		{	//make sure progress control doesn't screw up
			pUp->SetProgress(msg,100*(iB+1)/iSz);
		}
		catch(...)
		{}
		
		Write2Log(msg);
		BatchRec& oQB = vqb[iB];
		pClustView->m_oQB = oQB;

		Write2Log("bpf=%s cl=%s tetrode=%d prctp=%.2f outcl=%s",oQB.strBPF.c_str(),oQB.strCL.c_str(),
			oQB.iTetrode,oQB.dPrct,oQB.outCL.c_str());

		try
		{
			pClustView->DoLoadBPF();
			if(pClustView->m_oQB.bReadyToRun)
			{
				pClustView->OnFileLoadclusters();
				if(pClustView->m_oQB.bReadyToRun)
					pClustView->OnFileSaveclusters();
				else
					Write2Log("Batch error: Couldn't load CL file %s",oQB.strCL.c_str());
			}
			else
				Write2Log("Batch error: Couldn't load BPF file %s",oQB.strBPF.c_str());
		}
		catch(...)
		{
			Write2Log("Exception processing batch job %d of %d",iB+1,iSz);
		}
	}		

	return true;
}

bool CWClustView::RunQBatch(vector<BatchRec>& vqb)
{
	//params for QBThread
	BParams oParams;
	oParams.pClustView = this;
	oParams.pvb = &vqb;

	try
	{
		//create progress monitor and thread for entropy reduction clustering
		CUPDialog o(this->GetSafeHwnd(),QBThread,&oParams,"WClust - measure cluster quality batch",true);
		if(o.DoModal()==IDOK)
		{
			Write2Log("Finished measure cluster quality batch job.");
			return true;
		}
		Write2Log("Terminated measure cluster quality batch job.");
		return false;
	}
	catch(...)
	{
		MessageBox("Exception in batch!","WClust - error!",MB_ICONERROR);
		return false;
	}
}

bool TSBThread(const CUPDUPDATA* pUp)
{
	BParams* pParams = (BParams*) pUp->GetAppData();

	CWClustView* pClustView = pParams->pClustView;
	vector<BatchRec>& vb = *pParams->pvb;

	char msg[1024]={0};
	sprintf(msg,"Running TimeStamp Adjust Batch.");

	pUp->SetProgress(msg,0);

	CWClustDoc* pDoc = pClustView->GetDocument();	

	int iB = 0 , iSz = vb.size();
	for(iB=0;iB<iSz && !pUp->ShouldTerminate();iB++)
	{
		sprintf(msg,"Running TimeStamp Adjust Batch Job %d of %d",iB+1,iSz);
		
		try
		{	//make sure progress control doesn't screw up
			pUp->SetProgress(msg,100*(iB+1)/iSz);
		}
		catch(...)
		{}
		
		Write2Log(msg);
		BatchRec& oB = vb[iB];
		oB.dPrct = 1.0;

		if(!CopyFile(oB.strBPF.c_str(),oB.outBPF.c_str(),false))
		{
			Write2Log("Couldn't create %s output file!",oB.outBPF.c_str());
			continue;
		}
		else
		{
			oB.strBPF = oB.outBPF;
			Write2Log("Created BPF output file %s",oB.outBPF.c_str());
		}

		int MaxTetrode = 1;

		for(oB.iTetrode=0;oB.iTetrode < MaxTetrode;oB.iTetrode++)
		{ 
			Write2Log("bpf=%s tetrode=%d outbpf=%s",oB.strBPF.c_str(),oB.iTetrode,oB.outBPF.c_str());

			pClustView->m_oQB = oB;

			sprintf(msg,"Running AutoClustering Batch Job %d of %d : T%d",iB+1,iSz,oB.iTetrode+1);
			
			try
			{	//make sure progress control doesn't screw up
				pUp->SetProgress(msg,100*(iB+1)/iSz);
			}
			catch(...)
			{}

			try
			{
				pClustView->DoLoadBPF();
				MaxTetrode = pDoc->m_MainDataStack.NumTetrodeChans(); //only valid after load

				if(pClustView->m_oQB.bReadyToRun)
				{	
					if(!pDoc->m_MainDataStack.AdjustTimeStamps())
						Write2Log("Batch error: Couldn't adjust time stamps for file %s tetrode %d!",oB.strBPF.c_str(),oB.iTetrode);
					else 
					{
						pDoc->m_MainDataStack.whichDraw = CLUST_ORIG;
						pClustView->OnFileStoretobpfile();
					}
				}
				else
					Write2Log("Batch error: Couldn't load BPF file %s tetrode %d",oB.strBPF.c_str(),oB.iTetrode);
			}
			catch(...)
			{
				Write2Log("Exception processing batch job %d of %d",iB+1,iSz);
			}
		}
	}		

	return true;
}

bool CWClustView::RunTSBatch(vector<BatchRec>& vqb)
{
	//params for TSBThread
	BParams oParams;
	oParams.pClustView = this;
	oParams.pvb = &vqb;

	try
	{
		//create progress monitor and thread for entropy reduction clustering
		CUPDialog o(this->GetSafeHwnd(),TSBThread,&oParams,"WClust - TimeStamp Adjust Batch",true);
		if(o.DoModal()==IDOK)
		{
			Write2Log("Finished TimeStamp Adjust Batch Job.");
			return true;
		}
		Write2Log("Terminated TimeStamp Adjust Batch Job.");
		return false;
	}
	catch(...)
	{
		MessageBox("Exception in batch!","WClust - error!",MB_ICONERROR);
		return false;
	}
}

void CWClustView::OnBatchSamplebatchfile()
{
	// TODO: Add your command handler code here
	CDlgSampBatchF oD;
	oD.DoModal();
}

void CWClustView::OnClusterSetkldoptions()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CCluster& oC = pDoc->m_MainClusters;
	CVerxStack& oS = pDoc->m_MainDataStack;

	// TODO: Add your command handler code here
	DlgKLDOpt oD;
	oD.m_bFast = oC.m_oCQO.m_bFastKLD;
	oD.m_iNNToFind = oC.m_oCQO.m_iNNToFind;
	oD.m_bInterKLD = oC.m_oCQO.m_bInterKLD;
	oD.m_bDoKLDiv = oC.m_oCQO.m_bDoKLDiv;
	oD.m_bIsolationDist = oC.m_oCQO.m_bIsolationDist;
	oD.m_bFindBestDims = oC.m_oCQO.m_bFindBestDims;
	oD.m_iNumBestDims = oC.m_oCQO.m_iBestDims;
	oD.m_bSilhouette = oC.m_oCQO.m_bSilhouette;
	oD.m_iBestSliceD = oC.m_oCQO.m_b1DBest ? 1 : 2;
	oD.m_bNormKLD = oS.m_bNormFloatV;
	oD.m_bLRatio = oC.m_oCQO.m_bLRatio;
	oD.m_bUseDefDims = oC.m_oCQO.m_bUseDefDims;
	if(oD.DoModal()==IDOK)
	{
		oC.m_oCQO.m_bFastKLD = oD.m_bFast;
		oC.m_oCQO.m_iNNToFind = oD.m_iNNToFind;
		oC.m_oCQO.m_bInterKLD = oD.m_bInterKLD;
		oC.m_oCQO.m_bIsolationDist = oD.m_bIsolationDist;
		oC.m_oCQO.m_bDoKLDiv = oD.m_bDoKLDiv;
		oC.m_oCQO.m_iBestDims = oD.m_iNumBestDims;
		oC.m_oCQO.m_bFindBestDims = oD.m_bFindBestDims;
		oC.m_oCQO.m_bSilhouette = oD.m_bSilhouette;
		oC.m_oCQO.m_b1DBest = oD.m_iBestSliceD == 1 ? true : false;
		oS.m_bNormFloatV = oD.m_bNormKLD;
		oC.m_oCQO.m_bLRatio = oD.m_bLRatio;
		oC.m_oCQO.m_bUseDefDims = oD.m_bUseDefDims;
	}
}

void CWClustView::OnToolsExportspikes()
{
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CVerxStack& oS = pDoc->m_MainDataStack;
	if(oS.IsEmpty())
	{
		MessageBox("Must load BPF file before exporting spikes!","WClust - error",MB_ICONERROR);
		return;
	}

	static char BASED_CODE szFilter[] = "Text files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog oF(FALSE, "txt", "*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if(oF.DoModal()==IDCANCEL)
		return;	
	CString strFile = oF.GetPathName();	
	
	bool bUpsample=MessageBox("Would you like to upsample the spikes?", "WClust", MB_YESNO) == IDYES;	

	if(!oS.ExportSpikes(strFile.GetBuffer(),bUpsample))
	{
		CString strTmp;
		strTmp.Format("Couldn't export spikes to %s!",strFile);
		MessageBox(strTmp,"WClust - error",MB_ICONERROR);
	}
}

void CWClustView::OnClusterCalcclusterquality()
{
	// TODO: Add your command handler code here
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_MainClusters.CalcClusterInfo(pDoc->m_MainDataStack,true);
}

void CWClustView::OnToolsExporthqpc1()
{
	// TODO: Add your command handler code here
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CVerxStack& oS = pDoc->m_MainDataStack;
	if(oS.IsEmpty())
	{
		MessageBox("Must load BPF file before exporting HQ PC1+Energy!","WClust - error",MB_ICONERROR);
		return;
	}

	static char BASED_CODE szFilter[] = "Text files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog oF(FALSE, "txt", "*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if(oF.DoModal()==IDCANCEL)
		return;

	CString strFile = oF.GetPathName();
	if(!oS.ExportHQPC1Energy(strFile.GetBuffer()))
	{
		CString strTmp;
		strTmp.Format("Couldn't export spikes to %s!",strFile);
		MessageBox(strTmp,"WClust - error",MB_ICONERROR);
	}
}

void CWClustView::OnToolsImportspikes()
{
	// TODO: Add your command handler code here
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CVerxStack& oS = pDoc->m_MainDataStack;
	CCluster& oC = pDoc->m_MainClusters;
	
	int index;
	long errOffset;
	char errChar;

	//get waveform .txt file path
	static char BASED_CODE szFilter[] = "waveform text files (*.txt)|*.txt;|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "txt", "*.txt;",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if(dlg.DoModal()!=IDOK) return;
	CString cstrTXTPath = dlg.GetPathName();

	oC.Clear();

	char msg[1024];

	if(!oS.ImportSpikes(cstrTXTPath.GetBuffer()))
	{	sprintf(msg,"Couldn't import spikes from %s!",cstrTXTPath);
		return;
	}

	if(!oS.GetNumVerx())
	{	sprintf(msg,"File %s contains 0 spikes!",cstrTXTPath);
		MessageBox(msg,"WClust - error",MB_ICONERROR);
		return;
	}
	
	//finalize parameters and data structures

	oC.RemParamZeros(&pDoc->m_MainDataStack);
							
	m_FirstEv = 1;
	m_LastEv = oS.GetNumVerx();
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

	oS.CalcViewVx(m_FirstEv,m_LastEv,m_PercEv);
	
	oC.CalcParam(&oS);
	if(oS.GetAutoCheckNoise()) oS.CheckNoise();
	oS.CalcMinMax();
	oC.Clustering(&oS);
	
	//copy pca parameters to cluster
	oC.m_bUsePCA = oS.m_bUsePCA;
	oC.m_bCombinePCA = oS.m_bCombinePCA;
	oC.m_iNumPCsToStore = oS.m_iNumPCsToStore;
	oC.m_iPCAStartDim = oS.m_iPCAStartDim;
	oC.m_iPCAEndDim = oS.m_iPCAEndDim;

	for (index=0; index<6; index++)
	{
		if ( m_MiniView[index].m_AxesX > oS.Dimension )
			m_MiniView[index].m_AxesX = oS.Dimension;

		if ( m_MiniView[index].m_AxesY > oS.Dimension )
			m_MiniView[index].m_AxesY = oS.Dimension;

		if ( m_MiniView[index].m_AxesX == m_MiniView[index].m_AxesY)
		{
			if (m_MiniView[index].m_AxesX>1)
				m_MiniView[index].m_AxesX -=1;
			else
				m_MiniView[index].m_AxesX +=1;
		}
		m_MiniView[index].DoSize();
	}
	m_wndCmbOrigNew.SetCurSel(oS.whichDraw);
	CEditDlg EditDlg;
	EditDlg.m_MainClusters=&pDoc->m_MainClusters;
	EditDlg.m_MainDataStack=&pDoc->m_MainDataStack;
	EditDlg.m_MainPalette=&pDoc->m_MainPalette;
	int whichView;
	for (int i=0;i<6;i++)
	{	whichView=i;
		EditDlg.m_MyView=&m_MiniView[whichView];
		EditDlg.OnFitView();
	}
	Invalidate();
}

void CWClustView::OnBatchTimeStampAdjust()
{
	// TODO: Add your command handler code here
	m_iBatchMode = BATCH_TSA;
	
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	char strMsg[1024];

	CVerxStack oS;
	CCluster oC;

	static char BASED_CODE szFilter[] = "Batch text file (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "txt", "*.txt",OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()!=IDOK)
	{
		m_iBatchMode = BATCH_OFF;
		return;
	}

	CString batFileName = dlg.GetPathName();
	vector<string> vstr;
	if(!GetLines(batFileName.GetBuffer(),vstr))
	{
		sprintf(strMsg,"Couldn't open %s batch file for reading!",batFileName);
		MessageBox(strMsg,"WClust",MB_ICONERROR);
		m_iBatchMode = BATCH_OFF;
		return;
	}
	vector<BatchRec> vb;
	if(!ParseTSBatchLines(vstr,vb))
	{
		m_iBatchMode=BATCH_OFF;
		MessageBox("Invalid batch file format!","WClust - error!",MB_ICONERROR);
		return;
	}

	Write2Log("Starting TimeStamp Adjust batch run file %s",batFileName.GetBuffer());

	if(!RunTSBatch(vb))
		MessageBox("Couldn't run TimeStamp Adjust batch!","WClust",MB_ICONERROR);

	m_iBatchMode = BATCH_OFF;
}

struct SOMParams
{
	CWClustDoc* pDoc;
	int iIter;
	double dTau;
	int iSXGrid;
	int iSYGrid;
	bool bNormalize;
};

bool SOMClustThread(const CUPDUPDATA* pUp)
{
	SOMParams* pParams = (SOMParams*) pUp->GetAppData();
	int iIter = pParams->iIter, iSXGrid = pParams->iSXGrid,
		iSYGrid = pParams->iSYGrid;
	bool bNormalize = pParams->bNormalize;
	double dTau = pParams->dTau;
	CWClustDoc* pDoc = pParams->pDoc;
	pDoc->m_MainClusters.SOMClust(pDoc->m_MainDataStack,iIter,bNormalize,iSXGrid,iSYGrid,dTau,pUp);
	return true;
}

void CWClustView::OnClusterSomclustering()
{
	// TODO: Add your command handler code here
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc->m_MainDataStack.AreData)
	{	char pomT[80];
		sprintf(pomT,"No data loaded - can't perform auto-clustering!");
		CWnd::MessageBox(pomT,"WClust",MB_ICONERROR);
		return;
	}

	SOMParams oParams;
	oParams.bNormalize = false;
	oParams.iSXGrid = 20;
	oParams.iSYGrid = 20;
	oParams.dTau = 10.0;
	oParams.iIter = 2000;
	oParams.pDoc = pDoc;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//create progress monitor and thread for entropy reduction clustering
	CUPDialog o(this->GetSafeHwnd(),SOMClustThread,&oParams,"WClust - SOM Clustering",true);

	if(o.DoModal()==IDOK)
	{	//finished clustering
		pDoc->m_MainDataStack.whichDraw = CLUST_KM;
		MessageBox("Finished SOM-Clustering","WClust - finished SOM-Clustering",0);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		Invalidate();
	}
	else
	{	//clustering cancelled by user
		pDoc->m_MainClusters.m_pNumClusts[CLUST_KM] = 0;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));;
		MessageBox("Cancelled SOM-Clustering","WClust",MB_ICONINFORMATION);
	}
}

void CWClustView::OnBatchAutoclustering()
{
	// TODO: Add your command handler code here
	m_iBatchMode = BATCH_AUTO_CLUST;
	
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	char strMsg[1024];

	CVerxStack oS;
	CCluster oC;

	static char BASED_CODE szFilter[] = "Batch text file (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "txt", "*.txt",OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()!=IDOK)
	{
		m_iBatchMode = BATCH_OFF;
		return;
	}

	CString batFileName = dlg.GetPathName();
	vector<string> vstr;
	if(!GetLines(batFileName.GetBuffer(),vstr))
	{
		sprintf(strMsg,"Couldn't open %s batch file for reading!",batFileName);
		MessageBox(strMsg,"WClust",MB_ICONERROR);
		m_iBatchMode = BATCH_OFF;
		return;
	}
	vector<BatchRec> vb;
	if(!ParseACBatchLines(vstr,vb))
	{
		m_iBatchMode=BATCH_OFF;
		MessageBox("Invalid batch file format!","WClust - error!",MB_ICONERROR);
		return;
	}

	Write2Log("Starting AutoClustering batch run file %s",batFileName.GetBuffer());

	if(!RunACBatch(vb))
		MessageBox("Couldn't run AutoClustering batch!","WClust",MB_ICONERROR);

	m_iBatchMode = BATCH_OFF;
}

bool ACBThread(const CUPDUPDATA* pUp)
{
	BParams* pParams = (BParams*) pUp->GetAppData();

	CWClustView* pClustView = pParams->pClustView;
	vector<BatchRec>& vb = *pParams->pvb;

	char msg[1024]={0};
	sprintf(msg,"Running AutoClustering Batch.");

	pUp->SetProgress(msg,0);

	CWClustDoc* pDoc = pClustView->GetDocument();	

	int iB = 0 , iSz = vb.size();
	for(iB=0;iB<iSz && !pUp->ShouldTerminate();iB++)
	{
		sprintf(msg,"Running AutoClustering Batch Job %d of %d",iB+1,iSz);
		
		try
		{	//make sure progress control doesn't screw up
			pUp->SetProgress(msg,100*(iB+1)/iSz);
		}
		catch(...)
		{}
		
		Write2Log(msg);
		BatchRec& oB = vb[iB];
		oB.dPrct = 1.0;

		if(!CopyFile(oB.strBPF.c_str(),oB.outBPF.c_str(),false))
		{
			Write2Log("Couldn't create %s output file!",oB.outBPF.c_str());
			continue;
		}
		else
		{
			oB.strBPF = oB.outBPF;
			Write2Log("Created BPF output file %s",oB.outBPF.c_str());
		}

		int MaxTetrode = 1;

		for(oB.iTetrode=0;oB.iTetrode < MaxTetrode;oB.iTetrode++)
		{ 
			Write2Log("bpf=%s tetrode=%d outbpf=%s",oB.strBPF.c_str(),oB.iTetrode,oB.outBPF.c_str());

			pClustView->m_oQB = oB;

			sprintf(msg,"Running AutoClustering Batch Job %d of %d : T%d",iB+1,iSz,oB.iTetrode+1);
			
			try
			{	//make sure progress control doesn't screw up
				pUp->SetProgress(msg,100*(iB+1)/iSz);
			}
			catch(...)
			{}

			try
			{
				pClustView->DoLoadBPF();
				pDoc->m_MainDataStack.whichDraw = oB.iACType; 
				MaxTetrode = pDoc->m_MainDataStack.NumTetrodeChans(); //only valid after load

				if(pClustView->m_oQB.bReadyToRun)
				{	
					try
					{ 
						if(oB.iACType==CLUST_KM)
						{
							pDoc->m_MainClusters.KmeansClust(pDoc->m_MainDataStack,oB.iMinClust,oB.iMaxClust,oB.iIters,oB.iDBIters,true,false,pUp);
							pClustView->OnFileStoretobpfile();
						}
						else if(oB.iACType==CLUST_KK)
						{
							if(pDoc->m_MainClusters.KlustaKwikClust(pDoc->m_MainDataStack,oB.iMinClust,oB.iMaxClust,oB.iIters,oB.iMaxKKClust,pUp))
								pClustView->OnFileStoretobpfile();
							else
								Write2Log("Batch error: Couldnt perform KlustaKwik clustering for %s tetrode %d!",oB.strBPF.c_str(),oB.iTetrode);
						}						
					}
					catch(...){
						Write2Log("Batch error: Couldn't cluster file %s tetrode %d!",oB.strBPF.c_str(),oB.iTetrode);
					}					
				}
				else
					Write2Log("Batch error: Couldn't load BPF file %s tetrode %d",oB.strBPF.c_str(),oB.iTetrode);
			}
			catch(...)
			{
				Write2Log("Exception processing batch job %d of %d",iB+1,iSz);
			}
		}
	}		

	return true;
}

bool CWClustView::RunACBatch(vector<BatchRec>& vqb)
{
	//params for ACBThread
	BParams oParams;
	oParams.pClustView = this;
	oParams.pvb = &vqb;

	try
	{
		//create progress monitor and thread for auto-clustering batch
		CUPDialog o(this->GetSafeHwnd(),ACBThread,&oParams,"WClust - AutoClustering Batch",true);
		if(o.DoModal()==IDOK)
		{
			Write2Log("Finished AutoClustering Batch Job.");
			return true;
		}
		Write2Log("Terminated AutoClustering Batch Job.");
		return false;
	}
	catch(...)
	{
		MessageBox("Exception in batch!","WClust - error!",MB_ICONERROR);
		return false;
	}
}

//save user-defined cluster boundaries to text file
void CWClustView::OnToolsExportclusterbounds()
{
	// TODO: Add your command handler code here
	CWClustDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CVerxStack& oS = pDoc->m_MainDataStack;
	if(oS.IsEmpty()) //has a BPF file been loaded?
	{
		MessageBox("Must load BPF file before exporting cluster boundaries!","WClust - error",MB_ICONERROR);
		return;
	}

	CCluster& oC = pDoc->m_MainClusters;
	int iNumClusts = pDoc->m_MainClusters.GetCount() - 1;
	if(iNumClusts<1)//are there any clusters apart from backround distrib?
	{
		MessageBox("No user-defined clusters found!","WClust - error",MB_ICONERROR);
		return;
	}
	//select output text file
	static char BASED_CODE szFilter[] = "Text files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog oF(FALSE, "txt", "*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if(oF.DoModal()==IDCANCEL)
		return;
	char msg[1024];

	CString strF = oF.GetFileName();

	FILE* fp = fopen(strF.GetBuffer(),"w");
	if(!fp)
	{	sprintf(msg,"Couldn't open %s for writing!",strF.GetBuffer());
		MessageBox(msg,"WClust - error",MB_ICONERROR);
		return;
	}	
	fprintf(fp,"cluster projection point xaxis yaxis x y\n");//header for easy import into NQS
	int iC=0;
	CBoundary *Bndr;
	MY_STACK::iterator bindex;
	//each entry in m_BndrStack is for 1 cluster
	for(bindex=oC.m_BndrStack.begin();bindex!=oC.m_BndrStack.end();bindex++,iC++)
	{	Bndr=(CBoundary*) *bindex;
		if (Bndr->GetNumb()!=0) //are there any projections?
		{	MY_STACK::iterator pindex;
			int iProj = 0;//each projection is in a different 2D slice
			for(pindex=Bndr->m_ProjStack.begin();pindex!=Bndr->m_ProjStack.end();pindex++,iProj++)
			{	CProjection* Proj = (CProjection*) *pindex;
				MY_STACK::iterator nindex;
				int iPoint = 0;//go through the points that define the projection
				for(nindex=Proj->m_Pnt2DStack.begin();nindex!=Proj->m_Pnt2DStack.end();nindex++,iPoint++)
				{	CPoint2D*  p2D = (CPoint2D*) *nindex;
					fprintf(fp,"%d %d %d %d %d %g %g\n",iC,iProj,iPoint,Proj->m_AxesX,Proj->m_AxesY,p2D->m_X,p2D->m_Y);
				}
			}
		}
	}
	fclose(fp);
}
