// DlgAddWCP.cpp : implementation file
//

#include "stdafx.h"
#include "wclust.h"
#include "DlgAddWCP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddWCP dialog


CDlgAddWCP::CDlgAddWCP(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddWCP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddWCP)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgAddWCP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddWCP)
	DDX_Control(pDX, IDC_REFRESH, m_wndRefresh);
	DDX_Control(pDX, IDC_ADD_PARAM, m_wndAddParam);
	DDX_Control(pDX, IDC_LIST_POSSIBLE, m_listLeft);
	DDX_Control(pDX, IDC_LIST_CHOOSEN, m_listRight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddWCP, CDialog)
	//{{AFX_MSG_MAP(CDlgAddWCP)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_ADD_PARAM, OnAddParam)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_LBN_DBLCLK(IDC_LIST_POSSIBLE, OnDblclkListPossible)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddWCP message handlers

void CDlgAddWCP::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_MainDataStack->CheckWcpFiles();

	MY_WCP_FILES_STACK::iterator indFiles, indUsed;
	MY_WCP_FILES_STACK::iterator indLeft, indRight, indNotFound;
	for (indFiles = m_MainDataStack->wcpFilesStack.begin(); indFiles != m_MainDataStack->wcpFilesStack.end(); indFiles++)
	{
		indFiles->flag = 0; // not used
	}

	int iRight = 0;
	int iNotFound = 0;
	for (indUsed = m_MainClusters->usedExternWCP.begin(); indUsed != m_MainClusters->usedExternWCP.end(); indUsed++)
	{
		for (indFiles = m_MainDataStack->wcpFilesStack.begin(); indFiles != m_MainDataStack->wcpFilesStack.end(); indFiles++)
		{
			if ( indUsed->paramName.Compare(indFiles->paramName) == 0 )
			{	// parameter is USED
//				rightPanel.push_back( *indUsed );
//				(rightPanel.begin() + iRight)->flag = iRight;
//				iRight++;
		FILE *f = fopen("a.a","a+");
		fprintf(f,"%s %s\n", (LPCTSTR) indUsed->paramName, (LPCTSTR) indFiles->paramName);
		fclose(f);
				indFiles->flag = 1; // param is used, it will not be in left win
			}
/*			else
			{	// file with parameter is NOT FOUND
				paramNotFoundPanel.push_back( *indUsed );
				(paramNotFoundPanel.begin() + iNotFound)->flag = iNotFound;
				iNotFound++;
			}
*/		}
	}

	int iLeft = 0;
	for (indFiles = m_MainDataStack->wcpFilesStack.begin(); indFiles != m_MainDataStack->wcpFilesStack.end(); indFiles++)
	{
		if (indFiles->flag == 0)
		{
			leftPanel.push_back( *indFiles );
			(leftPanel.begin() + iLeft)->flag = iLeft;
			iLeft++;
		}
	}
		
	for (indLeft = leftPanel.begin(); indLeft != leftPanel.end(); indLeft++)
	{
		m_listLeft.AddString((LPCTSTR) indLeft->paramName);
	}
	
	for (indRight = rightPanel.begin(); indRight != rightPanel.end(); indRight++)
	{
		m_listRight.AddString((LPCTSTR) indRight->paramName);
	}

	if ( leftPanel.size() == 0 )
		m_wndAddParam.EnableWindow( false );
	m_wndRefresh.EnableWindow( false );
}

void CDlgAddWCP::OnAddParam() 
{
	int iShift = 0;
	int iLeft = 0;
	int iRight = m_listRight.GetCount();

	while (iLeft < m_listLeft.GetCount())
	{
		if ( m_listLeft.GetSel(iLeft) )
		{
			rightPanel.push_back( (sWcpFiles) *(leftPanel.begin() + iLeft) );
			(rightPanel.begin() + iRight)->flag = iRight;
			m_listRight.AddString( (LPCTSTR) (rightPanel.begin() + iRight)->paramName );
			iRight++;

			leftPanel.erase( leftPanel.begin() + iLeft );
			m_listLeft.DeleteString(iLeft);

			iShift++;
		}
		else
		{
			(leftPanel.begin() + iLeft )->flag -= iShift;
			iLeft++;
		}
	}
	if ( leftPanel.size() == 0 )
		m_wndAddParam.EnableWindow( false );
	Invalidate();
}

void CDlgAddWCP::OnOK() 
{


	MY_WCP_FILES_STACK::iterator indRight;
	for (indRight = rightPanel.begin(); indRight != rightPanel.end(); indRight++)
	{

		newParamsStack.push_back( *indRight );
	}
	CDialog::OnOK();
}

void CDlgAddWCP::OnRefresh() 
{
	
}

void CDlgAddWCP::OnCancel() 
{

	

	CDialog::OnCancel();
}

void CDlgAddWCP::OnDblclkListPossible() 
{
	int iShift = 0;
	int iLeft = 0;
	int iRight = m_listRight.GetCount();

	while (iLeft < m_listLeft.GetCount())
	{
		if ( m_listLeft.GetSel(iLeft) )
		{
			rightPanel.push_back( (sWcpFiles) *(leftPanel.begin() + iLeft) );
			(rightPanel.begin() + iRight)->flag = iRight;
			m_listRight.AddString( (LPCTSTR) (rightPanel.begin() + iRight)->paramName );
			iRight++;

			leftPanel.erase( leftPanel.begin() + iLeft );
			m_listLeft.DeleteString(iLeft);

			iShift++;
		}
		else
		{
			(leftPanel.begin() + iLeft )->flag -= iShift;
			iLeft++;
		}
	}
	if ( leftPanel.size() == 0 )
		m_wndAddParam.EnableWindow( false );
	Invalidate();
}
