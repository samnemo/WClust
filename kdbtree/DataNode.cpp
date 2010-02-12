#include "stdafx.h"
// FILENAME: DataNode.C
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
//#include <strings.h>
#include <string.h>
#include <float.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
//#include <sys/fcntl.h>

#include "DataNode.h"

extern int leftboundcmp(struct lineseg *a, struct lineseg *b);
extern int rightboundcmp(struct lineseg *a, struct lineseg *b);
extern int IsPresent(unsigned int aLeaf, unsigned int *leafList, int leafCount);

#ifdef IO_STATS
extern int page_read, page_write;
extern int insert_mode;
#endif

int DATANODECARD;
int DATASIZE;

extern int NUMDIMS;

void bzero(void* p,int isz)
{
	memset(p,0,isz);
}

Branch::Branch()
{
  child = new char[DATASIZE];

//  memset(child,0,sizeof(char)*DATASIZE);
  bzero(child,DATASIZE);
}

Branch::Branch(const Branch& that)
{
  child = new char[DATASIZE];
  bzero(child,DATASIZE);

  dataPtr = that.dataPtr;
  memcpy(child, that.child, DATASIZE);
}

Branch& Branch::operator=(const Branch& that)
{
  if(this==&that) return *this;

  dataPtr = that.dataPtr;
  memcpy(child, that.child, DATASIZE);

  return *this;
}

Branch::~Branch()
{
  delete []child;
}

void DataNode::Write()
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
  memcpy((char*) Buffer+ptr, (char*) &count, sizeof(short));
  ptr += sizeof(short);

  for(i=0; i< count; i++){
        memcpy((char*) Buffer+ptr, (char*) branch[i].dataPtr.pt, sizeof(RectReal)*NUMDIMS);
	ptr += sizeof(RectReal)*NUMDIMS;

	//if(branch[i].child!=NULL){
	memcpy((char*) Buffer+ptr, (char*) branch[i].child, DATASIZE);
	//}
	ptr += DATASIZE;
  }
  assert(ptr <= PGSIZE);

  // dump Buffer to disk

  fwrite(Buffer, PGSIZE, 1, tree);


}


void DataNode::Load(off_t off)
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

  if(offset%PGSIZE!=0) printf("read wrong.........\n");


  fseek(tree, offset, SEEK_SET);
  fread(Buffer, PGSIZE, 1, tree);


  memcpy((char*) &level, (char*) Buffer+ptr, sizeof(short));
  ptr += sizeof(short);
  memcpy((char*) &count, (char*) Buffer+ptr, sizeof(short));
  ptr += sizeof(short);

  for(i=0; i< count; i++){
        memcpy((char*) branch[i].dataPtr.pt, (char*) Buffer+ptr, sizeof(RectReal)*NUMDIMS);
	ptr += sizeof(RectReal)*NUMDIMS;

	memcpy((char*) branch[i].child, (char*) Buffer+ptr, DATASIZE);
	ptr += DATASIZE;
  }
  assert(ptr <= PGSIZE);

}



DataNode::DataNode(FILE* _tree): NodeBase(NodeBase::DATANODE) 
{
  int i;

  branch = new Branch[DATANODECARD];
  tree = _tree;
  offset = -1;
  type = NodeBase::DATANODE;
  level = -1;
  inmemory = 0;

  count = 0;
  for (i = 0; i < DATANODECARD; i++){
      branch[i].dataPtr.Init();
  }
}


DataNode::~DataNode(){
  delete []branch;
}

DataNode& DataNode::operator=(const DataNode &that)
{
  int i;

  if(this == &that) return *this;

  tree = that.tree;
  offset = that.offset;
  type = that.type;
  level = that.level;
  inmemory = that.inmemory;
  count = that.count;

  //memcpy(branch, that.branch, sizeof(Branch)*DATANODECARD);
  for(i=0;i<that.count;i++){
	branch[i] = that.branch[i];
  }

  return *this;
}


int DataNode::Count()
{
  return count;
}

void DataNode::Count(int newcount)
{
  count=newcount;
}

void DataNode::IncrCount()
{
  count++;
}

Branch DataNode::getBranch(int i)
{
  assert(i<count);
  return branch[i];
}

void DataNode::putBranch(int i, Branch b)
{
  branch[i]=b;
}

void DataNode::DisconnectBranch(int i)
{
  int j;
  assert(i>=0 && i<DATANODECARD);

  // do compaction
  for(j=0;j<count-i-1;j++){
	branch[i+j] = branch[i+1+j];
  }
  count--;

  branch[count].dataPtr.Init();
}


void DataNode::Print()
{  
  int i;
  printf("NODE %ld TYPE: %d  LEVEL=%d  COUNT=%d  \n", (long) offset, type, level, count);
  for (i=0; i<count; i++){
      printf("branch %d\n", i);
      printf("%d ", (int)(branch[i].child));
      (branch[i].dataPtr).Print();
  }
}

