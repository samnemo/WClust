// $Id: DlgKlustaKwik.h,v 1.1 2009/06/24 18:04:45 samn Exp $ 
#pragma once
#include "afxwin.h"


// DlgKlustaKwik dialog

class DlgKlustaKwik : public CDialog
{
	DECLARE_DYNAMIC(DlgKlustaKwik)

public:
	DlgKlustaKwik(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgKlustaKwik();

// Dialog Data
	enum { IDD = IDD_DIALOG_KLUSTAKWIK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_wndMinClusts;
	CEdit m_wndMaxClusts;
	CEdit m_wndStartIters;
	int m_iMinClusts;
	int m_iMaxClusts;
	int m_iStartIters;
	afx_msg void OnBnClickedOk();
	CEdit m_wndMaxPossClust;
	int m_iMaxPossClusts;
};
