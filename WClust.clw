; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSelectTT
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "wclust.h"
LastPage=0

ClassCount=21
Class1=cdxCSizingDialog
Class2=cdxCSizingPropSheet
Class3=cdxCSizingPropPage
Class4=CEditDlg
Class5=CMainFrame
Class6=CWClustApp
Class7=CAboutDlg
Class8=CWClustDoc
Class9=CWClustView

ResourceCount=26
Resource1=IDD_DIALLIST
Resource2=IDD_DIALOGBAR (English (U.S.))
Resource3=IDD_SELECT_TETRODE
Resource4=IDD_EDIT_SPIKES
Class10=CSetDial
Resource5=IDD_CHOOSE_WCP_DLG
Class11=CEditSpikes
Resource6=IDD_RANGEDIALOG (English (U.S.))
Resource7=IDD_WCLUST_FORM
Class12=CSelectTT
Resource8=IDD_POSITION_REG_DLG
Resource9=IDD_RANGEDIALOG
Resource10=IDR_MAINFRAME
Resource11=IDD_SELECT_TETRODE (English (U.S.))
Resource12=IDD_EDITDIALOG
Class13=CDialList
Resource13=IDD_ABOUTBOX (English (U.S.))
Resource14=IDD_TIME_EV (English (U.S.))
Resource15=IDD_CHOOSE_VERX_DLG (English (U.S.))
Resource16=IDD_POSITION_REG_DLG (English (U.S.))
Class14=CColorComboBox
Resource17=IDD_ABOUTBOX
Class15=CTimeEv
Class16=CClusteringInfoDlg
Resource18=IDD_CHOOSE_VERX_DLG
Class17=CDlgPositionReg
Resource19=IDD_TIME_EV
Class18=CChooseClDlg
Resource20=IDD_WCLUST_FORM (English (U.S.))
Resource21=IDD_DLG_WCP_NOT_FOUND
Resource22=IDD_DLG_INFO
Resource23=IDD_EDITDIALOG (English (U.S.))
Class19=CDlgAddWCP
Resource24=IDD_EDIT_SPIKES (English (U.S.))
Class20=CDlgWcpNotFound
Resource25=IDD_DIALLIST (English (U.S.))
Class21=CDlgFileInfo
Resource26=IDR_MAINFRAME (English (U.S.))

[CLS:cdxCSizingDialog]
Type=0
BaseClass=CDialog
HeaderFile=cdxCSizingDialog.h
ImplementationFile=cdxCSizingDialog.cpp

