// $Id: DlgEEGGain.h,v 1.2 2008/03/27 22:20:01 samn Exp $ 
#pragma once
#include "afxwin.h"
#include <vector>
#include <string>
#include "afxcmn.h"
#include "ColorComboBox.h"

using namespace std;

// DlgEEGGain dialog

class DlgEEGGain : public CDialog
{
	DECLARE_DYNAMIC(DlgEEGGain)

public:
	DlgEEGGain(CWnd* pParent,
		vector<double>& vEEGGains,
		vector<string>& vEEGNames,
		CPaletteStack* pPal);
	virtual ~DlgEEGGain();

	CString m_cstrEEGGain;
	vector<double> m_vEEGGains;
	vector<string> m_vEEGNames;

// Dialog Data
	enum { IDD = IDD_DIALOG_EEG_GAINS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CColorComboBox m_comboEEG;
	double m_dCurGain;
	afx_msg void OnDeltaposSpineeggain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboEegGainChan();
	CSpinButtonCtrl m_spinEEGGain;
	CString m_strCurGain;
	CEdit m_editEEGGain;
	afx_msg void OnEnKillfocusEditeeggain();
};