Rect* DataNode::Cover()
{
  register int i, first_time=1;
  Rect *r;
  Rect *new_r;
 	 
  r = new Rect();

  for (i = 0; i < count; i++){
	if (first_time){
	    *r=branch[i].dataPtr;
	    first_time = 0;
	}
	else {
	    new_r = r->CombineRect(&(branch[i].dataPtr));
	    delete r;
	    r = new_r;
	}
  }
  return r;
}

int DataNode::AddChild(Branch *b, DataNode** new_node, struct split** newsplit)
{
  assert(b);

  branch[count]=*b;
  count++;

  if (count >= DATANODECARD){ /* split it */
	assert(new_node);
	SplitNode(new_node, newsplit);

	Write();
	if(*new_node != NULL)
	    (*new_node)->Write();

	return 1;
  }

  Write();
  
  return 0;
}


int floatcmp(float *a, float *b)
{
  if (*a < *b) return -1;
  else if (*a > *b) return 1;
  else return 0;
}

int splitcmp(struct dimspread *a, struct dimspread *b)
{
  if (a->spread > b->spread) return -1;
  else if (a->spread < b->spread) return 1;
  else return 0;
}


void DataNode::CascadingSplitNode(DataNode** nn, int split_dim, float split_pos)
{
  DataNode *replacementNode;
  int target;
  int i;
  int best;

  // allocate replacement node and newnode
  replacementNode= new DataNode(tree);
  replacementNode->level=level;
  (*nn)=new DataNode(tree);
  (*nn)->level=level;

  for(i=0;i<count;i++){
	if(branch[i].dataPtr.pt[split_dim] < split_pos){
		//left..
		target = replacementNode->Count();
	        replacementNode->putBranch(target, branch[i]);
		replacementNode->IncrCount();
	}
	else{
		//right..
		target = (*nn)->Count();
	        (*nn)->putBranch(target, branch[i]);
		(*nn)->IncrCount();
	}
  }

  //assert(replacementNode->Count() > 0);
  //assert((*nn)->Count() > 0);

  CopyFrom(replacementNode);
  delete replacementNode;

  Write();
  (*nn)->Write();
}

void DataNode::SplitNode(DataNode** nn, struct split** newSplit)
{
  DataNode *replacementNode;
  int target;
  int i;
  int best;
  int minfill;
  int split_dim;
  float split_pos;
  float* f = new float[DATANODECARD];

  assert(count == DATANODECARD);

  // allocate replacement node and newnode
  replacementNode= new DataNode(tree);
  replacementNode->level=level;
  (*nn)=new DataNode(tree);
  (*nn)->level=level;

  split_dim = rand()%NUMDIMS; // split_dim.. randomly chosen

  for(i=0;i<count;i++){
	f[i] = branch[i].dataPtr.pt[split_dim];
  }

  qsort(f, count, sizeof(float), (int (*) (const void *, const void *)) floatcmp);
  if(f[0] == f[count-1]){
	for(i=0;i<count;i++){
		f[i] = branch[i].dataPtr.pt[split_dim];
	}
	qsort(f, count, sizeof(float), (int (*) (const void *, const void *)) floatcmp);
  }
  split_pos = f[count/2];

  for(i=0;i<count;i++){
	if(branch[i].dataPtr.pt[split_dim] < split_pos){
		//left..
		target = replacementNode->Count();
	        replacementNode->putBranch(target, branch[i]);
		replacementNode->IncrCount();
	}
	else if(branch[i].dataPtr.pt[split_dim] > split_pos){
		//right..
		target = (*nn)->Count();
	        (*nn)->putBranch(target, branch[i]);
		(*nn)->IncrCount();
	}
	else{
		// insert into the sub partition that has less data
		if( replacementNode->Count() > (*nn)->Count()){
			target = (*nn)->Count();
	        	(*nn)->putBranch(target, branch[i]);
			(*nn)->IncrCount();
		}
		else{
			target = replacementNode->Count();
	        	replacementNode->putBranch(target, branch[i]);
			replacementNode->IncrCount();
		}
	}
  }


  (*newSplit) = (struct split *) malloc (sizeof(struct split));
  (*newSplit)->splitDim=split_dim;
  (*newSplit)->splitPos=split_pos;

  assert(replacementNode->Count() > 0);
  assert((*nn)->Count() > 0);

  CopyFrom(replacementNode);
  delete replacementNode;
  
  delete [] f;

}

void DataNode::CopyFrom(DataNode *n)
{
  int i;
  /*
  count = n->count;
  //for (i = 0; i < DATANODECARD; i++){
  for (i = 0; i < count; i++){
	branch[i].dataPtr = n->branch[i].dataPtr;
	memcpy(branch[i].child, n->branch[i].child, DATASIZE);
  } */

  tree = n->tree;
  offset = n->offset;
  type = n->type;
  level = n->level;
  inmemory = n->inmemory;
  count = n->count;
  
  //memcpy(branch, that.branch, sizeof(Branch)*DATANODECARD);
  //for(i=0;i<n->count;i++){
  for(i=0;i<DATANODECARD;i++){
        branch[i] = n->branch[i];
  }

}


