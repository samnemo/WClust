#include "stdafx.h"
// FILENAME: KDBNode.C
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "KDBNode.h"
#include <sys/types.h>
//#include <sys/fcntl.h>

#ifdef IO_STATS
extern int page_read, page_write;
extern int insert_mode;
#endif

extern int floatcmp(float *, float *);

extern int NUMDIMS;

void KDBNode::Write()
{
  int i;
  char Buffer[PGSIZE];
  int  ptr=0;


  if(offset==-1){
        fseek(tree,0,SEEK_END);
        offset = ftell(tree);
  }
  else 
	fseek(tree, offset, SEEK_SET);


#ifdef IO_STATS
page_write++;
#endif

  memcpy((char*) Buffer+ptr, (char*) &level, sizeof(short));
  ptr += sizeof(short);
  memcpy((char*) Buffer+ptr, (char*) &numInternalNodes, sizeof(short));
  ptr += sizeof(short);
  memcpy((char*) Buffer+ptr, (char*) &numLeafNodes, sizeof(short));
  ptr += sizeof(short);

  // Write Internal Nodes Data
  memcpy((char*) Buffer+ptr, (char*) internal, numInternalNodes*sizeof(struct kdTreeInternalNode));
  ptr += numInternalNodes*sizeof(struct kdTreeInternalNode);

  // Write Leaf Nodes Data
  for(i=0; i< numLeafNodes; i++){
      memcpy((char*) Buffer+ptr, (char*) &leaf[i].parent, sizeof(unsigned short));
      ptr += sizeof(unsigned short);

      memcpy((char*) Buffer+ptr, (char*) &(leaf[i].child->offset), sizeof(off_t)); 
      ptr += sizeof(off_t);

  }


  fwrite(Buffer, PGSIZE, 1, tree);

}

void KDBNode::Load(off_t off)
{
  int i;
  char Buffer[PGSIZE];
  int  ptr=0;

#ifdef IO_STATS
  if(insert_mode == 1){
        if(inmemory == 1) return;
        inmemory = 1;
  }
  page_read++;
#else
  if(inmemory == 1) return;
  inmemory = 1;
#endif

  offset = off;


  fseek(tree, offset, SEEK_SET);
  fread(Buffer, PGSIZE, 1, tree);


  memcpy((char*) &level, (char*) Buffer+ptr, sizeof(short)); 
  ptr += sizeof(short);
  memcpy((char*) &numInternalNodes, (char*) Buffer+ptr, sizeof(short)); ;
  ptr += sizeof(short);
  memcpy((char*) &numLeafNodes, (char*) Buffer+ptr, sizeof(short)); 
  ptr += sizeof(short);


  memcpy((char*) internal, (char*) Buffer+ptr, numInternalNodes*sizeof(struct kdTreeInternalNode));

  ptr += numInternalNodes*sizeof(struct kdTreeInternalNode);

  for(i=0; i< numLeafNodes; i++){
      if(leaf[i].child == NULL){
	if(level == 1 ){   // child is a DATANODE
	    leaf[i].child = new DataNode(tree);
	}
	else if(level > 1) {
	    leaf[i].child = new KDBNode(tree);
	}
	else {
	    printf("Error.\n");
	    exit(1);
	}
      }
  }

  for(i=0; i< numLeafNodes; i++){
      memcpy((char*) &leaf[i].parent, (char*) Buffer+ptr, sizeof(unsigned short)); 
      ptr += sizeof(unsigned short);

      memcpy((char*) &(leaf[i].child->offset), (char*) Buffer+ptr, sizeof(off_t)); 
      ptr += sizeof(off_t);
  }

  //fclose(tree);

}


// some misc functions to check presence of an element in a set,
// comparison functions for structs (so can we can pass it to qsort etc.

int IsASplittingLeaf(unsigned int aLeaf, unsigned int *splittingLeaves, int splittingLeafCount)
{
  int i;

  for(i=0; i<splittingLeafCount; i++)
    if (splittingLeaves[i]==aLeaf) return 1;

  return 0;
}


int cascadingcmp(node_split_info *a, node_split_info *b)
{
  if (a->numCascadingSplits < b->numCascadingSplits) return -1;
  else if (a->numCascadingSplits == b->numCascadingSplits) return 0;
  else return 1;
}

int leftboundcmp(struct lineseg *a, struct lineseg *b)
{
  if (a->leftbound < b->leftbound) return -1;
  else if (a->leftbound == b->leftbound ){
  	if (a->rightbound < b->rightbound) return -1;
  	else if (a->rightbound == b->rightbound ) return 0;
	else return 1;
  }
  else return 1;
}


int rightboundcmp(struct lineseg *a, struct lineseg *b)
{
  if (a->rightbound > b->rightbound ) return -1;
  else if (a->rightbound == b->rightbound ){
	if (a->leftbound > b->leftbound ) return -1;
	else if (a->leftbound == b->leftbound ) return 0;
	else return 1;
  }
  else return 1;
}


int IsPresent(DataNode *aChild, DataNode **childList, int childCount)
{
  int i;
  for(i=0; i<childCount; i++)
    if (childList[i]==aChild) return 1;
  return 0;
}

int IsPresent(unsigned int aLeaf, unsigned int *leafList, int leafCount)
{
  int i;

  for(i=0; i<leafCount; i++)
    if (leafList[i]==aLeaf) return 1;
  return 0;
}

// end of misc functions

// constructor

