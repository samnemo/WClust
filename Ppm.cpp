// $Id: Ppm.cpp,v 1.2 2008/03/26 03:58:30 samn Exp $ 
#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "PPM.h"

void ppmImage::init()
{
	buf = 0;
	m_pRowPointers = 0;
	w = 0;
	h = 0;
}

ppmImage::ppmImage(const ppmImage& src)
{
	init();
	copyImage(src);
}

ppmImage::ppmImage(char *fname)
{
    init();
	if(fname==0)return;
    loadFile(fname);
}

ppmImage::ppmImage(int _w,int _h)
{
	init();
    create(_w,_h);
}

ppmImage& ppmImage::operator=(const ppmImage& src)
{
	destroy();
	copyImage(src);
	return *this;
}

void ppmImage::allocSpace()
{
	if ( buf ) {
		delete[] buf;
		buf = NULL;
	}
    buf = new color[ w * h ];

	if (!buf) {
		throw ("ppmImage::allocSpace: failed to allocate memory for buf!");
	}

	if ( m_pRowPointers ) {
		delete [] m_pRowPointers;
		m_pRowPointers = NULL;
	}

	m_pRowPointers = new color*[h];
	if (!m_pRowPointers) {
		delete[] buf;
		buf = NULL;
		throw ("ppmImage::allocSpace: failed to allocate memory for m_pRowPointers!");
	}

	color* pTmp = buf;
	for(int i=0;i<h;i++,pTmp+=w)
	{
		m_pRowPointers[i] = pTmp;
	}
}

ppmImage::~ppmImage()
{
	destroy();
}

void ppmImage::create(int _w,int _h)
{	
	//first clean up any allocated memory
	destroy();

    buf = 0;
	m_pRowPointers = 0;
    w = _w; 
    h = _h;
    allocSpace();
    header[0]='P';
    header[1]='6';
    header[2]='\n';
    header[3]='\0';
}

void ppmImage::destroy()
{
	if(buf)
	{
		delete[] buf;
		buf = NULL;
	}
	if(m_pRowPointers)
	{
		delete [] m_pRowPointers;
		m_pRowPointers = NULL;
	}
}

void ppmImage::copyImage(const ppmImage& src)
{
	w = src.w;
	h = src.h;
	allocSpace();
	memcpy(buf,src.buf,sizeof(color)*w*h);
	strcpy(header,src.header);
}

void ppmImage::reverseRedBlue()
{
	int sz = w*h;
	color* p = buf;
	for(int i=0;i<sz;i++,p++)
	{
		byte x = p->b;
		p->b = p->r;
		p->r = x;
	}
}

void ppmImage::setWhite()
{
    memset(buf, 255, w*h*sizeof(color));
}

void ppmImage::setBlack()
{
    memset(buf, 0, w*h*sizeof(color));
}

void ppmImage::loadFile(char * fname)
{
    FILE *fp = fopen(fname, "rb");
    char tstr[200];
    
    
	if (fp == NULL)
          return;
      
	fgets(header,sizeof(header),fp); // make sure signiture is P6
	do {
	  fgets(tstr,sizeof(tstr),fp);
	} while (tstr[0] == '#');
	
	sscanf(tstr, "%d %d", &w, &h);
	fscanf(fp, "%*d ");
	allocSpace();
	fread(buf, w*h*sizeof(color), 1, fp);
	
	//reverseRedBlue();

	
	fclose(fp);
}

void ppmImage::saveFile(char *fname)
{
	FILE *fp = fopen(fname, "wb");
	
	fprintf(fp, "%s", header);
	fprintf(fp, "%d %d\n255\n", w, h);
	 
	int sz = w*h;
	color* pData = buf;
	for(int i=0;i<sz;i++,pData++)
	{
		fputc(pData->r,fp);
		fputc(pData->g,fp);
		fputc(pData->b,fp);
	}

	fclose(fp);
	
}