[CLS:cdxCSizingPropSheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=cdxCSizingDialog.h
ImplementationFile=cdxCSizingDialog.cpp

[CLS:cdxCSizingPropPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=cdxCSizingDialog.h
ImplementationFile=cdxCSizingDialog.cpp

[CLS:CEditDlg]
Type=0
BaseClass=cdxCSizingDialog
HeaderFile=EditDlg.h
ImplementationFile=EditDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_EXPAND

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame
Filter=T
VirtualFilter=fWC

[CLS:CWClustApp]
Type=0
BaseClass=CWinApp
HeaderFile=WClust.h
ImplementationFile=WClust.cpp
LastObject=CWClustApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=WClust.cpp
ImplementationFile=WClust.cpp
LastObject=CAboutDlg

[CLS:CWClustDoc]
Type=0
BaseClass=CDocument
HeaderFile=WClustDoc.h
ImplementationFile=WClustDoc.cpp
LastObject=CWClustDoc

[CLS:CWClustView]
Type=0
BaseClass=CFormView
HeaderFile=WClustView.h
ImplementationFile=WClustView.cpp
Filter=D
VirtualFilter=VWC
LastObject=ID_TOOLS_INFO

[DLG:IDD_EDITDIALOG]
Type=1
Class=CEditDlg
ControlCount=24
Control1=IDC_COMBOCLUST,combobox,1344340499
Control2=IDC_HIDEPOLY,button,1342242816
Control3=IDC_NEWPOLY,button,1342242816
Control4=IDC_DRAWPOLY,button,1342242816
Control5=IDC_COMBOPEN,combobox,1344339971
Control6=IDC_REMOVEPOLY,button,1342242816
Control7=IDC_REMALLPOLY,button,1342242816
Control8=IDC_COPYCLUSTER,button,1342242816
Control9=IDC_ZOOMIN,button,1342242816
Control10=IDC_FIT_VIEW,button,1342242816
Control11=IDC_SPIN_Y,msctls_updown32,1342177312
Control12=IDC_SETTINGS,button,1342242816
Control13=IDC_DRAW_SPIKES,button,1342242816
Control14=IDC_SPIN_SPIKES,msctls_updown32,1342177312
Control15=IDOK,button,1342242817
Control16=IDC_SPIN_X,msctls_updown32,1342177376
Control17=IDC_SET_SPIKES,button,1342242816
Control18=IDC_CHECKFIT,button,1342242819
Control19=IDC_INITVIEW,button,1342242816
Control20=IDC_LIST,button,1342242816
Control21=IDC_BUT_UNDO,button,1342242816
Control22=IDC_TIME_EV,button,1342242816
Control23=IDC_EXPAND,button,1342242816
Control24=IDC_CMB_ORIG_NEW,combobox,1344339971

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_WCLUST_FORM]
Type=1
Class=CWClustView
ControlCount=11
Control1=IDC_BUT_SET,button,1342242816
Control2=IDC_SHOW,button,1342242816
Control3=IDC_HIDE,button,1342242816
Control4=IDC_BUTTON1,button,1342242816
Control5=IDC_BUTTON4,button,1342242816
Control6=IDC_BUTTON6,button,1342242816
Control7=IDC_BUTTON3,button,1342242816
Control8=IDC_BUTTON5,button,1342242816
Control9=IDC_BUTTON7,button,1342242816
Control10=IDC_COMBOCLUST,combobox,1344340499
Control11=IDC_CMB_ORIG_NEW1,combobox,1344339971

[DLG:IDD_WCLUST_FORM (English (U.S.))]
Type=1
Class=CWClustView
ControlCount=11
Control1=IDC_BUT_SET,button,1342242816
Control2=IDC_SHOW,button,1342242816
Control3=IDC_HIDE,button,1342242816
Control4=IDC_BUTTON1,button,1342242816
Control5=IDC_BUTTON4,button,1342242816
Control6=IDC_BUTTON6,button,1342242816
Control7=IDC_BUTTON3,button,1342242816
Control8=IDC_BUTTON5,button,1342242816
Control9=IDC_BUTTON7,button,1342242816
Control10=IDC_COMBOCLUST,combobox,1344340499
Control11=IDC_CMB_ORIG_NEW1,combobox,1344339971

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_OPENBPFILE
Command2=ID_FILE_LOADCLUSTERS
CommandCount=2

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_OPENBPFILE
Command2=ID_FILE_STORETOBPFILE
Command3=ID_LOAD_ARENA
Command4=ID_LOAD_ROOM
Command5=ID_APP_EXIT
Command6=IDC_NEWCLUST
Command7=ID_FILE_LOADCLUSTERS
Command8=ID_FILE_SAVECLUSTERS
Command9=ID_ENVIRONMENT_LOAD
Command10=ID_ENVIRONMENT_SAVE
Command11=ID_MODE_SWAPPOLARITY
Command12=ID_POS_REG_WIN
Command13=ID_TIME_EV_WIN
Command14=ID_TOOLS_INFO
Command15=ID_TOOLS_EXPORT
Command16=ID_APP_ABOUT
CommandCount=16

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_LOADCLUSTERS
Command2=ID_FILE_OPENBPFILE
Command3=ID_FILE_SAVECLUSTERS
CommandCount=3

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[CLS:CSetDial]
Type=0
HeaderFile=SetDial.h
ImplementationFile=SetDial.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK
VirtualFilter=dWC

[DLG:IDD_RANGEDIALOG]
Type=1
Class=CSetDial
ControlCount=13
Control1=IDC_EDIT_PERCENT,edit,1350631552
Control2=IDC_EDIT_FIRST_EV,edit,1350631552
Control3=IDC_EDIT_LAST_EV,edit,1350631552
Control4=IDC_RD_RANDOM,button,1342242825
Control5=IDC_RD_EXCLUDE,button,1342177289
Control6=IDC_RD_WEIGHTED,button,1342177289
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,button,1342177287

[CLS:CEditSpikes]
Type=0
HeaderFile=EditSpikes.h
ImplementationFile=EditSpikes.cpp
BaseClass=cdxCSizingDialog
Filter=D
LastObject=IDC_COMBOPROP
VirtualFilter=dWC

[DLG:IDD_EDIT_SPIKES]
Type=1
Class=CEditSpikes
ControlCount=15
Control1=IDC_PLAY,button,1342242816
Control2=IDC_NEXT,button,1342242816
Control3=IDC_STOP,button,1342242816
Control4=IDC_BCLEAR,button,1342242816
Control5=IDC_CHECK_CH1,button,1342242819
Control6=IDC_CHECK_CH2,button,1342242819
Control7=IDC_CHECK_CH3,button,1342242819
Control8=IDC_CHECK_CH4,button,1342242819
Control9=IDC_COMBOPROP,combobox,1344339971
Control10=IDC_CMB_SP_CLUST,combobox,1344340499
Control11=IDOK,button,1342242817
Control12=IDC_REMOVE,button,1342242816
Control13=IDC_SET_SP,button,1342242816
Control14=IDC_CMB_SEL_PARAM,combobox,1344339971
Control15=IDC_ADD,button,1342242816

[DLG:IDD_DIALOGBAR (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_SELECT_TETRODE]
Type=1
Class=CSelectTT
ControlCount=12
Control1=IDC_LIST_OF_ELECTRODES,listbox,1352728833
Control2=IDC_ED_AMOUNT,edit,1350639618
Control3=IDC_PERBUT1,button,1342242816
Control4=IDC_PERBUT2,button,1342242816
Control5=IDC_PERBUT3,button,1342242816
Control6=IDC_PERBUT4,button,1342242816
Control7=IDC_PERBUT5,button,1342242816
Control8=IDC_PERBUT6,button,1342242816
Control9=IDOK,button,1342242816
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352

[CLS:CSelectTT]
Type=0
HeaderFile=SelectTT.h
ImplementationFile=SelectTT.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_LIST_OF_ELECTRODES
VirtualFilter=dWC

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPENBPFILE
Command2=ID_FILE_LOADCLUSTERS
CommandCount=2

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPENBPFILE
Command2=ID_FILE_STORETOBPFILE
Command3=ID_LOAD_ARENA
Command4=ID_LOAD_ROOM
Command5=ID_APP_EXIT
Command6=IDC_NEWCLUST
Command7=ID_FILE_LOADCLUSTERS
Command8=ID_FILE_SAVECLUSTERS
Command9=ID_ENVIRONMENT_LOAD
Command10=ID_ENVIRONMENT_SAVE
Command11=ID_MODE_SWAPPOLARITY
Command12=ID_POS_REG_WIN
Command13=ID_TIME_EV_WIN
Command14=ID_APP_ABOUT
CommandCount=14

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_LOADCLUSTERS
Command2=ID_FILE_OPENBPFILE
Command3=ID_FILE_SAVECLUSTERS
CommandCount=3

[DLG:IDD_DIALLIST]
Type=1
Class=CDialList
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_LIST1,listbox,1352728833
Control3=IDCANCEL,button,1342242816

[CLS:CDialList]
Type=0
HeaderFile=DialList.h
ImplementationFile=DialList.cpp
BaseClass=cdxCSizingDialog
Filter=D
LastObject=IDCANCEL
VirtualFilter=dWC

[DLG:IDD_DIALLIST (English (U.S.))]
Type=1
Class=CDialList
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_LIST1,listbox,1352728833
Control3=IDCANCEL,button,1342242816

[DLG:IDD_SELECT_TETRODE (English (U.S.))]
Type=1
Class=CSelectTT
ControlCount=13
Control1=IDC_LIST_OF_ELECTRODES,listbox,1352728833
Control2=IDC_ED_AMOUNT,edit,1350639618
Control3=IDC_PERBUT1,button,1342242816
Control4=IDC_PERBUT2,button,1342242816
Control5=IDC_PERBUT3,button,1342242816
Control6=IDC_PERBUT4,button,1342242816
Control7=IDC_PERBUT5,button,1342242816
Control8=IDC_PERBUT6,button,1342242816
Control9=IDOK,button,1342242816
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_LIST_MESSAGE,listbox,1350648065

[DLG:IDD_EDITDIALOG (English (U.S.))]
Type=1
Class=CEditDlg
ControlCount=24
Control1=IDC_COMBOCLUST,combobox,1344340499
Control2=IDC_HIDEPOLY,button,1342242816
Control3=IDC_NEWPOLY,button,1342242816
Control4=IDC_DRAWPOLY,button,1342242816
Control5=IDC_COMBOPEN,combobox,1344339971
Control6=IDC_REMOVEPOLY,button,1342242816
Control7=IDC_REMALLPOLY,button,1342242816
Control8=IDC_COPYCLUSTER,button,1342242816
Control9=IDC_ZOOMIN,button,1342242816
Control10=IDC_FIT_VIEW,button,1342242816
Control11=IDC_SPIN_Y,msctls_updown32,1342177312
Control12=IDC_SETTINGS,button,1342242816
Control13=IDC_DRAW_SPIKES,button,1342242816
Control14=IDC_SPIN_SPIKES,msctls_updown32,1342177312
Control15=IDOK,button,1342242817
Control16=IDC_SPIN_X,msctls_updown32,1342177376
Control17=IDC_SET_SPIKES,button,1342242816
Control18=IDC_CHECKFIT,button,1342242819
Control19=IDC_INITVIEW,button,1342242816
Control20=IDC_LIST,button,1342242816
Control21=IDC_BUT_UNDO,button,1342242816
Control22=IDC_TIME_EV,button,1342242816
Control23=IDC_EXPAND,button,1342242816
Control24=IDC_CMB_ORIG_NEW,combobox,1344339971

[DLG:IDD_RANGEDIALOG (English (U.S.))]
Type=1
Class=CSetDial
ControlCount=13
Control1=IDC_EDIT_PERCENT,edit,1350631552
Control2=IDC_EDIT_FIRST_EV,edit,1350631552
Control3=IDC_EDIT_LAST_EV,edit,1350631552
Control4=IDC_RD_RANDOM,button,1342242825
Control5=IDC_RD_EXCLUDE,button,1342177289
Control6=IDC_RD_WEIGHTED,button,1342177289
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,button,1342177287

[DLG:IDD_EDIT_SPIKES (English (U.S.))]
Type=1
Class=CEditSpikes
ControlCount=15
Control1=IDC_PLAY,button,1342242816
Control2=IDC_NEXT,button,1342242816
Control3=IDC_STOP,button,1342242816
Control4=IDC_BCLEAR,button,1342242816
Control5=IDC_CHECK_CH1,button,1342242819
Control6=IDC_CHECK_CH2,button,1342242819
Control7=IDC_CHECK_CH3,button,1342242819
Control8=IDC_CHECK_CH4,button,1342242819
Control9=IDC_COMBOPROP,combobox,1344339971
Control10=IDOK,button,1342242817
Control11=IDC_REMOVE,button,1342242816
Control12=IDC_SET_SP,button,1342242816
Control13=IDC_CMB_SEL_PARAM,combobox,1344339971
Control14=IDC_ADD,button,1342242816
Control15=IDC_LIST_CLUST,SysListView32,1350631429

[CLS:CColorComboBox]
Type=0
HeaderFile=ColorComboBox.h
ImplementationFile=ColorComboBox.cpp
BaseClass=CComboBox
Filter=W
VirtualFilter=cWC
LastObject=CColorComboBox

[CLS:CTimeEv]
Type=0
HeaderFile=TimeEv.h
ImplementationFile=TimeEv.cpp
BaseClass=cdxCSizingDialog
Filter=D
LastObject=IDC_LIST_ARENA_CL
VirtualFilter=dWC

[DLG:IDD_TIME_EV]
Type=1
Class=CTimeEv
ControlCount=18
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SCROLL_TIME,scrollbar,1342242816
Control4=IDC_EDIT_TIME,edit,1350639746
Control5=IDC_SPIN_TIME,msctls_updown32,1342177312
Control6=IDC_CHECK_RASTER,button,1342242819
Control7=IDC_CHECK_SPIKE,button,1342242819
Control8=IDC_LIST_SCALE,SysListView32,1350631429
Control9=IDC_LIST_ARENA_CL,SysListView32,1350631429
Control10=IDC_LIST_ROOM_CL,SysListView32,1350631429
Control11=IDC_LIST_SPIKE_CL,SysListView32,1350631429
Control12=IDC_CMB_ORIG_NEW,combobox,1344339971
Control13=IDC_CMB_ROOM,combobox,1344339971
Control14=IDC_CMB_ARENA,combobox,1344339970
Control15=IDC_TE_PLAY,button,1342242816
Control16=IDC_TE_CLEAR,button,1342242816
Control17=IDC_MAX,button,1342242816
Control18=IDC_TE_TAKE_PICT,button,1342242816

[DLG:IDD_POSITION_REG_DLG]
Type=1
Class=CDlgPositionReg
ControlCount=14
Control1=IDC_RADIO_SYNC,button,1342373897
Control2=IDC_RADIO_CONST,button,1342177289
Control3=IDC_RADIO_OTHER,button,1342177289
Control4=IDC_RADIO_WITHOUT,button,1342177289
Control5=IDC_16_7,button,1342373897
Control6=IDC_20,button,1342177289
Control7=IDC_33_3,button,1342177289
Control8=IDC_40,button,1342177289
Control9=IDC_100,button,1342177289
Control10=IDC_EDIT,edit,1350631552
Control11=IDOK,button,1342242817
Control12=IDCANCEL,button,1342242816
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308352

[CLS:CDlgPositionReg]
Type=0
HeaderFile=DlgPositionReg.h
ImplementationFile=DlgPositionReg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_EDIT
VirtualFilter=dWC

[DLG:IDD_CHOOSE_VERX_DLG]
Type=1
Class=CChooseClDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_LIST_PNTS,SysListView32,1350631425
Control3=IDC_LIST_VERX,SysListView32,1350631425

[CLS:CChooseClDlg]
Type=0
HeaderFile=ChooseClDlg.h
ImplementationFile=ChooseClDlg.cpp
BaseClass=cdxCSizingDialog
Filter=D
LastObject=CChooseClDlg
VirtualFilter=dWC

[DLG:IDD_TIME_EV (English (U.S.))]
Type=1
Class=CTimeEv
ControlCount=18
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SCROLL_TIME,scrollbar,1342242816
Control4=IDC_EDIT_TIME,edit,1350631554
Control5=IDC_SPIN_TIME,msctls_updown32,1342177312
Control6=IDC_CHECK_RASTER,button,1342242819
Control7=IDC_CHECK_SPIKE,button,1342242819
Control8=IDC_LIST_SCALE,SysListView32,1350631429
Control9=IDC_LIST_ARENA_CL,SysListView32,1350631429
Control10=IDC_LIST_ROOM_CL,SysListView32,1350631429
Control11=IDC_LIST_SPIKE_CL,SysListView32,1350631429
Control12=IDC_CMB_ORIG_NEW,combobox,1344339971
Control13=IDC_CMB_ROOM,combobox,1344339971
Control14=IDC_CMB_ARENA,combobox,1344339970
Control15=IDC_TE_PLAY,button,1342242816
Control16=IDC_TE_CLEAR,button,1342242816
Control17=IDC_MAX,button,1342242816
Control18=IDC_TE_TAKE_PICT,button,1342242816

[DLG:IDD_POSITION_REG_DLG (English (U.S.))]
Type=1
Class=CDlgPositionReg
ControlCount=14
Control1=IDC_RADIO_SYNC,button,1342373897
Control2=IDC_RADIO_CONST,button,1342177289
Control3=IDC_RADIO_OTHER,button,1342177289
Control4=IDC_RADIO_WITHOUT,button,1342177289
Control5=IDC_16_7,button,1342373897
Control6=IDC_20,button,1342177289
Control7=IDC_33_3,button,1342177289
Control8=IDC_40,button,1342177289
Control9=IDC_100,button,1342177289
Control10=IDC_EDIT,edit,1350631552
Control11=IDOK,button,1342242817
Control12=IDCANCEL,button,1342242816
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308352

[DLG:IDD_CHOOSE_VERX_DLG (English (U.S.))]
Type=1
Class=CChooseClDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_LIST_PNTS,SysListView32,1350631425
Control3=IDC_LIST_VERX,SysListView32,1350631425

[DLG:IDD_CHOOSE_WCP_DLG]
Type=1
Class=CDlgAddWCP
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_REFRESH,button,1342242816
Control4=IDC_LIST_POSSIBLE,listbox,1352728833
Control5=IDC_LIST_CHOOSEN,listbox,1352728833
Control6=IDC_ADD_PARAM,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[CLS:CDlgAddWCP]
Type=0
HeaderFile=DlgAddWCP.h
ImplementationFile=DlgAddWCP.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgAddWCP
VirtualFilter=dWC

[CLS:CDlgWcpNotFound]
Type=0
HeaderFile=DlgWcpNotFound.h
ImplementationFile=DlgWcpNotFound.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgWcpNotFound
VirtualFilter=dWC

[DLG:IDD_DLG_WCP_NOT_FOUND]
Type=1
Class=CDlgWcpNotFound
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_REFRESH,button,1342242816
Control10=IDC_LIST,listbox,1352745217

[DLG:IDD_DLG_INFO]
Type=1
Class=CDlgFileInfo
ControlCount=13
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_FILE_NAME,edit,1350633600
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_SYNC,edit,1350633600
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_LIST_FOUND,listbox,1352745219
Control9=IDC_LIST_USED,listbox,1352745219
Control10=IDC_EDIT_ARENA,edit,1350633600
Control11=IDC_EDIT_ROOM,edit,1350633600
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352

[CLS:CDlgFileInfo]
Type=0
HeaderFile=DlgFileInfo.h
ImplementationFile=DlgFileInfo.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgFileInfo
VirtualFilter=dWC