KDBNode::KDBNode(FILE* _tree): NodeBase(NodeBase::KDBNODE) 
{
  int i;

  internal = new struct kdTreeInternalNode[KDBNODECARD-1];
  leaf = new struct kdTreeLeafNode[KDBNODECARD];

  tree = _tree;
  offset = -1;
  type = NodeBase::KDBNODE;
  level = -1;
  inmemory = 0;

  numInternalNodes = 0;
  numLeafNodes = 0;

  for (i = 0; i < KDBNODECARD-1; i++)
    InternalInit(i);
  for (i = 0; i < KDBNODECARD; i++)
    LeafInit(i);
}

// copy constructor
KDBNode::KDBNode(const KDBNode &that): NodeBase(NodeBase::KDBNODE)
{
  if(this == &that) return;
 
  tree = that.tree;
  offset = that.offset;
  type = NodeBase::KDBNODE;
  level = that.level;
  inmemory = that.inmemory;

  numInternalNodes = that.numInternalNodes;
  numLeafNodes = that.numLeafNodes;

  internal = new struct kdTreeInternalNode[KDBNODECARD-1];
  leaf = new struct kdTreeLeafNode[KDBNODECARD];

  memcpy(internal, that.internal, sizeof(struct kdTreeInternalNode)*(KDBNODECARD-1));
  memcpy(leaf, that.leaf, sizeof(struct kdTreeLeafNode)*KDBNODECARD);
}


// destructor
KDBNode::~KDBNode(){
  int i;

  delete []internal;
  internal = NULL;

//fprintf(stderr, "KDBNode destructor numLeafNodes = %d\n", numLeafNodes);
  //for(i=0; i< numLeafNodes; i++){
  for(i=0; i< KDBNODECARD; i++){
	if(leaf[i].child != NULL){
		if(leaf[i].child->type == NodeBase::KDBNODE) {
//fprintf(stderr,"%d: KDBNODE\n");
			delete ((KDBNode*) leaf[i].child);
		}
		else if(leaf[i].child->type == NodeBase::DATANODE) {
//fprintf(stderr,"%d: DATANODE\n");
			delete ((DataNode*) leaf[i].child);
		}
		leaf[i].child = NULL;
	} 
  }
  delete []leaf;
  leaf = NULL;
}


KDBNode& KDBNode::operator=(const KDBNode &that)
{
  if(this == &that) return *this;
  delete []internal;
  delete []leaf;
 
  tree = that.tree;
  offset = that.offset;
  numInternalNodes = that.numInternalNodes;
  numLeafNodes = that.numLeafNodes;

  internal = new struct kdTreeInternalNode[KDBNODECARD-1];
  leaf = new struct kdTreeLeafNode[KDBNODECARD];

  memcpy(internal, that.internal, sizeof(struct kdTreeInternalNode)*(KDBNODECARD-1));
  memcpy(leaf, that.leaf, sizeof(struct kdTreeLeafNode)*KDBNODECARD);

  return *this;
}


// initialize functions
void KDBNode::LeafInit(int i)
{
  leaf[i].parent=0;
  leaf[i].child=NULL;
}

void KDBNode::InternalInit(int i)
{
  internal[i].splitDim=0;
  internal[i].splitPos=  -FLT_MAX;
  internal[i].left=0;
  internal[i].right=0;

}

// accessor functions

int KDBNode::NumInternalNodes()
{
  return numInternalNodes;
}

void KDBNode::NumInternalNodes(int newnum)
{
  numInternalNodes=newnum;
}

void KDBNode::IncNumInternalNodes()
{
  numInternalNodes++;
}

int KDBNode::NumLeafNodes()
{
  return numLeafNodes;
}

void KDBNode::NumLeafNodes(int newnum)
{
  numLeafNodes=newnum;
}

void KDBNode::IncNumLeafNodes()
{
  numLeafNodes++;
}


struct kdTreeInternalNode KDBNode::Internal(int i)
{
  assert(i < numInternalNodes);
  return internal[i];
}

void KDBNode::Internal(int i, struct kdTreeInternalNode I)
{
  internal[i]=I;
}

void KDBNode::LeafParent(int i, int parentindex)
{
  leaf[i].parent=parentindex;
}


void KDBNode::Print(unsigned short i, Rect* parentNodeRect)
{
  Rect childRectlist[KDBNODECARD];

  FillChildList(0, childRectlist, parentNodeRect);

  // fill in printing stuff with proper indentation
  printf("internal %d-------------\n", i);
  printf("  splitDim: %d\n", internal[i].splitDim);
  printf("  splitPos: %f\n", internal[i].splitPos);
  printf("  left_child: %d\n", internal[i].left);
  printf("  right_child: %d\n", internal[i].right);
  if(internal[i].left >= KDBNODECARD) {
	childRectlist[internal[i].left - KDBNODECARD].Print();
  }
  else Print(internal[i].left, parentNodeRect);
  if(internal[i].right >= KDBNODECARD) {
	childRectlist[internal[i].right - KDBNODECARD].Print();
  }
  else Print(internal[i].right, parentNodeRect);
}


Rect* KDBNode::MinCover()
{
  int i;
  Rect* minCover;
  Rect *r, *new_cover;
  KDBNode *sn;
  DataNode *dn;

  minCover = new Rect();

  for(i=0; i< numLeafNodes; i++){
	switch(leaf[i].child->type){
            case NodeBase::DATANODE:
                dn = (DataNode*) leaf[i].child;
                dn->Load(dn->offset);
		new_cover = minCover->CombineRect(r = dn->Cover());
                break;
            case NodeBase::KDBNODE:
                sn = (KDBNode*) leaf[i].child;
                sn->Load(sn->offset);
		new_cover = minCover->CombineRect(r = sn->MinCover());
                break;
        }
	*minCover = *new_cover;
	delete r;
	delete new_cover;
  }

  return minCover;
}


