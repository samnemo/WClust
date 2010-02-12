// $Id: DlgExpNotes.cpp,v 1.1 2009/01/28 21:39:34 samn Exp $ 
// DlgExpNotes.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "DlgExpNotes.h"
#include "FileUtils.h"
#include "Log.h"
#include ".\dlgexpnotes.h"

// CDlgExpNotes dialog
IMPLEMENT_DYNAMIC(CDlgExpNotes, CDialog)
CDlgExpNotes::CDlgExpNotes(CWnd* pParent /*=NULL*/, CString strPath, CString strUser)
	: CDialog(CDlgExpNotes::IDD, pParent),
	  m_strPath(strPath),
	  m_strUser(strUser)
{
}

CDlgExpNotes::~CDlgExpNotes()
{
}

BOOL CDlgExpNotes::OnInitDialog()
{
	CDialog::OnInitDialog();
//	this->m_wndExpDate.Create(WS_TABSTOP | WS_CHILD | 
//			WS_VISIBLE | WS_BORDER,
//			CPoint(20, 20), this, 0x224);
	return TRUE;
}



void CDlgExpNotes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ANIMAL_TYPE, m_wndAnimalType);
	DDX_Control(pDX, IDC_EDIT_ANIMAL_WEIGHT, m_wndAnimalWeight);
	DDX_Control(pDX, IDC_EDIT_ANIMAL_AGE, m_wndAnimalAge);
	DDX_Control(pDX, IDC_EDIT_ANIMAL_SEX, m_wndAnimalSex);
	DDX_Control(pDX, IDC_EDIT_ANIMAL_DETAILS, m_wndAnimalDetails);
	DDX_Control(pDX, IDC_EDIT_IN1_TYPE, m_wndInputType1);
	DDX_Control(pDX, IDC_EDIT_IN2_TYPE, m_wndInputType2);
	DDX_Control(pDX, IDC_EDIT_IN3_TYPE, m_wndInputType3);
	DDX_Control(pDX, IDC_EDIT_IN4_TYPE, m_wndInputType4);
	DDX_Control(pDX, IDC_EDIT_IN1_TIME, m_wndInputTime1);
	DDX_Control(pDX, IDC_EDIT_IN2_TIME, m_wndInputTime2);
	DDX_Control(pDX, IDC_EDIT_IN3_TIME, m_wndInputTime3);
	DDX_Control(pDX, IDC_EDIT_IN4_TIME, m_wndInputTime4);
	DDX_Control(pDX, IDC_EDIT_IN1_AMOUNT, m_wndInputAmount1);
	DDX_Control(pDX, IDC_EDIT10, m_wndInputAmount2);
	DDX_Control(pDX, IDC_EDIT14, m_wndInputAmount3);
	DDX_Control(pDX, IDC_EDIT18, m_wndInputAmount4);
	DDX_Control(pDX, IDC_EDIT_IN1_LOC, m_wndInputLoc1);
	DDX_Control(pDX, IDC_EDIT_IN2_LOC, m_wndInputLoc2);
	DDX_Control(pDX, IDC_EDIT_IN3_LOC, m_wndInputLoc3);
	DDX_Control(pDX, IDC_EDIT_IN4_LOC, m_wndInputLoc4);
	DDX_Control(pDX, IDC_EDIT_EXP_COMMENTS, m_wndExpComments);
	DDX_Control(pDX, IDC_MONTHCALENDAR_EXP_DATE, m_wndExpDate);
}


