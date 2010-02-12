#if !defined(AFX_DLGWCPNOTFOUND_H__41613540_4B31_11D8_99E5_A0C524D9F764__INCLUDED_)
#define AFX_DLGWCPNOTFOUND_H__41613540_4B31_11D8_99E5_A0C524D9F764__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWcpNotFound.h : header file
//
#include "MyObj.h"
#include "Vertex.h"
#include "Cluster.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgWcpNotFound dialog

class CDlgWcpNotFound : public CDialog
{
public:
	CVerxStack		*m_MainDataStack;
	CCluster		*m_MainClusters;
	MY_WCP_FILES_STACK	notFoundStack;
	
public:	
	bool CheckWcp();
protected:
	void FillList();
	
// Construction
public:
	CDlgWcpNotFound(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CDlgWcpNotFound)
	enum { IDD = IDD_DLG_WCP_NOT_FOUND };
	CListBox	m_wndList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWcpNotFound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWcpNotFound)
	virtual void OnOK();
	afx_msg void OnRefresh();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWCPNOTFOUND_H__41613540_4B31_11D8_99E5_A0C524D9F764__INCLUDED_)