// add a child -- splits if full

int KDBNode::AddChild(NodeBase *newChildNode, KDBNode **new_node, int kdTreeLeafIndex, 
		struct split* dataNodeSplit, struct split **kdTreeSplit)
{
  int parent;

  parent=leaf[kdTreeLeafIndex].parent;

  if (internal[parent].left == (kdTreeLeafIndex+KDBNODECARD))
	internal[parent].left = numInternalNodes;
  else if (internal[parent].right == (kdTreeLeafIndex+KDBNODECARD))
	internal[parent].right = numInternalNodes;
  else
	assert(FALSE);   // shouldnt happen

  leaf[kdTreeLeafIndex].parent = numInternalNodes ;

  internal[numInternalNodes].splitDim= dataNodeSplit->splitDim;
  internal[numInternalNodes].splitPos= dataNodeSplit->splitPos;
  // this assertion is true for point data

  internal[numInternalNodes].left=KDBNODECARD+kdTreeLeafIndex;
  internal[numInternalNodes].right=KDBNODECARD+numLeafNodes;
  internal[numInternalNodes].parent=parent;
  leaf[numLeafNodes].child = newChildNode;
  leaf[numLeafNodes].parent = numInternalNodes ;

  numInternalNodes++;
  numLeafNodes++;
 
  if (numLeafNodes >= KDBNODECARD){        // the KDBnode is full, should be split
      SplitNode(new_node, kdTreeSplit);
      Write();
      if( *new_node != NULL)
	    (*new_node)->Write();

      return 1;
  }

  Write();

  return 0;

}



// changed pickchild
int KDBNode::FillPickList(Point *p, int currRoot, struct kdTreeLeafInfo *picklist, 
			int *pickcount, Rect *indexedRegion)
{
  struct kdTreeInternalNode *intNode;
  Rect leftIndexedRegion;
  Rect rightIndexedRegion;
  int overlapL=0, overlapR=0;
  assert(currRoot < (KDBNODECARD-1));   // must be an internal kdtree node

  intNode = &(internal[currRoot]);

  if (p->pt[intNode->splitDim] <= intNode->splitPos){
      // only lower partition => LEFT
      // dataRect to be inserted overlaps with left partition
      leftIndexedRegion=*indexedRegion;
      leftIndexedRegion.boundary[NUMDIMS+(intNode->splitDim)]=intNode->splitPos; 

      if (intNode->left >= KDBNODECARD){
	  picklist[*pickcount].leafNodeIndex=(intNode->left)-KDBNODECARD;
	  picklist[*pickcount].leafRect=leftIndexedRegion;
	  (*pickcount)++;
      }
      else{
	  overlapL = FillPickList(p, intNode->left, picklist, pickcount, &leftIndexedRegion);
      }
  }
  else{
      //if (p->pt[intNode->splitDim] >= intNode->splitPos){
      // only right partition => RIGHT
      // dataRect to be inserted overlaps with only right partition
      rightIndexedRegion=*indexedRegion;
      rightIndexedRegion.boundary[intNode->splitDim]=intNode->splitPos;
      if (intNode->right >= KDBNODECARD){
	  picklist[*pickcount].leafNodeIndex=(intNode->right)-KDBNODECARD;
	  picklist[*pickcount].leafRect=rightIndexedRegion;
	  (*pickcount)++;
      }
      else{
	  overlapR = FillPickList(p, intNode->right, picklist, pickcount, &rightIndexedRegion);
      }
  }

  return 0;
}




// Pick the branch in which the point should be inserted.  
// Pick the one which needs minimum enlargement. 
// Returns the index of the leafnode of the kdtree into 
// which the point should be inserted
// ===========================
// Default Insertion Algorithm
// ===========================
int KDBNode::PickChild(Point *p, Rect *parentNodeRect, Rect *childNodeRect)
{
  struct kdTreeLeafInfo picklist[KDBNODECARD];
  int pickcount=0;

  FillPickList(p, 0, picklist, &pickcount, parentNodeRect);

  assert(pickcount >0);  // at least there is one leaf overlapping with the point
  assert (pickcount == 1);  // point overlaps with only one node, this is the leaf

  (*childNodeRect)=picklist[0].leafRect;
  assert(picklist[0].leafNodeIndex < numLeafNodes);
  return (picklist[0].leafNodeIndex);
}
  

// get rect's of all children
void KDBNode::FillChildList(int currRoot, Rect *childRectlist,  Rect *indexedRegion)
{
  struct kdTreeInternalNode *intNode;
  Rect leftIndexedRegion;
  Rect rightIndexedRegion;
  assert(currRoot < (KDBNODECARD-1));   // must be an internal kdtree node

  if (numLeafNodes <= 0) return;
  intNode = &(internal[currRoot]);

  leftIndexedRegion=*indexedRegion;
  leftIndexedRegion.boundary[NUMDIMS+(intNode->splitDim)]=intNode->splitPos;

  if (intNode->left >= KDBNODECARD){
      childRectlist[(intNode->left)-KDBNODECARD]=leftIndexedRegion;
  }
  else{
      FillChildList(intNode->left, childRectlist, &leftIndexedRegion);
  }
 
  rightIndexedRegion=*indexedRegion;
  rightIndexedRegion.boundary[intNode->splitDim]=intNode->splitPos;

  if (intNode->right >= KDBNODECARD){
      childRectlist[(intNode->right)-KDBNODECARD]=rightIndexedRegion;
  }
  else{
      FillChildList(intNode->right, childRectlist, &rightIndexedRegion);
  }
}



