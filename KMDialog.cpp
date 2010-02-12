// $Id: KMDialog.cpp,v 1.2 2008/09/16 01:42:36 samn Exp $ 
// KMDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "KMDialog.h"

// KMDialog dialog

IMPLEMENT_DYNAMIC(KMDialog, CDialog)
KMDialog::KMDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KMDialog::IDD, pParent),
	  m_iMinClusts(4),
	  m_iMaxClusts(20),
	  m_iIters(100),
	  m_iDBIters(1),
	  m_iNormalizeData(0),
	  m_iUseMedians(0)
{
}

KMDialog::~KMDialog()
{
}

void KMDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_KM_MINC, m_winMinC);
	DDX_Control(pDX, IDC_EDIT_KM_MAXC, m_winMaxC);
	DDX_Control(pDX, IDC_EDIT_KM_NITERS, m_winNumIter);
	DDX_Control(pDX, IDC_EDIT4, m_winDBIter);
	DDX_Control(pDX, IDC_CHECK_KM_NORMALIZE, m_btnNorm);
	DDX_Control(pDX, IDC_CHECK_KM_MEDIANS, m_btnMed);
}

BOOL KMDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_winNumIter.SetWindowText("1");
	m_winDBIter.SetWindowText("1");
	m_btnNorm.SetCheck(BST_CHECKED);
	m_btnMed.SetCheck(BST_UNCHECKED);
	return TRUE;
}


BEGIN_MESSAGE_MAP(KMDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// KMDialog message handlers

void KMDialog::OnBnClickedOk()
{
	CString str;
	m_winMinC.GetWindowText(str);
	m_iMinClusts = atoi(str);

	m_winMaxC.GetWindowText(str);
	m_iMaxClusts = atoi(str);

	m_winNumIter.GetWindowText(str);
	m_iIters = atoi(str);

	m_winDBIter.GetWindowText(str);
	m_iDBIters = atoi(str);
	
	m_iNormalizeData = (m_btnNorm.GetCheck() == BST_CHECKED);
	
	m_iUseMedians = (m_btnMed.GetCheck() == BST_CHECKED);

	OnOK();
}
