// $Id: TimeEv.cpp,v 1.13 2008/11/27 20:22:13 samn Exp $ 
// TimeEv.cpp : implementation file
//

#include "stdafx.h"
#include "wclust.h"
#include "TimeEv.h"
#include "Vertex.h"
#include "Sync.h"
#include "Cluster.h"
#include ".\timeev.h"
#include "DlgEEGGain.h"
#include "StringUtils.h"
#include "Hist.h"
#include "VideoDlg.h"
#include "CBmpFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int IDT_CLUSTERING = 110;
const int IDT_PREPARE = 111;
const int IDT_PLAY = 112;
const int IDT_VIDEO_FRAME = 164;

/////////////////////////////////////////////////////////////////////////////
// CTimeEv dialog

CTimeEv::CTimeEv(CWnd* pParent, vector<string>* pvNames,
				 CVerxStack* MainDataStack,
				 CPaletteStack* MainPalette,
				 CCluster* MainClusters)
	: cdxCSizingDialog(CTimeEv::IDD, pParent)
	, m_iPlaySpeed(999)
	, m_MainDataStack(MainDataStack)
	, m_MainPalette(MainPalette)
	, m_MainClusters(MainClusters)
{
	//{{AFX_DATA_INIT(CTimeEv)
	m_EditTime = 0.0f;
	//}}AFX_DATA_INIT
	
	m_vGainList = vector<double>(m_MainDataStack->m_MainEEGStack.GetChannels(),1.0);
	InitEEGNames(pvNames);
	m_hdd=0;
	m_pDIBFrame = 0;
	m_iFrameH = 0;
	m_iFrameW = 0;
	m_iFrameOffset = 0;
}

CTimeEv::~CTimeEv()
{
	if(m_oAVIR.IsOpen())
		m_oAVIR.Close();
	if(m_hdd)
		DrawDibClose(m_hdd);
	if(m_pDIBFrame)
		delete [] m_pDIBFrame;
}

void CTimeEv::InitEEGNames(vector<string>* pNames)
{
	char str[1024];
	if(!pNames)
	{	int i;
		for(i=0;i<m_MainDataStack->m_MainEEGStack.GetChannels();i++)
		{	
			sprintf(str,"eeg.%d",i+1);
			m_vEEGNames.push_back(str);
		}
	}
	else
		m_vEEGNames = *pNames;
	m_vEEGNames.resize(m_MainDataStack->m_MainEEGStack.GetChannels());
}


void CTimeEv::DoDataExchange(CDataExchange* pDX)
{
	cdxCSizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeEv)
	DDX_Control(pDX, IDC_TE_TAKE_PICT, m_wndTakePict);
	DDX_Control(pDX, IDC_MAX, m_wndMax);
	DDX_Control(pDX, IDC_TE_PLAY, m_wndPlay);
	DDX_Control(pDX, IDC_TE_CLEAR, m_wndClear);
	DDX_Control(pDX, IDC_BUTTON_SET_GAINS, m_wndSetEEGGains);
	DDX_Control(pDX, IDC_CHECK_SPIKE, m_wndCheckSpike);
	DDX_Control(pDX, IDC_CHECK_RASTER, m_wndCheckRaster);
	DDX_Control(pDX, IDC_CMB_ROOM, m_wndCmbRoom);
	DDX_Control(pDX, IDC_CMB_ARENA, m_wndCmbArena);
	DDX_Control(pDX, IDC_CMB_ORIG_NEW, m_wndCmbOrigNew);
	DDX_Control(pDX, IDC_LIST_SPIKE_CL, m_wndListSpikeCl);
	DDX_Control(pDX, IDC_LIST_ROOM_CL, m_wndListRoomCl);
	DDX_Control(pDX, IDCANCEL, m_wndCancel);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_BUTTON_VID_DLG_OPEN, m_wndVideo);
	DDX_Control(pDX, IDC_LIST_ARENA_CL, m_wndListArenaCl);
	DDX_Control(pDX, IDC_LIST_SCALE, m_wndListScale);
	DDX_Control(pDX, IDC_SPIN_TIME, m_wndSpinTime);
	DDX_Control(pDX, IDC_SPINEEGPLAYSPEED, m_wndSpinPlaySpeed);
	DDX_Control(pDX, IDC_EDITEEGPLAYSPEED, m_wndPlaySpeed);
	DDX_Control(pDX, IDC_EDIT_TIME, m_wndEditTime);
	DDX_Control(pDX, IDC_SCROLL_TIME, m_wndScrollTime);
	DDX_Text(pDX, IDC_EDIT_TIME, m_EditTime);
	DDV_MinMaxFloat(pDX, m_EditTime, 0.f, 1.e+009f);
	DDX_Control(pDX, IDC_SPINEEGPLAYSPEED, m_wndSpinPlaySpeed);
	DDX_Text(pDX, IDC_EDITEEGPLAYSPEED, m_iPlaySpeed);
	DDV_MinMaxInt(pDX, m_iPlaySpeed, 1, 1000000);
	DDX_Control(pDX, IDC_BUTTON_SET_TIME, m_wndSetTime);
	DDX_Control(pDX, IDC_EDIT_SET_TIME, m_wndEditSetTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeEv, cdxCSizingDialog)
	//{{AFX_MSG_MAP(CTimeEv)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_EDIT_TIME, OnKillfocusEditTime)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ARENA_CL, OnItemchangedListArenaCl)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROOM_CL, OnItemchangedListRoomCl)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SPIKE_CL, OnItemchangedListSpikeCl)
	ON_CBN_SELCHANGE(IDC_CMB_ORIG_NEW, OnSelchangeCmbOrigNew)
	ON_BN_CLICKED(IDC_CHECK_RASTER, OnCheckRaster)
	ON_BN_CLICKED(IDC_CHECK_SPIKE, OnCheckSpike)
	ON_BN_CLICKED(IDC_MAX, OnMax)
	ON_CBN_SELCHANGE(IDC_CMB_ARENA, OnSelchangeCmbArena)
	ON_CBN_SELCHANGE(IDC_CMB_ROOM, OnSelchangeCmbRoom)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SCALE, OnItemchangedListScale)
	ON_BN_CLICKED(IDC_TE_PLAY, OnTePlay)
	ON_BN_CLICKED(IDC_TE_CLEAR, OnTeClear)
	ON_BN_CLICKED(IDC_TE_TAKE_PICT, OnTeTakePict)
	ON_WM_VSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME, OnDeltaposSpinTime)
	ON_EN_CHANGE(IDC_EDIT_TIME, OnEnChangeEditTime)
	ON_BN_CLICKED(IDC_BUTTON_SET_GAINS, OnBnClickedButtonSetGains)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINEEGPLAYSPEED, OnDeltaposSpineegplayspeed)
	ON_BN_CLICKED(IDC_BUTTON_VID_DLG_OPEN, OnBnClickedButtonVidDlgOpen)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SET_TIME, OnBnClickedButtonSetTime)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeEv message handlers

