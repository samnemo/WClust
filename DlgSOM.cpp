// $Id: DlgSOM.cpp,v 1.1 2009/06/22 03:52:15 samn Exp $ 
// DlgSOM.cpp : implementation file
//

#include "stdafx.h"
#include "WClust.h"
#include "DlgSOM.h"


// DlgSOM dialog

IMPLEMENT_DYNAMIC(DlgSOM, CDialog)
DlgSOM::DlgSOM(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSOM::IDD, pParent)
{
}

DlgSOM::~DlgSOM()
{
}

void DlgSOM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgSOM, CDialog)
END_MESSAGE_MAP()


// DlgSOM message handlers
