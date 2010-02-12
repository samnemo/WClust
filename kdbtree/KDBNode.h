// FILENAME: KDBNode.h
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#ifndef KDBNODE_H
#define KDBNODE_H

#include "decl.h"
#include "split.h"
#include "Node.h"
#include "DataNode.h"
#include "Rect.h"
#include "Query.h"

class KDBNode : public NodeBase {
 public:

  short numInternalNodes;   
  short numLeafNodes;   
  struct kdTreeInternalNode *internal;   //[KDBNODECARD-1]; 
  struct kdTreeLeafNode *leaf;    //[KDBNODECARD]; 
  
//#ifdef FOPEN
  KDBNode(FILE* tree);
//#else
//  KDBNode(int tree);
//#endif
  KDBNode(const KDBNode&);
  ~KDBNode();
  KDBNode& operator=(const KDBNode&);

  void InternalInit(int i);
  void LeafInit(int i);
  int NumInternalNodes();
  void NumInternalNodes(int newnum);
  void IncNumInternalNodes();
  int NumLeafNodes();
  void NumLeafNodes(int newnum);
  void IncNumLeafNodes();
  struct kdTreeInternalNode Internal(int i);
  void Internal(int i, struct kdTreeInternalNode I);
  struct kdTreeLeafNode Leaf(int i);
  void Leaf(int i, struct kdTreeLeafNode L);
  void LeafParent(int i, int parentindex);
  
  void Write();
  void Load(off_t off);

  void Print(unsigned short i, Rect* r);
  Rect* MinCover(); 
  int AddChild(NodeBase *newDataNode, KDBNode **New_node, int kdTreeLeafIndex, 
		struct split* dataNodeSplit, struct split** kdTreeSplit); 

  /* add the DataNode, produced due to splitting of the leaf (3rd argument) */
  int FillPickList(Point *r, int currRoot, struct kdTreeLeafInfo *picklist, 
		int *pickcount, Rect *indexedRegion);

  /* return the index the child the data should be inserted into */
  int PickChild(Point *p, Rect *parentNodeRect, Rect *childNodeRect); 

  /* return the index the child the point should be inserted into */
 
  // given the br of this node, get the BRs of all the children
  void FillChildList(int currRoot, Rect *childRectlist, Rect *indexedRegion); 
  /* for bounding box queries, r is the bounding box */
  void RangeQuery(Rect *r, int currRoot, int *visitlist, int *visitcount);

  /* functions to split an KDBNode */
  void GetkdtreeLeafBRs(int subtreeroot, Rect *indexedRegion, Rect *kdtreeLeafBRList);
  void GetNumLeavesInKDBSubTree(int index, int *numLeaves, unsigned int *listOfLeaves);
  int IsAncestor(unsigned int internalNodeIndex, unsigned int leafNodeIndex);
                struct numleaves GetNumLeavesInSubtree(node_split_info *splitInfo, 
		unsigned int subtreeroot, int option);
  int GetNextKDBkdTreeNode(int option, int currnodeptr, node_split_info *splitInfo); 
  void ConstructKDBSubtree(KDBNode *subtreeNode, node_split_info *splitInfo, int option, NodeBase **);
  void SplitKDBNodekdTree(KDBNode **right, node_split_info *splitInfo, NodeBase**, NodeBase**);
  void GuaranteedAssessSplits(node_split_info *split_choice_array);

  int GetNumLeavesInSubTree(int index);
  void GetSplitInfo(unsigned int subtreeroot, unsigned int stopnode, 
		unsigned short splitdim, float splitpos, struct split_info *newSplitInfo);
  void GetListOfSplittingDataNodes(unsigned int subtreeroot, unsigned int stopnode, 
		unsigned short splitdim, float splitpos, unsigned int *list, int *listcount);
  struct split_info GetkdTreeSplitInfo(int index);
  int GetNextNode(int splitnode, int option, int currnodeptr);
  int RemoveLeaf(int i);
  void SplitNode(KDBNode **nn, struct split **newSplit);
  void CascadingSplitNode(KDBNode **nn, int splitdim, float splitpos);
  node_split_info GetKDBNodeSplitInfo(int);

  struct split MinOverlappingPartition(Rect*, int);

  /* function to create the first KDBNode, the root */

  void CreatekdTree(NodeBase*, NodeBase*, struct split*);
  void CopyFrom(KDBNode *);

  float TotalOverlap(Rect *parentNodeRect);

};

#endif
  
