// $Id: DlgSOM.h,v 1.1 2009/06/22 03:52:06 samn Exp $ 
#pragma once


// DlgSOM dialog

class DlgSOM : public CDialog
{
	DECLARE_DYNAMIC(DlgSOM)

public:
	DlgSOM(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgSOM();

// Dialog Data
	enum { IDD = IDD_SOM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