// for bounding box queries where r is the bounding box
void KDBNode::RangeQuery(Rect *q, int currRoot, int *visitlist, int *visitcount)
{
  struct kdTreeInternalNode *intNode;
  assert(currRoot < (KDBNODECARD-1));   // must be an internal kdtree node

  if (numLeafNodes <= 0){
       *visitcount=0;
       return;
  }

  if (numLeafNodes == 1){
      visitlist[*visitcount]=0;
      (*visitcount)++;
      return;
  }

  intNode = &(internal[currRoot]);

  //query rect. overlaps  with left partition
  if (q->Low(intNode->splitDim) <= intNode->splitPos){
      if (intNode->left >= KDBNODECARD){
	  visitlist[*visitcount]=(intNode->left)-KDBNODECARD;
	  (*visitcount)++;
      }
      else{
	  RangeQuery(q, intNode->left, visitlist, visitcount);
      }
  }


  // query rect overlaps with right partition
  if (q->High(intNode->splitDim) >= intNode->splitPos){
      if (intNode->right >= KDBNODECARD){
	  visitlist[*visitcount]=(intNode->right)-KDBNODECARD;
	  (*visitcount)++;
      }
      else{
	  RangeQuery(q, intNode->right, visitlist, visitcount);
      }
  }

}



// splitting functions for KDBNode


// the following set of functions are to split a KDBnode
void KDBNode::GetkdtreeLeafBRs(int subtreeroot, Rect *indexedRegion, Rect *kdtreeLeafBRList)
{
  Rect leftRegion, rightRegion;
  unsigned short thissplitDim;
  float splitPos; 

  if(numInternalNodes==0){
      kdtreeLeafBRList[0]=(*indexedRegion);
      return;
  }

  if (subtreeroot >= KDBNODECARD){
      kdtreeLeafBRList[subtreeroot-KDBNODECARD]=(*indexedRegion);
  }
  else{ /* internal node */
      thissplitDim=internal[subtreeroot].splitDim;
      splitPos=internal[subtreeroot].splitPos;

      leftRegion=*indexedRegion;
      leftRegion.boundary[NUMDIMS+thissplitDim]=splitPos;
      GetkdtreeLeafBRs(internal[subtreeroot].left, &leftRegion, kdtreeLeafBRList);
      
      rightRegion=*indexedRegion;
      rightRegion.boundary[thissplitDim]=splitPos;
      GetkdtreeLeafBRs(internal[subtreeroot].right, &rightRegion, kdtreeLeafBRList);
  }
}



void KDBNode::GetNumLeavesInKDBSubTree(int index, int *numLeaves, unsigned int *listOfLeaves)
{
  if (index < KDBNODECARD){ /* internal node */
      GetNumLeavesInKDBSubTree(internal[index].left, numLeaves, listOfLeaves);
      GetNumLeavesInKDBSubTree(internal[index].right, numLeaves, listOfLeaves);
  }
  else{ /* leaf node */
      listOfLeaves[(*numLeaves)]=(index-KDBNODECARD);
      (*numLeaves)++;
  }
}



/* return split information i.e. number of cascading splits,
   utilization etc. if internal[index] is used as the split
   hyperplane 
*/

node_split_info KDBNode::GetKDBNodeSplitInfo(int index)
{
  node_split_info newSplitInfo;
  unsigned short splitdim= internal[index].splitDim;

  /* initialize */
  // newSplitInfo.splitNode=index;
  newSplitInfo.splitdim=splitdim;
  newSplitInfo.numCascadingSplits=0;
  newSplitInfo.numLeftChildren=0;
  newSplitInfo.numRightChildren=0;

  if (index == 0){ // root of the kd-tree
      GetNumLeavesInKDBSubTree(internal[index].left, &(newSplitInfo.numLeftChildren), newSplitInfo.listOfLeftChildren);
      GetNumLeavesInKDBSubTree(internal[index].right,&(newSplitInfo.numRightChildren), newSplitInfo.listOfRightChildren);
  }
  else{
  }
  assert((newSplitInfo.numRightChildren+newSplitInfo.numLeftChildren+newSplitInfo.numCascadingSplits)== KDBNODECARD);
  return newSplitInfo;
}



int KDBNode::IsAncestor(unsigned int internalNodeIndex, unsigned int leafNodeIndex)
{
  unsigned int ancestor;
  ancestor=leaf[leafNodeIndex].parent;

  for(;;){
      if (ancestor >= KDBNODECARD) break; // reached the root of the kd-tree
      if (ancestor == internalNodeIndex) return 1; // the internal node is an ancestor of the leaf 
      ancestor=internal[ancestor].parent;
  }
  return 0;
}

struct numleaves KDBNode::GetNumLeavesInSubtree(node_split_info *splitInfo, unsigned int subtreeroot, int option)
{
  struct numleaves leafInfo;
  int leaf, i;
  int newsubtreeroot;

  leafInfo.numLeavesInSubtree=0;
  leafInfo.numLeavesUnderLeftChild=0;  
  

