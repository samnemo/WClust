// $Id: DlgKLDOpt.cpp,v 1.14 2009/06/24 18:08:00 samn Exp $ 
// DlgKLDOpt.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "DlgKLDOpt.h"
#include ".\dlgkldopt.h"


// DlgKLDOpt dialog

IMPLEMENT_DYNAMIC(DlgKLDOpt, CDialog)
DlgKLDOpt::DlgKLDOpt(CWnd* pParent /*=NULL*/)
	: CDialog(DlgKLDOpt::IDD, pParent)
	, m_iNNToFind(1)
	, m_bFast(FALSE)
	, m_bInterKLD(TRUE)
	, m_bIsolationDist(FALSE)
	, m_bDoKLDiv(TRUE)
	, m_bFindBestDims(TRUE)
	, m_iNumBestDims(8)
	, m_bSilhouette(FALSE)
	, m_iBestSliceD(2)
	, m_bNormKLD(TRUE)
	, m_bLRatio(FALSE)
	, m_bUseDefDims(FALSE)
{
	m_editBestDims.SetWindowText("8");
}

DlgKLDOpt::~DlgKLDOpt()
{
}

void DlgKLDOpt::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow,nStatus);
}

BOOL DlgKLDOpt::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	str.Format("%d",m_iNumBestDims);
	m_editBestDims.SetWindowText(str);
	if(m_checkBestDims.GetCheck()==BST_CHECKED)
	{
		m_checkUseDefDims.SetCheck(BST_UNCHECKED);
		m_bUseDefDims = FALSE;
	}
	else if(m_checkBestDims.GetCheck()==BST_UNCHECKED || !m_bFindBestDims)
	{
		m_editBestDims.EnableWindow(FALSE);
		m_btnSetNumBestDims.EnableWindow(FALSE);
	}
	if(m_bSilhouette)
	{
		m_checkSilh.EnableWindow(TRUE);
		m_checkSilh.SetCheck(BST_CHECKED);
	}
	if(m_iBestSliceD == 1)
	{
		m_radSlice1D.SetCheck(BST_CHECKED);
		m_radSlice2D.SetCheck(BST_UNCHECKED);
	}
	else
	{
		m_radSlice2D.SetCheck(BST_CHECKED);
		m_radSlice1D.SetCheck(BST_UNCHECKED);
	}
	if(m_bUseDefDims)
	{
		m_checkUseDefDims.SetCheck(BST_CHECKED);
	}
	return TRUE;
}

void DlgKLDOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_FAST_KLD, m_bFast);
	DDX_Check(pDX, IDC_CHECK_INTER_KLD, m_bInterKLD);
	DDX_Check(pDX, IDC_CHECK_ISOLATION_D, m_bIsolationDist);
	DDX_Check(pDX, IDC_CHECK_KLD_DO, m_bDoKLDiv);
	DDX_Check(pDX, IDC_CHECK_FIND_BEST_DIMS, m_bFindBestDims);
	DDX_Control(pDX, IDC_EDIT_BEST_DIMS, m_editBestDims);
	DDX_Control(pDX, IDC_CHECK_FIND_BEST_DIMS, m_checkBestDims);
	DDX_Control(pDX, IDC_BUTTON_SET_NUM_BEST_DIMS, m_btnSetNumBestDims);
	DDX_Control(pDX, IDC_CHECK_INTER_KLD, m_checkInter);
	DDX_Control(pDX, IDC_CHECK_FAST_KLD, m_checkFast);
	DDX_Control(pDX, IDC_CHECK_SILHOUETTE_WIDTH, m_checkSilh);
	DDX_Control(pDX, IDC_RADIO_1D_SLICE, m_radSlice1D);
	DDX_Control(pDX, IDC_RADIO_2D_SLICES, m_radSlice2D);
	DDX_Check(pDX, IDC_CHECK_NORM, m_bNormKLD);
	DDX_Check(pDX, IDC_CHECK_LRATIO, m_bLRatio);
	DDX_Check(pDX, IDC_CHECK_KLD_DEFAULT_DIMS, m_bUseDefDims);
	DDX_Control(pDX, IDC_CHECK_KLD_DEFAULT_DIMS, m_checkUseDefDims);
}


