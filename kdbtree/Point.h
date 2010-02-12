// FILENAME: Rect.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#ifndef POINT_H
#define POINT_H

#include "decl.h"

class Point {
 public:
  float *pt; 

  Point();
  Point(const Point &that);
  Point& operator=(const Point& that);
  ~Point();

  void Init();

  void Print();

};

#endif
