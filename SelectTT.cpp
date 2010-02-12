// $Id: SelectTT.cpp,v 1.4 2008/08/17 05:23:20 samn Exp $ 
// SelectTT.cpp : implementation file
//

#include "stdafx.h"
#include "wclust.h"
#include "SelectTT.h"
#include ".\selecttt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectTT dialog


CSelectTT::CSelectTT(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectTT::IDD, pParent),
	  m_bUseEEG(true),
	  m_bUsePCA(false),
	  m_bCatPCA(true),
	  m_iNumPCs(3)
	  , m_strNumPCs(_T(""))
{
	//{{AFX_DATA_INIT(CSelectTT)
	m_edAmount = 100;
	//}}AFX_DATA_INIT
}


void CSelectTT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectTT)
	DDX_Control(pDX, IDC_LIST_MESSAGE, m_wndListMessage);
	DDX_Control(pDX, IDC_ED_AMOUNT, m_wndEdAmount);
	DDX_Control(pDX, IDC_PERBUT6, m_wndPB6);
	DDX_Control(pDX, IDC_PERBUT5, m_wndPB5);
	DDX_Control(pDX, IDC_PERBUT4, m_wndPB4);
	DDX_Control(pDX, IDC_PERBUT3, m_wndPB3);
	DDX_Control(pDX, IDC_PERBUT2, m_wndPB2);
	DDX_Control(pDX, IDC_PERBUT1, m_wndPB1);
	DDX_Control(pDX, IDC_LIST_OF_ELECTRODES, m_wndList);
	DDX_Text(pDX, IDC_ED_AMOUNT, m_edAmount);
	DDX_Control(pDX, IDC_CHECK_PCA, m_BtnDoPCA);
	DDX_Control(pDX, IDC_CHECK_PCA_CONCAT, m_BtnCatPCA);
	DDX_Text(pDX, IDC_EDIT_NUM_PCS, m_strNumPCs);
	DDX_Control(pDX, IDC_CHECK_LD_EEG, m_btnLoadEEG);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectTT, CDialog)
	//{{AFX_MSG_MAP(CSelectTT)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_LIST_OF_ELECTRODES, OnSelchangeListOfElectrodes)
	ON_LBN_DBLCLK(IDC_LIST_OF_ELECTRODES, OnDblclkListOfElectrodes)
	ON_BN_CLICKED(IDC_PERBUT1, OnPerbut1)
	ON_BN_CLICKED(IDC_PERBUT2, OnPerbut2)
	ON_BN_CLICKED(IDC_PERBUT3, OnPerbut3)
	ON_BN_CLICKED(IDC_PERBUT4, OnPerbut4)
	ON_BN_CLICKED(IDC_PERBUT5, OnPerbut5)
	ON_BN_CLICKED(IDC_PERBUT6, OnPerbut6)
	ON_BN_CLICKED(IDC_CHECK_PCA, OnBnClickedCheckPca)
	ON_BN_CLICKED(IDC_CHECK_PCA_CONCAT, OnBnClickedCheckPcaConcat)
	ON_EN_UPDATE(IDC_EDIT_NUM_PCS, OnEnUpdateEditNumPcs)
	ON_BN_CLICKED(IDC_CHECK_LD_EEG, OnBnClickedCheckLdEeg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectTT message handlers

BOOL CSelectTT::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit* p = (CEdit*)GetDlgItem(IDC_EDIT_NUM_PCS);
	p->SetWindowText("3");		

	if(m_bUsePCA)
	{
		m_BtnDoPCA.SetCheck(BST_CHECKED);
		m_BtnCatPCA.SetCheck(BST_CHECKED);
	}
	OnBnClickedCheckPca();

	m_btnLoadEEG.SetCheck(BST_CHECKED);

	return TRUE;
}

void CSelectTT::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	MY_INT_VECT::iterator indEl, indCh, indAm;
	
	NoLines = 0;
	for (int i=0; i<max_elde; i++)
	{
		for (int j=0; j<256; j++)
		{
			if ( ChannelNo[i*256 + j] > 0 )
			{
				EldeNoStack.push_back(i+1);
				ChannelNoStack.push_back(j);
				AmountStack.push_back(ChannelNo[i*256 + j]);
				NoLines++;
			}
		}
	}
	indEl = EldeNoStack.begin();
	indCh = ChannelNoStack.begin();
	indAm = AmountStack.begin();
	char lineText[100];
	while ( indEl != EldeNoStack.end() )
	{
		switch ( *(indEl) )
		{
		case 1:
			sprintf(lineText,"Single        ch.%d:  %d samples",*(indCh),*(indAm));
			m_wndList.AddString(lineText);
			break;
		case 2:
			sprintf(lineText,"Stereode      ch.%d:  %d samples",*(indCh),*(indAm));
			m_wndList.AddString(lineText);
			break;
		case 3:
			sprintf(lineText,"Tetrode       ch.%d:  %d samples",*(indCh),*(indAm));
			m_wndList.AddString(lineText);
			break;
		}
		indEl++;
		indCh++;
		indAm++;
	}

	if ( FileType == 1 && dFileOK == 0 )
	{
		sprintf(lineText,"WARNING: Last record in this file is corrupted");
		m_wndListMessage.AddString(lineText);
		sprintf(lineText,"Last rec. type: %c, Pos.: %d (0x%x)",dLastType, dLastPos,dLastPos);
		m_wndListMessage.AddString(lineText);
		sprintf(lineText,"WClust automaticaly repair it in saving process");
		m_wndListMessage.AddString(lineText);
	}

	m_wndList.SetCurSel(0);
	EldeCh = 0;
	EldeType = 0;

	if ( FileType == 2)
	{
		m_wndPB1.EnableWindow(false);
		m_wndPB2.EnableWindow(false);
		m_wndPB3.EnableWindow(false);
		m_wndPB4.EnableWindow(false);
		m_wndPB5.EnableWindow(false);
		m_wndPB6.EnableWindow(false);
		m_wndEdAmount.EnableWindow(false);
	}	
}

