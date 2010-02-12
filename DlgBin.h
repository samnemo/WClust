#pragma once


// CDlgBin dialog

class CDlgBin : public CDialog
{
	DECLARE_DYNAMIC(CDlgBin)

public:
	CDlgBin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBin();

// Dialog Data
	enum { IDD = IDD_BINDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClusterSetbins();
	afx_msg void OnBnClickedOk();
};