BEGIN_MESSAGE_MAP(DlgKLDOpt, CDialog)
	ON_BN_CLICKED(IDC_CHECK_FIND_BEST_DIMS, OnBnClickedCheckFindBestDims)
	ON_BN_CLICKED(IDC_BUTTON_SET_NUM_BEST_DIMS, OnBnClickedButtonSetNumBestDims)
	ON_BN_CLICKED(IDC_CHECK_INTER_KLD, OnBnClickedCheckInterKld)
	ON_BN_CLICKED(IDC_CHECK_FAST_KLD, OnBnClickedCheckFastKld)
	ON_BN_CLICKED(IDC_CHECK_SILHOUETTE_WIDTH, OnBnClickedCheckSilhouetteWidth)
	ON_BN_CLICKED(IDC_RADIO_1D_SLICE, OnBnClickedRadio1dSlice)
	ON_BN_CLICKED(IDC_RADIO_2D_SLICES, OnBnClickedRadio2dSlices)
	ON_BN_CLICKED(IDC_CHECK_KLD_DEFAULT_DIMS, OnBnClickedCheckKldDefaultDims)
END_MESSAGE_MAP()


// DlgKLDOpt message handlers


void DlgKLDOpt::OnBnClickedCheckFindBestDims()
{
	// TODO: Add your control notification handler code here
	if(m_checkBestDims.GetCheck()==BST_CHECKED)
	{
		m_bFast = FALSE;
		m_checkFast.SetCheck(BST_UNCHECKED);
		m_btnSetNumBestDims.EnableWindow(TRUE);
		m_editBestDims.EnableWindow(TRUE);
		m_bUseDefDims = FALSE; 
		m_checkUseDefDims.SetCheck(BST_UNCHECKED);
	}
	else
	{
		m_btnSetNumBestDims.EnableWindow(FALSE);
		m_editBestDims.EnableWindow(FALSE);
	}
}

void DlgKLDOpt::OnBnClickedButtonSetNumBestDims()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_editBestDims.GetWindowText(str);
	try
	{
		int iTmp = atoi(str);
		if(iTmp < 1)
		{
			MessageBox("Must enter positive number!","WClust - error",MB_ICONERROR);
			return;
		}
		else if(iTmp > 24)
		{
			MessageBox("Must enter number <= 24!","WClust - error",MB_ICONERROR);
			return;
		}
		m_iNumBestDims = iTmp;
	}
	catch(...)
	{
		MessageBox("Must enter a positive integer!","WClust - error",MB_ICONERROR);
	}
}

void DlgKLDOpt::OnBnClickedCheckInterKld()
{
	// TODO: Add your control notification handler code here
	m_bInterKLD = m_checkInter.GetCheck()==BST_CHECKED;
}

void DlgKLDOpt::OnBnClickedCheckFastKld()
{
	// TODO: Add your control notification handler code here
	if(m_checkFast.GetCheck()==BST_CHECKED)
	{
		m_checkBestDims.SetCheck(BST_UNCHECKED);
		m_bFindBestDims = FALSE;
		m_btnSetNumBestDims.EnableWindow(FALSE);
		m_editBestDims.EnableWindow(FALSE);
	}
}

void DlgKLDOpt::OnBnClickedCheckSilhouetteWidth()
{
	// TODO: Add your control notification handler code here
	m_bSilhouette = m_checkSilh.GetCheck()==BST_CHECKED;
}

void DlgKLDOpt::OnBnClickedRadio1dSlice()
{
	// TODO: Add your control notification handler code here
	m_iBestSliceD = 1;
	m_radSlice2D.SetCheck(BST_UNCHECKED);
}

void DlgKLDOpt::OnBnClickedRadio2dSlices()
{
	// TODO: Add your control notification handler code here
	m_iBestSliceD = 2;
	m_radSlice1D.SetCheck(BST_UNCHECKED);
}

void DlgKLDOpt::OnBnClickedCheckKldDefaultDims()
{
	// TODO: Add your control notification handler code here
	m_bUseDefDims = m_checkUseDefDims.GetCheck()==BST_CHECKED;
	if(m_bUseDefDims)
	{
		m_checkBestDims.SetCheck(BST_UNCHECKED);
		m_bFindBestDims = FALSE;
		m_btnSetNumBestDims.EnableWindow(FALSE);
		m_editBestDims.EnableWindow(FALSE);
	}
}