  if (option == LEFT){
      for(i=0; i< splitInfo->numLeftChildren; i++){
	  leaf = splitInfo->listOfLeftChildren[i];
	  if (IsAncestor(subtreeroot, leaf)){
	      (leafInfo.numLeavesInSubtree)++;
	      leafInfo.leafNode=leaf;
	  }
      }

      if (leafInfo.numLeavesInSubtree>1){
	  newsubtreeroot=internal[subtreeroot].left;
	  if (newsubtreeroot >= KDBNODECARD){
	      for(i=0; i< splitInfo->numLeftChildren; i++){
		  if (splitInfo->listOfLeftChildren[i]==(newsubtreeroot-KDBNODECARD)){
		      leafInfo.numLeavesUnderLeftChild=1;
		      break;
		  }
	      }
	  }
	  else{
	      for(i=0; i< splitInfo->numLeftChildren; i++){
		  leaf = splitInfo->listOfLeftChildren[i];
		  if (IsAncestor(newsubtreeroot, leaf))
		      (leafInfo.numLeavesUnderLeftChild)++;
	      }
	  }
      }
  }

  if (option == RIGHT){
      for(i=0; i< splitInfo->numRightChildren; i++){
	  leaf = splitInfo->listOfRightChildren[i];
	  if (IsAncestor(subtreeroot, leaf)){
	      (leafInfo.numLeavesInSubtree)++;
	      leafInfo.leafNode=leaf;
	  }
      }

      if (leafInfo.numLeavesInSubtree>1){
	  newsubtreeroot=internal[subtreeroot].left;
	  if (newsubtreeroot >= KDBNODECARD){
	      for(i=0; i< splitInfo->numRightChildren; i++){
		  if (splitInfo->listOfRightChildren[i]==(newsubtreeroot-KDBNODECARD)){
		      leafInfo.numLeavesUnderLeftChild=1;
		      break;
		  }
	      }
	  }
	  else{
	      for(i=0; i< splitInfo->numRightChildren; i++){
		  leaf = splitInfo->listOfRightChildren[i];
		  if (IsAncestor(newsubtreeroot, leaf))
		      (leafInfo.numLeavesUnderLeftChild)++;
	      }
	  }
      }
  }
  return leafInfo;
}
  


int KDBNode::GetNextKDBkdTreeNode(int option, int currnodeptr, node_split_info *splitInfo)
{
  int ptr = currnodeptr;
  struct numleaves leafInfo;

  assert(ptr < (KDBNODECARD-1));   /* pointing to an internal node */

  if(numInternalNodes==0) return KDBNODECARD;

  for(;;){
      if (ptr >= KDBNODECARD) return ptr;    /* pointing to a leaf node */

      if (option == LEFT){
	  if (internal[ptr].splitDim == splitInfo->splitdim){
	      if (internal[ptr].splitPos >= splitInfo->splitpos){
		  ptr=internal[ptr].left;
		  continue;
	      }
	      if (internal[ptr].splitPos < splitInfo->splitpos)
		return ptr; 
	  }

	  return ptr;
      }

      if (option == RIGHT){
	  if (internal[ptr].splitDim == splitInfo->splitdim) {
	      if (internal[ptr].splitPos <= splitInfo->splitpos) {
		  ptr=internal[ptr].right;
		  continue;
	      }
	      if (internal[ptr].splitPos > splitInfo->splitpos)
		return ptr;
	  }

	  return ptr;
      }
  }

  return -1;
}



void KDBNode::ConstructKDBSubtree(KDBNode *subtreeNode, node_split_info *splitInfo, int option, NodeBase **leaflist)
{
  int i;
  int target, leftchild, rightchild;
  int rootptr;
  int ptr=0;

  rootptr=0;           /* initially points to the root of the kdtree */

  /* get the root of the kdtree being constructed */
  rootptr=GetNextKDBkdTreeNode(option, rootptr, splitInfo);  

  /* pointing to an internal node */
    
  if(rootptr == -1) {
	return; //emptry
  }
  if(rootptr >= KDBNODECARD){
	subtreeNode->numLeafNodes=1;
	subtreeNode->leaf[0] = leaf[rootptr-KDBNODECARD];
	// revise..
	for(i=0;i<splitInfo->numCascadingSplits;i++){
		if((rootptr-KDBNODECARD) == splitInfo->listOfSplittingChildren[i]) {
		    subtreeNode->leaf[0].child = leaflist[rootptr-KDBNODECARD];
		    break;
		}
	}
	subtreeNode->leaf[0].parent = 0;
	return;
  }

  subtreeNode->internal[0]=internal[rootptr];
  subtreeNode->internal[0].parent=KDBNODECARD;
  subtreeNode->numInternalNodes++;
  
  while(ptr < subtreeNode->numInternalNodes){
      leftchild=(subtreeNode->internal[ptr]).left;
      if (leftchild < KDBNODECARD) {
	leftchild=GetNextKDBkdTreeNode(option, leftchild, splitInfo);
      }
      if ( leftchild >= KDBNODECARD) // left child of current node is a leaf node
      {
          assert( (leftchild-KDBNODECARD) < numLeafNodes);
	  target=subtreeNode->numLeafNodes++;
	  subtreeNode->leaf[target] = leaf[leftchild-KDBNODECARD];

	  // revise..
	  for(i=0;i<splitInfo->numCascadingSplits;i++){
		if((leftchild-KDBNODECARD) == splitInfo->listOfSplittingChildren[i]) {
		    subtreeNode->leaf[target].child = leaflist[leftchild-KDBNODECARD];
		    break;
		}
	  }
	
	  subtreeNode->internal[ptr].left=target+KDBNODECARD;
	  subtreeNode->leaf[target].parent = ptr;
//printf("[leaf: %d]\n", leftchild-KDBNODECARD);
      }
      else // left child of current node is an internal node
      {
          assert( leftchild < numInternalNodes);
	  target=subtreeNode->numInternalNodes++;
	  subtreeNode->internal[target] = internal[leftchild];
	  subtreeNode->internal[ptr].left=target;
	  subtreeNode->internal[target].parent = ptr;
      }
      
      rightchild=(subtreeNode->internal[ptr]).right;
      if (rightchild < KDBNODECARD) {
	rightchild=GetNextKDBkdTreeNode(option, rightchild, splitInfo);
      }
      if ( rightchild >= KDBNODECARD) // right child of current node is a leaf node
      {
          assert( (rightchild-KDBNODECARD) < numLeafNodes);
	  target=subtreeNode->numLeafNodes++;
	  subtreeNode->leaf[target] = leaf[rightchild-KDBNODECARD];

	  // revise..
	  for(i=0;i<splitInfo->numCascadingSplits;i++){
		if((rightchild-KDBNODECARD) == splitInfo->listOfSplittingChildren[i]) {
		    subtreeNode->leaf[target].child = leaflist[rightchild-KDBNODECARD];
		    break;
		}
	  }

	  subtreeNode->internal[ptr].right=target+KDBNODECARD;
	  subtreeNode->leaf[target].parent = ptr;
//printf("[leaf: %d]\n", rightchild-KDBNODECARD);
      }
      else // right child of current node is an internal node
      {
	  assert( rightchild < numInternalNodes);
	  target=subtreeNode->numInternalNodes++;
	  subtreeNode->internal[target] = internal[rightchild];
	  subtreeNode->internal[ptr].right=target;
	  subtreeNode->internal[target].parent = ptr;
      }
      ptr ++;
  }
}


