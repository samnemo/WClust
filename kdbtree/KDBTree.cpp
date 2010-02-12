#include "stdafx.h"
// FILENAME: KDBTree.C
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#include <cstdio>
//#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <malloc.h>
#include <cmath>
#include <cassert>
#include <sys/types.h>
//#include <sys/fcntl.h>
#include <sys/stat.h>

#include "KDBTree.h"

extern int DATANODECARD; 
extern int DATASIZE;

#ifdef IO_STATS
int insert_mode;
int page_acc_count;
int page_read, page_write;
#endif

//const int PGSIZE = 4096;
#define PGSIZE 4096
int NUMDIMS;

float MINNODEUTIL;


// ------------------------------------------------------------------
// Function: KDBTree::KDBTree
//
// Purpose:  Default constructor  
//
// Note: 
//
// ------------------------------------------------------------------
//
KDBTree::KDBTree(){
  KDBTreeRoot = NULL;
  height=1;
  firstitem=0;
  count=0;
}

// ------------------------------------------------------------------
// Function: KDBTree::~KDBTree
//
// Purpose:  Default destructor  
//
// Note: 
//
// ------------------------------------------------------------------
//
KDBTree::~KDBTree(){
  if(KDBTreeRoot != NULL){
	if(height==1) delete (DataNode*) KDBTreeRoot;
	else delete (KDBNode*) KDBTreeRoot;
  }
}

// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeCreate
//
// Purpose:  To create an KDBTree 
//
// Parameter: dimension : # of dimension
//            pageSize  : the size of a disk page, ex) 4096, 8192
//            dataSize  : the size of a data block in leaf nodes
//
// ------------------------------------------------------------------
//
void KDBTree::KDBTreeCreate(const char *path, int dimension, int dataSize, 
			int pageSize)
{
  char *Buffer = new char[pageSize];
  float nodeUtil=0.0;

  root_offset = 0;
  
  tree = fopen(path, "wb+");
  if(tree == NULL){
	printf("KDBTreeCreate failed\n");
	return;
  }

#ifdef DIRECT_IO
  directio(fileno(tree), DIRECTIO_ON);
#endif

  NUMDIMS = dimension;
//  PGSIZE = pageSize;
  DATASIZE = dataSize;
  DATANODECARD = (PGSIZE-(3*sizeof(short))) / (NUMDIMS*sizeof(RectReal) + DATASIZE);
  MINNODEUTIL = nodeUtil;
  rootRect = new Rect();

#ifdef IO_STATS
printf("KDBNODECARD == %d\n", KDBNODECARD);
printf("DATANODECARD == %d\n", DATANODECARD);
#endif

  fwrite(Buffer, PGSIZE, 1, tree);
  fseek(tree, 0, SEEK_SET);

  fwrite(&dimension, sizeof(int), 1, tree);
  fwrite(&dataSize, sizeof(int), 1, tree);
  fwrite(&pageSize, sizeof(int), 1, tree);
  fwrite(&nodeUtil, sizeof(float), 1, tree);

  fwrite(&root_offset, sizeof(off_t), 1, tree);
  fwrite(rootRect->boundary, sizeof(RectReal)*2*NUMDIMS, 1, tree);
  fwrite(&height, sizeof(int), 1, tree);

  // the first node is the root as well as leaf (i.e. a data node)
  NodeBase *root = new DataNode(tree) ; 
  root->level=0;
  KDBTreeRoot = root;
  height=1;
  firstitem=1;
  count=0;

  delete[] Buffer;
}

// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeCreateRect
//
// Purpose:  To create an KDBTree 
//
// Parameter: dimension : # of dimension
//            pageSize  : the size of a disk page, ex) 4096, 8192
//            dataSize  : the size of a data block in leaf nodes
//
// ------------------------------------------------------------------
//
void KDBTree::KDBTreeCreateRect(const char *path, int dimension, int dataSize, 
			int pageSize)
{
  char *Buffer = new char[pageSize];
  float nodeUtil=0.0;

  root_offset = 0;
  
  tree = fopen(path, "wb+");
  if(tree == NULL){
	printf("KDBTreeCreate failed\n");
	return;
  }

#ifdef DIRECT_IO
  directio(fileno(tree), DIRECTIO_ON);
#endif

  dimension *=2;
  NUMDIMS = dimension;
//  PGSIZE = pageSize;
  DATASIZE = dataSize;
  DATANODECARD = (PGSIZE-(3*sizeof(short))) / (NUMDIMS*sizeof(RectReal) + DATASIZE);
  MINNODEUTIL = nodeUtil;
  rootRect = new Rect();

#ifdef IO_STATS
printf("KDBNODECARD == %d\n", KDBNODECARD);
printf("DATANODECARD == %d\n", DATANODECARD);
#endif


  fwrite(Buffer, PGSIZE, 1, tree);
  fseek(tree, 0, SEEK_SET);

  fwrite(&dimension, sizeof(int), 1, tree);
  fwrite(&dataSize, sizeof(int), 1, tree);
  fwrite(&pageSize, sizeof(int), 1, tree);
  fwrite(&nodeUtil, sizeof(float), 1, tree);

  fwrite(&root_offset, sizeof(off_t), 1, tree);
  fwrite(rootRect->boundary, sizeof(RectReal)*2*NUMDIMS, 1, tree);
  fwrite(&height, sizeof(int), 1, tree);

  // the first node is the root as well as leaf (i.e. a data node)
  NodeBase *root = new DataNode(tree) ; 
  root->level=0;
  KDBTreeRoot = root;
  height=1;
  firstitem=1;
  count=0;

  delete[] Buffer;
}

// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeOpen
//
// Purpose:  To open an KDBTree 
//
// Note: 
//
// ------------------------------------------------------------------
//
void KDBTree::KDBTreeOpen(const char *path)
{
  NodeBase *root;

  int dimension;
  int dataSize;
  int pageSize;
  float nodeUtil;

  tree= fopen(path, "r+");
  if(tree == NULL){
	printf("KDBTreeOpen failed\n");
	return;
  }


#ifdef DIRECT_IO
  directio(fileno(tree), DIRECTIO_ON);
#endif

  fseek(tree, 0, SEEK_SET);

  fread(&dimension, sizeof(int), 1, tree);
  fread(&dataSize, sizeof(int), 1, tree);
  fread(&pageSize, sizeof(int), 1, tree);
  fread(&nodeUtil, sizeof(float), 1, tree);


  NUMDIMS = dimension;
//  PGSIZE = pageSize;
  DATASIZE = dataSize;
  DATANODECARD = (PGSIZE-(3*sizeof(short))) / (NUMDIMS*sizeof(RectReal) + DATASIZE);
  MINNODEUTIL = nodeUtil;

  rootRect = new Rect();

  fread(&root_offset, sizeof(off_t), 1, tree);
  fread(rootRect->boundary, sizeof(RectReal)*2*NUMDIMS, 1, tree);
  fread(&height, sizeof(int), 1, tree);


  if (height == 1) {
        root = new DataNode(tree);
  }
  else {
        root = new KDBNode(tree);
  }


  // the first node is root as well as leaf (i.e. a data node)
  root->level=0;

  if (height == 1) {
	if(root_offset!=-1)
	((DataNode*)root)->Load(root_offset);
  }
  else{
	((KDBNode*)root)->Load(root_offset);
  }

  KDBTreeRoot = root;
}

// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeGetRoot
//
// Purpose:  To get a root bounding box of an KDBTree 
//
// Parameter: float lb[], hb[] : root bounding box 
//
// ------------------------------------------------------------------
//
int KDBTree::KDBTreeGetRoot(float lb[], float hb[])
{
    int i;

    if(lb==NULL || hb==NULL) return 0;
    
    if (firstitem){
	return 0;
    }
    else {
	for(i=0;i<NUMDIMS;i++){
		lb[i] = rootRect->Low(i);
		hb[i] = rootRect->High(i);
	}
    }

    return 1;
}


// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeStore
//
// Purpose:  To insert a point data into an KDBTree 
//
// Parameter: bbx[] : the bounding box (or point) information of data
//            *data : a pointer to a data block
//
// ------------------------------------------------------------------
//
int KDBTree::KDBTreeStore(float pt[], char* data)
{
   int i, ret;
   Point p;

   for(i=0;i<NUMDIMS;i++){
	p.pt[i] = pt[i];
   }

    if (firstitem){
	*rootRect=p;
	firstitem=0;
	ret = 1;
    }
    else {
	ret = 1 - rootRect->Contain(&p);

	Rect *temp = rootRect->CombineRect(&p);
	*rootRect = *temp;
	delete temp;
    }

#ifdef IO_STATS
    insert_mode = 1;
#endif
    Insert(&p, data, &KDBTreeRoot, 0);

    return ret;
}

// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeStoreRect
//
// Purpose:  To insert a rectangular data into an KDBTree 
//
// Parameter: bbx[] : the bounding box (or point) information of data
//            *data : a pointer to a data block
//
// ------------------------------------------------------------------
//
int KDBTree::KDBTreeStoreRect(float lb[], float hb[], char* data)
{
   int i, ret;
   Point p;

   for(i=0;i<NUMDIMS/2;i++){
	p.pt[2*i] = lb[i];
	p.pt[2*i+1] = hb[i];
   }

    if (firstitem){
	*rootRect=p;
	firstitem=0;
	ret = 1;
    }
    else {
	ret = 1 - rootRect->Contain(&p);

	Rect *temp = rootRect->CombineRect(&p);
	*rootRect = *temp;
	delete temp;
    }

#ifdef IO_STATS
    insert_mode = 1;
#endif
    Insert(&p, data, &KDBTreeRoot, 0);

    return ret;
}


// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeDelete
//
// Purpose:  To delete a data into an KDBTree 
//
// Parameter: bbx[] : the bounding box (or point) information of data
//            *data : a pointer to a data block
//
// ------------------------------------------------------------------
//
int KDBTree::KDBTreeDelete(float pt[])
{
   int i;
   Rect r;

   for(i=0;i<NUMDIMS;i++){
	r.Low(i, pt[i]);
	r.High(i, pt[i]);
   }

   // REVISE: adjust bbx here !!!!!?????????

   return Remove(KDBTreeRoot, &r);
}


// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeRangeQuery
//
// Purpose:  To search data in an KDBTree 
//
// Parameter: ranges[] : the bounding box of query range
//            *res : a pointer to the query result 
//
// ------------------------------------------------------------------
//
int KDBTree::KDBTreeRangeQuery(float lb[], float hb[], KDBTreeResult* res)
{
  int i; 
  Rect dataRange;

  //res->queue = WBQueueOpen(NULL,0,0,0);

  for(i=0;i<NUMDIMS;i++){
	dataRange.Low(i, lb[i]);
	dataRange.High(i, hb[i]);
  }

  res->numObjects=0;

#ifdef IO_STATS
  // this line is stupid but necessary for fair comparisons
  ((KDBNode*) KDBTreeRoot)->Load(KDBTreeRoot->getOffset());
  insert_mode = 0;
#endif

  Search(KDBTreeRoot, &dataRange, res);         // bounding box based

  return res->numObjects;
}

// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeRangeQueryRect
//
// Purpose:  To search rectangular data in an KDBTree 
//
// Parameter: ranges[] : the bounding box of query range
//            *res : a pointer to the query result 
//
// ------------------------------------------------------------------
//
int KDBTree::KDBTreeRangeQueryRect(float lb[], float hb[], KDBTreeResult* res)
{
  int i; 
  Rect dataRange;

  for(i=0;i<NUMDIMS/2;i++){
	dataRange.Low(2*i, -HUGE_VAL);
	dataRange.High(2*i, hb[i]);

	dataRange.Low(2*i+1, lb[i]);
	dataRange.High(2*i+1, HUGE_VAL);
  }
//dataRange.Print();

  res->numObjects=0;

#ifdef IO_STATS
  // this line is stupid but necessary for fair comparisons
  ((KDBNode*) KDBTreeRoot)->Load(KDBTreeRoot->getOffset());
  insert_mode = 0;
#endif

  Search(KDBTreeRoot, &dataRange, res);         // bounding box based

  return res->numObjects;
}


// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeGetNextResult
//
// Purpose:  To get next query result 
//
// Note:
//
// ------------------------------------------------------------------
//
int KDBTree::KDBTreeGetNextResult(KDBTreeResult *res, KDBTreeRecord* record)
{
  int i;

  KDBTreeRecord *next = (KDBTreeRecord*) WBQueuePop(res->queue, NULL);

  if(next == NULL) return 0;

  record->pt = next->pt;
  memcpy(record->data, next->data, DATASIZE);

  delete next;

  return 1;
}


// ------------------------------------------------------------------
// Function: KDBTree::KDBTreeClose
//
// Purpose:  To close an KDBTree 
//
// Note:
//
// ------------------------------------------------------------------
//
void KDBTree::KDBTreeClose()
{
  root_offset = KDBTreeRoot->getOffset();

  fseek(tree, 3*sizeof(int) + sizeof(float), SEEK_SET);

  fwrite(&root_offset, sizeof(off_t), 1, tree);
  fwrite(rootRect->boundary, sizeof(RectReal)*2*NUMDIMS, 1, tree);
  fwrite(&height, sizeof(int), 1, tree);

  fclose(tree);

  if(height==1) delete (DataNode*) KDBTreeRoot;
  else delete (KDBNode*) KDBTreeRoot;
  KDBTreeRoot = NULL;

  delete rootRect;
}


// ------------------------------------------------------------------
// Function: KDBTree::Print
//
// Purpose:  To print out an KDBTree 
//
// Note: 
//
// ------------------------------------------------------------------
//
void KDBTree::Print()
{
    Print(KDBTreeRoot, rootRect);
    if(height == 1)
	((DataNode*) KDBTreeRoot)->Print();
    else
	((KDBNode*) KDBTreeRoot)->Print(0, rootRect);
}


// ------------------------------------------------------------------
// Function: KDBTree::Print
//
// Purpose:  To print out an KDBTree 
//
// Note: 
//
// ------------------------------------------------------------------
//
int KDBTree::Print(NodeBase *n, Rect *parentRect)
{
    int i;
    Rect childRectlist[KDBNODECARD];
    KDBNode *N;

    if(n->level==0) return 1;
    if (n->type == NodeBase::KDBNODE){  // minimum overlap node
	N = (KDBNode*) n;
	N->FillChildList(0, childRectlist,  parentRect);

	printf("KDB level %d\n", N->level);
	printf("numLeafNodes = %d\n", N->numLeafNodes);
	for(i=0; i<N->numLeafNodes; i++){
		printf("%d: ", i);
		childRectlist[i].Print();
		printf("splitDim = %d\n",N->internal[N->leaf[i].parent].splitDim);
		printf("splitPos1 = %f\n",N->internal[N->leaf[i].parent].splitPos);
	}

/*
	for(i=0; i<N->numLeafNodes; i++)
	{
		KDBNode   *kn;

		if(!childRectlist[i].Contained(parentRect)){
		    printf("KDB level %d parentRect:\n", N->level);
		    parentRect->Print();
		    printf("childRect not contained:\n");
		    childRectlist[i].Print();
		}

		switch(N->leaf[i].child->type){
	            case NodeBase::KDBNODE:
	                kn = (KDBNode*) N->leaf[i].child;
	                kn->Load(kn->offset);
	            	Print(kn, &childRectlist[i]);
	                break;
	            case NodeBase::DATANODE:
			break;
	        }
	}
*/
    }

    return 1;
}

