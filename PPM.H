// $Id: PPM.H,v 1.2 2008/03/26 03:58:41 samn Exp $ 
#ifndef __PPM_H__
#define __PPM_H__

#ifndef byte
typedef unsigned char byte;
#endif

#include <string.h>
#include "color.h"

#include "BBox.h"

//class for raw rgb images in ppm format
class ppmImage
{
    protected:
    
      int w;		//width
      int h;  		//height
      color *buf;
	  color** m_pRowPointers;
      char header[5];   //first line in ppm file
           
      void loadFile(char *fname);
      void saveFile(char *fname);
	  //allocate pointers
      void allocSpace();     
      void copyImage(const ppmImage& src);
	  //reverse red and blue components to align properly in color struct
	  void reverseRedBlue();
	  //init members
	  void init();
    public:
      color& pixel(int x,int y){ return buf[x + y * w]; }

#ifdef __XY_PAIR__

      class subx {
         ppmImage &i;
         int x;
         public:
            subx(ppmImage& _i,int _x): i(_i),x(_x){}
            color& operator[](int y){ return i.pixel(x,y); }
      };
      subx operator[](int x){ return subx(*this,x); }
#else
	  color* operator[](int y){ return m_pRowPointers[y]; }
	  color* operator[](int y) const { return m_pRowPointers[y]; }
#endif
	  //copy constructor
      ppmImage(const ppmImage& src);
	  //operator=
      ppmImage& operator=(const ppmImage& src);
	  //construct from file
      ppmImage(char *str = 0);
	  //initialize to w x h
      ppmImage(int w,int h);
     ~ppmImage(); 
      bool isLegal(){ return buf != NULL; }
	  //free memory
      void destroy();
      int width(){ return w; }
      int height(){ return h; }
      int width() const { return w; }
      int height() const { return h; }
	  //create this to w x h
      void create(int _w,int _h);
	  //set all pixels to white
      void setWhite();
	  //set all pixels to black
      void setBlack();
	  void getDIB(byte*& pdib,int xRes,int yRes);//gets image as dib
	  void getGrayDIB(byte*& dib,int xres,int yres);//gets grayscale version of ppm
      byte* address() { return (byte*)buf; };
      friend void operator>>(ppmImage &i,char *str);	// Dump image to file
      friend void operator<<(ppmImage &i,char *str);	// Read from file

	  void fillBox(UBBox& oBox,const color& oColor);
	  void xorBox(IBBox& oBox);
	  void line(int x0,int y0,int x1,int y1,color& oC);
};


//// Inline functions definitions
inline void operator>>(ppmImage &i,char *str){
    i.saveFile(str);
}

inline void operator<<(ppmImage &i,char *str){
    i.loadFile(str);
}

#endif  // __PPM_H__
