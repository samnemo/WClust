// $Id: KMDialog.h,v 1.2 2008/09/16 01:42:01 samn Exp $ 
#pragma once
#include "afxwin.h"


// KMDialog dialog

class KMDialog : public CDialog
{
	DECLARE_DYNAMIC(KMDialog)

public:
	KMDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KMDialog();

	int MinClust(){ return m_iMinClusts; }
	int MaxClust(){ return m_iMaxClusts; }
	int Iters(){ return m_iIters; }
	int DBIters(){ return m_iDBIters; }
	bool Normalize(){ return (bool) m_iNormalizeData; }
	bool Medians(){ return (bool) m_iUseMedians; }

// Dialog Data
	enum { IDD = IDD_KM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	int m_iMinClusts;
	int m_iMaxClusts;
	int m_iIters;
	int m_iDBIters;
	int m_iNormalizeData;
	int m_iUseMedians;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_winMinC;
	CEdit m_winMaxC;
	CEdit m_winNumIter;
	CEdit m_winDBIter;
	CButton m_btnNorm;
	CButton m_btnMed;
};
