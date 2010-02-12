// FILENAME: Query.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#ifndef QUERY_H
#define QUERY_H

#include "Node.h"
#include "decl.h"
#include "Rect.h"
#include <vector>
#include <stack>
#include <deque>
#include <algorithm>
#include <functional>
//#include <iostream.h>

#include "memsl.h"

class KDBTreeResult{
  public:
    int numObjects;
    WBQUEUE *queue;

    KDBTreeResult(){
	queue = WBQueueOpen(NULL,0,0,0);
    };
    ~KDBTreeResult(){
	WBQueueClose(queue);
    };
};


typedef std::deque<long> NODELIST;

struct kdTreeLeafInfo
{
    int leafNodeIndex;
    Rect leafRect;
};

struct numleaves
{
    int numLeavesInSubtree;
    unsigned int leafNode;  // valid iff numLeavesInSubtree==1
    int numLeavesUnderLeftChild;  // valid iff numLeavesInSubtree>1
};

struct lineseg
{
    int leafNodeIndex;
    float leftbound;
    float rightbound;
};

#endif

