#ifndef COLOR_H
#define COLOR_H

template<class T>
inline T Sq(T t){ return t*t; }

typedef unsigned char byte;

//struct member order is b g r so will correspond to windows
//RGB dib format for faster transfer of images between the two types
struct color
{
  byte b;
  byte g;
  byte r;
  
  int SqDif(color& t);
  color operator+(const color& c);
  color operator-(const color& c);
  color operator*(const color& c);
  color operator/(const color& c);
  color operator*(int i);
  color operator/(int i);
  color();
  color(byte r,byte g,byte b);
  bool operator==(const color& c);
  bool operator!=(const color& c);
};


#endif