// ------------------------------------------------------------------
// Function: KDBTree::Insert2
//
// Purpose : Recursive sub-function that inserts a new data rectangle 
//           into an index structure.
//
// Return  : 0 if node was not split, 
//           1 if node was split.
//
// Note    : If node was split, returns 1 and sets the rect pointed to 
//           by new_node to rect to the new node.  Old node updated to 
//           become one of two.
//
// ------------------------------------------------------------------
//
int KDBTree::Insert2(Point *p, char *data, NodeBase *n, NodeBase **new_node, 
			struct split **newsplit, int level, Rect *parentNodeRect)
{
    int i;
    Branch b;
    NodeBase *n2 = NULL;
    struct split *lastsplit;
    Rect childNodeRect;
    Rect childNodeRectR;
    Rect backup;
    Rect coverRect;
    int ret_val;

    assert(n);
    assert(new_node);
    assert(level >= 0 && level <= n->level);

    //
    // Still above level for insertion, go down tree recursively
    //
    if (n->level > level){
	if (n->type == NodeBase::KDBNODE){  // minimum overlap node
	      KDBNode *N=(KDBNode *)n;

	      i = N->PickChild(p, parentNodeRect, &childNodeRect);
	      assert(i>=0 && i < N->numLeafNodes);
	      assert(N->leaf[i].child);
	      //bsnam assert
	      assert(childNodeRect.Contained(parentNodeRect));

	      // Read from disk
	      if(N->leaf[i].child->type == NodeBase::KDBNODE){
		((KDBNode*)N->leaf[i].child)->Load((N->leaf[i].child)->offset);
	      }
	      if(N->leaf[i].child->type == NodeBase::DATANODE){
		((DataNode*)N->leaf[i].child)->Load((N->leaf[i].child)->offset);
	      }

	      if (!Insert2(p, data, N->leaf[i].child, &n2, &lastsplit, level, 
				&childNodeRect)){
		  // node was not split
		  // nothing needs to be done
		  return 0;
	      }
	      else{   // node was split and n2 is the new node
		  ret_val=N->AddChild(n2, (KDBNode **) new_node, i, lastsplit, newsplit);

		  if (*new_node != NULL) 
			assert((((KDBNode *)*new_node)->numLeafNodes < KDBNODECARD) &&  
				(((KDBNode *)*new_node)->numLeafNodes > 0));

                  free(lastsplit);
                  return ret_val;
	      }
	}
    }
    else if (n->level == level){
	// Have reached level for insertion. Add rect, split if necessary
	//
	DataNode *N=(DataNode *)n;
	b.dataPtr = *p;
	memcpy(b.child, (char *) data, DATASIZE);

	/* child field of leaves contains tid of data record */
	// printf("adding data item to data node \n");
	return N->AddChild(&b, (DataNode **) new_node, newsplit);

    }

    /* Not supposed to happen */
    assert (FALSE);
    return 0;

}


