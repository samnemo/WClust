// $Id: DlgPositionReg.h,v 1.2 2008/01/23 23:37:37 samn Exp $ 
#if !defined(AFX_DLGPOSITIONREG_H__5502953C_B2E3_4234_8BAF_D4C9FE3019C1__INCLUDED_)
#define AFX_DLGPOSITIONREG_H__5502953C_B2E3_4234_8BAF_D4C9FE3019C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPositionReg.h : header file
//
#include "MyObj.h"
#include "Vertex.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgPositionReg dialog

class CDlgPositionReg : public CDialog
{
public:
	CVerxStack *m_MainDataStack;
	
// Construction
public:
	CDlgPositionReg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPositionReg)
	enum { IDD = IDD_POSITION_REG_DLG };
	float	m_Edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPositionReg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void InitUI();
	void DisableConst();

	char m_type;
	float m_param;

	// Generated message map functions
	//{{AFX_MSG(CDlgPositionReg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRadioSync();
	afx_msg void OnRadioConst();
	afx_msg void OnRadioOther();
	afx_msg void OnRadioWithout();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPOSITIONREG_H__5502953C_B2E3_4234_8BAF_D4C9FE3019C1__INCLUDED_)
