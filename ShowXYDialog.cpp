// APClustDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "APClustDialog.h"
#include ".\apclustdialog.h"


// CAPClustDialog dialog

IMPLEMENT_DYNAMIC(CAPClustDialog, CDialog)
CAPClustDialog::CAPClustDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAPClustDialog::IDD, pParent),
	  m_iNumIters(500),
	  m_iConvIters(50),
	  m_dDamp(0.5),
	  m_dPref(5.0)
{
}

CAPClustDialog::~CAPClustDialog()
{
}

void CAPClustDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAPClustDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAPClustDialog message handlers

void CAPClustDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_AP_CLUST);
	edit->SetFocus();
	CString strEdit;
	edit->GetWindowText(strEdit);

	sscanf(strEdit.GetString(),"%d%d%lf%lf",&m_iNumIters,&m_iConvIters,&m_dDamp,&m_dPref);

	OnOK();
}