void CSelectTT::OnSelchangeListOfElectrodes() 
{
	int i=0;
	while (i<NoLines && !m_wndList.GetSel(i))
	{
		i++;
	}
	EldeCh = *(ChannelNoStack.begin() + i);
	EldeType = *(EldeNoStack.begin() + i);
	EldeMaxAmount = *(ChannelNo + i);
}

void CSelectTT::OnDblclkListOfElectrodes() 
{
	int i=0;
	while (i<NoLines && !m_wndList.GetSel(i))
	{
		i++;
	}
	EldeCh = *(ChannelNoStack.begin() + i);
	EldeType = *(EldeNoStack.begin() + i);
	EldeMaxAmount = *(ChannelNo + i);

	EndDialog(IDOK);	
}

void CSelectTT::OnOK() 
{
	int iLen = m_strNumPCs.GetLength()+1 < 16 ? m_strNumPCs.GetLength()+1 : 16;
	char* ptmp = new char[iLen];
	memset(ptmp,0,iLen*sizeof(char));
	int j = 0;
	for(j=0;j<iLen;j++) ptmp[j] = m_strNumPCs.GetAt(j);
	int iTmp = atoi(ptmp);
	if(iTmp < 1 || iTmp > 64)
	{
		MessageBox("Invalid # of PCs! Using 3!","WClust - error",MB_ICONERROR);
		m_iNumPCs = 3;
	}
	else
		m_iNumPCs = iTmp;	
	delete [] ptmp;

	int i=0;
	while (i<NoLines && !m_wndList.GetSel(i))
	{
		i++;
	}
	EldeCh = *(ChannelNoStack.begin() + i);
	EldeType = *(EldeNoStack.begin() + i);

	CDialog::OnOK();
}

void CSelectTT::OnCancel() 
{
	// Here is nothing
	// Esc pressing eliminated
}


int CSelectTT::GetAmount()
{
	int Amount;

	Amount = m_edAmount;
	
	// range check
	if (Amount < 1)
		Amount = 1;
	if (Amount > 100)
		Amount = 100;

	return Amount;
}

void CSelectTT::OnPerbut1() 
{
	m_edAmount = 1;
	UpdateData(false);
}

void CSelectTT::OnPerbut2() 
{
	m_edAmount = 5;
	UpdateData(false);		
}

void CSelectTT::OnPerbut3() 
{
	m_edAmount = 10;
	UpdateData(false);
}

void CSelectTT::OnPerbut4() 
{
	m_edAmount = 20;
	UpdateData(false);
}

void CSelectTT::OnPerbut5() 
{
	m_edAmount = 50;
	UpdateData(false);
}

void CSelectTT::OnPerbut6() 
{
	m_edAmount = 100;
	UpdateData(false);
}

void CSelectTT::OnBnClickedCheckPca()
{
	// TODO: Add your control notification handler code here
	if(BST_UNCHECKED == m_BtnDoPCA.GetCheck())
	{
		m_BtnCatPCA.SetCheck(BST_INDETERMINATE);
		m_bUsePCA = false;
		m_bCatPCA = false;
	}
	else
	{
		m_BtnCatPCA.SetCheck(BST_CHECKED);
		m_bUsePCA = true;
		m_bCatPCA = true;
	}
}

void CSelectTT::OnCbnSelchangeComboNumPcs()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_ComboNumPCs.UpdateData();
	m_iNumPCs = 1 + m_ComboNumPCs.GetCurSel();
}

void CSelectTT::OnBnClickedCheckPcaConcat()
{
	// TODO: Add your control notification handler code here
	if(m_BtnDoPCA.GetCheck()==BST_CHECKED)
	{
		if(m_BtnCatPCA.GetCheck()==BST_INDETERMINATE)
		{
			m_BtnCatPCA.SetCheck(BST_UNCHECKED);
			m_bCatPCA = false;
		}
		else if(m_BtnCatPCA.GetCheck()==BST_UNCHECKED)
			m_bCatPCA = false;
		else
			m_bCatPCA = true;
	}
	else
	{
		m_BtnCatPCA.SetCheck(BST_INDETERMINATE);
		m_bCatPCA = false;
	}
}




void CSelectTT::OnCbnDropdownComboNumPcs()
{
	//MessageBox("here","here");
	// TODO: Add your control notification handler code here
//	this->m_ComboNumPCs.UpdateData();
}

void CSelectTT::OnCbnEditchangeComboNumPcs()
{
	// TODO: Add your control notification handler code here
	//MessageBox("here2","here2");
}

void CSelectTT::OnCbnEditupdateComboNumPcs()
{
	// TODO: Add your control notification handler code here
	//MessageBox("here3","here3");
	//m_ComboNumPCs.GetItemDataPtr(
}

void CSelectTT::OnEnUpdateEditNumPcs()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	CEdit* p = (CEdit*)GetDlgItem(IDC_EDIT_NUM_PCS);
	p->GetWindowText(m_strNumPCs);
}

void CSelectTT::OnBnClickedCheckLdEeg()
{
	// TODO: Add your control notification handler code here
	if(m_BtnCatPCA.GetCheck()==BST_CHECKED)
	{
		m_bUseEEG = true;
	}
	else
	{
		m_bUseEEG = false;
	}
}