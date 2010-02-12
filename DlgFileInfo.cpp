// DlgFileInfo.cpp : implementation file
//

#include "stdafx.h"
#include "wclust.h"
#include "DlgFileInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFileInfo dialog


CDlgFileInfo::CDlgFileInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFileInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFileInfo)
	m_Arena = 0;
	m_Sync = 0;
	m_FileName = _T("");
	m_Room = 0;
	//}}AFX_DATA_INIT
}


void CDlgFileInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFileInfo)
	DDX_Control(pDX, IDC_LIST_USED, m_wndListUsed);
	DDX_Control(pDX, IDC_LIST_FOUND, m_wndListFound);
	DDX_Text(pDX, IDC_EDIT_ARENA, m_Arena);
	DDX_Text(pDX, IDC_EDIT_SYNC, m_Sync);
	DDX_Text(pDX, IDC_EDIT_FILE_NAME, m_FileName);
	DDX_Text(pDX, IDC_EDIT_ROOM, m_Room);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFileInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgFileInfo)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFileInfo message handlers

void CDlgFileInfo::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_FileName = CString( m_MainDataStack->m_FileName );
	m_Arena = m_MainDataStack->m_MainSyncStack.NoArena;
	m_Room = m_MainDataStack->m_MainSyncStack.NoRoom;
	m_Sync = m_MainDataStack->m_MainSyncStack.SyncStack.size();
	
	UpdateData(false);

	m_MainDataStack->CheckWcpFiles();

	// LEFT panel
	MY_WCP_FILES_STACK::iterator indWCP;
	for ( indWCP = m_MainDataStack->wcpFilesStack.begin(); indWCP != m_MainDataStack->wcpFilesStack.end(); indWCP++ )
	{
		m_wndListFound.AddString( (LPCTSTR) (*indWCP).paramName );
	}

	// RIGHT panel
	for ( indWCP = m_MainClusters->usedExternWCP.begin(); indWCP != m_MainClusters->usedExternWCP.end(); indWCP++ )
	{
		m_wndListUsed.AddString( (LPCTSTR) (*indWCP).paramName );
	}
}
