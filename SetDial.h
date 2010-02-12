#if !defined(AFX_SETDIAL_H__9C590D20_9D4E_11D6_9179_80FFAF9A077C__INCLUDED_)
#define AFX_SETDIAL_H__9C590D20_9D4E_11D6_9179_80FFAF9A077C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetDial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetDial dialog

class CSetDial : public CDialog
{
public:
	char m_Type;
	int m_ClMode;
	
// Construction
public:
	CSetDial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetDial)
	enum { IDD = IDD_RANGEDIALOG };
	int		m_EdFirstEv;
	int		m_EdLastEv;
	int		m_EdPrecent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetDial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetDial)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETDIAL_H__9C590D20_9D4E_11D6_9179_80FFAF9A077C__INCLUDED_)
