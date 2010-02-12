// $Id: WClustView.h,v 1.14 2009/06/27 20:02:08 samn Exp $ 
// WClustView.h : interface of the CWClustView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WCLUSTVIEW_H__17C5D815_0D6C_4450_9BA5_C0D78C6646FF__INCLUDED_)
#define AFX_WCLUSTVIEW_H__17C5D815_0D6C_4450_9BA5_C0D78C6646FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorComboBox.h"
#include "RateDlg.h"
#include "FileUtils.h"

class CWClustView : public CFormView
{
protected:
	CMiniView	m_MiniView[6];
	CRect		m_ViewRect,m_ColorRect;
	int			m_Redraw;
	int			MIN_WIN_X,MIN_WIN_Y;
	int			m_FirstEv,m_LastEv,m_PercEv;		// vectors in the main 6 views
	int			m_FirstEdEv,m_LastEdEv,m_PercEdEv;	// vectors in the edit window
	int			m_FirstSp,m_LastSp,m_PercSp;		// spikes in the small view
	int			m_FirstEdSp,m_LastEdSp,m_PercEdSp;	// spikes in the editation mode
	int			m_SpikeEdSize;
	
	int			AutoSaveClMinutes;

	RateDlg*    m_pRDlg;
	int        m_iBatchMode;

	void UpdateControlsFromDoc();
	void UpdateDocFromControls();
	void CheckRateDlg();
	bool RunQBatch(vector<BatchRec>& vqb);
	bool RunTSBatch(vector<BatchRec>& vqb);
	bool RunACBatch(vector<BatchRec>& vqb);
	bool DoLoadBPF();
	bool GetRatingString(CString& strRating);
	

protected: // create from serialization only
	CWClustView();
	DECLARE_DYNCREATE(CWClustView)

public:
	friend bool QBThread(const CUPDUPDATA* pUp);
	friend bool TSBThread(const CUPDUPDATA* pUp);
	friend bool ACBThread(const CUPDUPDATA* pUp);
	BatchRec m_oQB;
	void CallEditDlg();
	//{{AFX_DATA(CWClustView)
	enum { IDD = IDD_WCLUST_FORM };
	CComboBox	m_wndCmbOrigNew;
	CColorComboBox	m_wndComboClust;
	CButton	m_wndShow;
	CButton	m_wndHide;
	//}}AFX_DATA

// Attributes
public:
	CWClustDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWClustView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWClustView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWClustView)
	afx_msg void OnFileOpenbpfile();
	afx_msg void OnEditdlg();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNewclust();
	afx_msg void OnFileSaveclusters();
	afx_msg void OnFileLoadclusters();
	afx_msg void OnButSet();
	afx_msg void OnFileStoretobpfile();
	afx_msg void OnUpdateFileStoretobpfile(CCmdUI* pCmdUI);
	afx_msg void OnEnvironmentLoad();
	afx_msg void OnEnvironmentSave();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnButton1();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnSelchangeComboclust();
	afx_msg void OnHide();
	afx_msg void OnShow();
	afx_msg void OnLoadArena();
	afx_msg void OnLoadRoom();
	afx_msg void OnUpdateLoadArena(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLoadRoom(CCmdUI* pCmdUI);
	afx_msg void OnModeSwappolarity();
	afx_msg void OnUpdateModeSwappolarity(CCmdUI* pCmdUI);
	afx_msg void OnPosRegWin();
	afx_msg void OnUpdatePosRegWin(CCmdUI* pCmdUI);
	afx_msg void OnTimeEvWin();
	afx_msg void OnUpdateTimeEvWin(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeCmbOrigNew1();
	afx_msg void OnToolsInfo();
	afx_msg void OnUpdateToolsInfo(CCmdUI* pCmdUI);
	afx_msg void OnToolsExport();
	afx_msg void OnUpdateToolsExport(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClusterInfoclust();
	afx_msg void OnClusterK();
	afx_msg void OnClusterResistorinfoclust();
	afx_msg void OnClusterApclust();
	afx_msg void OnClusterKlustakwikclust();
	afx_msg void OnClusterUserspikex();
	afx_msg void OnClusterUserspikexy();
	afx_msg void OnUpdateClusterUseSpikexy(CCmdUI *pCmdUI);
	afx_msg void OnClusterUseSpikexy();
	afx_msg void OnClusterEntropyinfoclust();
	afx_msg void OnToolsAutoFindNoise();
	afx_msg void OnBatchQualityrun();
	afx_msg void OnBatchSamplebatchfile();
	afx_msg void OnClusterSetkldoptions();
	afx_msg void OnToolsExportspikes();
	afx_msg void OnClusterCalcclusterquality();
	afx_msg void OnToolsExporthqpc1();
	afx_msg void OnToolsImportspikes();
	afx_msg void OnBatchTimeStampAdjust();
	afx_msg void OnClusterFlameclustering();
	afx_msg void OnClusterSomclustering();
	afx_msg void OnBatchAutoclustering();
	afx_msg void OnToolsExportclusterbounds();
};

#ifndef _DEBUG  // debug version in WClustView.cpp
inline CWClustDoc* CWClustView::GetDocument()
   { return (CWClustDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCLUSTVIEW_H__17C5D815_0D6C_4450_9BA5_C0D78C6646FF__INCLUDED_)
