#include "stdafx.h"
// FILENAME: Point.C
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#include <cstdio>
#include <cstdlib>
#include <cstring>
//#include <strings.h>
#include <cassert>
#include <float.h>
#include <cmath>
#include "Point.h"

#define BIG_NUM (FLT_MAX/4.0)

extern int NUMDIMS;

Point::Point()
{
  pt = new float[NUMDIMS];

  for (int i=0; i<NUMDIMS; i++){
    pt[i] = (float)0.0;
  }
}

Point::Point(const Point &that)
{
  pt = new float[NUMDIMS];

  memcpy(pt, that.pt, sizeof(float)*NUMDIMS);
}


Point::~Point(){
  delete []pt;
}

Point& Point::operator=(const Point &that)
{
  if(this == &that) return *this;

  memcpy(pt, that.pt, sizeof(float)*NUMDIMS);

  return *this;
}

void Point::Init()
{
  int i;
  for (i=0; i<NUMDIMS; i++){
    pt[i] = (float)0.0;
  }
}


void Point::Print()
{
  int i;
  
  printf("point:\n");
  for (i = 0; i < NUMDIMS; i++) {
	printf("\t%f", pt[i]);
  }
  printf("\n");
}

