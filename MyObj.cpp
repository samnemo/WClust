// MyObj.cpp: implementation of the CMyObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wclust.h"
#include "MyObj.h"
#include <set>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyObject::CMyObject()
{

}

CMyObject::~CMyObject()
{

}



//////////////////////////////////////////////////////////////////////
// CPaletteStack
CPaletteStack::CPaletteStack()
{
	NumColors=0;

	AddSColor(RGB(0x90,0x90,0x90));//0
	AddSColor(RGB(0xFF,0x00,0x00));
	AddSColor(RGB(0x00,0x00,0xFF));//2
	AddSColor(RGB(0x00,0xFF,0x00));

	AddSColor(RGB(0xF0,0x00,0xF0));//4
	AddSColor(RGB(0x90,0x00,0xF0));
	AddSColor(RGB(0x00,0xF0,0xF0));//6

	AddSColor(RGB(0x00,0x00,0x00));
	AddSColor(RGB(0xF0,0x90,0x00));//8
	AddSColor(RGB(0xF0,0x00,0x90));

	AddSColor(RGB(0x90,0x00,0x90));//10
	AddSColor(RGB(0x90,0x00,0xF0));
	AddSColor(RGB(0x00,0xF0,0x90));//12
	AddSColor(RGB(0x90,0xF0,0x00));
	AddSColor(RGB(0x00,0x90,0x90));//14
	AddSColor(RGB(0x90,0x90,0x00));
	
	AddSColor(RGB(0x90,0x90,0xF0));//16
	AddSColor(RGB(0x90,0xF0,0x90));
	AddSColor(RGB(0xF0,0x90,0x90));//18

	//add some random colors 
	int i = 0;
	for(i=0;i<250;i++)
	{
		unsigned long tmpCol = RGB(rand()%255,rand()%255,rand()%255);
		while(m_setColors.find(tmpCol)!=m_setColors.end())
		{
			char R = rand()%255, G = rand()%255, B = rand()%255;
			while( (R == 255 && G == 255 && B == 255) ||
				   (R == 204 && G == 204 && B == 204) ||
				   (R == 0   && G == 0   && B == 0 ) )
			{
				R = rand()%255, G = rand()%255, B = rand()%255;
			}
			tmpCol = RGB(R,G,B);
		}
		AddSColor(tmpCol);
	}
}
void CPaletteStack::AddSColor(unsigned long newColor)
{
	m_PalStack.push_back(newColor);
	NumColors+=1;
	m_setColors.insert(newColor);
}

unsigned long CPaletteStack::GetSColor(int ColorPos)
{
	MY_PAL_STACK::iterator m_Index;

	if (ColorPos+1>NumColors || ColorPos<0)
		return 0;
	else 
		return *(m_PalStack.begin()+ColorPos);
}

void CPaletteStack::SetSColor(int ColorPos,unsigned long newValue)
{
	//TODO
}

void CPaletteStack::SetEmpty()
{
	MY_PAL_STACK::iterator Index;
	
	m_PalStack.clear();
	m_setColors.clear();
}


//////////////////////////////////////////////////////////////////////
// CMiniView
CMiniView::CMiniView()
{
	m_DataArea.SetValue(-10.0,10.0,10.0,-10.0);
	m_DefaultDataArea.SetValue(-10.0,10.0,10.0,-10.0);

	m_AxesX=1;
	m_AxesY=2;
	m_Focus=0;
	m_PointsSize=3;
	
	m_ClustToView = vector<unsigned char>(256);
	std::fill(m_ClustToView.begin(),m_ClustToView.end(),3);	
}

void CMiniView::DoSize()
{
	float x,y;
	x = m_DataArea.GetSizeX()/12;
	m_DataArea.left += x;
	m_DataArea.right -= x;

	y = m_DataArea.GetSizeY()/12;
	m_DataArea.bottom += y;
	m_DataArea.top -= y;
}

void CMiniView::SetEmpty()
{
	m_ClustToView.clear();
}
