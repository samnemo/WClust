// WClustDoc.h : interface of the CWClustDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WCLUSTDOC_H__CF3A439C_4C3C_4D9B_8AA7_1B8543AD8222__INCLUDED_)
#define AFX_WCLUSTDOC_H__CF3A439C_4C3C_4D9B_8AA7_1B8543AD8222__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////
// My includes
#include <vector>
#include <deque>
#include "MyObj.h"
#include "Vertex.h"
#include "Cluster.h"


using namespace std;


////////////////////////////////////////////////////////////////////////
// CWClustDoc
class CWClustDoc : public CDocument
{
public:
	CVerxStack m_MainDataStack;
	CCluster m_MainClusters;
	CPaletteStack m_MainPalette;
public:
	

protected: // create from serialization only
	CWClustDoc();
	DECLARE_DYNCREATE(CWClustDoc)
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWClustDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWClustDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWClustDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCLUSTDOC_H__CF3A439C_4C3C_4D9B_8AA7_1B8543AD8222__INCLUDED_)
