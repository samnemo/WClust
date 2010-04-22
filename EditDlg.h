// $Id: EditDlg.h,v 1.3 2008/08/11 20:56:52 samn Exp $ 
#if !defined(AFX_EDITDLG_H__AF617A31_FF7C_4CD1_8ED6_B41F7B25934D__INCLUDED_)
#define AFX_EDITDLG_H__AF617A31_FF7C_4CD1_8ED6_B41F7B25934D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditDlg.h : header file
//
#include "cdxCSizingDialog.h"
#include "MyObj.h"
#include "Vertex.h"
#include "Cluster.h"
#include "EditSpikes.h"
#include "ColorComboBox.h"
#include "ChooseClDlg.h"
#include "afxwin.h"
#include "RateDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog

class CEditDlg : public cdxCSizingDialog
{
// Construction
public:
	CVerxStack		*m_MainDataStack;
	CPaletteStack	*m_MainPalette;
	CCluster		*m_MainClusters;
	CChooseClDlg	*m_colDlg;
	CMiniView		*m_MyView;
	HCURSOR			m_HCross;
	CRect			m_rect, m_wndRect;
	CRect			m_DrawingRect, m_DrawingSpikesRect;
	RateDlg*        m_pRDlg;

	bool m_bNoiseMode;//whether to set points in currently user drawn boundary as points
	
	char flagChoose;

	int m_FirstEdEv,m_LastEdEv,m_PercEdEv;	// vectors in the edit window
	int m_FirstSp,m_LastSp,m_PercSp;		// spikes in the small view
	int m_FirstEdSp,m_LastEdSp,m_PercEdSp;	// spikes in the editation mode

	int m_SpikeEdSize;

	int m_StartSpikes, m_StopSpikes, HeightSpike;

	int m_State;		//1-store data 	
	int m_StoringMode;   // 0-pravotociva, 1-levotociva
	int m_StoringBack;
	int m_DrawSpikes;
	int m_InDrawArea;
	int m_Dragging;
	int m_First;
	int m_IsVisible;
	int m_TwoLines;
	CPoint m_OldPoint;
	CPoint m_Origin;
	CPoint m_TempPoint;
	CPoint m_OrigClient;
	CBoundary *m_NewPoly;
	CProjection *m_NewProj;
	CPoint2D *m_NewPoint2D;
	CVect2D *m_NewVect2D;
protected:
	int	m_colDlgState;

protected:
	void RemPolyFromActualProj(int Selected);
	void SetActiveButton();
	void FittingView();
	bool UserCheating();
	void CheckComboClust();
public:
	CEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditDlg)
	enum { IDD = IDD_EDITDIALOG };
	CComboBox	m_wndCmbOrigNew;
	CButton	m_wndExpand;
	CButton	m_wndTimeEv;
	CButton	m_wndUndo;
	CColorComboBox	m_wndComboClust;
	CButton	m_wndList;
	CButton	m_wndInitView;
	CButton	m_wndCheckFit;
	CButton	m_wndSetSpikes;
	CSpinButtonCtrl	m_wndSpinX;
	CButton	m_wndFitView;
	CSpinButtonCtrl	m_wndSpinSpikes;
	CButton	m_wndSettings;
	CButton	m_wndDrawSpikes;
	CComboBox	m_wndComboPen;
	CButton	m_wndCopyCluster;
	CButton	m_wndZoomIn;
	CSpinButtonCtrl	m_wndSpinY;
	CButton	m_wndOK;
	CButton	m_wndRemovePoly;
	CButton	m_wndRemAllPoly;
	CButton	m_wndNewPoly;
	CButton	m_wndHidePoly;
	CButton	m_wndDrawPoly;
	CButton m_NoiseButton;
	CButton m_BtnRate;
	CButton m_BtnCInf;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNewpoly();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnZoomin();
	afx_msg void OnSelchangeComboclust();
	afx_msg void OnHidepoly();
	afx_msg void OnRemovepoly();
	afx_msg void OnRemallpoly();
	afx_msg void OnDrawpoly();
	afx_msg void OnCopycluster();
	virtual void OnOK();
	afx_msg void OnSelchangeCombopen();
	afx_msg void OnSettings();
	afx_msg void OnFitView();
	afx_msg void OnDrawSpikes();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSetSpikes();
	afx_msg void OnInitview();
	afx_msg void OnList();
	afx_msg void OnButUndo();
	afx_msg void OnTimeEv();
	afx_msg void OnExpand();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeCmbOrigNew();
	afx_msg void OnClose();
	afx_msg LRESULT OnGoodbye(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedMakeNoiseBtn();
	afx_msg void OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRate();
	afx_msg void OnBnClickedButtonCinf();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:




	friend class CWClustView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITDLG_H__AF617A31_FF7C_4CD1_8ED6_B41F7B25934D__INCLUDED_)