/*
void ppmImage::getDIB(byte*& pdib,int xres,int yres)
{
	if(!h || !w || !buf)
	{
		pdib = NULL;
		return;
	}
	//set up color output dib////////////////////////////////
	if(!makeRgbDIB(pdib,w,h,xres,yres))
		return;

	int nOutRowBytes = ((w*24+31)/32)*4;
	int nOutHeaderSize = sizeof(BITMAPINFOHEADER);
	byte* tmpOut = pdib+nOutHeaderSize;
	
	////////////////////////////////////////////////////////////

	color* inBuf = buf;
	
	byte* outBuf = tmpOut + (h-1)*nOutRowBytes;

	for(int row=0;row<h;row++,outBuf-=nOutRowBytes,inBuf+=w)
	{
		memcpy(outBuf,inBuf,3*w);
	}
}

void ppmImage::getGrayDIB(byte*& grayDib,int xres,int yres)
{
	if(!h || !w || !buf)
	{
		grayDib = NULL;
		return;
	}
	//set up grayscale output dib////////////////////////////////
	if(!makeGrayDIB(grayDib,w,h,xres,yres))
		return;

	int nOutRowBytes = ((w*8+31)/32)*4;
	int nOutHeaderSize = sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);
	byte* tmpOut = grayDib+nOutHeaderSize;
	////////////////////////////////////////////////////////////

	color* inBuf = buf;
	
	byte* outBuf = tmpOut + (h-1)*nOutRowBytes;

	for(int row=0;row<h;row++,outBuf-=nOutRowBytes)
	{
		byte* pOutBuf = outBuf;
		for(int x=0;x<w;x++)
		{
			double r = inBuf->r;
			double g = inBuf->g;
			double b = inBuf->b;
			inBuf++;
			double res = (0.299 * r + 0.587 * g + 0.114 * b);
			*pOutBuf++ = (res > 255.0 ? (byte)255 : (res >= 0.0 ? (byte)res : 0));
		}
	}
}
*/

void ppmImage::fillBox(UBBox& oBox,const color& oColor)
{
	for(int y=oBox.top;y<=oBox.bottom;y++)
	{
		for(int x=oBox.left;x<=oBox.right;x++)
		{
			m_pRowPointers[y][x] = oColor;
		}
	}
}

void ppmImage::xorBox(IBBox& oBox)
{
	color white(255,255,255);
	for(int y=oBox.top;y<=oBox.bottom;y++)
	{
		for(int x=oBox.left;x<=oBox.right;x++)
		{
			m_pRowPointers[y][x] = white - m_pRowPointers[y][x];
		}
	}
}

void ppmImage::line(int x0,int y0,int x1,int y1,color& oC)
{
    int dx, dy;
    int xinc, yinc;

    register int res1;
    int res2;

    xinc = 1;
    yinc = 1;
    if ((dx = x1-x0) < 0)
    {
        xinc = -1;
        dx = -dx;
    }
    if ((dy = y1-y0) < 0)
    {
        yinc = -1;
        dy = -dy;
    }
    res1 = 0;
    res2 = 0;

    if (dx > dy)
    {
        while (x0 != x1)
        {
            pixel(x0, y0) = oC;
            if (res1 > res2)
            {
                res2 += dx - res1;
                res1 = 0;
                y0 += yinc;
            }
            res1 += dy;
            x0 += xinc;
        }
    }
    else if (dx < dy)
    {
        while (y0 != y1)
        {
            pixel(x0, y0) = oC;
            if (res1 > res2)
            {
                res2 += dy - res1;
                res1 = 0;
                x0 += xinc;
            }
            res1 += dx;
            y0 += yinc;
        }
    }
    else
    {
        while (x0 != x1)
        {
            pixel(x0, y0) = oC;
            y0 += yinc;
            x0 += xinc;
        }
    }

	pixel(x1,y1) = oC;
}
