// $Id: DlgKlustaKwik.cpp,v 1.1 2009/06/24 18:04:33 samn Exp $ 
// DlgKlustaKwik.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "DlgKlustaKwik.h"
#include ".\dlgklustakwik.h"


// DlgKlustaKwik dialog

IMPLEMENT_DYNAMIC(DlgKlustaKwik, CDialog)
DlgKlustaKwik::DlgKlustaKwik(CWnd* pParent /*=NULL*/)
	: CDialog(DlgKlustaKwik::IDD, pParent)
{
}

DlgKlustaKwik::~DlgKlustaKwik()
{
}

void DlgKlustaKwik::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_KK_MINCLUSTS, m_wndMinClusts);
	DDX_Control(pDX, IDC_EDIT_KK_MAXCLUSTS, m_wndMaxClusts);
	DDX_Control(pDX, IDC_EDIT_KK_START_ITERS, m_wndStartIters);
	DDX_Control(pDX, IDC_EDIT_KK_MAX_POSSIBLE_CLUSTERS, m_wndMaxPossClust);
}


BEGIN_MESSAGE_MAP(DlgKlustaKwik, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DlgKlustaKwik message handlers

void DlgKlustaKwik::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_wndMinClusts.GetWindowText(str);
	m_iMinClusts = atoi(str.GetBuffer());
	m_wndMaxClusts.GetWindowText(str);
	m_iMaxClusts = atoi(str.GetBuffer());
	m_wndStartIters.GetWindowText(str);
	m_iStartIters = atoi(str.GetBuffer());
	m_wndMaxPossClust.GetWindowText(str);
	m_iMaxPossClusts = atoi(str.GetBuffer());
	if(m_iMinClusts < 2) m_iMinClusts = 2;
	if(m_iMaxClusts < m_iMinClusts) m_iMaxClusts = m_iMinClusts;
	if(m_iStartIters < 1) m_iStartIters = 1;
	if(m_iMaxPossClusts < m_iMaxClusts) m_iMaxPossClusts = m_iMaxClusts;
	OnOK();
}
