// $Id: TimeEv.h,v 1.9 2008/04/24 23:41:34 samn Exp $ 
#if !defined(AFX_TIMEEV_H__EF9E6A1A_8E9D_48EB_A46F_55720AA3334B__INCLUDED_)
#define AFX_TIMEEV_H__EF9E6A1A_8E9D_48EB_A46F_55720AA3334B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeEv.h : header file
//
#include "cdxCSizingDialog.h"
#include "MyObj.h"
#include "Vertex.h"
#include "Cluster.h"
#include "ColorComboBox.h"
#include "AVIReader.h"

/////////////////////////////////////////////////////////////////////////////
// CTimeEv dialog

class CTimeEv : public cdxCSizingDialog
{
public:
	CVerxStack		*m_MainDataStack;
	CPaletteStack	*m_MainPalette;
	CCluster		*m_MainClusters;
	char			ShowType; // 0 - raster, 1 - spike/raster, 2 - spike

protected:

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////// AVI related ////////////////////////////////////////////
	AVIReader m_oAVIR;					//for reading avi
	HDRAWDIB m_hdd;						//for drawing video
	byte* m_pDIBFrame;					//frame at 
	int m_iFrameW;						//frame width
	int m_iFrameH;						//frame height
	int m_iFrameOffset;					//frame offset (relative to EEG)
	void PaintVideoFrame(CPaintDC& dc);	//paint video frame if AVI file is opened
	void UpdateAVIFrame();				//update current frame of AVI player based on current time
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	
	vector<double> m_vGainList; //list of multipliers for eeg channels
	vector<string> m_vEEGNames;	//names of eeg channels, if not specified by user take on default names

	CRect	m_MainRect, m_SpikesRect, m_RoomRect, m_ArenaRect, m_videoRect;
	
	int		MIN_TIME_WINDOW;	// 1*
	int		MIN_STEP_WINDOW;
	int		MAX_TIME_WINDOW;
	int		MAX_STEP_TIMES;
	int		MAX_TIME_TIMES;
	int		timeWindow;
	int		timeWTimes;
	int		startTime;
	int		startStepTimes;
	int		timeStampFirst;	//TimeStamp of the first spike

	int		pStartTimeTimes;
	char	pMode;
	int		pAddTimes;

	char			maxCluster;
	vector<unsigned char>	clView;	// 1 = spike, 2 = arena, 4 = room 
	unsigned char	scaleItems;
		
	CFile	file;

	HCURSOR m_HCross;
	char	mPrepareFlag;
	char	fileOpen;
	int		mClustered;
	MY_SPIKE_STACK::iterator iSpike;

	CImageList m_imageList;


protected:
	void			FillClList(char numb);	// 1 = spike, 2 = arena, 4 = room
	void			FillImageList();
	void			FillScaleList();
	unsigned char	GetParameters(unsigned char *spikes, unsigned char *eeg);
	void			InitEEGNames(vector<string>* pNames);
	double			GetTimeMS();//get current time in milliseconds

public:
//	int PrepareData();

// Construction
public:
	CTimeEv(CWnd* pParent, vector<string>* pNames,
			CVerxStack* m_MainDataStackm,
			CPaletteStack* m_MainPalette,
			CCluster* m_MainClusters);   // standard constructor
	virtual ~CTimeEv();
	
// Dialog Data
	//{{AFX_DATA(CTimeEv)
	enum { IDD = IDD_TIME_EV };
	CButton	m_wndTakePict;
	CButton	m_wndMax;
	CButton	m_wndPlay;
	CButton	m_wndClear;
	CButton	m_wndSetEEGGains;
	CButton	m_wndCheckSpike;
	CButton	m_wndCheckRaster;
	CComboBox	m_wndCmbRoom;
	CComboBox	m_wndCmbArena;
	CComboBox	m_wndCmbOrigNew;
	CListCtrl	m_wndListSpikeCl;
	CListCtrl	m_wndListRoomCl;
	CButton	m_wndCancel;
	CButton	m_wndOK;
	CButton m_wndVideo;
	CListCtrl	m_wndListArenaCl;
	CListCtrl	m_wndListScale;
	CSpinButtonCtrl	m_wndSpinTime;
	CSpinButtonCtrl	m_wndSpinPlaySpeed;
	CEdit   m_wndPlaySpeed;
	CEdit	m_wndEditTime;
	CScrollBar	m_wndScrollTime;
	float	m_EditTime;
	CButton m_wndSetTime;
	CEdit m_wndEditSetTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeEv)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeEv)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusEditTime();
	afx_msg void OnItemchangedListArenaCl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListRoomCl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSpikeCl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCmbOrigNew();
	afx_msg void OnCheckRaster();
	afx_msg void OnCheckSpike();
	afx_msg void OnMax();
	afx_msg void OnSelchangeCmbArena();
	afx_msg void OnSelchangeCmbRoom();
	afx_msg void OnItemchangedListScale(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTePlay();
	afx_msg void OnTeClear();
	afx_msg void OnTeTakePict();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposSpinTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditTime();
	afx_msg void OnBnClickedButtonSetGains();
	afx_msg void OnDeltaposSpineegplayspeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonVidDlgOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_iPlaySpeed;
	afx_msg void OnBnClickedButtonSetTime();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEEV_H__EF9E6A1A_8E9D_48EB_A46F_55720AA3334B__INCLUDED_)
