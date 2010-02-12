// FILENAME: DataNode.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------

#ifndef DATANODE_H
#define DATANODE_H

#include "decl.h"
#include "Node.h"
#include "Rect.h"
#include "Point.h"
#include "Query.h"
#include "split.h"

class Rect;

struct dimspread
{
  unsigned short dimension;
  float spread;
};


class Branch
{
 public:
  Point dataPtr;
  char *child;    /* can either be pointer to node or data block */

  Branch();
  Branch(const Branch& that);
  Branch& operator=(const Branch& that);
  ~Branch();
};


class DataNode : public NodeBase {
 public:
  short count;
  Branch *branch; 
  

  DataNode(FILE* tree);
  ~DataNode();
  DataNode& operator=(const DataNode &that);

  void   	Write();
  void   	Load(off_t off);

  //void   	Init();
  void   	BranchInit(int);
  int    	Count();
  void   	Count(int);
  void   	IncrCount();
  Branch	getBranch(int);
  void   	putBranch(int, Branch);
  void   	DisconnectBranch(int i);
  void   	Print();
  Rect*   	Cover(); 
  int    	AddChild(Branch *, DataNode**, struct split**);
  void   	SplitNode(DataNode**, struct split**);
  void   	CascadingSplitNode(DataNode**, int split_dim, float split_pos);
  void   	CopyFrom(DataNode *n);

};

#endif
