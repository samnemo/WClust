#if !defined(AFX_CHOOSECLDLG_H__7823CAF3_D7FA_472A_A17B_59A8FB57C2F1__INCLUDED_)
#define AFX_CHOOSECLDLG_H__7823CAF3_D7FA_472A_A17B_59A8FB57C2F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseClDlg.h : header file
//
#include "MyObj.h"
#include "Vertex.h"
#include "Cluster.h"
#include "cdxCSizingDialog.h"

#define WM_GOODBYE WM_USER + 5

/////////////////////////////////////////////////////////////////////////////
// CChooseClDlg dialog

class CChooseClDlg : public cdxCSizingDialog
{
public:
	char			*flag;
	vector<unsigned char>& toView;
	CVerxStack		*m_MainDataStack;
	CCluster		*m_MainClusters;
protected:
	CWnd	*m_parent;
	CImageList m_imageList;
	char	maxCluster;
	char	listFilled;
	void	FillImageList();
	void	FillClList(char select);
	
// Construction
public:
	BOOL	Create(CWnd *parent);
	CChooseClDlg(CWnd* pParent, std::vector<unsigned char>& toView_);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CChooseClDlg)
	enum { IDD = IDD_CHOOSE_VERX_DLG };
	CButton	m_wndOK;
	CListCtrl	m_wndListVerx;
	CListCtrl	m_wndListPnts;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseClDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseClDlg)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg void OnItemchangedListPnts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListVerx(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSECLDLG_H__7823CAF3_D7FA_472A_A17B_59A8FB57C2F1__INCLUDED_)