void KDBNode::SplitKDBNodekdTree(KDBNode **rightnode, node_split_info *splitInfo, NodeBase **leaf1, NodeBase **leaf2)
{
  int i;
  KDBNode* leftnode;

  leftnode=new KDBNode(tree); 
  (*rightnode)=new KDBNode(tree); 
  leftnode->level=level;
  (*rightnode)->level=level;
    
//printf("LEFT--\n");
  ConstructKDBSubtree(leftnode, splitInfo, LEFT, leaf1);   /* construct left subtree */
//printf("RIGHT--\n");
  ConstructKDBSubtree(*rightnode, splitInfo, RIGHT, leaf2); /* construct right subtree */
  
  CopyFrom(leftnode);

  //for(i=0; i< leftnode->numLeafNodes; i++){
  for(i=0; i< KDBNODECARD; i++){
	leftnode->leaf[i].child = NULL;
  }
  delete leftnode;
}  


void KDBNode::GuaranteedAssessSplits(node_split_info *split_choice)
{
  int i,j;
  int diff=numInternalNodes;
  int left,right;
  struct split_info newSplitInfo[KDBNODECARD];;
  Rect allspace;
  Rect leafBR[KDBNODECARD];

  for(i=0;i<NUMDIMS;i++){
	allspace.boundary[i] = -HUGE_VAL;
	allspace.boundary[i+NUMDIMS] = HUGE_VAL;
  }
//Print(0, &allspace);

  GetkdtreeLeafBRs(0, &allspace, leafBR);

  for(i=0; i< numInternalNodes; i++)
  {

      //GetSplitInfo(0, i, internal[i].splitDim, internal[i].splitPos, &newSplitInfo[i]);
      left=0;right=0;
      for(j=0;j<numLeafNodes;j++){
	if(leafBR[j].boundary[internal[i].splitDim+NUMDIMS] <= internal[i].splitPos){
		left++;
	}
	else if(internal[i].splitPos <= leafBR[j].boundary[internal[i].splitDim]){
		right++;
	} 
      }

      if( abs(left-right) < diff){
	// found middle split
	diff = abs(left-right);
	split_choice->splitdim = internal[i].splitDim;
	split_choice->splitpos = internal[i].splitPos;
	//printf("left = %d\n", left);
	//printf("right = %d\n", right);
      }
  }
//printf("dim: %d\n", split_choice->splitdim);
//printf("pos: %f\n", split_choice->splitpos);

  split_choice->numLeftChildren=0;
  split_choice->numRightChildren=0;
  split_choice->numCascadingSplits=0;
  for(i=0;i<numLeafNodes;i++){
	if(leafBR[i].boundary[split_choice->splitdim+NUMDIMS] <= split_choice->splitpos){
		// left
		split_choice->listOfLeftChildren[split_choice->numLeftChildren++] = i;
	}
	else if(split_choice->splitpos <= leafBR[i].boundary[split_choice->splitdim]){
		// right
		split_choice->listOfRightChildren[split_choice->numRightChildren++] = i;
	} 
	else{
		// cascading splits
		split_choice->listOfSplittingChildren[split_choice->numCascadingSplits++] = i;
	}
  }
//printf("\nLEFT : %d\n", split_choice->numLeftChildren);
//for(i=0;i<split_choice->numLeftChildren;i++)
//  printf("%d ", split_choice->listOfLeftChildren[i]);
//printf("\n");
//printf("RIGHT : %d\n", split_choice->numRightChildren);
//for(i=0;i<split_choice->numRightChildren;i++)
//  printf("%d ", split_choice->listOfRightChildren[i]);
//printf("\n");
//printf("CASCADING : %d\n", split_choice->numCascadingSplits);
//for(i=0;i<split_choice->numCascadingSplits;i++)
//  printf("%d ", split_choice->listOfSplittingChildren[i]);
//printf("\n");

}

