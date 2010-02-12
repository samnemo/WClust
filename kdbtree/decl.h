// FILENAME: decl.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#ifndef DECL_H
#define DECL_H 1

#include <sys/types.h>
//#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>

#define NUM_PAGES_IN_MEMORY 0

//#define GREEDY 1

#define NDEBUG
#define LEFT 0
#define RIGHT 1
#define LEAF 10
#define INTERNAL 20
#define MIN(a,b) ( (a) < (b) ? (a) : (b))
#define MAX(a,b) ( (a) > (b) ? (a) : (b))

#define NOTFOUND 0
#define FOUND 1
#define BOUNDARYUPDATE 2
#define UNDERUTIL 3

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

//extern int PGSIZE;
#define PGSIZE 4096
//extern int NUMDIMS;
//extern float MINNODEUTIL;

//#ifndef DIRECTIO
//#define DIRECTIO 1
#ifdef DIRECTIO
extern "C"{
int directio(int, int);
}
#endif


#endif