// ------------------------------------------------------------------
// Function: KDBTree::Insert
//
// Purpose:  To insert a new data rectangle into the index structure.
//
// Return: 0 if node was not split, 1 if node was split.
//
// Note: If node was split, returns 1 and sets the rect pointed to by
//       new_node to rect to the new node.  Old node updated to become 
//       one of two.
//
// ------------------------------------------------------------------
//
int KDBTree::Insert(Point *p, char *data, NodeBase **Root, int Level)
{
    NodeBase **root = Root;
    int level = Level;
    NodeBase *newnode = NULL;
    Branch b;
    struct split *newSplit;

    assert(p && root);
    assert(level >= 0 && level <= (*root)->level);

    assert(*root);

    if (Insert2(p, data, *root, &newnode, &newSplit, level, rootRect)){ /* root split */
	if ((*root)->level == 0){  
	// current root is a data node but new root would be a KDBNode
		assert((*root)->type == (NodeBase::DATANODE));
		KDBNode *newroot = new KDBNode(tree);  /*grow a new root, & tree taller*/

		// write a new root information
		newroot->level = (*root)->level + 1;
		newroot->CreatekdTree(*root /*left*/ , newnode /*right*/ , newSplit);

		newroot->Write();
		root_offset = newroot->getOffset();

		*root = (NodeBase*) newroot;
		height++;
	}
	else{  // current root is ordinary node, new root is also ordinary root
		// identical to R_tree case
		assert((*root)->type == NodeBase::KDBNODE);
		KDBNode *newroot= new KDBNode(tree); /* grow a new root, & tree taller */

		// write a new root information
		newroot->level = (*root)->level + 1;
		newroot->CreatekdTree(*root, newnode, newSplit);
		// from now..
		// newroot->leaf[0].child == *root;
		// newroot->leaf[1].child == newnode;
//fprintf(stderr,"newroot->leaf[0].child = %x\n", newroot->leaf[0].child);
//fprintf(stderr,"newroot->leaf[1].child = %x\n", newroot->leaf[1].child);

		newroot->Write();
		root_offset = newroot->getOffset();


		*root = (NodeBase*) newroot;
		height++;
	}


	free(newSplit);

	return 1;
    }
    else
	return 0;

}


// ------------------------------------------------------------------
// Function: KDBTree::IsPageFault
//
// Purpose:  To simulate buffer with lru replacement policy
//
// Return: 0 if page is in memory, 1 if page fault.
//
// Note: 
//
// ------------------------------------------------------------------
//
int KDBTree::IsPageFault(NodeBase *node)
{
  // check whether the page is there in the buffer
  NODELIST::iterator ptr;
  // we are using the memory addresses of the nodes to identify them, as their id
  // this will not work when the nodes are disk-based; but then we won't need a buffer simulator

  if (NUM_PAGES_IN_MEMORY <= 0) return 1;
  assert(pages_in_memory.size() <= NUM_PAGES_IN_MEMORY);
  ptr = find(pages_in_memory.begin(), pages_in_memory.end(), (long)node);
  if (ptr != pages_in_memory.end())  // page is there in memory
  {
      pages_in_memory.erase(ptr);  // remove the page
      pages_in_memory.push_back((long)node); // insert it at the end
      return 0;  // not a page fault
  }
  else   // page is not there in memory
  {
      if (pages_in_memory.size() < NUM_PAGES_IN_MEMORY)  // can just bring it in memory, no one needs to be thrown out
      {
          // nanosleep(&io_delay, NULL);   // do the disk access
          pages_in_memory.push_back((long)node); // bring in the page
      }
      else   // some one has to be thrown out, so the oldest guy is chosen (first guy in list)
      {
          pages_in_memory.pop_front();  // throw the oldest guy out
          // nanosleep(&io_delay, NULL);   // do the disk access
          pages_in_memory.push_back((long)node); // bring in the page
      }

      return 1;
  }
}

// ------------------------------------------------------------------
// Function: KDBTree::ClearBuffer
//
// Purpose:  To clear buffer 
//
// Note: 
//
// ------------------------------------------------------------------
//
void KDBTree::ClearBuffer()
{
  while(!(pages_in_memory.empty())) pages_in_memory.pop_front();
}


