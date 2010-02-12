#include "stdafx.h"
// FILENAME: Rect.C
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
#include "Rect.h"

extern int NUMDIMS;

Rect::Rect()
{
  boundary = new RectReal[2*NUMDIMS];

  for (int i=0; i<2*NUMDIMS; i++){
    boundary[i] = (RectReal)0.0;
  }
}

Rect::Rect(const Rect &that)
{
  boundary = new RectReal[2*NUMDIMS];

  memcpy(boundary, that.boundary, sizeof(RectReal)*2*NUMDIMS);
}


Rect::~Rect(){
  delete []boundary;
}

Rect& Rect::operator=(const Rect &that)
{
  if(this == &that) return *this;

  memcpy(boundary, that.boundary, sizeof(RectReal)*2*NUMDIMS);

  return *this;
}

Rect& Rect::operator=(const Point &that)
{
  int i;

  for(i=0;i<NUMDIMS;i++){
	boundary[i] = that.pt[i];
	boundary[i+NUMDIMS] = that.pt[i];
  }

  return *this;
}

void Rect::Init()
{
  int i;
  for (i=0; i<2*NUMDIMS; i++){
    boundary[i] = (RectReal)0.0;
  }
}


RectReal Rect::Low(int i)
{
  return boundary[i];
}

RectReal Rect::High(int i)
{
  return boundary[NUMDIMS+i];
}

void Rect::Low(int i, RectReal value)
{
  boundary[i]=value;
}

void Rect::High(int i, RectReal value)
{
  boundary[NUMDIMS+i]=value;
}


void Rect::Print()
{
  int i;
  float tmp1, tmp2;
  
  printf("\trect:\n");
  for (i = 0; i < NUMDIMS; i++) {
	tmp1 = (float) boundary[i];
	tmp2 = (float) boundary[i + NUMDIMS];
	printf("\t%f\t%f\n", tmp1, tmp2);
  }
}


RectReal Rect::Volume()
{
  int i;
  RectReal volume = (RectReal)1;
  
  for(i=0; i<NUMDIMS; i++)
    volume *= boundary[i+NUMDIMS] - boundary[i];
  assert(volume >= 0.0);
  return volume;
}

Rect* Rect::CombineRect(Rect *Rr)
{
  Rect *rr = Rr;
  int i,j;
  Rect *new_rect = new Rect();
  
  for (i = 0; i < NUMDIMS; i++)
  {
      new_rect->Low(i, (MIN(boundary[i], rr->Low(i))));
      j=NUMDIMS+i;
      new_rect->High(i, (MAX(boundary[j], rr->High(i))));
  }
  return new_rect;
}

Rect* Rect::CombineRect(Point *Pr)
{
  int i,j;
  Rect *new_rect = new Rect();
  
  for (i = 0; i < NUMDIMS; i++)
  {
      new_rect->Low(i, (MIN(boundary[i], Pr->pt[i])));
      j=NUMDIMS+i;
      new_rect->High(i, (MAX(boundary[j], Pr->pt[i])));
  }
  return new_rect;
}

int Rect::Overlap(Point *s)
{
  int i, j;
  
  for (i=0; i<NUMDIMS; i++){
      j = i + NUMDIMS;  // index for high sides 
      if (boundary[i] > s->pt[i] || s->pt[i] > boundary[j])
	return FALSE;
  }

  return TRUE;
}


int Rect::Overlap(Rect *s)
{
  int i, j;
  
  for (i=0; i<NUMDIMS; i++){
      j = i + NUMDIMS;  // index for high sides 
      if (boundary[i] > s->High(i) || s->Low(i) > boundary[j])
	return FALSE;
  }

  return TRUE;
}


float Rect::AmountOfOverlap(Rect *S)
{
  Rect *s = S;
  int i, j;
  float overlap=1.0;
 
  for (i=0; i<NUMDIMS; i++){
      j = i + NUMDIMS;  // index for high sides 
      if (boundary[i] >= s->High(i) || s->Low(i) >= boundary[j])
	return 0;
      else overlap *= MIN(boundary[j], s->High(i)) - MAX(boundary[i], s->Low(i));
  }

  return overlap;
}

int Rect::Contain(Point *P)
{
  Point *p = P;
  int i, j, result;
  
  result = TRUE;
  for (i = 0; i < NUMDIMS; i++)
   {
     j = i + NUMDIMS;  /* index for high sides */
     result = result
	& boundary[i] <= p->pt[i]
	& boundary[j] >= p->pt[i];
    }
  return result;
}

int Rect::Contained(Rect *S)
{
  Rect *s = S;
  int i, j, result;
  
  result = TRUE;
  for (i = 0; i < NUMDIMS; i++)
   {
     j = i + NUMDIMS;  /* index for high sides */
     result = result
	& boundary[i] >= s->Low(i)
	& boundary[j] <= s->High(i);
    }
  return result;
}


int Rect::Equal(Rect *S)
{
  Rect *s = S;
  int i;
  
  for (i=0; i<NUMDIMS; i++){
      if (boundary[i] != s->Low(i) || boundary[i+NUMDIMS] != s->High(i))
	return FALSE;
  }

  return TRUE;
}
