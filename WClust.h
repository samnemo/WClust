// WClust.h : main header file for the WCLUST application
//

#if !defined(AFX_WCLUST_H__B30B7250_4A0C_4357_A075_81DCD3C55F12__INCLUDED_)
#define AFX_WCLUST_H__B30B7250_4A0C_4357_A075_81DCD3C55F12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWClustApp:
// See WClust.cpp for the implementation of this class
//

class CWClustApp : public CWinApp
{
public:
	CWClustApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWClustApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWClustApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClusterSetbins32800();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCLUST_H__B30B7250_4A0C_4357_A075_81DCD3C55F12__INCLUDED_)
