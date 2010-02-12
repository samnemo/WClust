// FILENAME: Split.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#ifndef SPLIT_H
#define SPLIT_H 1

#include "Node.h"

struct kdTreeLeafNode
{
  NodeBase *child;
  unsigned short parent;
};

struct kdTreeInternalNode
{
  unsigned short splitDim;  /* dimension of split
                               assumption is #dims < 256 */
  float splitPos;           /* position of split */
  unsigned short left;        /* left child */
  unsigned short right;       /* right child */
                            /* Convention: array_index + KDBNODECARD if
                               child is leaf, else just array_index, in
                               this way, we can directly say whether
                               the child is a leaf or not  */
  unsigned short parent;      /* KDBNODECARD is stored in parent field for
                               root, all others have parent < KDBNODECARD */
};

#define KDBNODECARD (int)((PGSIZE-(4*sizeof(short)) + sizeof(struct kdTreeInternalNode))/(sizeof(kdTreeInternalNode) + sizeof(unsigned short) + sizeof(short) + sizeof(off_t)))

struct split
{
        unsigned short splitDim;  /* dimension of split
                                     assumption is #dims < 256 */
        float splitPos;           /* position of split */
};

struct split_info
{
  unsigned int splitNode;   // index of internal node used to split the kd-tree
  int numCascadingSplits;   // number of cascading splits
  int numLeftChildren;      // left children (excluding replicated nodes)
  int numRightChildren;     // right children (excluding replicated nodes)
};

class node_split_info
{
  public:

  unsigned short splitdim; // new split dimension 
  float splitpos;          // new split position  

  int numCascadingSplits;   // number of cascading splits
  unsigned int *listOfSplittingChildren;  // list of chilren who would split

  int numLeftChildren;               // left children (excluding replicated nodes)
  unsigned int *listOfLeftChildren;  

  int numRightChildren;              // right children (excluding replicated nodes)
  unsigned int *listOfRightChildren; 

  node_split_info(){
        listOfSplittingChildren = new unsigned int[KDBNODECARD];
        listOfLeftChildren = new unsigned int[KDBNODECARD];
        listOfRightChildren = new unsigned int[KDBNODECARD];

        numLeftChildren = 0;
        numRightChildren = 0;
  };

  node_split_info(const node_split_info &that){
        listOfSplittingChildren = new unsigned int[KDBNODECARD];
        listOfLeftChildren = new unsigned int[KDBNODECARD];
        listOfRightChildren = new unsigned int[KDBNODECARD];
  
        splitdim = that.splitdim;
        numCascadingSplits =  that.numCascadingSplits;  
        splitpos = that.splitpos; 
        numLeftChildren = that.numLeftChildren;  
        numRightChildren = that.numRightChildren;  

        memcpy(listOfSplittingChildren, that.listOfSplittingChildren, sizeof(unsigned int)*KDBNODECARD);
        memcpy(listOfLeftChildren, that.listOfLeftChildren, sizeof(unsigned int)*KDBNODECARD);
        memcpy(listOfRightChildren, that.listOfRightChildren, sizeof(unsigned int)*KDBNODECARD);
  }; 

  node_split_info& operator=(const node_split_info &that){
        if(this == &that) return *this;

        splitdim = that.splitdim;
        numCascadingSplits =  that.numCascadingSplits;
        splitpos = that.splitpos;
        numLeftChildren = that.numLeftChildren;
        numRightChildren = that.numRightChildren;   
  
        memcpy(listOfSplittingChildren, that.listOfSplittingChildren, sizeof(unsigned int)*KDBNODECARD);
        memcpy(listOfLeftChildren, that.listOfLeftChildren, sizeof(unsigned int)*KDBNODECARD);
        memcpy(listOfRightChildren, that.listOfRightChildren, sizeof(unsigned int)*KDBNODECARD);
  
        return *this;
  };
  
  ~node_split_info(){
        delete []listOfSplittingChildren;
        delete []listOfLeftChildren; 
        delete []listOfRightChildren;
  };
};

#endif
