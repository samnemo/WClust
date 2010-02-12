#include "stdafx.h"
/*****************************************************************************\
**                                                                           **
**  WW     WW IIIIIIII NNN   NN DDDDDDD  BBBBBBB     AA     SSSSSS EEEEEEEE  **
**  WW  W  WW    II    NNNN  NN DD    DD BB    BB  AA  AA  SS      EE        **
**  WW  W  WW    II    NN NN NN DD    DD BBBBBBB  AAAAAAAA  SSSSSS EEEEEE    **
**   WW W WW     II    NN  NNNN DD    DD BB    BB AA    AA      SS EE        **
**    WWWWW   IIIIIIII NN   NNN DDDDDDD  BBBBBBB  AA    AA SSSSSS  EEEEEEEE  **
**                                                                           **
**   SSSSSS  OOOOOO  FFFFFFFF TTTTTTTT WW     WW    AA    RRRRRRR  EEEEEEEE  **
**  SS      OO    OO FF          TT    WW  W  WW  AA  AA  RR    RR EE        **
**   SSSSS  OO    OO FFFFF       TT    WW  W  WW AAAAAAAA RRRRRRR  EEEEEE    **
**       SS OO    OO FF          TT     WW W WW  AA    AA RR   RR  EE        **
**  SSSSSS   OOOOOO  FF          TT      WWWWW   AA    AA RR    RR EEEEEEEE  **
**                                                                           **
*********** NOTICE ************************************************************
**        This file contains valuable trade secrets and proprietary          **
**        assets of Windbase Software Inc.  Embodying substantial            **
**        creative efforts and confidential information.  Unauthorized       **
**        use, copying, decompiling, translating, disclosure or              **
**        transfer, of any kind, is strictly prohibited.                     **
**                                                                           **
**        COPYRIGHT (C) 1992, 1993, 1994.  Windbase Software Inc.            **
**        ALL RIGHTS RESERVED.                                               **
\*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "memsl.h"

WBQUEUE *WBQueueOpen(WBERROR *errhdl, unsigned int pitemsize, unsigned int parraysize, int copyitem)
{
    WBQUEUE *queue = NULL;

    WBTrcEntry(WBTRC_QUEUE,"WBQueueOpen",("%p, %u, %u, %d",queue,pitemsize,parraysize,copyitem));

    if ((queue = (WBQUEUE *) malloc(sizeof(WBQUEUE))) != NULL)
    {
	memset(queue,0,sizeof(WBQUEUE));

	queue->errhdl = errhdl;
	queue->copyitem = copyitem;

	if (copyitem)
	  queue->itemsize = pitemsize;
	else
	  queue->itemsize = sizeof(void *);

	if (parraysize == 0)
	  queue->linked = 1;
	else
	{
	    queue->queunion.quearray.arraysize = parraysize;

	    if ((queue->queunion.quearray.array = (void *) malloc(parraysize*queue->itemsize)) == NULL)
	    {
		free(queue);

		queue = NULL;
	    }
	}
    }

    //if (queue == (WBQUEUE *) 0 && errhdl)
    //  WBErrorLog(errhdl,0,0,"WBQueueOpen(): Could not allocate enough memory.\n");

    WBTrcReturn(WBTRC_QUEUE,queue,("%p",queue));
}

void WBQueueDeleteF(WBQUEUE *queue, void *userparam, void (*delfunct)(void *, void *))
{
    WBTrcEntry(WBTRC_QUEUE,"WBQueueDeleteF",("%p, %p, %p",queue,userparam,delfunct));

    if (queue == NULL)
      WBTrcVReturn(WBTRC_QUEUE,("queue is null"));

    queue->delparam = userparam;
    queue->delfunct = delfunct;

    WBTrcVReturn(WBTRC_QUEUE,(""));
}

#ifdef WBTHREADS

void WBQueueLockF(WBQUEUE *queue, void *userparam, int (*lockfunct)(void *))
{
    WBTrcEntry(WBTRC_QUEUE,"WBQueueLockF",("%p, %p, %p",queue,userparam,lockfunct));

    if (queue == NULL)
      WBTrcVReturn(WBTRC_QUEUE,("queue is null"));

    queue->lockparam = userparam;
    queue->lockfunct = lockfunct;

    WBTrcVReturn(WBTRC_QUEUE,(""));
}

void WBQueueUnlockF(WBQUEUE *queue, void *userparam, int (*unlockfunct)(void *))
{
    WBTrcEntry(WBTRC_QUEUE,"WBQueueUnlockF",("%p, %p, %p",queue,userparam,unlockfunct));

    if (queue == NULL)
      WBTrcVReturn(WBTRC_QUEUE,("queue is null"));

    queue->unlockparam = userparam;
    queue->unlockfunct = unlockfunct;

    WBTrcVReturn(WBTRC_QUEUE,(""));
}

void WBQueueRLockF(WBQUEUE *queue, void *userparam, int (*lockfunct)(void *))
{
    WBTrcEntry(WBTRC_QUEUE,"WBQueueRLockF",("%p, %p, %p",queue,userparam,lockfunct));

    if (queue == NULL)
      WBTrcVReturn(WBTRC_QUEUE,("queue is null"));

    queue->rlockparam = userparam;
    queue->rlockfunct = lockfunct;

    WBTrcVReturn(WBTRC_QUEUE,(""));
}

void WBQueueRUnlockF(WBQUEUE *queue, void *userparam, int (*unlockfunct)(void *))
{
    WBTrcEntry(WBTRC_QUEUE,"WBQueueRUnlockF",("%p, %p, %p",queue,userparam,unlockfunct));

    if (queue == NULL)
      WBTrcVReturn(WBTRC_QUEUE,("queue is null"));

    queue->runlockparam = userparam;
    queue->runlockfunct = unlockfunct;

    WBTrcVReturn(WBTRC_QUEUE,(""));
}

#endif

void WBQueueClose(WBQUEUE *queue)
{
    struct single_link *tptr;

    WBTrcEntry(WBTRC_QUEUE,"WBQueueClose",("%p",queue));

    if (queue == NULL)
      WBTrcVReturn(WBTRC_QUEUE,("queue is null"));

    WBTHREAD_CLSLOCK(queue);

    if (queue->linked)
    {
	while (queue->queunion.quelist.baseptr)
	{
	    tptr = queue->queunion.quelist.baseptr;
	    queue->queunion.quelist.baseptr = queue->queunion.quelist.baseptr->next;
	    if (queue->copyitem)
	      free(tptr->item);
	    else if (queue->delfunct)
	      (*queue->delfunct)(queue->delparam,tptr->item);

	    free(tptr);
	}
    }
    else if (queue->queunion.quearray.array)
      free(queue->queunion.quearray.array);

    free(queue);

    WBTHREAD_CLSUNLOCK();

    WBTrcVReturn(WBTRC_QUEUE,(""));
}

void WBQueueClear(WBQUEUE *queue)
{
    struct single_link *tptr;

    WBTrcEntry(WBTRC_QUEUE,"WBQueueClear",("%p",queue));

    if (queue == NULL)
      WBTrcVReturn(WBTRC_QUEUE,("queue is null"));

    WBTHREAD_LOCK(queue);

    if (queue->linked)
    {
	while (queue->queunion.quelist.baseptr)
	{
	    tptr = queue->queunion.quelist.baseptr;
	    queue->queunion.quelist.baseptr = queue->queunion.quelist.baseptr->next;
	    if (queue->copyitem)
	      free(tptr->item);
	    else if (queue->delfunct)
	      (*queue->delfunct)(queue->delparam,tptr->item);

	    free(tptr);
	}
	queue->queunion.quelist.endptr = NULL;
    }
    else
    {
	queue->queunion.quearray.numitems = (long) 0;
	queue->queunion.quearray.bqueptr = 0;
	queue->queunion.quearray.equeptr = 0;
    }
    queue->numitems = (long) 0;

    WBTHREAD_UNLOCK(queue);

    WBTrcVReturn(WBTRC_QUEUE,(""));
}

int WBQueuePush(WBQUEUE *queue, void *item)
{
    struct single_link *tptr;

    WBTrcEntry(WBTRC_QUEUE,"WBQueuePush",("%p, %p",queue,item));

    if (queue == NULL)
      WBTrcReturn(WBTRC_QUEUE,0,("queue is null"));

    WBTHREAD_LOCK(queue);

    if (queue->linked)
    {
	if ((tptr = (struct single_link *) malloc(sizeof(struct single_link))) != NULL)
	{
	    memset(tptr,0,sizeof(struct single_link));
	    if (queue->copyitem)
	    {
		if ((tptr->item = (char *) malloc(queue->itemsize)) != NULL)
		  memcpy((char *)tptr->item,(char *)item,queue->itemsize);
		else
		{
		    free(tptr);

		    tptr = NULL;
		}
	    }
	    else
	      tptr->item = item;

	    if (tptr)
	    {
		tptr->next = NULL;
		if (queue->queunion.quelist.baseptr == NULL)
		{
		    queue->queunion.quelist.baseptr = tptr;
		    queue->queunion.quelist.endptr = tptr;
		}
		else
		{
		    queue->queunion.quelist.endptr->next = tptr;
		    queue->queunion.quelist.endptr = tptr;
		}
		queue->numitems++;

		WBTHREAD_VUNLOCK(queue);

		WBTrcReturn(WBTRC_QUEUE,1,("1"));
	    }
	}
    }
    else if (queue->queunion.quearray.numitems < queue->queunion.quearray.arraysize)
    {
	if (queue->copyitem)
	  memcpy((char *)queue->queunion.quearray.array+(queue->queunion.quearray.equeptr*queue->itemsize),(char *)item,queue->itemsize);
	else
	  memcpy((char *)queue->queunion.quearray.array+(queue->queunion.quearray.equeptr*queue->itemsize),&item,queue->itemsize);
	queue->queunion.quearray.equeptr = (queue->queunion.quearray.equeptr + 1) % queue->queunion.quearray.arraysize;
	queue->queunion.quearray.numitems++;
	queue->numitems++;

	WBTHREAD_VUNLOCK(queue);

	WBTrcReturn(WBTRC_QUEUE,1,("1"));
    }

    WBTHREAD_UNLOCK(queue);

    WBTrcReturn(WBTRC_QUEUE,0,("0"));
}

void *WBQueuePop(WBQUEUE *queue, void *item)
{
    struct single_link *tptr;
    void *ritem = NULL;

    WBTrcEntry(WBTRC_QUEUE,"WBQueuePop",("%p, %p",queue,item));

    if (queue == NULL)
      WBTrcReturn(WBTRC_QUEUE,NULL,("queue is null"));

    WBTHREAD_LOCK(queue);

    ritem = item;

    if (queue->linked)
    {
	if (queue->queunion.quelist.baseptr)
	{
	    tptr = queue->queunion.quelist.baseptr;
	    if (queue->copyitem && item)
	    {
		memcpy(item,tptr->item,queue->itemsize);
		free(tptr->item);
	    }
	    else
	      ritem = tptr->item;
	    queue->queunion.quelist.baseptr = queue->queunion.quelist.baseptr->next;
	    free(tptr);
	    queue->numitems--;
	}
	else ritem = NULL;
    }
    else
    {
	if (queue->queunion.quearray.numitems > (long) 0)
	{
	    if (queue->copyitem && item)
		memcpy((char *)item,(char *)queue->queunion.quearray.array+(queue->queunion.quearray.bqueptr*queue->itemsize),queue->itemsize);
	    else if (queue->copyitem == 0)
		memcpy(&ritem,(char *)queue->queunion.quearray.array+(queue->queunion.quearray.bqueptr*queue->itemsize),queue->itemsize);
	    else ritem = NULL;
		queue->queunion.quearray.bqueptr = (queue->queunion.quearray.bqueptr + 1) % queue->queunion.quearray.arraysize;
	    queue->queunion.quearray.numitems--;
	    queue->numitems--;
	}
	else ritem = NULL;
    }

    WBTHREAD_UNLOCK(queue);

    WBTrcReturn(WBTRC_QUEUE,ritem,("%p",ritem));
}

void *WBQueuePeek(WBQUEUE *queue, void *item)
{
    struct single_link *tptr;
    void *ritem = NULL;

    WBTrcEntry(WBTRC_QUEUE,"WBQueuePeek",("%p, %p",queue,item));

    if (queue == NULL)
      WBTrcReturn(WBTRC_QUEUE,NULL,("queue is null"));

    WBTHREAD_LOCK(queue);

    ritem = item;

    if (queue->linked){
	if (queue->queunion.quelist.baseptr){
	    tptr = queue->queunion.quelist.baseptr;

	    if (queue->copyitem && item)
		memcpy(item,tptr->item,queue->itemsize);
	    else
		ritem = tptr->item;
	}
	else ritem = NULL;
    }
    else {
	if (queue->queunion.quearray.numitems > (long) 0){
	    if (queue->copyitem && item)
		memcpy((char *)item,(char *)queue->queunion.quearray.array+(queue->queunion.quearray.bqueptr*queue->itemsize),queue->itemsize);
	    else if (queue->copyitem == 0)
		memcpy(&ritem,(char *)queue->queunion.quearray.array+(queue->queunion.quearray.bqueptr*queue->itemsize),queue->itemsize);
	    else ritem = NULL;
		queue->queunion.quearray.bqueptr = (queue->queunion.quearray.bqueptr + 1) % queue->queunion.quearray.arraysize;
	}
	else ritem = NULL;
    }

    WBTHREAD_UNLOCK(queue);
    WBTrcReturn(WBTRC_QUEUE,ritem,("%p",ritem));
}

long WBQueueNumItems(WBQUEUE *queue)
{
    long numitems = (long) 0;

    WBTrcEntry(WBTRC_QUEUE,"WBQueueNumItems",("%p",queue));

    if (queue == NULL) WBTrcReturn(WBTRC_QUEUE,0,("queue is null"));

    WBTHREAD_RLOCK(queue);
    numitems = queue->numitems;
    WBTHREAD_RUNLOCK(queue);
    WBTrcReturn(WBTRC_QUEUE,numitems,("%ld",numitems));
}

long WBQueueMaxItems(WBQUEUE *queue)
{
    WBTrcEntry(WBTRC_QUEUE,"WBQueueMaxItems",("%p",queue));

    if (queue == NULL) WBTrcReturn(WBTRC_QUEUE,0,("queue is null"));

    WBTHREAD_RLOCK(queue);

    if (queue->linked){
	WBTHREAD_VRUNLOCK(queue);
	WBTrcReturn(WBTRC_QUEUE,0,("0"));
    }

    WBTHREAD_RUNLOCK(queue);
    WBTrcReturn(WBTRC_QUEUE,queue->queunion.quearray.arraysize,("%ld",queue->queunion.quearray.arraysize));
}


int WBQueueIsEmpty(WBQUEUE *queue)
{
    WBTrcEntry(WBTRC_QUEUE,"WBQueueIsEmpty",("%p",queue));

    if (queue == NULL)
	WBTrcReturn(WBTRC_QUEUE,1,("queue is null"));

    WBTHREAD_RLOCK(queue);

    if (queue->numitems){
	WBTHREAD_VRUNLOCK(queue);
	WBTrcReturn(WBTRC_QUEUE,0,("0"));
    }

    WBTHREAD_RUNLOCK(queue);
    WBTrcReturn(WBTRC_QUEUE,1,("1"));
}


