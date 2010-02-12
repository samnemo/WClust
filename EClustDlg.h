// $Id: EClustDlg.h,v 1.3 2008/09/16 01:40:22 samn Exp $ 
#pragma once
#include "afxwin.h"


// EClustDlg dialog

class EClustDlg : public CDialog
{
	DECLARE_DYNAMIC(EClustDlg)

public:
	EClustDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~EClustDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ECLUST };

	int MinClust(){ return m_iMinClust; }
	int MaxClust(){ return m_iMaxClust; }
	int Iters(){ return m_iIters; }
	int ClustIters(){ return m_iClustIters; }
	double ErrPrct(){ return m_dErrPrct; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	int m_iMinClust;
	int m_iMaxClust;
	int m_iIters;
	int m_iClustIters;
	double m_dErrPrct;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_wndMinC;
	CEdit m_wndMaxC;
	CEdit m_wndNIter;
	CEdit m_wndNIterC;
	CEdit m_wndErrPrct;
};
