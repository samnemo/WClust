#pragma once


// CDlgClustNum dialog

class CDlgClustNum : public CDialog
{
	DECLARE_DYNAMIC(CDlgClustNum)

public:
	CDlgClustNum(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgClustNum();

// Dialog Data
	enum { IDD = IDD_DLG_CLUST_NUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
