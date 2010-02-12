// DlgBin.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "DlgBin.h"
#include ".\dlgbin.h"


// CDlgBin dialog

IMPLEMENT_DYNAMIC(CDlgBin, CDialog)
CDlgBin::CDlgBin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBin::IDD, pParent)
{
}

CDlgBin::~CDlgBin()
{
}

void CDlgBin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgBin, CDialog)
	ON_COMMAND(ID_CLUSTER_SETBINS, OnClusterSetbins)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgBin message handlers

void CDlgBin::OnClusterSetbins()
{
	// TODO: Add your command handler code here
}

void CDlgBin::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT1);
	edit->SetFocus();
	CString strEdit;
	edit->GetWindowText(strEdit);

	int tmp[4];
	sscanf(strEdit.GetString(),"%d %d %d %d",&tmp[0],&tmp[1],&tmp[2],&tmp[3]);

	int i;
	for(i=0;i<4;i++)
	{
		if(tmp[i]<=0)
		{
			MessageBox("Must use > 0 bins!!","Incorrect # of bins!!",MB_ICONERROR);
			OnOK();
			return;
		}
	}

	extern int pBins[4];
	memcpy(pBins,tmp,4*sizeof(int));

	CString strO;
	strO.Format("Set bins to : %d %d %d %d",pBins[0],pBins[1],pBins[2],pBins[3]);

	MessageBox(strO.GetString(),"Set Bins",MB_ICONINFORMATION);

	OnOK();
}
