#pragma once
#include "afxcmn.h"


// AutoCProgDlg dialog

class AutoCProgDlg : public CDialog
{
	DECLARE_DYNAMIC(AutoCProgDlg)

public:
	AutoCProgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AutoCProgDlg();

	BOOL Create(UINT nID, CWnd * pWnd){ 	m_bRunning = true; return CDialog::Create(nID,pWnd);}

	virtual BOOL OnInitDialog(){ CDialog::OnInitDialog(); 	m_bRunning = true; return TRUE; }

	virtual void PostNcDestroy() {delete this;}

// Dialog Data
	enum { IDD = IDD_DIALOG_PROG_AUTO_CLUST };

		bool m_bRunning;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support



	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_progBar;

	bool IsRunning(){ return m_bRunning; }

	void SetProgRange(int iLow,int iHigh,int iStep)
	{
		m_progBar.SetRange(iLow,iHigh);
		m_progBar.SetStep(iStep);
	}

	void StepProg()
	{
		int iLower,iUpper;
		m_progBar.GetRange(iLower,iUpper);
		m_progBar.SetPos((m_progBar.GetPos()+1)%iUpper);
	}

	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();
	afx_msg UINT OnGetDlgCode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
