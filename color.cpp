#include "stdafx.h"
#include "color.h"

int color::SqDif(color& t)
{
	return Sq(t.r - r) + Sq(t.b - b) + Sq(t.g - g);
}
color color::operator+(const color& c)
{
	int iRSum = r + c.r;
	if(iRSum > 255) iRSum = 255;
	int iGSum = g + c.g;
	if(iGSum > 255) iGSum = 255;
	int iBSum = b + c.b;
	if(iBSum > 255) iBSum = 255;

	return color(iRSum,iGSum,iBSum);
}

color color::operator-(const color& c)
{
	int iRSum = r - c.r;
	if(iRSum < 0) iRSum = 0;
	int iGSum = g - c.g;
	if(iGSum < 0) iGSum = 0;
	int iBSum = b - c.b;
	if(iBSum < 0) iBSum = 0;

	return color(iRSum,iGSum,iBSum);
}

color color::operator*(const color& c)
{
	int iRSum = r * c.r;
	if(iRSum > 255) iRSum = 255;
	int iGSum = g * c.g;
	if(iGSum > 255) iGSum = 255;
	int iBSum = b * c.b;
	if(iBSum > 255) iBSum = 255;

	return color(iRSum,iGSum,iBSum);
}

color::color()
{
	r=g=b=0;
}

color::color(byte r_,byte g_,byte b_)
{
	r=r_;
	g=g_;
	b=b_;
}

bool color::operator==(const color& c)
{
	return b == c.b &&
		   g == c.g &&
		   r == c.r;
}

bool color::operator!=(const color& c)
{
	return b != c.b ||
		   g != c.g ||
		   r != c.r;
}