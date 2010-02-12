#include "stdafx.h"
// FILENAME: main.C
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam
// ------------------------------------------------------------

#include<stdio.h>
#include <iostream>

#include "KDBTree.h"

int bulk_insert(KDBTree* h, char *filename, int ndim)
{
  int    i;
  long   count=0;
  float  lb[ndim],hb[ndim];
  FILE   *fp = fopen(filename, "rt");

  if(fp == NULL){
        printf("bulk_insert: file open error %s\n", filename);
        return -1;
  }

  while(!feof(fp)){
        for(i=0;i<ndim;i++){
                fscanf(fp, "%f %f ", &lb[i], &hb[i]);
        }
        fscanf(fp,"\n");
	h->KDBTreeStoreRect(lb, hb, (char*) &count); 
	count++;
  }

  fclose(fp);

  return 0;
}

int main(int argc, char **argv)
{
  int    ndim;
  int    i, j, hit;
  long   leaf_id;
  float  *lb,*hb;
  char   filename[100];
  char   idxname[100];
  long   total_hit;

  FILE *fp;
  KDBTree *h;
  KDBTreeResult result;
  KDBTreeRecord record;

  if (argc < 5){
      printf("Usage: %s <dim> filename idxname 0/1 (create/search)\n",argv[0]);
      exit(0);
  }
 
  strcpy(filename, argv[2]);
  strcpy(idxname, argv[3]);
  int option=atoi(argv[4]);    // 0 for construct, 1 for load and query

  ndim = atoi(argv[1]);

  if (option == 0)    // construct and dump
  {
	// construct
	h = new KDBTree();
	h->KDBTreeCreateRect(idxname, ndim,  sizeof(int) /*leaf size*/, 500 /*page_size*/);
	bulk_insert(h, filename, ndim);
	h->KDBTreeClose();
  }
  else if(option==1) // load and execute queries
  {
	h = new KDBTree();
	h->KDBTreeOpen(idxname);

        fp = fopen(filename, "rt");
	if(fp == NULL){
		printf("file open error %s\n", filename);
		exit(1);
	}

	lb = new float[ndim];
	hb = new float[ndim];

	total_hit = 0;
	while (!feof(fp)) {
	        for (i=0; i<ndim; i++) {
	            fscanf(fp,"%f %f ", &lb[i], &hb[i]);
	        }
		fscanf(fp,"\n");
		total_hit += (hit = h->KDBTreeRangeQueryRect(lb, hb, &result));

		for(i=0;i<hit;i++){
			record.pt = new float[2*ndim];
			record.data = (char*) &leaf_id; // long type
			h->KDBTreeGetNextResult(&result, &record);

			//printf("%ld: ", leaf_id);
			for(j=0;j<ndim;j++){
			//	printf("%f %f ", record.bbx[j].min, record.bbx[j].max);
			}
			//printf("\n");
			delete record.pt;
		}

		printf("hit: %d\n", hit);
	}

	delete lb;
	delete hb;

	printf("HIT = %ld\n", total_hit);

	fclose(fp);
	h->KDBTreeClose();
  }
  else if(option==2) // load and execute queries
  {
	h = new KDBTree();
	h->KDBTreeOpen(idxname);
	h->Print();
	h->KDBTreeClose();
   }

  return(0);
}