BOOL CTimeEv::OnInitDialog() 
{
	cdxCSizingDialog::OnInitDialog();
	RestoreWindowPosition(_T("Main\\TimeWindow")); 	

	AddSzControl( m_wndScrollTime, mdResize, mdRepos); 
	AddSzControl( m_wndListArenaCl, mdNone, mdRepos); 
	AddSzControl( m_wndListRoomCl, mdNone, mdRepos); 
	AddSzControl( m_wndCmbArena, mdNone, mdRepos); 
	AddSzControl( m_wndCmbRoom, mdNone, mdRepos); 

	AddSzControl( m_wndCmbOrigNew, mdRepos, mdRepos); 
	AddSzControl( m_wndSpinTime, mdRepos, mdRepos); 
	AddSzControl( m_wndEditTime, mdRepos, mdRepos);
	AddSzControl( m_wndCheckRaster, mdRepos, mdRepos);
	AddSzControl( m_wndCheckSpike, mdRepos, mdRepos);
	AddSzControl( m_wndPlay, mdRepos, mdRepos); 
	AddSzControl( m_wndClear, mdRepos, mdRepos); 
	AddSzControl( m_wndPlaySpeed, mdRepos, mdRepos);
	AddSzControl( m_wndSpinPlaySpeed, mdRepos, mdRepos);
	AddSzControl( m_wndSetEEGGains, mdRepos, mdRepos);
	AddSzControl( m_wndVideo, mdRepos, mdRepos);
	AddSzControl( m_wndSetTime, mdRepos, mdRepos);
	AddSzControl( m_wndEditSetTime, mdRepos, mdRepos);
	AddSzControl( m_wndMax, mdRepos, mdRepos); 
	
	AddSzControl( m_wndTakePict, mdRepos, mdRepos); 
	AddSzControl( m_wndOK, mdRepos, mdRepos); 
	AddSzControl( m_wndCancel, mdRepos, mdRepos); 

	AddSzControl( m_wndListScale, mdRepos, mdResize); 
	AddSzControl( m_wndListSpikeCl, mdRepos, mdRepos); 

	m_wndCheckSpike.SetCheck(0);
	m_wndCheckRaster.SetCheck(1);
	m_wndCmbOrigNew.AddString(TEXT("Your new cl."));
	m_wndCmbOrigNew.AddString(TEXT("Original cl."));
	m_wndCmbArena.AddString(TEXT("points"));
	m_wndCmbArena.AddString(TEXT("circles"));
	m_wndCmbArena.AddString(TEXT("vectors"));
	m_wndCmbRoom.AddString(TEXT("points"));
	m_wndCmbRoom.AddString(TEXT("circles"));
	m_wndCmbRoom.AddString(TEXT("vectors"));
	m_wndCmbArena.SetCurSel(0);
	m_wndCmbRoom.SetCurSel(0);
	mPrepareFlag = 0;

	m_HCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);

	GetClientRect(m_MainRect);
	m_videoRect = m_MainRect;
	m_videoRect.left += 11;
	m_videoRect.bottom -= 15; //45
	m_videoRect.top = m_videoRect.bottom - 130; 
	m_videoRect.right = m_videoRect.left + 130;
	m_videoRect.left += 1;
	m_videoRect.top += 1;
	m_videoRect.right -= 1;
	m_videoRect.bottom -= 1;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	cdxCSizingDialog::OnShowWindow(bShow, nStatus);
	
	HICON pom1 = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CWnd::SetIcon(pom1,true);

	// for init clusters and spikes (it can be in thread)
	SetTimer(IDT_PREPARE,1,NULL);

	clView = vector<unsigned char>(256);
	std::fill(clView.begin(),clView.end(),(unsigned char)0xFF);

	if(!m_MainDataStack->m_bUseEEG)
		m_wndSetEEGGains.EnableWindow(0);

	int iW = m_MainDataStack->whichDraw;
	switch(iW)
	{
	case CLUST_INFO:
	case CLUST_KM:
	case CLUST_AP:
	case CLUST_KK:
	case CLUST_FL:
		maxCluster = m_MainClusters->m_pNumClusts[iW];
	default:
		maxCluster = ( m_MainDataStack->m_NumOriginalCl > (m_MainClusters->m_Count - 1) ) ? m_MainDataStack->m_NumOriginalCl : ( m_MainClusters->m_Count - 1 );
	}

	FillImageList();
	FillScaleList();
	FillClList(1);	// spikes
	FillClList(2);	// arena
	FillClList(4);	// room

	fileOpen = 0;
	if (m_MainDataStack->FileType == 1 && m_MainDataStack->m_NumVerx < m_MainDataStack->m_NumVerxAll)
	{
		fileOpen = 1;
		if (! file.Open(m_MainDataStack->m_FileName,CFile::modeRead) )
		{
			fileOpen = 0;
		}
	}

	timeWTimes = 10; // this is default, it can be arbitrary
	MIN_TIME_WINDOW = m_MainDataStack->NUM_SAMPLES*1000 /m_MainDataStack->SAMPLE_FREQ ;
	if (MIN_TIME_WINDOW == 0)
		MIN_TIME_WINDOW = 1;
	MIN_STEP_WINDOW = MIN_TIME_WINDOW;

	timeStampFirst = ((m_MainDataStack->m_SpikesStack.begin()))->TimeStamp - m_MainDataStack->NUM_SAMPLES*10000.0 /m_MainDataStack->SAMPLE_FREQ;
	int TSLast = ((m_MainDataStack->m_SpikesStack.end() - 1))->TimeStamp;
	if ( (m_MainDataStack->InBpfExist & 3) || (m_MainDataStack->LoadedAdd & 3) )
	{
		if ( ((CSync*)*(m_MainDataStack->m_MainSyncStack.SyncStack.begin()))->GetTimeStamp() < timeStampFirst )
			timeStampFirst = ((CSync*)*(m_MainDataStack->m_MainSyncStack.SyncStack.begin()))->GetTimeStamp();
		if ( ((CSync*)*(m_MainDataStack->m_MainSyncStack.SyncStack.begin() + m_MainDataStack->m_MainSyncStack.SyncStack.size() - 1))->GetTimeStamp() > ((m_MainDataStack->m_SpikesStack.end()-1))->TimeStamp)
			TSLast = ((CSync*)*(m_MainDataStack->m_MainSyncStack.SyncStack.begin() + m_MainDataStack->m_MainSyncStack.SyncStack.size() - 1))->GetTimeStamp();
	}

	MAX_TIME_WINDOW = ( TSLast - timeStampFirst) / 10 + 5 * MIN_STEP_WINDOW;
	MAX_STEP_TIMES = MAX_TIME_WINDOW / MIN_STEP_WINDOW + 1;
	if ( MAX_STEP_TIMES > 32767 )  // see Help of CScrollBar -> SetScrollBarRange, it cannot be grater than 32.767
	{
		MIN_STEP_WINDOW = MAX_TIME_WINDOW / 32767 + ( (MAX_TIME_WINDOW % 32767) ? 1 : 0 );  
		MAX_STEP_TIMES = MAX_TIME_WINDOW / MIN_STEP_WINDOW + MIN_STEP_WINDOW;
	}
	MAX_TIME_TIMES = MAX_TIME_WINDOW / MIN_TIME_WINDOW;
	
	timeWindow = timeWTimes * MIN_TIME_WINDOW;  // initialization
	m_EditTime = float(timeWindow) / 10;
	UpdateData(false);
	startStepTimes = 0;
	startTime = startStepTimes * MIN_STEP_WINDOW;

	pStartTimeTimes = startTime / MIN_TIME_WINDOW;
	pMode = 0;
	pAddTimes = 1;

	m_wndScrollTime.SetScrollRange(0,MAX_STEP_TIMES);
	m_wndCmbOrigNew.SetCurSel( m_MainDataStack->whichDraw );


	m_wndTakePict.EnableWindow(false);
	m_wndClear.EnableWindow(false);
	m_wndSpinTime.SetRange(-10,10);
	m_wndSpinTime.SetPos(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// this is for OnPaint functions
unsigned char CTimeEv::GetParameters(unsigned char *spikes,unsigned char *eeg ) 
{
	int Selected;
	// Spike clusts to view
	for ( int i = 0; i <= maxCluster + 1; i++ )
	{
		if ( i == 0 )
			Selected = 255;
		else
			Selected = i - 1;
		if ( m_wndListSpikeCl.GetCheck(i) )
			clView[ Selected ] |= 1;
		else
			clView[ Selected ] &= 0xFE;
	}
	// Arena clusts to view
	for ( i = 0; i <= maxCluster + 1; i++ )
	{
		if ( i == 0 )
			Selected = 255;
		else
			Selected = i - 1;
		if ( m_wndListArenaCl.GetCheck(i) )
			clView[ Selected ] |= 2;
		else
			clView[ Selected ] &= 0xFD;
	}
	// Room clusts to view
	for ( i = 0; i <= maxCluster + 1; i++ )
	{
		if ( i == 0 )
			Selected = 255;
		else
			Selected = i - 1;
		if ( m_wndListRoomCl.GetCheck(i) )
			clView[ Selected ] |= 4;
		else
			clView[ Selected ] &= 0xFB;
	}

	// Scale checked items
	*spikes = 0;
	*eeg = 0;
	unsigned char checked = 0;
	for ( i = 0; i < scaleItems; i++ )
	{
		if ( m_wndListScale.GetCheck( i ) )
		{
			checked++;
			if ( i < m_MainDataStack->NUM_CHANNELS )
				*spikes += 1;
			if ( i >= m_MainDataStack->NUM_CHANNELS )
				*eeg += 1;
		}
	}
	return checked;
}

bool SaveBMP(BITMAPINFOHEADER* pBMP, char* fname);

bool makeRgbDIB(byte*& pdib,int w,int h,int xres,int yres)
{
	//set up color output dib////////////////////////////////
	int nOutRowBytes = ((w*24+31)/32)*4;
	long OutLen = nOutRowBytes * h;
	int nOutHeaderSize = sizeof(BITMAPINFOHEADER);
	pdib = new byte[OutLen+nOutHeaderSize];

	if(!pdib)
	{
		return false;
	}

	BITMAPINFO* outbmpinfo = (BITMAPINFO*)pdib;
	outbmpinfo->bmiHeader.biWidth =  w;
	outbmpinfo->bmiHeader.biHeight = h;
	outbmpinfo->bmiHeader.biSizeImage = OutLen;
	outbmpinfo->bmiHeader.biXPelsPerMeter = (xres*39.372+0.5);
	outbmpinfo->bmiHeader.biYPelsPerMeter = (yres*39.372+0.5);

	outbmpinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	outbmpinfo->bmiHeader.biCompression = BI_RGB;

	outbmpinfo->bmiHeader.biBitCount = 24;
	outbmpinfo->bmiHeader.biClrUsed = 0;
	outbmpinfo->bmiHeader.biClrImportant = 0;
	outbmpinfo->bmiHeader.biPlanes = 1;

	byte* tmpOut = pdib+nOutHeaderSize;

	ZeroMemory(tmpOut,OutLen);
	////////////////////////////////////////////////////////////
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	GetClientRect(m_MainRect);
	m_SpikesRect = m_MainRect;
	m_SpikesRect.left += 11;
	m_SpikesRect.top += 11;
	m_SpikesRect.right -= 100; // -10
	m_SpikesRect.bottom -= 219; //  179; // 209

	CDC dcTemp;
	CBitmap bmTemp;
	bmTemp.CreateCompatibleBitmap(&dc,m_SpikesRect.right,m_SpikesRect.bottom);
	dcTemp.CreateCompatibleDC(&dc);
	CBitmap* bmOld = dcTemp.SelectObject(&bmTemp);
	
	// main RECTANGLE
	dcTemp.Rectangle(m_SpikesRect);
	
	m_SpikesRect.left += 1;
	m_SpikesRect.right -= 1;
	m_SpikesRect.bottom -= 1;
	m_SpikesRect.top += 1;

	// AXIS for SCALE
	int raster = m_SpikesRect.Width() / 10;
	CPen rasterPen(PS_SOLID,1, RGB(230,230,230)) , rasterPenRed(PS_SOLID,1,RGB(200,0,0));
	CPen *pOldRasterPen = dcTemp.SelectObject(&rasterPen);
	for (int iRaster = 1; iRaster < 10; iRaster++)
	{	if(iRaster==5) //paint middle line as red
		{	dcTemp.SelectObject(&rasterPenRed);
			dcTemp.MoveTo(m_SpikesRect.left + iRaster*raster, m_SpikesRect.top);
			dcTemp.LineTo(m_SpikesRect.left + iRaster*raster, m_SpikesRect.bottom);
			dcTemp.SelectObject(&rasterPen);
		}
		else	//paint other lines as gray
		{	dcTemp.MoveTo(m_SpikesRect.left + iRaster*raster, m_SpikesRect.top);
			dcTemp.LineTo(m_SpikesRect.left + iRaster*raster, m_SpikesRect.bottom);
		}
	}
	dcTemp.SelectObject(pOldRasterPen);

	m_ArenaRect = m_MainRect;
	m_ArenaRect.left += 280;
	m_ArenaRect.bottom -= 47;  // 45
	m_ArenaRect.top = m_ArenaRect.bottom - 130; 
	m_ArenaRect.right = m_ArenaRect.left + 130;
	m_ArenaRect.left += 1;
	m_ArenaRect.top += 1;
	m_ArenaRect.right -= 1;
	m_ArenaRect.bottom -= 1;

	m_RoomRect = m_MainRect;
	m_RoomRect.left += 11;
	m_RoomRect.bottom -= 47; //45
	m_RoomRect.top = m_RoomRect.bottom - 130; 
	m_RoomRect.right = m_RoomRect.left + 130;
	m_RoomRect.left += 1;
	m_RoomRect.top += 1;
	m_RoomRect.right -= 1;
	m_RoomRect.bottom -= 1;

	int oldBkMode = dc.SetBkMode( TRANSPARENT );
	dc.TextOut(m_ArenaRect.left, m_ArenaRect.top - 10, "Arena");
	dc.TextOut(m_RoomRect.left, m_RoomRect.top - 10, "Room");
	dc.TextOut(m_MainRect.right - 227, m_MainRect.bottom - 207, "Time window [ms]");
	dc.TextOut(m_MainRect.right - 167, m_MainRect.bottom - 137, "PlaySpeed");
	dc.SetBkMode( oldBkMode );

	unsigned char checked = 0;
	char			params;
	unsigned char	spikesCh, eegCh;
	CRect			signalRect = m_SpikesRect;
	float oneWinY = 0.0f;
	
	if(mPrepareFlag)
		checked = GetParameters( & spikesCh, & eegCh );
	
	if (checked)
	{
		oneWinY = m_SpikesRect.Height() / ( spikesCh + eegCh );

		dc.Ellipse(m_ArenaRect);
		dc.Ellipse(m_RoomRect);

		// Arena win
		if ( (m_MainDataStack->InBpfExist & 1) || (m_MainDataStack->LoadedAdd & 1) )
		{
			params = 2; // iixxxooo, xxx - points/circles/vectors, ooo - arena/room
			params += 8 * m_wndCmbArena.GetCurSel();
				m_MainDataStack->m_MainSyncStack.DrawArenaRoom(&dc,m_ArenaRect, timeStampFirst + 10*startTime, timeWindow, params, &clView[0], m_MainDataStack->whichDraw);
		}

		// Room win
		if ( (m_MainDataStack->InBpfExist & 2) || (m_MainDataStack->LoadedAdd & 2) )
		{
			params = 4; // iixxxooo, xxx - points/circles/vectors, ooo - arena/room
			params += 8 * m_wndCmbRoom.GetCurSel();
				m_MainDataStack->m_MainSyncStack.DrawArenaRoom(&dc,m_RoomRect, timeStampFirst + 10*startTime, timeWindow, params, &clView[0], m_MainDataStack->whichDraw);
		}

		if ( spikesCh > 0 )
		{
			//	Spike win
			//	params : xxeeeeii, ii - R/S, eeee - electrode to show
			if ( timeWindow / MIN_TIME_WINDOW < 50 )
			{
				if ( !m_wndCheckSpike.GetCheck() )	// choice of R/S
					params = 0; // Raster
				else{
					if ( m_wndCheckRaster.GetCheck() )
						params = 1; // R/S
					else 
						params = 2; // Spike
				}
			}
			else
				params = 0;
			unsigned char bite = 4; // 100 b
			for ( int i = 0; i < m_MainDataStack->NUM_CHANNELS; i++ )
				params |= (m_wndListScale.GetCheck( i )) ? ( bite << i ) : 0 ;
			signalRect.bottom = spikesCh * (m_SpikesRect.bottom - m_SpikesRect.top) / checked;
			switch(m_MainDataStack->whichDraw)
			{
			case CLUST_ORIG:
			case CLUST_USER:
				m_MainDataStack->DrawTimeSpikes(&dcTemp, signalRect, startTime, timeWindow, timeStampFirst, params, &clView[0], &file);
				break;
			default:
				m_MainDataStack->DrawTimeSpikesAutoC(&dcTemp, signalRect, startTime, timeWindow, timeStampFirst, params, &clView[0], &file);
				break;
			}

			int j=0;
			for (i = 0; i < spikesCh; i++)
			{
				while ( !m_wndListScale.GetCheck( j ) )
					j++;
				char textCh[10];
				sprintf(textCh,"ch.%d",j+1);
				dcTemp.TextOut(m_SpikesRect.left + 1, m_SpikesRect.top + i * oneWinY,textCh);
				j++;
			}
		}

		if ( eegCh > 0 )
		{
			//	Spike win - EEG
			//	params : xxeeeeii, ii - R/S, eeee - electrode to show
			unsigned int paramInt=0;
			unsigned int bite = 4; // 100 b
			for ( int i = m_MainDataStack->NUM_CHANNELS; i < m_MainDataStack->NUM_CHANNELS + m_MainDataStack->m_MainEEGStack.GetChannels(); i++ )
				paramInt |= (m_wndListScale.GetCheck( i )) ? ( bite << (i-m_MainDataStack->NUM_CHANNELS) ) : 0;
			
			signalRect.top = spikesCh * (m_SpikesRect.bottom - m_SpikesRect.top) / checked;
			signalRect.bottom = m_SpikesRect.bottom;

			int iEEGChans = m_vEEGNames.size();
			double dGainSum = 0.0, dGainMin = 0.0;
			for(i=0;i<iEEGChans;i++)
			{
				if(!m_wndListScale.GetCheck( i + m_MainDataStack->NUM_CHANNELS))
					continue;
				dGainSum += m_vGainList[i];
				if(m_vGainList[i] < dGainMin)
					dGainMin = m_vGainList[i];
			}

			m_MainDataStack->m_MainEEGStack.DrawEEG(&dcTemp, signalRect, 10*startTime + timeStampFirst, timeWindow, paramInt, m_vGainList);

			int iYPos = m_SpikesRect.top + spikesCh*oneWinY;
			for (i = 0; i < iEEGChans; i++)
			{	if( !m_wndListScale.GetCheck( i + m_MainDataStack->NUM_CHANNELS) )
					continue;
				dcTemp.TextOut(m_SpikesRect.left + 1, iYPos ,m_vEEGNames[i].c_str());
				char textCh[32];
				sprintf(textCh,"%.1fX",m_vGainList[i]);
				dcTemp.TextOut(m_SpikesRect.right - 47, iYPos ,textCh);
				iYPos+=(m_vGainList[i]/dGainSum)*signalRect.Height();;
			}
		}
	}

	char textTime[20]; // time corresponds to middle of window
	sprintf(textTime,"time: %5.3f s", GetTimeMS()/1000.0 );
	dcTemp.TextOut(m_SpikesRect.left + 50,m_SpikesRect.top ,textTime);

	if(m_oAVIR.IsOpen())
		PaintVideoFrame(dc);

	dc.BitBlt(m_SpikesRect.left-1,m_SpikesRect.top-1,m_SpikesRect.Width()+2,m_SpikesRect.Height()+2,&dcTemp,m_SpikesRect.left-1,m_SpikesRect.top-1,SRCCOPY);
	dcTemp.SelectObject(bmOld);

//	char a[200];
//	sprintf(a,"%d " ,m_MainDataStack->FileType);
//	dc.TextOut(10,40,a);

	// Do not call cdxCSizingDialog::OnPaint() for painting messages
}

void CTimeEv::PaintVideoFrame(CPaintDC& dc)
{
	if(!m_hdd)
		return;

	m_videoRect = m_MainRect;
	
	m_videoRect.left += 11 + 550;
	m_videoRect.bottom -= 4; //45
	m_videoRect.top = m_videoRect.bottom - 206;
	m_videoRect.right = m_videoRect.left + 275;

	m_videoRect.left += 1;
	m_videoRect.top += 1;
	m_videoRect.right -= 1;
	m_videoRect.bottom -= 1;

	bool bChangeSz = false;

	if(m_videoRect.Width() != m_iFrameW)
	{
		m_iFrameW = m_videoRect.Width();
		bChangeSz = true;
	}
	
	if(m_videoRect.Height() != m_iFrameH)
	{
		m_iFrameH = m_videoRect.Height();
		bChangeSz = true;
	}

	if(bChangeSz || !m_pDIBFrame)
	{
		if(m_pDIBFrame) delete [] m_pDIBFrame;
		makeRgbDIB(m_pDIBFrame,m_iFrameW,m_iFrameH,0,0);
	}
		
	LPBITMAPINFOHEADER pBMP = m_oAVIR.GetCurrentFrame();

//	SaveBMP(pBMP,"orig_frame.bmp");

	if(!pBMP)
	{	Write2Log("Couldn't extract frame %d",m_oAVIR.CurrentFrame());
		return;
	}

	BITMAPINFOHEADER* pbmpnew = (BITMAPINFOHEADER*)m_pDIBFrame;

	StretchDIB(pbmpnew,(byte*)pbmpnew+sizeof(BITMAPINFOHEADER),0,0,pbmpnew->biWidth,pbmpnew->biHeight,
		pBMP,(byte*)pBMP+sizeof(BITMAPINFOHEADER),0,0,pBMP->biWidth,pBMP->biHeight);

	//byte* pdata=(byte*)pBMP+pBMP->biSize+pBMP->biClrUsed*sizeof(RGBQUAD);
	byte* pdata = (byte*)pbmpnew+sizeof(BITMAPINFOHEADER);

//	SaveBMP(pbmpnew,"new_frame.bmp");

	// Convert Data To Requested Bitmap Format & Draw IT!!!
	DrawDibDraw(m_hdd, dc.GetSafeHdc(), m_videoRect.left, m_videoRect.top, pbmpnew->biWidth, pbmpnew->biHeight, pbmpnew, pdata, 0, 0, pbmpnew->biWidth, pbmpnew->biHeight, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
	case IDT_PREPARE:
		KillTimer(IDT_PREPARE);
//		MSG message;
		mClustered = 0;
//		SetTimer(IDT_CLUSTERING,100,NULL);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		if ( (m_MainDataStack->InBpfExist & 3) || (m_MainDataStack->LoadedAdd & 3) )
			m_MainDataStack->m_MainSyncStack.PrepareClustering();
		for ( iSpike = m_MainDataStack->m_SpikesStack.begin(); iSpike != m_MainDataStack->m_SpikesStack.end(); iSpike++ )
		{
			
			if ( iSpike->type == 0)  // 0 == SPIKE_ON_DISK
			{ // SPIKE_ON_DISK
				CVertex* verx = (CVertex*) new CVertex(m_MainPalette);

				m_MainDataStack->LoadBPFElde(verx,&file,iSpike->offset);
			
				if (m_MainClusters->GetSwap())
				{
					verx->Swap(m_MainDataStack->NUM_CHANNELS,m_MainDataStack->NUM_SAMPLES);
				}
				m_MainClusters->CalcParamOneSpike( m_MainDataStack, verx, &(*iSpike) );
				verx->CheckNoise();
				m_MainClusters->Clustering( verx );
				iSpike->cluster = verx->GetClust();
				delete verx;
			}
			else
			{ // SPIKE_IN_MEMORY
				iSpike->cluster = m_MainDataStack->GetClust(iSpike->offset);
			}
			if ( (m_MainDataStack->InBpfExist & 3) || (m_MainDataStack->LoadedAdd & 3) )
				m_MainDataStack->m_MainSyncStack.Clustering( iSpike->TimeStamp, iSpike->cluster, iSpike->origCluster );
			mClustered++;
			// WINDOWS
//			if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
//			{
//				::TranslateMessage(&message);
//				::DispatchMessage(&message);
//			}
			// BACK
		}
		if ( (m_MainDataStack->InBpfExist & 3) || (m_MainDataStack->LoadedAdd & 3) )
			m_MainDataStack->m_MainSyncStack.FinishClustering();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
//		KillTimer(IDT_CLUSTERING);
		mPrepareFlag = 1;

		InvalidateRect(m_SpikesRect,false);
		InvalidateRect(m_ArenaRect,false);
		InvalidateRect(m_RoomRect,false);

		break;
	case IDT_CLUSTERING:
//		m_wndProgress.SetPos(mClustered*100/m_MainDataStack->m_NumVerxAll);
		break;
	case IDT_PLAY:
		if ( pStartTimeTimes >= MAX_TIME_TIMES - timeWTimes)
		{
			KillTimer( IDT_PLAY );
			SetDlgItemText(IDC_TE_PLAY,TEXT("&Play"));
			pMode = 0;
		}
		else 
		{
			startStepTimes = m_wndScrollTime.GetScrollPos() + (1+m_iPlaySpeed/1000);

			startTime = startStepTimes * MIN_STEP_WINDOW;
			pStartTimeTimes = startTime / MIN_TIME_WINDOW;

			m_wndScrollTime.SetScrollPos( startStepTimes );

			InvalidateRect(m_SpikesRect,false);
			InvalidateRect(m_ArenaRect,false);
			InvalidateRect(m_RoomRect,false);
		}
		if(m_oAVIR.IsOpen())
			UpdateAVIFrame();
		break;
	}
	cdxCSizingDialog::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::UpdateAVIFrame()
{
	//time corresponds to red bar in middle of window
	int iFrame = -m_iFrameOffset + GetTimeMS() / m_oAVIR.MSPerFrame() ;
	if(iFrame < m_oAVIR.LastFrame() && iFrame >= m_oAVIR.FirstFrame())
		m_oAVIR.GoToFrame(iFrame);
	else
	{
		Write2Log("Out of frames at fMilliSeconds=%5.3f, m_iFrameOffset=%d",GetTimeMS(),m_iFrameOffset);
		//char msg[1024];
		//sprintf(msg,"Out of frames at fMilliSeconds=%5.3f!",fMilliSeconds);
		//MessageBox(msg);
		//KillTimer(IDT_PLAY);
		//pMode = 0;
		//SetDlgItemText(IDC_TE_PLAY,TEXT("&Play"));
	}
	InvalidateRect(m_videoRect,false);		
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTimeEv::DestroyWindow() 
{
	if (fileOpen)
		file.Close();

	return cdxCSizingDialog::DestroyWindow();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnOK() 
{
	StoreWindowPosition(_T("Main\\TimeWindow")); 
	cdxCSizingDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnCancel() 
{
	cdxCSizingDialog::OnCancel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	const int SHIFTING = 1;
	
	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_SCROLL_TIME:
		switch (nSBCode)
		{
		case SB_LEFT:
			break;
		case SB_ENDSCROLL:
			break;
		case SB_LINELEFT:
			startStepTimes = m_wndScrollTime.GetScrollPos() - 1;
			if (startStepTimes<0)
				startStepTimes = 0;
			m_wndScrollTime.SetScrollPos(startStepTimes);
			break;
		case SB_LINERIGHT:
			startStepTimes = m_wndScrollTime.GetScrollPos() + 1;
			m_wndScrollTime.SetScrollPos(startStepTimes);
			break;
		case SB_PAGELEFT:
			break;
		case SB_PAGERIGHT:
			break;
		case SB_RIGHT:
			break;
		case SB_THUMBPOSITION:
			m_wndScrollTime.SetScrollPos(nPos);
			startStepTimes = nPos;
			break;
		case SB_THUMBTRACK:
			m_wndScrollTime.SetScrollPos(nPos);
			startStepTimes = nPos;
			break;
		}
		break;
	case IDC_SPIN_TIME:
		break;
	}
	startTime = startStepTimes * MIN_STEP_WINDOW;
	pStartTimeTimes = startTime / MIN_TIME_WINDOW;

	InvalidateRect(m_SpikesRect,false);
	InvalidateRect(m_ArenaRect,false);
	InvalidateRect(m_RoomRect,false);

	if(m_oAVIR.IsOpen())
		UpdateAVIFrame();
	
	// TODO: Add your message handler code here and/or call default
	cdxCSizingDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnMouseMove(UINT nFlags, CPoint point) 
{
//	::SetCursor(m_HCross);
	
	cdxCSizingDialog::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnKillfocusEditTime() 
{
	UpdateData();
	if (m_EditTime*10 < MIN_TIME_WINDOW)
		m_EditTime = float(MIN_TIME_WINDOW)/10;
	if (m_EditTime*10 > MAX_TIME_WINDOW)
//		m_EditTime = float(MAX_TIME_WINDOW)/10; // ?????????????????

	UpdateData(false);//this won't get called if if above is false, is that intent????????
	timeWindow = 10*m_EditTime;
	timeWTimes = timeWindow/MIN_TIME_WINDOW;

	//Write2Log("timeWindow=%d timeWTimes=%d MIN_TIME_WINDOW=%d",timeWindow,timeWTimes,MIN_TIME_WINDOW);

	int pos = m_wndScrollTime.GetScrollPos();
	int timeWStepT = ( ( timeWindow / MIN_STEP_WINDOW ) );
	m_wndScrollTime.SetScrollRange(0,MAX_STEP_TIMES - timeWStepT );
	if ( pos > MAX_STEP_TIMES - timeWStepT )
	{
		m_wndScrollTime.SetScrollPos( MAX_STEP_TIMES - timeWStepT );
		startStepTimes = MAX_STEP_TIMES - timeWStepT;
		startTime = startStepTimes * MIN_STEP_WINDOW;
		pStartTimeTimes = startStepTimes;
	}

	pAddTimes = timeWindow / (10 * MIN_TIME_WINDOW); //Write2Log("pAddTimes=%d",pAddTimes);
	if ( pAddTimes == 0 )
		pAddTimes++;

	InvalidateRect(m_SpikesRect,false);
	InvalidateRect(m_ArenaRect,false);
	InvalidateRect(m_RoomRect,false);

	if(m_oAVIR.IsOpen())
		UpdateAVIFrame();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnMax() 
{
	m_EditTime = float(MAX_TIME_WINDOW)/10;
	UpdateData(false);
	timeWindow = 10 * m_EditTime;
	timeWTimes = timeWindow/MIN_TIME_WINDOW;

	int pos = m_wndScrollTime.GetScrollPos();
	m_wndScrollTime.SetScrollRange(0,1);
	if ( pos > MAX_STEP_TIMES - timeWTimes )
	{
		m_wndScrollTime.SetScrollPos( 0 );
		startStepTimes = 0;
		startTime = startStepTimes * MIN_STEP_WINDOW;
		pStartTimeTimes = startStepTimes;
	}

	InvalidateRect(m_SpikesRect,false);
	InvalidateRect(m_ArenaRect,false);
	InvalidateRect(m_RoomRect,false);

	if(m_oAVIR.IsOpen())
		UpdateAVIFrame();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnCheckRaster() 
{
	if ( ! m_wndCheckSpike.GetCheck() && ! m_wndCheckRaster.GetCheck() )
		m_wndCheckRaster.SetCheck(1);
	else
		InvalidateRect(m_SpikesRect,false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnCheckSpike() 
{
	if ( ! m_wndCheckSpike.GetCheck() && ! m_wndCheckRaster.GetCheck() )
		m_wndCheckSpike.SetCheck(1);
	else
		InvalidateRect(m_SpikesRect,false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::FillImageList()
{
	CBitmap bitmap;
	m_imageList.Create(16, 16, ILC_COLORDDB , 8, 255); // 32, 32 for large icons

	bitmap.LoadBitmap(IDB_CCCCCC);	// NOISE
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_909090);	// non cl
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_FF0000);	// cl 1
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_0000FF);	// cl 2
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_00FF00);	// cl 3
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_F000F0);	// cl 4
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_9000F0);	// cl 5
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_00F0F0);	// cl 6
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_000000);	// cl 7
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F09000);	// cl 8
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F00090);	// cl 9
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 

	bitmap.LoadBitmap(IDB_900090);	// cl 10
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_9000F0);	// cl 11
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_00F090);	// cl 12
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_90F000);	// cl 13
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_009090);	// cl 14
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_909000);	// cl 15
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	
	bitmap.LoadBitmap(IDB_9090F0);	// cl 16
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_90F090);	// cl 17
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
	bitmap.LoadBitmap(IDB_F09090);	// cl 18
	m_imageList.Add(&bitmap,COLORREF(RGB(0,0,0))); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::FillScaleList()
{
	m_wndListScale.InsertColumn(0,"Signals",LVCFMT_LEFT,76);
	m_wndListScale.SetExtendedStyle( LVS_EX_CHECKBOXES );
	CString Str;
	int shift = 0;
	for (int i = 0; i < m_MainDataStack->NUM_CHANNELS; i++)
	{
		m_wndListScale.InsertItem(i + shift,Str);
		Str.Format("ch. %d",(i + 1 + shift));
		m_wndListScale.SetItemText(i + shift,0,Str);
		m_wndListScale.SetCheck(i + shift, true);
	}
	shift += m_MainDataStack->NUM_CHANNELS;

	if(m_MainDataStack->m_bUseEEG)
	{
		for (i = 0; i < m_MainDataStack->m_MainEEGStack.GetChannels(); i++)
		{
			m_wndListScale.InsertItem(i + shift,Str);
			m_wndListScale.SetItemText(i + shift,0,this->m_vEEGNames[i].c_str());
			m_wndListScale.SetCheck(i + shift, true);
		}
		shift += m_MainDataStack->m_MainEEGStack.GetChannels();
	}

	scaleItems = shift;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::FillClList(char numb)
{
	CString Str;
	CListCtrl *list;
	switch (numb)
	{
	case 1:	// spikes
		list = & m_wndListSpikeCl;
		Str.Format("Spikes cl.");
		break;
	case 2:	// arena
		list = & m_wndListArenaCl;
		Str.Format("Arena cl.");
		break;
	case 4:	//room
		list = & m_wndListRoomCl;
		Str.Format("Room cl.");
		break;
	}
	list->InsertColumn(0,Str,LVCFMT_LEFT,65);
	list->SetImageList(&m_imageList,LVSIL_SMALL);
	list->SetExtendedStyle( LVS_EX_CHECKBOXES  );

	
	list->InsertItem(0,Str,0);
	Str.Format("noise");
	list->SetItemText(0,0,Str);
	list->SetCheck(0,clView[255] & numb);
		
	list->InsertItem(1,Str,1);
	Str.Format("non cl");
	list->SetItemText(1,0,Str);
	list->SetCheck(1,clView[0] & numb);
	
	for (int i = 1; i <= maxCluster ; i++)
	{
		list->InsertItem(i+1,Str,i+1);
		Str.Format("cl %d",i);
		list->SetItemText(i+1,0,Str);
		list->SetCheck(i+1,(clView[i] && numb) ? true : false );		
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnItemchangedListArenaCl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	InvalidateRect(m_ArenaRect,false);	
	
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnItemchangedListRoomCl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	InvalidateRect(m_RoomRect,false);
	
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnItemchangedListSpikeCl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	InvalidateRect(m_SpikesRect,false);
	
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnItemchangedListScale(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	InvalidateRect(m_SpikesRect,false);
	
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnSelchangeCmbOrigNew() 
{
	if ( m_MainDataStack->whichDraw != m_wndCmbOrigNew.GetCurSel() )
	{
		m_MainDataStack->whichDraw = m_wndCmbOrigNew.GetCurSel();
		InvalidateRect(m_SpikesRect,false);
		InvalidateRect(m_ArenaRect,false);
		InvalidateRect(m_RoomRect,false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnSelchangeCmbArena() 
{
	InvalidateRect(m_ArenaRect,false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTimeEv::OnSelchangeCmbRoom() 
{
	InvalidateRect(m_RoomRect,false);
}


void CTimeEv::OnTePlay() 
{
	if ( pMode == 0 )
	{
		pMode = 1;
		SetDlgItemText(IDC_TE_PLAY,TEXT("Sto&p"));
		SetTimer(IDT_PLAY,1000.0/m_iPlaySpeed,NULL);
	}
	else 
	{
		pMode = 0;
		SetDlgItemText(IDC_TE_PLAY,TEXT("&Play"));
		KillTimer(IDT_PLAY);
	}
}

void CTimeEv::OnTeClear() 
{
	
}

RGBQUAD QuadFromWord(WORD b16)
{
   BYTE bytVals[] =
   {
     0,  16, 24, 32,  40, 48, 56, 64,
     72, 80, 88, 96, 104,112,120,128,
     136,144,152,160,168,176,184,192,
     200,208,216,224,232,240,248,255
   };

   WORD wR = b16;
   WORD wG = b16;
   WORD wB = b16;

//	wR <<= 1; wR >>= 11;
//	wG <<= 6; wG >>= 11;
//	wB <<= 11; wB >>= 11;

	wR = (wR & 0xF800) >> 11;
	wG = (wG & 0x07E0) >> 6;
	wB = (wB & 0x001F);


   RGBQUAD rgb;

   rgb.rgbReserved = 0;
   rgb.rgbBlue     = bytVals[wB];
   rgb.rgbGreen    = bytVals[wG];
   rgb.rgbRed      = bytVals[wR];

   return rgb;
}


void CreatePCXHeader(CFile *file, short width, short height)
{
	char buff[64];
	buff[0] = 0x0A; buff[1] = 0x05; buff[2] = 0x01; buff[3] = 0x08;
	buff[4] = 0x00; buff[5] = 0x00; buff[6] = 0x00; buff[7] = 0x00;
	file->Write(buff,8);
	width--;
	file->Write(&width,2);
	width++;
	height--;
	file->Write(&height,2);
	height++;
	file->Write(&width,2);
	file->Write(&height,2);
	for (char i=0; i<64; i++)
		buff[i] = 0;
	file->Write(&buff,48);
	buff[0] = 0x00; buff[1] = 0x03; 
	file->Write(&buff,2);

	
	if (width % 2)
		width++;
	width *= 1; //3;
	file->Write(&width,2);
	
	buff[0] = 0x00; buff[1] = 0x00; 
	file->Write(&buff,60);
}

void CTimeEv::OnTeTakePict() 
{
//	CWnd*		pWnd = CWnd::GetDesktopWindow();
	CWindowDC	dc(this);
	CDC			memDC;
	CRect		rect;
	CBitmap		bitmap;
	CClientDC	pDC(this);


//	pWnd->GetWindowRect(rect);
	GetWindowRect(rect);
	memDC.CreateCompatibleDC(&dc);
//	bitmap.CreateCompatibleBitmap(&dc,rect.right,rect.bottom);
	bitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());

	CBitmap *oldBM = memDC.SelectObject(&bitmap);
//	memDC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
	
	BITMAP bm;
	bitmap.GetObject(sizeof(bm),&bm);

//	pDC.BitBlt(0, 0, rect.right, rect.bottom, &memDC, 0, 0, SRCCOPY);
	pDC.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	

	CFile file;
	file.Open("a.pcx",CFile::modeWrite | CFile::modeCreate);
	CreatePCXHeader(&file,rect.Width(),rect.Height());
	
	short *bmBit;
	RGBQUAD *rgbQuad;
	bmBit = (short*) malloc (bm.bmHeight * bm.bmWidthBytes * 2);
	int n = bitmap.GetBitmapBits(bm.bmHeight * bm.bmWidthBytes, bmBit);

	rgbQuad = (RGBQUAD*) malloc (bm.bmHeight * bm.bmWidthBytes * sizeof(RGBQUAD));
	

	for (int y=0; y< rect.Height();y++)
	{
		for (char hl=0; hl<3; hl++)
		{
			for (int x=0; x < rect.Width();x++)
			{
				char byte; RGBQUAD rgb;
				rgb = QuadFromWord( bmBit[y*rect.Width() + x]);
				if (hl == 0)
					byte = rgb.rgbRed;
				if (hl == 1)
					byte = rgb.rgbGreen;
				if (hl == 2)
					byte = rgb.rgbBlue;
				char toWr;
				toWr = (char) 0xC1;
				file.Write(&toWr,1);
				file.Write(&byte,1);
			}
		}
	}		

	free(bmBit);
	free(rgbQuad);
	
	file.Close();

	char txt[300];
	sprintf(txt,"type:%d, width:%d, height:%d, widthbytes:%d, planes:%d, BitsPixel:%d,",
		bm.bmType, bm.bmWidth, bm.bmHeight, bm.bmWidthBytes, bm.bmPlanes, bm.bmBitsPixel);
//	sprintf(txt,"%d %d", rect.left,rect.right);
	pDC.TextOut(10,10,txt);

	memDC.SelectObject(oldBM);
	MessageBeep(0);
}

void CTimeEv::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_SPIN_TIME:
		switch (nSBCode)
		{
		case SB_THUMBPOSITION:
			UpdateData();
			if ((int) nPos > 0)
			{
				m_EditTime *= 2;
				if (m_EditTime*10 > MAX_TIME_WINDOW)
					m_EditTime = float(MAX_TIME_WINDOW) / 10;
			}
			if ((int) nPos < 0)
			{
				m_EditTime /= 2;
				if (m_EditTime*10 < MIN_TIME_WINDOW)
					m_EditTime = float(MIN_TIME_WINDOW) / 10;
			}
			m_wndSpinTime.SetPos(0);
			UpdateData(false);
			timeWindow = 10 * m_EditTime;
			timeWTimes = timeWindow/MIN_TIME_WINDOW;

			int pos = m_wndScrollTime.GetScrollPos();
			int timeWStepT = ( ( timeWindow / MIN_STEP_WINDOW ) );
			m_wndScrollTime.SetScrollRange(0,MAX_STEP_TIMES - timeWStepT );
			if ( pos > MAX_STEP_TIMES - timeWStepT )
			{
				m_wndScrollTime.SetScrollPos( MAX_STEP_TIMES - timeWStepT );
				startStepTimes = MAX_STEP_TIMES - timeWStepT;
				startTime = startStepTimes * MIN_STEP_WINDOW;
				pStartTimeTimes = startStepTimes;
			}

			pAddTimes = timeWindow / (10 * MIN_TIME_WINDOW);
			if ( pAddTimes == 0 )
				pAddTimes++;


			break;
		}		
	
		InvalidateRect(m_SpikesRect,false);
		InvalidateRect(m_ArenaRect,false);
		InvalidateRect(m_RoomRect,false);

		if(m_oAVIR.IsOpen())
			UpdateAVIFrame();

		break;
	}
	
	cdxCSizingDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CTimeEv::OnDeltaposSpinTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CTimeEv::OnEnChangeEditTime()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the cdxCSizingDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CTimeEv::OnBnClickedButtonSetGains()
{
	// TODO: Add your control notification handler code here
	DlgEEGGain oG(0,m_vGainList,m_vEEGNames,m_MainPalette);
	if(oG.DoModal()==IDOK)
	{
		m_vGainList = oG.m_vEEGGains;
		Invalidate();
	}
}

void CTimeEv::OnDeltaposSpineegplayspeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	pNMUpDown->iDelta *= -1;
	m_iPlaySpeed += pNMUpDown->iDelta;
	if(m_iPlaySpeed < 1)
		m_iPlaySpeed = 1;

	CString strTmp;
	strTmp.Format("%d",m_iPlaySpeed);
	m_wndPlaySpeed.SetWindowText(strTmp);
	
	if(pMode==1)	//if already playing
	{	KillTimer( IDT_PLAY ); //restart play with new speed
		SetTimer(IDT_PLAY,1000.0/m_iPlaySpeed,NULL);
	}
}

void CTimeEv::OnBnClickedButtonVidDlgOpen()
{
	// TODO: Add your control notification handler code here
	m_oAVIR.Close(); //close before re-opening

	static char BASED_CODE szFilter[] = "AVI video files (*.avi)|*.avi|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "avi", "*.avi", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if(dlg.DoModal()!=IDOK)
		return;

	CString cstrAVIPath=dlg.GetPathName();
	CString strMsg;

	if(!m_oAVIR.Open(cstrAVIPath))
	{	strMsg.Format("Couldn't open %s",cstrAVIPath);
		MessageBox(strMsg);
	}
	else
	{
		float fMilliSeconds = GetTimeMS();
		m_iFrameOffset = fMilliSeconds / m_oAVIR.MSPerFrame() ;
		
		strMsg.Format("Opened %s w=%d h=%d firstf=%d lastf=%d",
			cstrAVIPath,
			m_oAVIR.Width(),
			m_oAVIR.Height(),
			m_oAVIR.FirstFrame(),
			m_oAVIR.LastFrame());
		Write2Log(strMsg);

		Write2Log("timeWindow=%d, time=%.2f, m_iFrameOffset=%d",timeWindow,fMilliSeconds,m_iFrameOffset);

		InvalidateRect(NULL,FALSE);

		if(!m_hdd)
			m_hdd=DrawDibOpen();
	}
}

double CTimeEv::GetTimeMS()
{
	return (timeStampFirst+10.0*startTime)/10.0 + timeWindow/2.0;
}
void CTimeEv::OnBnClickedButtonSetTime()
{
	// TODO: Add your control notification handler code here
	CString strTime;
	m_wndEditSetTime.GetWindowText(strTime);
	try
	{
		double dMS=atof(strTime);
		startTime = (((10.0*(dMS-timeWindow/2.0))-timeStampFirst)/10.0);
		m_wndScrollTime.SetScrollPos( startTime / MIN_TIME_WINDOW  );
		InvalidateRect(NULL,FALSE);
	}
	catch(...)
	{
	}
}
