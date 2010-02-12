// $Id: DlgAutoDeNoise.h,v 1.3 2009/06/24 18:07:36 samn Exp $ 
#pragma once

#include "Vertex.h"
#include "afxwin.h"

// CDlgAutoDeNoise dialog

class CDlgAutoDeNoise : public CDialog
{
	DECLARE_DYNAMIC(CDlgAutoDeNoise)

public:
	CDlgAutoDeNoise(CWnd* pParent = NULL, CVerxStack* p = 0);   // standard constructor
	virtual ~CDlgAutoDeNoise();

// Dialog Data
	enum { IDD = IDD_DLG_AUTO_DENOISE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	double m_dHighSat;
	double m_dLowSat;

	void InitCombo();
	
	// Generated message map functions
	//{{AFX_MSG(CDlgAutoDeNoise)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CVerxStack* pDataStack;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboLowSat;
	CComboBox m_ComboHighSat;
	CComboBox m_ComboFracSat;
	CEdit m_wndMinSpikeAmp;
	double m_dMinSpikeAmp;
};
