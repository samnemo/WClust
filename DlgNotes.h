#pragma once


// CDlgNotes dialog

class CDlgNotes : public CDialog
{
	DECLARE_DYNAMIC(CDlgNotes)

public:
	CDlgNotes(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNotes();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