void KDBNode::CascadingSplitNode(KDBNode **new_node, int splitdim, float splitpos)
{
  KDBNode *replacementNode=NULL;
  int i;
  int ptr;
  node_split_info split_choice;
  KDBNode *kdbn1[KDBNODECARD];  // left
  KDBNode *kdbn2[KDBNODECARD];  // right
  DataNode *dn1[KDBNODECARD];   // left
  DataNode *dn2[KDBNODECARD];   // right
  NodeBase **n1, **n2;

  Rect allspace;
  Rect leafBR[KDBNODECARD];

  for(i=0;i<NUMDIMS;i++){
	allspace.boundary[i] = -HUGE_VAL;
	allspace.boundary[i+NUMDIMS] = HUGE_VAL;
  }
//Print(0, &allspace);

  GetkdtreeLeafBRs(0, &allspace, leafBR);

  split_choice.splitdim=splitdim;
  split_choice.splitpos=splitpos;
  split_choice.numLeftChildren=0;
  split_choice.numRightChildren=0;
  split_choice.numCascadingSplits=0;
  for(i=0;i<numLeafNodes;i++){
	if(leafBR[i].boundary[splitdim+NUMDIMS] <= splitpos){
		// left
		split_choice.listOfLeftChildren[split_choice.numLeftChildren++] = i;
	}
	else if(splitpos <= leafBR[i].boundary[splitdim]){
		// right
		split_choice.listOfRightChildren[split_choice.numRightChildren++] = i;
	} 
	else{
		// cascading splits
		split_choice.listOfSplittingChildren[split_choice.numCascadingSplits++] = i;
	}
  }

//printf("\n==============\n");
//printf("split_choice.numLeftChildren: = %d\n",split_choice.numLeftChildren);
//printf("split_choice.numRightChildren: = %d\n",split_choice.numRightChildren);
//printf("split_choice.numCascadingSplits: = %d\n",split_choice.numCascadingSplits);

  for(i=0;i<split_choice.numCascadingSplits;i++){
	ptr =  split_choice.listOfSplittingChildren[i];
	if( leaf[ptr].child->type == KDBNODE){
//printf("KDBNODE::CascadingSplit\n");
		kdbn1[ptr] = (KDBNode*) leaf[ptr].child;
		kdbn1[ptr]->Load(kdbn1[ptr]->offset);
		kdbn1[ptr]->CascadingSplitNode(&kdbn2[ptr], splitdim, splitpos);
		n1 = (NodeBase**) kdbn1; n2 = (NodeBase**) kdbn2;
	}
	else if( leaf[ptr].child->type == DATANODE){
//fprintf(stderr, "DATANODE::CascadingSplit\n");
		dn1[ptr] = (DataNode*) leaf[ptr].child;
		dn1[ptr]->Load(dn1[ptr]->offset);
		dn1[ptr]->CascadingSplitNode(&dn2[ptr], splitdim, splitpos);
		n1 = (NodeBase**) dn1; n2 = (NodeBase**) dn2;
	}
  }

  SplitKDBNodekdTree(new_node, &split_choice, (NodeBase**) n1, (NodeBase**) n2);

//printf("numLeafNodes = %d\n", numLeafNodes);
//printf("new_node->numLeafNodes = %d\n", (*new_node)->numLeafNodes);

  Write();
  (*new_node)->Write();

  return;
}

void KDBNode::SplitNode(KDBNode **new_node, struct split **newSplit)
{
  int i;
  int ptr;
  node_split_info split_choice;
  KDBNode *kdbn1[KDBNODECARD];  // left
  KDBNode *kdbn2[KDBNODECARD];  // right
  DataNode *dn1[KDBNODECARD];   // left
  DataNode *dn2[KDBNODECARD];   // right
  NodeBase **n1, **n2;

  assert(numInternalNodes == (KDBNODECARD-1)); // node is full, hence splitting

  GuaranteedAssessSplits(&split_choice);

  (*newSplit) = (struct split *) malloc (sizeof(struct split));
  (*newSplit)->splitDim=split_choice.splitdim;
  (*newSplit)->splitPos=split_choice.splitpos;

//printf("split_choice.numCascadingSplits: = %d\n",split_choice.numCascadingSplits);
  for(i=0;i<split_choice.numCascadingSplits;i++){
	ptr =  split_choice.listOfSplittingChildren[i];
//printf("ptr = %d\n", ptr);
	if( leaf[ptr].child->type == KDBNODE){
//printf("KDBNODE::CascadingSplit\n");
		kdbn1[ptr] = (KDBNode*) leaf[ptr].child;
		kdbn1[ptr]->Load(kdbn1[ptr]->offset);
		kdbn1[ptr]->CascadingSplitNode(&kdbn2[ptr], split_choice.splitdim, split_choice.splitpos);
		n1 = (NodeBase**) kdbn1; n2 = (NodeBase**) kdbn2;
	}
	else if( leaf[ptr].child->type == DATANODE){
//fprintf(stderr, "DATANODE::CascadingSplit\n");
		dn1[ptr] = (DataNode*) leaf[ptr].child;
		dn1[ptr]->Load(dn1[ptr]->offset);
		dn1[ptr]->CascadingSplitNode(&dn2[ptr], split_choice.splitdim, split_choice.splitpos);
		n1 = (NodeBase**) dn1; n2 = (NodeBase**) dn2;
	}
  }


  SplitKDBNodekdTree(new_node, &split_choice, n1, n2);

//printf("--------\n");
//printf("SplitNode: LEFT->numLeafNodes = %d\n", numLeafNodes);
//printf("SplitNode: RIGHT->numLeafNodes = %d\n", (*new_node)->numLeafNodes);

  return;
}

