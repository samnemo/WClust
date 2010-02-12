// $Id: DialList.cpp,v 1.12 2008/08/28 04:07:09 samn Exp $ 
// DialList.cpp : implementation file
//

#include "stdafx.h"
#include "wclust.h"
#include "DialList.h"
#include ".\diallist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialList dialog


CDialList::CDialList(CWnd* pParent /*=NULL*/)
	: cdxCSizingDialog(CDialList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialList)
	//}}AFX_DATA_INIT
}


void CDialList::DoDataExchange(CDataExchange* pDX)
{
	cdxCSizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialList)
	DDX_Control(pDX, IDCANCEL, m_wndCancel);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_LIST1, m_wndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialList, cdxCSizingDialog)
	//{{AFX_MSG_MAP(CDialList)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialList message handlers

BOOL CDialList::OnInitDialog() 
{
	cdxCSizingDialog::OnInitDialog();
	
	RestoreWindowPosition(_T("Main\\ListWindow")); 

	AddSzControlEx( m_wndList,exIgnore,exMaximum,exIgnore,exMaximum);
	AddSzControl( m_wndOK,mdRepos,mdNone);
	AddSzControl( m_wndCancel,mdRepos,mdNone);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialList::AddItem(int iAxX,int iAxY,int iCl,char* str)
{
	if(str)
	{
		m_wndList.AddString(str);
		AxStackX.push_back(iAxX); 
		AxStackY.push_back(iAxY);
		ClStack.push_back(iCl);
		ListNum++;
	}
}

void CDialList::Clear()
{
	AxStackX.clear(); //AxStackX.empty();//?? empty just checks if the deque is empty
	AxStackY.clear(); //AxStackY.empty();
	ClStack.clear(); //ClStack.empty();
	ListNum=0;
}

void CDialList::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	cdxCSizingDialog::OnShowWindow(bShow, nStatus);

	MY_STACK::iterator Index,indProj;

	char toWrite[4196];

	Clear();

	int iC = 1;

	int which = m_MainDataStack->whichDraw;

	vector<ClusterInfo>& vInfo = m_MainClusters->m_vInfo[which];

	char* strStar = "*********************************************************";

	if(which == CLUST_USER)
	{	
		if (m_MainClusters->m_BndrStack.begin()+1 != m_MainClusters->m_BndrStack.end())
		{
			for (Index = m_MainClusters->m_BndrStack.begin()+1; Index != m_MainClusters->m_BndrStack.end(); Index++,iC++)
			{
				CBoundary *bndry;
				bndry = (CBoundary*) *Index;

				AddItem(0,0,0,strStar);
				
				if(bndry->m_bNoise)
					sprintf(toWrite,"NoiseCluster %d",bndry->m_Numb);
				else
					sprintf(toWrite,"Cluster %d",bndry->m_Numb);

				AddItem(0,0,0,toWrite);				
				AddItem(0,0,0,"{");

				for (indProj = bndry->m_ProjStack.begin(); indProj != bndry->m_ProjStack.end(); indProj++)
				{
					CProjection *proj;
					proj = (CProjection*) *indProj;
					
					CString *strT1,*strT2;

					strT1 = m_MainDataStack->GetAxesName(proj->m_AxesX);
					strT2 = m_MainDataStack->GetAxesName(proj->m_AxesY);
					
					sprintf(toWrite,"   %s, %s",*strT1,*strT2);

					AddItem(proj->m_AxesX,proj->m_AxesY,bndry->m_Numb,toWrite);
				}

				//only write info stats for non-noise clusters
				//info stats of noise clusters are meaningless
				if(!bndry->m_bNoise)
				{	if(vInfo.size()>iC)
					{	sprintf(toWrite,"   NumLoadedSpikes = %d",vInfo[iC].m_iSz); AddItem(0,0,bndry->m_Numb,toWrite);
						sprintf(toWrite,"   TotalKLD = %.2f ",vInfo[iC].m_fBGInfoGain+vInfo[iC].m_fInterClustGain);
						AddItem(0,0,bndry->m_Numb,toWrite);
						if(m_MainClusters->m_oCQO.m_bInterKLD)
						{	sprintf(toWrite,"   BGKLD=%.2f  InterKLD=%.2f  ClosestClusterID=%d",
								vInfo[iC].m_fBGInfoGain,vInfo[iC].m_fInterClustGain,vInfo[iC].m_iClosestID);
							AddItem(0,0,bndry->m_Numb,toWrite);
						}
						if(m_MainClusters->m_oCQO.m_bFastKLD)
						{	sprintf(toWrite,"   KNNCC=%.4f",vInfo[iC].m_fPrctKNNInClust);
							AddItem(0,0,bndry->m_Numb,toWrite);
						}
						if(m_MainClusters->m_oCQO.m_bSilhouette)
						{	sprintf(toWrite,"   SilhouetteWidth=%.4f",vInfo[iC].m_fSilhouetteWidth);
							AddItem(0,0,bndry->m_Numb,toWrite);
						}
					}
					if(m_MainClusters->m_oCQO.m_bIsolationDist && vInfo.size()>iC)
					{	sprintf(toWrite,"   IsolationDist = %.2f ",vInfo[iC].m_dIsolationDist);
						AddItem(0,0,bndry->m_Numb,toWrite);
					}
					if(m_MainClusters->m_oCQO.m_bLRatio && vInfo.size()>iC)
					{	sprintf(toWrite,"   LRatio = %g ",vInfo[iC].m_dLRatio);
						AddItem(0,0,bndry->m_Numb,toWrite);
					}
					CCluster& oC = *m_MainClusters;
					if(oC.m_oCQO.m_bFindBestDims && oC.m_vBestDims.Rows()>=iC)
					{	A2D<int>& vBestDims = oC.m_vBestDims;
						int iD = 0, iBestDims = oC.m_oCQO.m_iBestDims;
						MY_STR_STACK& vAxes = m_MainDataStack->m_AxesStack;
						sprintf(toWrite,"      Best %d dims{   Entropy  Mean  Stdev  Min  Max  Range",iBestDims);AddItem(0,0,0,toWrite);
						for(iD=iBestDims-1;iD>=0;iD--)
						{	sprintf(toWrite,"         %s", (*vAxes[vBestDims[iC][iD]]).GetBuffer() );
							if(m_MainDataStack->m_MainMean.size()>=iD)
							{	char tmp[1024];
								sprintf(tmp,"     %.2f    %.2f    %.2f   %.2f   %.2f   %.2f",
												m_MainDataStack->m_MainEntropy[vBestDims[iC][iD]+1],
												m_MainDataStack->m_MainMean[vBestDims[iC][iD]+1],
												m_MainDataStack->m_MainStdev[vBestDims[iC][iD]+1],
												m_MainDataStack->m_MainMin[vBestDims[iC][iD]+1],
												m_MainDataStack->m_MainMax[vBestDims[iC][iD]+1],
												m_MainDataStack->m_MainRange[vBestDims[iC][iD]+1]); 
								strcat(toWrite,tmp);
							}
							AddItem(0,0,0,toWrite); 
						}
						AddItem(0,0,0,"   }");
					}
				}	
				AddItem(0,0,0,"}");
			}
		}
	}
	else
	{
		int iC = 1;
		int iClusts = which == CLUST_ORIG ? m_MainDataStack->m_NumOriginalCl : m_MainClusters->m_pNumClusts[which];

		for (iC = 1; iC <= iClusts; iC++)
		{
			AddItem(0,0,0,strStar);
			
			sprintf(toWrite,"Cluster %d",iC);
			AddItem(0,0,0,toWrite);
			
			AddItem(0,0,0,"{");

			if(vInfo.size()>iC)
			{	sprintf(toWrite,"   NumLoadedSpikes = %d",vInfo[iC].m_iSz); AddItem(0,0,0,toWrite);
				sprintf(toWrite,"   TotalKLD = %.2f ",vInfo[iC].m_fBGInfoGain+vInfo[iC].m_fInterClustGain);
				AddItem(0,0,0,toWrite);

				if(m_MainClusters->m_oCQO.m_bInterKLD)
				{	sprintf(toWrite,"   BGKLD=%.2f  InterKLD=%.2f  ClosestClusterID=%d",
							vInfo[iC].m_fBGInfoGain,vInfo[iC].m_fInterClustGain,vInfo[iC].m_iClosestID);
					AddItem(0,0,0,toWrite);
				}
				if(m_MainClusters->m_oCQO.m_bFastKLD)
				{	sprintf(toWrite,"   KNNCC=%.4f",vInfo[iC].m_fPrctKNNInClust); AddItem(0,0,0,toWrite);
				}
				if(m_MainClusters->m_oCQO.m_bSilhouette)
				{	sprintf(toWrite,"   SilhouetteWidth=%.4f",vInfo[iC].m_fSilhouetteWidth); AddItem(0,0,0,toWrite);
				}
			}
			if(m_MainClusters->m_oCQO.m_bIsolationDist && vInfo.size()>iC)
			{	sprintf(toWrite,"   IsolationDist = %.2f ",vInfo[iC].m_dIsolationDist);
				AddItem(0,0,0,toWrite);
			}
			if(m_MainClusters->m_oCQO.m_bLRatio && vInfo.size()>iC)
			{	sprintf(toWrite,"   LRatio = %g ",vInfo[iC].m_dLRatio);
				AddItem(0,0,0,toWrite);
			}
			AddItem(0,0,0,"}");
		}
	}
	HICON pom1 = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CWnd::SetIcon(pom1,true);
}

void CDialList::OnOK() 
{
	StoreWindowPosition(_T("Main\\ListWindow")); 	
	cdxCSizingDialog::OnOK();
}

void CDialList::OnSelchangeList1() 
{
	int i = m_wndList.GetCurSel();

	AxX = AxStackX[i];
	AxY = AxStackY[i];
	Cl =  ClStack[i];
}

void CDialList::OnDblclkList1() 
{
	OnOK();
}

void CDialList::OnBnClickedOk()
{
	OnOK();
}
