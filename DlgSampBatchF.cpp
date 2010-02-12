// $Id: DlgSampBatchF.cpp,v 1.3 2008/03/30 02:14:27 samn Exp $ 
// DlgSampBatchF.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "DlgSampBatchF.h"


// CDlgSampBatchF dialog

IMPLEMENT_DYNAMIC(CDlgSampBatchF, CDialog)
CDlgSampBatchF::CDlgSampBatchF(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSampBatchF::IDD, pParent)
{
}

CDlgSampBatchF::~CDlgSampBatchF()
{
}

void CDlgSampBatchF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BATCH_SAMP_TXT, m_EditSampBatchTxt);
}

BOOL CDlgSampBatchF::OnInitDialog(){
	CDialog::OnInitDialog();
	CString str("base_directory\r\ninput_bpf_file TAB input_cl_file TAB tetrode TAB prct_of_points_to_load TAB output_cl_file TAB [cluster_1_rating,cluster_2_rating,...]\r\n\r\n");
	str+="C:\\Users\\samn\\\r\ntest.bpf\t08jan8_test.cl\t0\t1.0\tqbout\\test.cl\t[1,1,1,1]\r\n\r\n";
	str+="7-25-TTX.bpf\t7-25-TTX-af.cl\t0\t0.2\tqbout\\7-25-TTX-af.cl\t[3,3,3,3]\r\n\r\n";
	str+="b3n_5_es1_t1_c3.bpf\tb3n_5_es1_t1_c3.cl\t0\t0.2\tqbout\\b3n_5_es1_t1_c3.cl\t[1,2,3,1,2,3,1,2,3,1,2,3,1,2,3]\r\n\r\n";
	m_EditSampBatchTxt.SetWindowText(str);	
	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgSampBatchF, CDialog)
END_MESSAGE_MAP()


// CDlgSampBatchF message handlers
