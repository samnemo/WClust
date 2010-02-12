// WClustDoc.cpp : implementation of the CWClustDoc class
//

#include "stdafx.h"
#include "WClust.h"

#include "WClustDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CWClustDoc

IMPLEMENT_DYNCREATE(CWClustDoc, CDocument)

BEGIN_MESSAGE_MAP(CWClustDoc, CDocument)
	//{{AFX_MSG_MAP(CWClustDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWClustDoc construction/destruction

CWClustDoc::CWClustDoc()
{
	m_MainDataStack.SetPalette(&m_MainPalette);
	m_MainClusters.SetMyPalette(&m_MainPalette);
}

CWClustDoc::~CWClustDoc()
{
	m_MainDataStack.SetEmpty();
}

BOOL CWClustDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWClustDoc serialization

void CWClustDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWClustDoc diagnostics

#ifdef _DEBUG
void CWClustDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWClustDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWClustDoc commands

