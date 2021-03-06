// $Id: DlgExpNotes.h,v 1.1 2009/01/28 21:40:04 samn Exp $ 
#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// CDlgExpNotes dialog

class CDlgExpNotes : public CDialog
{
	DECLARE_DYNAMIC(CDlgExpNotes)

public:
	CDlgExpNotes(CWnd* pParent,CString strPath,CString strUser);   // standard constructor
	virtual ~CDlgExpNotes();

// Dialog Data
	enum { IDD = IDD_DIALOG_NOTES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_wndAnimalType;
	CEdit m_wndAnimalWeight;
	CEdit m_wndAnimalAge;
	CEdit m_wndAnimalSex;
	CEdit m_wndAnimalDetails;
	CEdit m_wndInputType1;
	CEdit m_wndInputType2;
	CEdit m_wndInputType3;
	CEdit m_wndInputType4;
	CEdit m_wndInputTime1;
	CEdit m_wndInputTime2;
	CEdit m_wndInputTime3;
	CEdit m_wndInputTime4;
	CEdit m_wndInputAmount1;
	CEdit m_wndInputAmount2;
	CEdit m_wndInputAmount3;
	CEdit m_wndInputAmount4;
	CEdit m_wndInputLoc1;
	CEdit m_wndInputLoc2;
	CEdit m_wndInputLoc3;
	CEdit m_wndInputLoc4;
	CEdit m_wndExpComments;
	CMonthCalCtrl m_wndExpDate;
	CString m_strPath;
	CString m_strUser;
	bool SaveExpNotes();
	BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

bool CheckExpNotes(const char* str);