void KDBNode::CreatekdTree(NodeBase *leftNode, NodeBase *rightNode, struct split *newSplit)
{
  //assert(leftDataNode->type == DATANODE);
  //assert(rightDataNode->type == DATANODE);
  numInternalNodes=1;
  numLeafNodes=2;
  internal[0].splitDim=newSplit->splitDim;
  internal[0].splitPos=newSplit->splitPos;
  internal[0].left=KDBNODECARD;
  internal[0].right=KDBNODECARD+1;
  internal[0].parent=KDBNODECARD; // means junk, no parent
  leaf[0].parent=0;
  leaf[0].child=leftNode;
  leaf[1].parent=0;
  leaf[1].child=rightNode;
}




// the following set of functions are to split a KDB node
// the KDB node is split in a KDB manner, this may cause
// cascading splits. however, the cascade is retricted to
// just a one level since there is only one level below the
// level of KDB nodes. we choose the split with least number
// of cascading splits which satisfies the min. utilization


// return the number of leaves below the node pointed by index
int KDBNode::GetNumLeavesInSubTree(int index)
{
  if (index < KDBNODECARD) // internal node 
    return(GetNumLeavesInSubTree(internal[index].left) + GetNumLeavesInSubTree(internal[index].right));
  else return 1;          // leaf node 
}


// internal[stopnode] is the node being used to split, so how many cascading splits, 
// how many left children, how may right children etc.

void KDBNode::GetSplitInfo(unsigned int subtreeroot, unsigned int stopnode, unsigned short splitdim, float splitpos, struct split_info *newSplitInfo)
{
  if ((int)subtreeroot >= KDBNODECARD) newSplitInfo->numCascadingSplits++;
  else{ // internal node 
      if (internal[subtreeroot].splitDim != splitdim){
	  GetSplitInfo(internal[subtreeroot].left, stopnode, splitdim, splitpos, newSplitInfo);
	  GetSplitInfo(internal[subtreeroot].right, stopnode, splitdim, splitpos, newSplitInfo);
      }
      else { // n->rest.ofnode.internal[subtreeroot].splitDim == splitdim 
	  if (internal[subtreeroot].splitPos <= splitpos)
	    newSplitInfo->numLeftChildren += GetNumLeavesInSubTree(internal[subtreeroot].left);
	  else
	    GetSplitInfo(internal[subtreeroot].left, stopnode, splitdim, splitpos, newSplitInfo);
	  
	  if (internal[subtreeroot].splitPos >= splitpos) 
	    newSplitInfo->numRightChildren += GetNumLeavesInSubTree(internal[subtreeroot].right);
	  else
	    GetSplitInfo(internal[subtreeroot].right, stopnode, splitdim, splitpos, newSplitInfo);
      }
  }
}


void KDBNode::GetListOfSplittingDataNodes(unsigned int subtreeroot, unsigned int stopnode, unsigned short splitdim, float splitpos, unsigned int *list, int *listcount)
{
  if ((int)subtreeroot >= KDBNODECARD){
      list[*listcount]=subtreeroot;
      (*listcount)++;
  }
  else { // internal node 
      if (subtreeroot != stopnode){
	  if (internal[subtreeroot].splitDim != splitdim){
	      GetListOfSplittingDataNodes(internal[subtreeroot].left, stopnode, splitdim, splitpos, list, listcount);
	      GetListOfSplittingDataNodes(internal[subtreeroot].right, stopnode, splitdim, splitpos,list, listcount);
	  }
	  else { // n->rest.ofnode.internal[subtreeroot].splitDim == splitdim 
	      if (internal[subtreeroot].splitPos < splitpos){
		  GetListOfSplittingDataNodes(internal[subtreeroot].right, stopnode, splitdim, splitpos,list, listcount);
	      }
	      if (internal[subtreeroot].splitPos > splitpos){
		  GetListOfSplittingDataNodes(internal[subtreeroot].left, stopnode, splitdim, splitpos, list, listcount);
	      }
	  }
      }
  }
}

int KDBNode::GetNextNode(int splitnode, int option, int currnodeptr)
{
  int ptr = currnodeptr;
  assert(ptr < (KDBNODECARD-1));   // pointing to an internal node 

  for(;;){
      if (ptr >= KDBNODECARD) return ptr;    // pointing to a leaf node 
      if (internal[ptr].splitDim == internal[splitnode].splitDim){
	  if (ptr == splitnode){
	      if (option == LEFT) ptr=internal[ptr].left;
	      else ptr=internal[ptr].right;
	  }
	  else {
	      if (option == LEFT && internal[ptr].splitPos >= internal[splitnode].splitPos)
		ptr=internal[ptr].left;
	      else if (option == RIGHT && internal[ptr].splitPos <= internal[splitnode].splitPos)
		ptr=internal[ptr].right;
	      else return ptr;
	  }
      }
      else return ptr;
  }
}


void KDBNode::CopyFrom(KDBNode *n) 
{
  int i;
  numInternalNodes = n->numInternalNodes;
  numLeafNodes = n->numLeafNodes;

  for (i = 0; i < KDBNODECARD-1; i++)
	internal[i]=n->internal[i];
  for (i = 0; i < KDBNODECARD; i++)
	leaf[i]=n->leaf[i];
}

