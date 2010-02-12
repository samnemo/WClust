// FILENAME: KDBTree.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------

#ifndef KDBTREE_H
#define KDBTREE_H

#include "decl.h"
#include "Node.h"
#include "DataNode.h"
#include "KDBNode.h"
#include "Rect.h"
#include "Point.h"
#include "Query.h"
#include "KDBTree_C.h"


class KDBTree{
 public:

    NodeBase *KDBTreeRoot;
    int height;
    Rect* rootRect;

    FILE* tree;

    off_t root_offset;

    int firstitem;   // is this the first data item being inserted
    NODELIST pages_in_memory;  // list of pages in memory

    // creation, insertion, range query, etc. 
    // KDBTree.C contains these
    KDBTree();
    ~KDBTree();

    // API functions
    void KDBTreeCreate(const char *dumpfile, int dimension, int dataSize, int pageSize);
    void KDBTreeCreateRect(const char *dumpfile, int dimension, int dataSize, int pageSize);
    void KDBTreeOpen(const char *dumpfile); // loading from disk
    int KDBTreeGetRoot(float lb[], float hb[]);
    int KDBTreeStore(float pt[], char* data);
    int KDBTreeStoreRect(float lb[], float hb[], char* data);
    int KDBTreeDelete(float pt[]);
    int KDBTreeRangeQuery(float lb[], float hb[], KDBTreeResult* res);
    int KDBTreeRangeQueryRect(float lb[], float hb[], KDBTreeResult* res);
    int KDBTreeGetNextResult(KDBTreeResult *res, KDBTreeRecord* record);
    void KDBTreeClose();
    void Print();

 private:
    int count;

    // insertion code
    int Insert2(Point *p, char *data, NodeBase *n, NodeBase **new_node, 
		struct split **newsplit, int level, Rect *parentNodeRect);
    int Insert(Point *P, char *data, NodeBase **Root, int Level);
    int Remove(NodeBase *N, Rect *R);

    // deletion
    //int Delete(Rect *P, int Tid);
    //int KDBTreeDeleteRect2(Rect *P, int Tid, Node *N, int *isUnderFull);

    // misc
    int IsPageFault(NodeBase *n);
    void ClearBuffer();

    // end of functions in KDBTree.C

    // PRIVATE, NOT TO BE USED BY USER, RANGE QUERIES
    // RANGE QUERIES, contained in KDBTreeRangeSearch.C

    // BOUNDING BOX BASED SEARCH
    int Search(NodeBase *N, Rect *R, KDBTreeResult *res);

    int Print(NodeBase *n, Rect *parentRect);

    // end of functions related to RANGE QUERIES
};

#endif

