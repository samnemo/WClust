#ifndef KDBTREE_C_H
#define KDBTREE_C_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
        float *pt;
        char* data;
}KDBTreeRecord;

typedef void KDBTree_C;
typedef void KDBTreeResult_C;

KDBTree_C *KDBTreeCreate_C(const char *path, int dimension, 
			int dataSize, int pageSize);
KDBTree_C *KDBTreeCreateRect_C(const char *path, int dimension, 
			int dataSize, int pageSize);

KDBTree_C *KDBTreeOpen_C(const char *dumpfile); 

int KDBTreeGetRoot_C(KDBTree_C *file, float *lb, float *hb);

int KDBTreeStore_C(KDBTree_C *file, float pt[], char* data);
int KDBTreeStoreRect_C(KDBTree_C *file, float lb[], float hb[], char* data);

int KDBTreeDelete_C(KDBTree_C *file, float pt[]);

int KDBTreeRangeQuery_C(KDBTree_C *file, float lb[], float hb[], KDBTreeResult_C** res);
int KDBTreeRangeQueryRect_C(KDBTree_C *file, float lb[], float hb[], KDBTreeResult_C** res);

int KDBTreeGetNextResult_C(KDBTree_C *file, KDBTreeResult_C *res, KDBTreeRecord* record);

void KDBTreeFreeResult(KDBTreeResult_C* res);

void KDBTreeClose_C(KDBTree_C *file);

#ifdef __cplusplus
};
#endif

#endif