// ------------------------------------------------------------------
// Internal function : bounding box queries
// return hitCount
// ------------------------------------------------------------------
int KDBTree::Search(NodeBase *n, Rect *r, KDBTreeResult *res)
{
  int hitCount = 0;
  int i, j;
  int visitlist[KDBNODECARD];
  int visitcount=0;

  assert(n);
  assert(n->level >= 0);
  assert(r);

#ifdef IO_STATS
  page_acc_count++;
#endif

  if (n->level > 0){  // this is an internal node in the tree 
        assert(n->type== NodeBase::KDBNODE);
	KDBNode *KN=(KDBNode *)n;

        KN->RangeQuery(r, 0, visitlist, &visitcount);
        for(i=0; i<visitcount; i++)
	{
	    DataNode *dn;
	    KDBNode   *kn;
	    switch((KN->leaf[visitlist[i]]).child->type){
		case NodeBase::DATANODE: 
			dn = (DataNode*) KN->leaf[visitlist[i]].child;
			dn->Load(dn->offset);
			break;
		case NodeBase::KDBNODE: 
			kn = (KDBNode*) KN->leaf[visitlist[i]].child;
			kn->Load(kn->offset);
			break;
	    }

	    hitCount += Search((KN->leaf[visitlist[i]]).child, r, res);
	}
  }
  else // this is a leaf node 
  {
	DataNode *DN=(DataNode *)n;

	for (i=0; i<DN->count; i++){
		assert(DN->branch[i].child);
		if (r->Overlap(&(DN->branch[i].dataPtr))){
			//printf(" overlap\n");
			//DN->branch[i].dataRect.Print();

			KDBTreeRecord *record = new KDBTreeRecord();
			record->pt = new float[NUMDIMS];
			for(j=0;j<NUMDIMS;j++){
				record->pt[j] = DN->branch[i].dataPtr.pt[j];
			}
			record->data = new char[DATASIZE];
			memcpy(record->data, DN->branch[i].child, DATASIZE);

			WBQueuePush(res->queue, record);
			res->numObjects++;

			hitCount++;
		}
	}
  }
  return hitCount;
}


// ------------------------------------------------------------------
// Internal function : remove data 
// ------------------------------------------------------------------
int KDBTree::Remove(NodeBase *n, Rect *r)
{
  int ret = 0;
  int i;
  int visitlist[KDBNODECARD];
  int visitcount=0;

  assert(n);
  assert(n->level >= 0);
  assert(r);

  if (n->level > 0){  // this is an internal node in the tree 
        assert(n->type== NodeBase::KDBNODE);
	KDBNode *KN=(KDBNode *)n;

        KN->RangeQuery(r, 0, visitlist, &visitcount);

        for(i=0; i<visitcount; i++)
	{
	    DataNode *dn;
	    KDBNode   *kn;
	    switch((KN->leaf[visitlist[i]]).child->type){
		case NodeBase::DATANODE: 
			dn = (DataNode*) KN->leaf[visitlist[i]].child;
			dn->Load(dn->offset);
			break;
		case NodeBase::KDBNODE: 
			kn = (KDBNode*) KN->leaf[visitlist[i]].child;
			kn->Load(kn->offset);
			break;
	    }

	    ret = Remove((KN->leaf[visitlist[i]]).child, r);

	    if(ret == BOUNDARYUPDATE){ 
		// need to adjust the split of KDBNode KN
		// REVISE !!!
		ret = 1;
		ret = 2;
	    }
	    else if(ret == UNDERUTIL){
		// merge with sibling
		// GetSibling();
	    }

	}
  }
  else // this is a leaf node 
  {
	DataNode *DN=(DataNode *)n;

	for (i=0; i<DN->count; i++){
		assert(DN->branch[i].child);
// REVISE!!!!!!!!!!!!!!
//		if (r->Equal(&(DN->branch[i].dataPtr))){
//			break;
//		}
	}
	if(i != DN->count){
		Rect *pre_cover = DN->Cover();

		DN->DisconnectBranch(i);

		Rect *post_cover = DN->Cover();

		if(pre_cover->Equal(post_cover)) ret = FOUND;
		else ret = BOUNDARYUPDATE;

		if(DN->count < DATANODECARD*MINNODEUTIL)
			ret = UNDERUTIL;

		delete pre_cover;
		delete post_cover;
	
		DN->Write();
	}
	else {
		// not found the same data
		ret = NOTFOUND;
	}
  }

  return ret;

}
