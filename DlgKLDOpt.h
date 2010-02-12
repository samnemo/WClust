// $Id: DlgKLDOpt.h,v 1.10 2009/06/24 18:08:19 samn Exp $ 
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// DlgKLDOpt dialog

class DlgKLDOpt : public CDialog
{
	DECLARE_DYNAMIC(DlgKLDOpt)

public:
	DlgKLDOpt(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgKLDOpt();

// Dialog Data
	enum { IDD = IDD_DIALOG_KLD_DLG_OPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()
public:
	int m_iNNToFind;
	BOOL m_bFast;
	BOOL m_bInterKLD;
	BOOL m_bIsolationDist;
	BOOL m_bDoKLDiv;
	BOOL m_bFindBestDims;
	int m_iBestSliceD;
	BOOL m_bSilhouette;
	int m_iNumBestDims;
	CEdit m_editBestDims;
	CButton m_checkBestDims;
	afx_msg void OnBnClickedCheckFindBestDims();
	CButton m_btnSetNumBestDims;
	afx_msg void OnBnClickedButtonSetNumBestDims();
	CButton m_checkInter;
	CButton m_checkFast;
	afx_msg void OnBnClickedCheckInterKld();
	afx_msg void OnBnClickedCheckFastKld();
	CButton m_checkSilh;
	afx_msg void OnBnClickedCheckSilhouetteWidth();
	afx_msg void OnBnClickedRadio1dSlice();
	afx_msg void OnBnClickedRadio2dSlices();
	CButton m_radSlice1D;
	CButton m_radSlice2D;
	BOOL m_bNormKLD;
	BOOL m_bLRatio;
	BOOL m_bUseDefDims;
	CButton m_checkUseDefDims;
	afx_msg void OnBnClickedCheckKldDefaultDims();
};
