// MyObj.h: interface for the CMyObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYOBJ_H__5A3F74F7_6453_4F3B_88DE_599AF834722F__INCLUDED_)
#define AFX_MYOBJ_H__5A3F74F7_6453_4F3B_88DE_599AF834722F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <deque>
#include <set>
#include <memory>
#include <float.h>

using namespace std;

typedef deque <unsigned long> MY_PAL_STACK;
////////////////////////////////////////////////////////////////////////
// CPaletteStack
class CPaletteStack : public CObject
{
protected: 
	 MY_PAL_STACK m_PalStack;
	 set<unsigned long> m_setColors;
	 int NumColors;
public:
	CPaletteStack();
	virtual ~CPaletteStack(){ SetEmpty(); };
	void AddSColor(unsigned long newColor);
	unsigned long GetSColor(int ColorPos);
	void SetSColor(int ColorPos,unsigned long newValue);
	void SetEmpty();

};

////////////////////////////////////////////////////////////////////////
// CFloatRect
class CFloatRect : public CObject
{
public:
	float left,top,right,bottom;
public:
	CFloatRect(float x1,float y1,float x2, float y2){left=x1;top=y1;right=x2;bottom=y2;};
	CFloatRect(){};
	virtual ~CFloatRect(){};
	void GetValue(float x1,float y1,float x2, float y2){x1=left;y1=top;x2=right;y2=bottom;};
	void SetValue(float x1,float y1,float x2, float y2){left=x1;top=y1;right=x2;bottom=y2;};
	float GetLeft(){return left;};
	float GetRight(){return right;};
	float GetTop(){return top;};
	float GetBottom(){return bottom;};
	void SetLeUp(float x1,float y1){left=x1;top=y1;};
	void SetRiBo(float x2, float y2){right=x2;bottom=y2;};
	int IsInRect(float x1,float y1){if ((x1>=left)&&(x1<=right)&&(y1<=top)&&(y1>=bottom))return 1;else return 0;};
	float GetSizeX(){return right-left;};
	float GetSizeY(){return top-bottom;};
};


////////////////////////////////////////////////////////////////////////
// CMyObject
class CMyObject : public CObject  
{
protected:
	CPaletteStack *m_PalStack;
public:
	CMyObject(CPaletteStack *MyPalette){m_PalStack=MyPalette;};
	CMyObject();
	void SetMyPalette(CPaletteStack *MyPalette){m_PalStack=MyPalette;};
	virtual ~CMyObject();
	virtual void GetValue(){};
	virtual void GetValue(float X, float Y){}; 
	virtual void Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes){};
	virtual void Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust){};
	virtual void Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,int Color){};
	virtual void Draw(CDC *pDC,CRect DrawWin,CFloatRect *whichValues,CPoint whichAxes,unsigned char *Clust,int PntSize){};
};

////////////////////////////////////////////////////////////////////////
// Containers
typedef vector <float> VERTEX;
typedef vector <int> MY_INT_VECT;
typedef deque <CMyObject*> MY_STACK;
typedef deque <CString*> MY_STR_STACK;
typedef deque <int> MY_INT_STACK;

////////////////////////////////////////////////////////////////////////
// CMiniView
class CMiniView : public CMyObject
{
public:

	vector<unsigned char> m_ClustToView;  // bit: 0 - points, 1 - vectors
public:
	int m_AxesX,m_AxesY;
	int m_Focus;
	int m_PointsSize;
	CRect m_WindowArea, m_DrawingArea;
	CFloatRect m_DataArea, m_DefaultDataArea;
public:	
	CMiniView();
	virtual ~CMiniView(){ SetEmpty(); };
	void DoSize();
	void SetEmpty();
};


//probability type
typedef float prob_t;

//#if sizeof(prob_t) = sizeof(float)
  const float MAX_PROB_T = FLT_MAX;
  const float MIN_PROB_T = FLT_MIN;
//#else
//  const double MAX_PROB_T = DBL_MAX;
//  const double MIN_PROB_T = DBL_MIN;
//#endif

const prob_t one = 1.0, two = 2.0;

#endif // !defined(AFX_MYOBJ_H__5A3F74F7_6453_4F3B_88DE_599AF834722F__INCLUDED_)

