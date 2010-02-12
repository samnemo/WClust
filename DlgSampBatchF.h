// $Id: DlgSampBatchF.h,v 1.1 2008/02/02 21:26:23 samn Exp $ 
#pragma once
#include "afxwin.h"


// CDlgSampBatchF dialog

class CDlgSampBatchF : public CDialog
{
	DECLARE_DYNAMIC(CDlgSampBatchF)

public:
	CDlgSampBatchF(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSampBatchF();

// Dialog Data
	enum { IDD = IDD_DIALOG_SAMPLE_BATCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDlgSampBatchF)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditSampBatchTxt;
};
