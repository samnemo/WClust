#if !defined(AFX_DLGFILEINFO_H__6C38ECA0_4D97_11D8_99E5_D1D6C23C3766__INCLUDED_)
#define AFX_DLGFILEINFO_H__6C38ECA0_4D97_11D8_99E5_D1D6C23C3766__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFileInfo.h : header file
//
#include "MyObj.h"
#include "Vertex.h"
#include "Cluster.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgFileInfo dialog

class CDlgFileInfo : public CDialog
{
public:
	CVerxStack		*m_MainDataStack;
	CCluster		*m_MainClusters;
	MY_WCP_FILES_STACK	newParamsStack;
	
	
	// Construction
public:
	CDlgFileInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFileInfo)
	enum { IDD = IDD_DLG_INFO };
	CListBox	m_wndListUsed;
	CListBox	m_wndListFound;
	int		m_Arena;
	int		m_Sync;
	CString	m_FileName;
	int		m_Room;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFileInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFileInfo)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFILEINFO_H__6C38ECA0_4D97_11D8_99E5_D1D6C23C3766__INCLUDED_)
