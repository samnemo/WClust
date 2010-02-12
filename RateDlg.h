// $Id: RateDlg.h,v 1.3 2008/03/30 02:12:40 samn Exp $ 
#pragma once

#include "MyObj.h"
#include "Vertex.h"
#include "Cluster.h"
#include "ColorComboBox.h"
#include "ChooseClDlg.h"
#include "afxwin.h"

#define RATE_UNKNOWN 0
#define RATE_POOR 1
#define RATE_FAIR 2
#define RATE_GOOD 3

CString GetRatingString(vector<double>& vRatings);

// RateDlg dialog

class RateDlg : public CDialog
{
	DECLARE_DYNAMIC(RateDlg)

	bool m_bCancelled;

	bool m_bSkipPaint;

	void InitCombo();

	void CheckRatings();

public:
	RateDlg(CWnd* pParent = NULL);   // standard constructor
	RateDlg(CWnd* pParent,CVerxStack* pVerx,
		    CPaletteStack* pPal,CCluster* pClust);
	virtual ~RateDlg();

	void GetRatingString(CString& str);

	bool FinishedRating();
	bool Cancelled(){ return m_bCancelled; }

// Dialog Data
	enum { IDD = IDD_DLG_RATE };

	CVerxStack		*m_MainDataStack;
	CPaletteStack	*m_MainPalette;
	CCluster		*m_MainClusters;
	CChooseClDlg	*m_colDlg;
	CMiniView		*m_MyView;
	HCURSOR			m_HCross;
	CRect			m_rect, m_wndRect;
	CRect			m_DrawingRect, m_DrawingSpikesRect;
	vector<double>     m_vRatings;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(CEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckGood();
	CColorComboBox m_Combo;
	afx_msg void OnBnClickedCheckFair();
	afx_msg void OnBnClickedCheckPoor();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboRate();
	int m_radio;
	CButton m_CheckPoor;
	CButton m_CheckFair;
	CButton m_CheckGood;
};
