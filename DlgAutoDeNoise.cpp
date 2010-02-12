// $Id: DlgAutoDeNoise.cpp,v 1.2 2009/06/24 18:06:38 samn Exp $ 
// DlgAutoDeNoise.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "DlgAutoDeNoise.h"
#include ".\dlgautodenoise.h"


// CDlgAutoDeNoise dialog

IMPLEMENT_DYNAMIC(CDlgAutoDeNoise, CDialog)
CDlgAutoDeNoise::CDlgAutoDeNoise(CWnd* pParent /*=NULL*/, CVerxStack* p)
	: CDialog(CDlgAutoDeNoise::IDD, pParent)
	, m_dHighSat(9.5)
	, m_dLowSat(-9.5)
	, pDataStack(p)
	, m_dMinSpikeAmp(-20.0)
{
	InitCombo();
}

CDlgAutoDeNoise::~CDlgAutoDeNoise()
{
}

void CDlgAutoDeNoise::InitCombo()
{
	m_ComboLowSat.AddString("-7.0");
	m_ComboLowSat.AddString("-7.5");
	m_ComboLowSat.AddString("-8.0");
	m_ComboLowSat.AddString("-8.5");
	m_ComboLowSat.AddString("-9.0");
	m_ComboLowSat.AddString("-9.5");
	m_ComboLowSat.AddString("-9.75");
	m_ComboLowSat.SetCurSel(6);

	m_ComboHighSat.AddString("7.0");
	m_ComboHighSat.AddString("7.5");
	m_ComboHighSat.AddString("8.0");
	m_ComboHighSat.AddString("8.5");
	m_ComboHighSat.AddString("9.0");
	m_ComboHighSat.AddString("9.5");
	m_ComboHighSat.AddString("9.75");
	m_ComboHighSat.SetCurSel(6);

	m_ComboFracSat.AddString("0.1");
	m_ComboFracSat.AddString("0.25");
	m_ComboFracSat.AddString("0.333333");
	m_ComboFracSat.AddString("0.4");
	m_ComboFracSat.AddString("0.5");
	m_ComboFracSat.AddString("0.75");
	m_ComboFracSat.SetCurSel(2);
}

BOOL CDlgAutoDeNoise::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitCombo();
	return TRUE;
}

void CDlgAutoDeNoise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LOW_SAT, m_ComboLowSat);
	DDX_Control(pDX, IDC_COMBO_HIGH_SAT, m_ComboHighSat);
	DDX_Control(pDX, IDC_COMBO_FRAC_SAT, m_ComboFracSat);
	DDX_Control(pDX, IDC_EDIT_MIN_SPIKE_AMP, m_wndMinSpikeAmp);
}


BEGIN_MESSAGE_MAP(CDlgAutoDeNoise, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgAutoDeNoise message handlers

void CDlgAutoDeNoise::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//char tmp[1024];
	CString strLow,strHigh,strFrac,strAmp;
	m_ComboLowSat.GetLBText(m_ComboLowSat.GetCurSel(),strLow);
	m_ComboHighSat.GetLBText(m_ComboHighSat.GetCurSel(),strHigh);
	m_ComboFracSat.GetLBText(m_ComboFracSat.GetCurSel(),strFrac);
	m_dLowSat=atof(strLow.GetBuffer());
	m_dHighSat=atof(strHigh.GetBuffer());
	double dSat=atof(strFrac.GetBuffer());
	m_wndMinSpikeAmp.GetWindowText(strAmp);
	m_dMinSpikeAmp = atof(strAmp.GetBuffer());
	//sprintf(tmp,"%s %s low=%f high=%f frac=%f",strLow.GetBuffer(),strHigh.GetBuffer(),m_dLowSat,m_dHighSat,dSat);
	//MessageBox(tmp,"WClust",0);
	if(pDataStack)
	{
		int iAutoNoiseFound = pDataStack->CheckNoise(m_dLowSat,m_dHighSat,dSat,m_dMinSpikeAmp);
		char tmp[1024];
		sprintf(tmp,"Automatically Found %d Noise Waveforms",iAutoNoiseFound);
		MessageBox(tmp,"WClust",MB_ICONINFORMATION);
	}
	else
	{
		MessageBox("Didn't do any noise auto-detection!","WClust",MB_ICONERROR);
	}
	OnOK();
}

void CDlgAutoDeNoise::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