BEGIN_MESSAGE_MAP(CDlgExpNotes, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgExpNotes message handlers

bool CDlgExpNotes::SaveExpNotes()
{
	FILE* fp = fopen(m_strPath.GetBuffer(),"a");
	if(!fp)
		return false;
	CString strBuf;
	fprintf(fp,"Info entered by %s @ %s\n\n",m_strUser.GetBuffer(),GetDateTime(time(0)).c_str());
	//experiment info
	CTime oTime;
	m_wndExpDate.GetCurSel(oTime);
	strBuf.Format("%s", oTime.Format("%A, %B %d, %Y"));
	fprintf(fp,"Experiment information: date=%s\n\n",strBuf.GetBuffer());	
	//animal information
	fprintf(fp,"Animal information:\n");
	m_wndAnimalType.GetWindowText(strBuf); fprintf(fp,"\ttype=%s\n",strBuf.GetBuffer());
	m_wndAnimalWeight.GetWindowText(strBuf); fprintf(fp,"\tweight=%s\n",strBuf.GetBuffer());
	m_wndAnimalAge.GetWindowText(strBuf); fprintf(fp,"\tage=%s\n",strBuf.GetBuffer());
	m_wndAnimalSex.GetWindowText(strBuf); fprintf(fp,"\tsex=%s\n",strBuf.GetBuffer());
	m_wndAnimalDetails.GetWindowText(strBuf); fprintf(fp,"\tdetails=%s\n\n",strBuf.GetBuffer());
	//input information
	CEdit *wndInputTypes[4] = { &m_wndInputType1, &m_wndInputType2, &m_wndInputType3, &m_wndInputType4 },
		*wndInputTimes[4] = { &m_wndInputTime1, &m_wndInputTime2, &m_wndInputTime3, &m_wndInputTime4 },
		*wndInputAmounts[4] = { &m_wndInputAmount1, &m_wndInputAmount2, &m_wndInputAmount3, &m_wndInputAmount4} ,
		*wndInputLocs[4] = { &m_wndInputLoc1, &m_wndInputLoc2, &m_wndInputLoc3, &m_wndInputLoc4} ;
	int i,j;
	CString strI[4];
	fprintf(fp,"Input information:\n");
	for(i=0;i<4;i++)
	{	j=0; 
		wndInputTypes[i]->GetWindowText(strI[j++]);
		wndInputTimes[i]->GetWindowText(strI[j++]);
		wndInputAmounts[i]->GetWindowText(strI[j++]);
		wndInputLocs[i]->GetWindowText(strI[j++]);
		fprintf(fp,"\tInput %d: type=%s, time=%s, amount=%s, location=%s\n",
			i+1,strI[0].GetBuffer(),strI[1].GetBuffer(),strI[2].GetBuffer(),strI[3].GetBuffer());			
	}
	m_wndExpComments.GetWindowText(strBuf);
	fprintf(fp,"\nExperiment additional information:\n%s\n\n",strBuf.GetBuffer());
	fclose(fp);
	return true;
}

bool CheckExpNotes(const char* str)
{
	//check if .nb.txt file has been setup
	char msg[1024]={0} , sUser[1024]={0};
	DWORD dLen = 1024;
	if(!GetUserName(sUser,&dLen)) sprintf(sUser,"John Doe");

	CString strNB(str);
	strNB += ".nb.txt";
	if(FileSize(strNB.GetBuffer())<=0)
	{	sprintf(msg,"%s, would you like to enter notes on your experiment?",sUser);
		if(IDNO==MessageBox(0,msg,"WClust - please enter notes.", MB_YESNO))
		{
			sprintf(msg,"%s, I forgot to mention that you have no choice.",sUser);
			MessageBox(0,msg,"WClust - sorry!",MB_ICONERROR);
		}
		CString strUser(sUser);
		CDlgExpNotes oDlg(NULL,strNB,strUser);
		oDlg.DoModal();
	}
	return true;
}


void CDlgExpNotes::OnBnClickedOk()
{	char msg[1024];
	if(!SaveExpNotes())
	{	
		sprintf(msg,"Sorry, %s couldn't save experimental notes to %s!\n",m_strUser.GetBuffer(),m_strPath.GetBuffer());
		Write2Log(msg);
		MessageBox(msg,"WClust - error",MB_ICONERROR);
	}
	else
	{
		sprintf(msg,"Thanks %s, you saved notes to %s",m_strUser.GetBuffer(),m_strPath.GetBuffer());
		MessageBox(msg,"WClust",MB_ICONINFORMATION);
	}
	// TODO: Add your control notification handler code here
	OnOK();
}

void CDlgExpNotes::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	char msg[1024];
	sprintf(msg,"%s, thought you could get out of this?!?! Think again.",m_strUser.GetBuffer());
	MessageBox(msg,"WClust - insubordination error",MB_ICONERROR);
	return;
}
