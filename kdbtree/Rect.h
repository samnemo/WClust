// FILENAME: Rect.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#ifndef RECT_H
#define RECT_H

#include "decl.h"
#include "Point.h"

typedef float RectReal;

class Rect {
 public:
  RectReal *boundary; /* xmin,ymin,...,xmax,ymax,... */

  Rect();
  Rect(const Rect &that);
  Rect& operator=(const Rect& that);
  Rect& operator=(const Point& that);
  ~Rect();

  void Init();
  RectReal Low(int);
  RectReal High(int);

  void Low(int, RectReal);
  void High(int, RectReal);

  void Print();
  RectReal Volume();

  Rect* CombineRect(Rect*);
  Rect* CombineRect(Point*);

  int Overlap(Rect*);
  int Overlap(Point*);

  float AmountOfOverlap(Rect*);
  int Contain(Point *P);
  int Contained(Rect *S);

  int Equal(Rect*);

};

#endif
