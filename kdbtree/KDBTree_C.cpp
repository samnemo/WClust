#include "stdafx.h"
#include "KDBTree_C.h"
#include "KDBTree.h"

typedef void KDBTree_C;
    
//-----------------------------------
//  Create a KDB-Tree    
//-----------------------------------
KDBTree_C *KDBTreeCreate_C(const char *path, int dimension,
                         int dataSize, int pageSize)
{
    KDBTree *file;

    file = new KDBTree();

    file->KDBTreeCreate(path, dimension, dataSize, pageSize);

    return (KDBTree_C*) file;
}

//-----------------------------------
//  Create a KDB-Tree for Rectangular Data
//-----------------------------------
KDBTree_C *KDBTreeCreateRect_C(const char *path, int dimension,
                         int dataSize, int pageSize)
{
    KDBTree *file;

    file = new KDBTree();

    file->KDBTreeCreateRect(path, dimension, dataSize, pageSize);

    return (KDBTree_C*) file;
}

//-----------------------------------
//  Open a KDB-Tree
//-----------------------------------
KDBTree_C *KDBTreeOpen_C(const char *path)
{
    KDBTree *file;

    file = new KDBTree();

    file->KDBTreeOpen(path);

    return (KDBTree_C*) file;
}

//-----------------------------------
//  Get the Root BBX
//-----------------------------------
int KDBTreeGetRoot_C(KDBTree_C *file, float *lb, float *hb)
{
    return ((KDBTree*)file)->KDBTreeGetRoot(lb, hb);
}

//-----------------------------------
//  Store a Point Data
//-----------------------------------
int KDBTreeStore_C(KDBTree_C *file, float ptr[], char* data)
{
    return ((KDBTree*)file)->KDBTreeStore(ptr, data);
}

//-----------------------------------
//   Store a Rectangular Data
//-----------------------------------
int KDBTreeStoreRect_C(KDBTree_C *file, float lb[], float hb[], char* data)
{
    return ((KDBTree*)file)->KDBTreeStoreRect(lb, hb, data);
}

//-----------------------------------
//  Delete a Point Data
//-----------------------------------
int KDBTreeDelete_C(KDBTree_C *file, float ptr[])
{
    return ((KDBTree*)file)->KDBTreeDelete(ptr);
}

//-----------------------------------
// Submit a Range Query  
//-----------------------------------
int KDBTreeRangeQuery_C(KDBTree_C *file, float lb[], float hb[], KDBTreeResult_C** _res)
{
    KDBTreeResult *res;

    res = new KDBTreeResult();
    (*_res) = (KDBTreeResult_C*) res;

    return ((KDBTree*)file)->KDBTreeRangeQuery(lb, hb, (KDBTreeResult*) res);
}

//-----------------------------------
// Submit a Range Query for Rectangular Data
//-----------------------------------
int KDBTreeRangeQueryRect_C(KDBTree_C *file, float lb[], float hb[], KDBTreeResult_C** _res)
{
    KDBTreeResult *res;

    res = new KDBTreeResult();
    (*_res) = (KDBTreeResult_C*) res;

    return ((KDBTree*)file)->KDBTreeRangeQueryRect(lb, hb, (KDBTreeResult*) res);
}

//-----------------------------------
// Get Next Result
//-----------------------------------
int KDBTreeGetNextResult_C(KDBTree_C *file, KDBTreeResult_C *res, KDBTreeRecord* record)
{
    return ((KDBTree*)file)->KDBTreeGetNextResult((KDBTreeResult*) res, record);
}

//-----------------------------------
// Free Result
//-----------------------------------
void KDBTreeFreeResult(KDBTreeResult_C* res)
{
    delete (KDBTreeResult*) res;
}

//-----------------------------------
// Close
//-----------------------------------
void KDBTreeClose_C(KDBTree_C *file)
{
    ((KDBTree*)file)->KDBTreeClose();

    delete ((KDBTree*) file);
}

