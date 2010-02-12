// $Id: EClustDlg.cpp,v 1.3 2008/09/16 01:42:49 samn Exp $ 
// EClustDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "EClustDlg.h"
#include ".\eclustdlg.h"


// EClustDlg dialog

IMPLEMENT_DYNAMIC(EClustDlg, CDialog)
EClustDlg::EClustDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EClustDlg::IDD, pParent),
	  m_iMinClust(0),
	  m_iMaxClust(0),
	  m_iIters(1),
	  m_iClustIters(1),
	  m_dErrPrct(0.0)
{
	m_wndNIter.SetWindowText("1");
	m_wndNIterC.SetWindowText("1");
}

EClustDlg::~EClustDlg()
{
}

BOOL EClustDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_wndNIter.SetWindowText("1");
	m_wndNIterC.SetWindowText("1");
	m_wndErrPrct.SetWindowText("0.0");
	return TRUE;
}

void EClustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_EMINC, m_wndMinC);
	DDX_Control(pDX, IDC_EDIT_EMAXC, m_wndMaxC);
	DDX_Control(pDX, IDC_EDIT_ENUMITER, m_wndNIter);
	DDX_Control(pDX, IDC_EDIT_ENUMITER2, m_wndNIterC);
	DDX_Control(pDX, IDC_EDIT_ERR_PRCT_E, m_wndErrPrct);
}


BEGIN_MESSAGE_MAP(EClustDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// EClustDlg message handlers

void EClustDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_iMinClust = m_iMaxClust = m_iIters = m_iClustIters = 0;

	CString str;
	m_wndMinC.GetWindowText(str);
	m_iMinClust = atoi(str);

	m_wndMaxC.GetWindowText(str);
	m_iMaxClust = atoi(str);

	m_wndNIter.GetWindowText(str); 
	m_iIters = atoi(str);

	m_wndNIterC.GetWindowText(str);
	m_iClustIters = atoi(str);

	m_wndErrPrct.GetWindowText(str);
	m_dErrPrct = atof(str);

	OnOK();
}
