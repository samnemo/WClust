// ColorComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "wclust.h"
#include "ColorComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox

CColorComboBox::CColorComboBox()
{
	isInitialized = false;
}

CColorComboBox::~CColorComboBox()
{
}


BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	//{{AFX_MSG_MAP(CColorComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox message handlers

void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	static		CString	sColor;	// No Need To Reallocate Each Time

	CDC			dcContext;
	CRect		rItemRect( lpDrawItemStruct -> rcItem );
	CRect		rBlockRect( rItemRect );
	CRect		rTextRect( rBlockRect );
	CBrush		brFrameBrush;
	int			iFourthWidth = 0;
	int			iItem = lpDrawItemStruct -> itemID;
	int			iAction = lpDrawItemStruct -> itemAction;
	int			iState = lpDrawItemStruct -> itemState;
	COLORREF	crColor = 0;
	COLORREF	crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF	crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF	crText = GetSysColor( COLOR_WINDOWTEXT );

	if( !dcContext.Attach( lpDrawItemStruct -> hDC ) )		// Attach CDC Object
		return;												// Stop If Attach Failed

	iFourthWidth = ( rBlockRect.Width() / 4 );				// Get 1/4 Of Item Area
	brFrameBrush.CreateStockObject( BLACK_BRUSH );			// Create Black Brush

	if( iState & ODS_SELECTED )								// If Selected
	{														// Set Selected Attributes
		dcContext.SetTextColor(
				( 0x00FFFFFF & ~( crText ) ) );				// Set Inverted Text Color (With Mask)
		dcContext.SetBkColor( crSelected );					// Set BG To Highlight Color
		dcContext.FillSolidRect( &rBlockRect, crSelected );	// Erase Item
	}
	else													// If Not Selected
	{														// Set Standard Attributes
		dcContext.SetTextColor( crText );					// Set Text Color
		dcContext.SetBkColor( crNormal );					// Set BG Color
		dcContext.FillSolidRect( &rBlockRect, crNormal );	// Erase Item
	}
	if( iState & ODS_FOCUS )								// If Item Has The Focus
		dcContext.DrawFocusRect( &rItemRect );				// Draw Focus Rect

	//
	//	Calculate Text Area
	//
	rTextRect.left += ( iFourthWidth + 2 );					// Set Start Of Text
	rTextRect.top += 2;										// Offset A Bit

	//
	//	Calculate Color Block Area
	//
	rBlockRect.DeflateRect( CSize( 2, 2 ) );				// Reduce Color Block Size
	rBlockRect.right = iFourthWidth;						// Set Width Of Color Block

	//
	//	Draw Color Text And Block
	//
	if( iItem != -1 )										// If Not An Empty Item
	{
		GetLBText( iItem, sColor );							// Get Color Text
		if( iState & ODS_DISABLED )							// If Disabled
		{
			crColor = GetSysColor( COLOR_INACTIVECAPTIONTEXT );
			dcContext.SetTextColor( crColor );				// Set Text Color
		}
		else												// If Normal
// for NOISE
//			crColor = (isInitialized) ? m_palStack->GetSColor(iItem) : RGB(255,255,255);// Get Color Value
			if (isInitialized)
				crColor = (iItem != 0) ? m_palStack->GetSColor(iItem-1) : RGB(204,204,204);// Get Color Value

		dcContext.SetBkMode( TRANSPARENT );					// Transparent Background
		dcContext.TextOut( rTextRect.left, rTextRect.top,
				sColor );									// Draw Color Name

		dcContext.FillSolidRect( &rBlockRect, crColor );	// Draw Color
				
		dcContext.FrameRect( &rBlockRect, &brFrameBrush );	// Draw Frame
	}
	dcContext.Detach();		
}
