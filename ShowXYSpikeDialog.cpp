// ShowXYSpikeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "ShowXYSpikeDialog.h"
#include ".\showxyspikedialog.h"


// ShowXYSpikeDialog dialog

IMPLEMENT_DYNAMIC(ShowXYSpikeDialog, CDialog)
ShowXYSpikeDialog::ShowXYSpikeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ShowXYSpikeDialog::IDD, pParent),
	  m_bShowXYSpikes(false)
	  , m_iRadioCollapse(0),
	  m_iCollapseFactor(8)
{	
}

ShowXYSpikeDialog::~ShowXYSpikeDialog()
{
}

void ShowXYSpikeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SPIKE_XY, m_BtnShow);
}


BEGIN_MESSAGE_MAP(ShowXYSpikeDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_SPIKE_XY, OnBnClickedCheckSpikeXy)
	ON_BN_CLICKED(IDC_RADIO_XY_32, OnBnClickedRadioXy32)
	ON_BN_CLICKED(IDC_RADIO_XY_16, OnBnClickedRadioXy16)
	ON_BN_CLICKED(IDC_RADIO_XY_8, OnBnClickedRadioXy8)
	ON_BN_CLICKED(IDC_RADIO_XY_4, OnBnClickedRadioXy4)
	ON_BN_CLICKED(IDC_RADIO_XY_2, OnBnClickedRadioXy2)
END_MESSAGE_MAP()


// ShowXYSpikeDialog message handlers

void ShowXYSpikeDialog::OnBnClickedOk()
{
	OnOK();
}

void ShowXYSpikeDialog::OnBnClickedCheckSpikeXy()
{
	// TODO: Add your control notification handler code here
	m_bShowXYSpikes = (BST_CHECKED == m_BtnShow.GetCheck());
}


void ShowXYSpikeDialog::OnBnClickedRadioXy32()
{
	// TODO: Add your control notification handler code here
	m_iCollapseFactor = 32;
}

void ShowXYSpikeDialog::OnBnClickedRadioXy16()
{
	// TODO: Add your control notification handler code here
	m_iCollapseFactor = 16;
}

void ShowXYSpikeDialog::OnBnClickedRadioXy8()
{
	// TODO: Add your control notification handler code here
	m_iCollapseFactor = 8;
}

void ShowXYSpikeDialog::OnBnClickedRadioXy4()
{
	// TODO: Add your control notification handler code here
	m_iCollapseFactor = 4;
}

void ShowXYSpikeDialog::OnBnClickedRadioXy2()
{
	// TODO: Add your control notification handler code here
	m_iCollapseFactor = 2;
}
