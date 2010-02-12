#if !defined(AFX_DLGADDWCP_H__EAF8CF00_39E8_11D8_99E5_D2E647DB587B__INCLUDED_)
#define AFX_DLGADDWCP_H__EAF8CF00_39E8_11D8_99E5_D2E647DB587B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddWCP.h : header file
//
#include "MyObj.h"
#include "Vertex.h"
#include "Cluster.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAddWCP dialog

class CDlgAddWCP : public CDialog
{
public:
	CVerxStack		*m_MainDataStack;
	CCluster		*m_MainClusters;
	MY_WCP_FILES_STACK	newParamsStack;
	
protected:
	MY_WCP_FILES_STACK	leftPanel;
	MY_WCP_FILES_STACK	rightPanel;
	MY_WCP_FILES_STACK	paramNotFoundPanel;

// Construction
public:
	CDlgAddWCP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddWCP)
	enum { IDD = IDD_CHOOSE_WCP_DLG };
	CButton	m_wndRefresh;
	CButton	m_wndAddParam;
	CListBox	m_listLeft;
	CListBox	m_listRight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddWCP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAddWCP)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnAddParam();
	virtual void OnOK();
	afx_msg void OnRefresh();
	virtual void OnCancel();
	afx_msg void OnDblclkListPossible();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDWCP_H__EAF8CF00_39E8_11D8_99E5_D2E647DB587B__INCLUDED_)
