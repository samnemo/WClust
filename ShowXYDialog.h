#pragma once


// CAPClustDialog dialog

class CAPClustDialog : public CDialog
{
	DECLARE_DYNAMIC(CAPClustDialog)

public:
	CAPClustDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAPClustDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_APS };

	bool GetShowXYSpikes(){ return m_bShowXYSpikes; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	bool m_bShowXYSpikes;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
