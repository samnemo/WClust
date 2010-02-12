// $Id: VideoDlg.h,v 1.1 2008/03/29 03:14:14 samn Exp $ 
#pragma once

#include "cdxCSizingDialog.h"
#include "MyObj.h"
#include "AVIReader.h"

// VideoDlg dialog

class VideoDlg : public cdxCSizingDialog
{
	DECLARE_DYNAMIC(VideoDlg)

public:
	VideoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VideoDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_VID };


protected:

	AVIReader m_oAVIReader;
	CDC m_memDC;
	HDRAWDIB m_hdd;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//{{AFX_MSG(VideoDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPickVid();
};
