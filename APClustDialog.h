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

	int GetNumIters(){ return m_iNumIters; }
	int GetConvIters(){ return m_iConvIters; }
	double GetDamp(){ return m_dDamp; }
	double GetPref(){ return m_dPref; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	int m_iNumIters;
	int m_iConvIters;
	double m_dDamp;
	double m_dPref;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
