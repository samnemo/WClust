// FILENAME: Node.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#ifndef NODE_H
#define NODE_H
#include <stdio.h>
#include <sys/types.h>

class NodeBase{
 public:
  enum {DATANODE, KDBNODE};

  FILE *tree;

  off_t offset;

  short type;                /* 1 if ordinary node, 2 if KDBnode */
  short level;               /* 0 is leaf, others positive */
  int inmemory;
  
  NodeBase(int); 
  off_t getOffset();
};


#endif
