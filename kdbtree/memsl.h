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

#ifndef memsl_h
#define memsl_h

#define WBSTDC  1
#define WBPROTOS 1

/*************************************************************************\
**  Defines
\*************************************************************************/

#define WB_TRUE 1
#define WB_FALSE 0

#define WB_EMPTY   0x0001
#define WB_CURRENT 0x0002
#define WB_BETWEEN 0x0002
#define WB_FIRST   0x0004
#define WB_LAST    0x0008
#define WB_ROOT    0x0010
#define WB_BASE    0x0010
#define WB_PREV    0x0020
#define WB_NEXT    0x0040
#define WB_CLEAR   0x0080
#define WB_SORTED  0x0100

#define WBTRC_ARRAY 100
#define WBTRC_SLIST 200
#define WBTRC_DLIST 300
#define WBTRC_HASH 400
#define WBTRC_DHASH 500
#define WBTRC_PHASH 600
#define WBTRC_STACK 700
#define WBTRC_QUEUE 800
#define WBTRC_DQUEUE 900
#define WBTRC_TREE 1000
#define WBTRC_TTREE 1100
#define WBTRC_AVL 1200
#define WBTRC_TAVL 1300
#define WBTRC_SET 1400
#define WBTRC_TABLE 1500
#define WBTRC_DICT 1600
#define WBTRC_PQUEUE 1700
#define WBTRC_PHEAP 1720
#define WBTRC_ERROR 1800
#define WBTRC_MEMORY 1900
#define WBTRC_MALLOC 1950

#define WBBSEARCH_EQ    0x01
#define WBBSEARCH_GR    0x02
#define WBBSEARCH_LS    0x04
#define WBBSEARCH_DUP   0x08

#ifdef WBTHREADS

#  define WBTHREAD_LOCK(aa)                                             \
       if ((aa->lockfunct && (*aa->lockfunct)(aa->lockparam) == 0)      \
	 || aa->lockfunct == NULL)                                      \
	 {

#  define WBTHREAD_UNLOCK(aa)                                           \
	   if (aa->unlockfunct)                                         \
	     (*aa->unlockfunct)(aa->unlockparam);                       \
	 }

#  define WBTHREAD_VUNLOCK(aa)                                          \
     {                                                                  \
       if (aa->unlockfunct)                                             \
	 (*aa->unlockfunct)(aa->unlockparam);                           \
     }

#  define WBTHREAD_RLOCK(aa)                                            \
       if ((aa->rlockfunct &&                                           \
	   (aa->lockerr = (*aa->rlockfunct)(aa->rlockparam)) == 0)      \
	 || (aa->lockerr == 0 && aa->lockfunct &&                       \
	    (*aa->lockfunct)(aa->lockparam) == 0)                       \
	 || (aa->lockfunct == NULL && aa->rlockfunct == NULL))          \
	 {

#  define WBTHREAD_RUNLOCK(aa)                                          \
	   if (aa->runlockfunct)                                        \
	     (*aa->runlockfunct)(aa->runlockparam);                     \
	   else if (aa->unlockfunct)                                    \
	     (*aa->unlockfunct)(aa->unlockparam);                       \
	 }

#  define WBTHREAD_VRUNLOCK(aa)                                         \
     {                                                                  \
       if (aa->runlockfunct)                                            \
	 (*aa->runlockfunct)(aa->runlockparam);                         \
       else if (aa->unlockfunct)                                        \
	 (*aa->unlockfunct)(aa->unlockparam);                           \
     }

#ifdef WBSTDC

#  define WBTHREAD_CLSLOCK(aa)                                          \
       if ((aa->lockfunct && (*aa->lockfunct)(aa->lockparam) == 0)     \
	 || aa->lockfunct == NULL)                                      \
	 {                                                              \
	   void *lunlockparam = aa->unlockparam;                        \
	   int (*lunlockfunct)(void *) = aa->unlockfunct;

#else

#  define WBTHREAD_CLSLOCK(aa)                                          \
       if ((aa->lockfunct && (*aa->lockfunct)(aa->lockparam) == 0)      \
	 || aa->lockfunct == NULL)                                      \
	 {                                                              \
	   void *lunlockparam = aa->unlockparam;                        \
	   int (*lunlockfunct)() = aa->unlockfunct;

#endif

#  define WBTHREAD_CLSUNLOCK()                                          \
	   if (lunlockfunct)                                            \
	     (*lunlockfunct)(lunlockparam);                             \
	 }

#else

#  define WBTHREAD_LOCK(aa)
#  define WBTHREAD_UNLOCK(aa)
#  define WBTHREAD_VUNLOCK(aa)
#  define WBTHREAD_RLOCK(aa)
#  define WBTHREAD_RUNLOCK(aa)
#  define WBTHREAD_VRUNLOCK(aa)
#  define WBTHREAD_CLSLOCK(aa)
#  define WBTHREAD_CLSUNLOCK()

#endif


/*************************************************************************\
**  bit manipulation macros
\*************************************************************************/

#define BITON(aa,bb) (aa |= (bb))
#define BITOFF(aa,bb) (aa = ((aa&(bb)) == (bb) ? (aa^(bb)) : aa))
#define BITTOGGLE(aa,bb) (aa = aa^(bb))
#define ALLBITSON(aa) (aa = 0, aa = ~aa)
#define ALLBITSOFF(aa) (aa = 0)
#define ISBITON(aa,bb) (aa&(bb))


/*************************************************************************\
**  Structures
\*************************************************************************/

struct single_link
  {
    struct single_link *next;
    void *item;
  };

struct double_link
  {
    struct double_link *last,
                       *next;
    void *item;
  };

struct binary_tree
  {
    struct binary_tree *left,
                       *right;
    void *item;
  };

struct threaded_tree
  {
    unsigned int ltag : 1,
                 rtag : 1;
    struct threaded_tree *left,
                         *right;
    void *item;
  };

struct avl_tree
  {
    int bal;
    struct avl_tree *left,
                    *right;
    void *item;
  };

struct tavl_tree
  {
    int bal;
    unsigned int ltag : 1,
                 rtag : 1;
    struct tavl_tree *left,
                     *right;
    void *item;
  };


/*************************************************************************\
**  Entry/Exit Tracing
\*************************************************************************/

#ifndef WBTRC_MAXFNAME
#  define WBTRC_MAXFNAME 256
#endif

#ifndef WBTRC_MAXENTRY
#  define WBTRC_MAXENTRY 50
#endif

#ifndef WBTRC_MAXSTR
#  define WBTRC_MAXSTR 2048
#endif

#define WBTRC_LVL1STR
#ifndef WBTRC_LVL1MAXSTR
#  define WBTRC_LVL1MAXSTR 128
#endif

#define WBTRC_ENTRY 0
#define WBTRC_RETURN 1
#define WBTRC_VRETURN 2
#define WBTRC_EXIT 3
#define WBTRC_ABORT 4
#define WBTRC_ASSERT 5

struct trace_watch
  {
    char *procname,
         *watchname,
         *filename,
         *addr;
    long chksum;
    int len,
        linenum,
        indent;
    struct trace_watch *next;
  };

struct proc_trace
  {
    int indent,
        line,
	type,
        errlvl;
    char *name,
	 *file,
         str[WBTRC_LVL1MAXSTR];
  };

#ifdef WBTHREADS

#  define WBTRC_LOCK()                                                  \
	 if ((gwbtrc_lockfunct && (*gwbtrc_lockfunct)(gwbtrc_lockparam) == 0) \
	   || gwbtrc_lockfunct == NULL)                                 \
           {

#  define WBTRC_UNLOCK()                                                \
	     if (gwbtrc_unlockfunct)                                    \
               (*gwbtrc_unlockfunct)(gwbtrc_unlockparam);               \
	   }

#else

#  define WBTRC_LOCK()
#  define WBTRC_UNLOCK()

#endif

#if defined(WBTRC_LEVEL1) || defined(WBTRC_LEVEL2)

extern void *gwbtrc_lockparam,
            *gwbtrc_unlockparam;

#ifdef WBSTDC
  extern int (*gwbtrc_lockfunct)(void *),
             (*gwbtrc_unlockfunct)(void *);
#else
  extern int (*gwbtrc_lockfunct)(),
	     (*gwbtrc_unlockfunct)();
#endif

  extern struct trace_watch *gwbtrc_basewatch;
  extern struct proc_trace gwbtrc_proctrace[WBTRC_MAXENTRY];
  extern char gwbtrc_strbuf[WBTRC_MAXSTR+1];
  extern int gwbtrc_entrynum,
             gwbtrc_indent1,
             gwbtrc_indent2,
             gwbtrc_printtrace,
             gwbtrc_tracing,
             gwbtrc_tracewatch,
             gwbtrc_tracerange1,
             gwbtrc_tracerange2,
             gwbtrc_errorlevel,
             gwbtrc_bracesize,
             gwbtrc_indentsize;

#endif

#ifdef WBPROTOS

  void WBTraceInit(int trclvl, int forcetrc, char *progname);
  void WBTraceLvl1Str(char *str1, char *str2, int len);
#ifdef WBTHREADS
  void WBTraceLockF(void *userparam, int (*lockfunct)(void *));
  void WBTraceUnlockF(void *userparam, int (*unlockfunct)(void *));
#endif
  void WBTracePrintFD(char *format, ...);
  void WBTracePrintF(char *format, ...);
  void WBTraceSPrintF(char *format, ...);
  void WBTraceHexDump(char *procname, char *dumpname, char *filename, int linenum, char *buff, int len);
  void WBTracePrintEnv(void);
  void WBTracePrintArgs(int, char **);
  void WBTraceWatch(char *procname, int indent, char *filename, int linenum, char *watchname, char *addr, int len);
  void WBTraceRmWatch(char *addr);
  void WBTraceRmGrpWatch(char *procname, int indent);
  void WBTraceChkWatch(void);

#else

  void WBTraceInit();
  void WBTraceLvl1Str();
#ifdef WBTHREADS
  void WBTraceLockF();
  void WBTraceUnlockF();
#endif
  void WBTracePrintFD();
  void WBTracePrintF();
  void WBTraceSPrintF();
  void WBTraceHexDump();
  void WBTracePrintEnv();
  void WBTracePrintArgs();
  void WBTraceWatch();
  void WBTraceRmWatch();
  void WBTraceRmGrpWatch();
  void WBTraceChkWatch();

#endif

#ifdef WBTRC_LEVEL1

#  ifdef WBTRC_LVL1STR

#    define WBTrc__Lvl1FmtStr(str, format_args)                         \
         WBTraceSPrintF format_args;                                    \
         WBTraceLvl1Str(str,gwbtrc_strbuf,WBTRC_LVL1MAXSTR-1);

#  else

#    define WBTrc__Lvl1FmtStr(str, format_args)

#  endif

#  define WBTrc__MainEntry()                                            \
     if (gwbtrc_tracing >= 1)                                           \
       {                                                                \
	 gwbtrc_proctrace[gwbtrc_entrynum].name = wbtrc_functname;      \
	 gwbtrc_proctrace[gwbtrc_entrynum].type = WBTRC_ENTRY;          \
	 gwbtrc_proctrace[gwbtrc_entrynum].file = __FILE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].line = __LINE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].errlvl = 0;                  \
	 gwbtrc_proctrace[gwbtrc_entrynum].indent = gwbtrc_indent1;     \
	 gwbtrc_entrynum = (gwbtrc_entrynum+1)%WBTRC_MAXENTRY;          \
	 gwbtrc_indent1+=gwbtrc_indentsize;                             \
       }

#  define WBTrc__Entry(errorlvl, pname, format_args)                    \
     if (gwbtrc_tracing >= 1)                                           \
       {                                                                \
	 WBTRC_LOCK();                                                  \
	 gwbtrc_proctrace[gwbtrc_entrynum].name = wbtrc_functname;      \
	 WBTrc__Lvl1FmtStr(gwbtrc_proctrace[gwbtrc_entrynum].str, format_args); \
	 gwbtrc_proctrace[gwbtrc_entrynum].type = WBTRC_ENTRY;          \
	 gwbtrc_proctrace[gwbtrc_entrynum].file = __FILE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].line = __LINE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].errlvl = errorlvl;           \
	 gwbtrc_proctrace[gwbtrc_entrynum].indent = gwbtrc_indent1;     \
	 gwbtrc_entrynum = (gwbtrc_entrynum+1)%WBTRC_MAXENTRY;          \
	 gwbtrc_indent1+=gwbtrc_indentsize;                             \
	 WBTRC_UNLOCK();                                                \
       }

#  define WBTrc__Return(errorlvl, val, format_args)                     \
     if (gwbtrc_tracing >= 1)                                           \
       {                                                                \
	 WBTRC_LOCK();                                                  \
	 gwbtrc_proctrace[gwbtrc_entrynum].name = wbtrc_functname;      \
	 WBTrc__Lvl1FmtStr(gwbtrc_proctrace[gwbtrc_entrynum].str, format_args); \
	 gwbtrc_proctrace[gwbtrc_entrynum].type = WBTRC_RETURN;         \
	 gwbtrc_proctrace[gwbtrc_entrynum].file = __FILE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].line = __LINE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].errlvl = errorlvl;           \
	 gwbtrc_indent1-=gwbtrc_indentsize;                             \
	 gwbtrc_proctrace[gwbtrc_entrynum].indent = gwbtrc_indent1;     \
	 gwbtrc_entrynum = (gwbtrc_entrynum+1)%WBTRC_MAXENTRY;          \
	 WBTRC_UNLOCK();                                                \
       }

#  define WBTrc__VReturn(errorlvl, format_args)                         \
     if (gwbtrc_tracing >= 1)                                           \
       {                                                                \
	 WBTRC_LOCK();                                                  \
	 gwbtrc_proctrace[gwbtrc_entrynum].name = wbtrc_functname;      \
	 WBTrc__Lvl1FmtStr(gwbtrc_proctrace[gwbtrc_entrynum].str, format_args); \
	 gwbtrc_proctrace[gwbtrc_entrynum].type = WBTRC_VRETURN;        \
	 gwbtrc_proctrace[gwbtrc_entrynum].file = __FILE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].line = __LINE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].errlvl = errorlvl;           \
	 gwbtrc_indent1-=gwbtrc_indentsize;                             \
	 gwbtrc_proctrace[gwbtrc_entrynum].indent = gwbtrc_indent1;     \
	 gwbtrc_entrynum = (gwbtrc_entrynum+1)%WBTRC_MAXENTRY;          \
	 WBTRC_UNLOCK();                                                \
       }

#  define WBTrc__Exit(errorlvl, val, format_args)                       \
     if (gwbtrc_tracing >= 1)                                           \
       {                                                                \
	 WBTRC_LOCK();                                                  \
	 gwbtrc_proctrace[gwbtrc_entrynum].name = wbtrc_functname;      \
	 WBTrc__Lvl1FmtStr(gwbtrc_proctrace[gwbtrc_entrynum].str, format_args); \
	 gwbtrc_proctrace[gwbtrc_entrynum].type = WBTRC_EXIT;           \
	 gwbtrc_proctrace[gwbtrc_entrynum].file = __FILE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].line = __LINE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].errlvl = errorlvl;           \
	 gwbtrc_proctrace[gwbtrc_entrynum].indent = gwbtrc_indent1;     \
	 gwbtrc_entrynum = (gwbtrc_entrynum+1)%WBTRC_MAXENTRY;          \
	 gwbtrc_printtrace = 1;                                         \
	 WBTRC_UNLOCK();                                                \
       }

#  define WBTrc__Abort(errorlvl, format_args)                           \
     if (gwbtrc_tracing >= 1)                                           \
       {                                                                \
	 WBTRC_LOCK();                                                  \
	 gwbtrc_proctrace[gwbtrc_entrynum].name = wbtrc_functname;      \
	 WBTrc__Lvl1FmtStr(gwbtrc_proctrace[gwbtrc_entrynum].str, format_args); \
	 gwbtrc_proctrace[gwbtrc_entrynum].type = WBTRC_ABORT;          \
	 gwbtrc_proctrace[gwbtrc_entrynum].file = __FILE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].line = __LINE__;             \
	 gwbtrc_proctrace[gwbtrc_entrynum].errlvl = errorlvl;           \
	 gwbtrc_proctrace[gwbtrc_entrynum].indent = gwbtrc_indent1;     \
	 gwbtrc_entrynum = (gwbtrc_entrynum+1)%WBTRC_MAXENTRY;          \
	 gwbtrc_printtrace = 1;                                         \
	 WBTRC_UNLOCK();                                                \
       }

#  define WBTrc__Assert(errorlvl, test, format_args)                    \
     if (gwbtrc_tracing >= 1)                                           \
       {                                                                \
	 WBTRC_LOCK();                                                  \
	 if (test == 0)                                                 \
	   {                                                            \
	     gwbtrc_proctrace[gwbtrc_entrynum].name = wbtrc_functname;  \
	     WBTrc__Lvl1FmtStr(gwbtrc_proctrace[gwbtrc_entrynum].str, format_args); \
	     gwbtrc_proctrace[gwbtrc_entrynum].type = WBTRC_ASSERT;     \
	     gwbtrc_proctrace[gwbtrc_entrynum].file = __FILE__;         \
	     gwbtrc_proctrace[gwbtrc_entrynum].line = __LINE__;         \
	     gwbtrc_proctrace[gwbtrc_entrynum].errlvl = errorlvl;       \
	     gwbtrc_proctrace[gwbtrc_entrynum].indent = gwbtrc_indent1; \
	     gwbtrc_entrynum = (gwbtrc_entrynum+1)%WBTRC_MAXENTRY;      \
	     gwbtrc_printtrace = 1;                                     \
	   }                                                            \
	 WBTRC_UNLOCK();                                                \
       }

#else

#  define WBTrc__MainEntry()
#  define WBTrc__Entry(errorlvl, pname, format_args)
#  define WBTrc__Return(errorlvl, val, format_args)
#  define WBTrc__VReturn(errorlvl, format_args)
#  define WBTrc__Exit(errorlvl, val, format_args)
#  define WBTrc__Abort(errorlvl, format_args)
#  define WBTrc__Assert(errorlvl, test, format_args)

#endif

#ifdef WBTRC_LEVEL2

#  define WBTrcFrcMainEntry(forcetrc)                                   \
     char *wbtrc_functname="main";                                      \
	 WBTraceInit(2,forcetrc,argv[0]);                               \
	 if (gwbtrc_tracing == 2)                                       \
	   {                                                            \
	     gwbtrc_errorlevel = 0;                                     \
	     WBTracePrintEnv();                                         \
	     WBTracePrintFD("main(\n");                                 \
	     WBTracePrintArgs(argc, argv);                              \
	     WBTracePrintFD("    ) /* %s:%d */\n",__FILE__,__LINE__);   \
	     WBTracePrintFD("%*s{\n",gwbtrc_bracesize,"");              \
	     gwbtrc_indent2+=gwbtrc_indentsize;                         \
	   }                                                            \
	 WBTrc__MainEntry()

#  define WBTrcMainEntry()  WBTrcFrcMainEntry(0)

#  define WBTrcEntry(errorlvl, pname, format_args)                      \
     char *wbtrc_functname=pname;                                       \
	 if (gwbtrc_tracing == 2                                        \
	   && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	   {                                                            \
	     WBTRC_LOCK();                                              \
	     gwbtrc_errorlevel = errorlvl;                              \
	     if (gwbtrc_basewatch)                                      \
	       WBTraceChkWatch();                                       \
	     WBTracePrintFD("%s(",wbtrc_functname);                     \
	     WBTracePrintF format_args;                                 \
	     WBTracePrintF(") /* %s:%d */\n",__FILE__,__LINE__);        \
	     WBTracePrintFD("%*s{\n",gwbtrc_bracesize,"");              \
	     gwbtrc_indent2+=gwbtrc_indentsize;                         \
	     WBTRC_UNLOCK();                                            \
	   }                                                            \
	 WBTrc__Entry(errorlvl, pname, format_args)

#  define WBTrcReturn(errorlvl, val, format_args)                       \
     {                                                                  \
       if (gwbtrc_tracing == 2                                          \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   gwbtrc_errorlevel = errorlvl;                                \
	   if (gwbtrc_basewatch)                                        \
	     {                                                          \
	       WBTraceChkWatch();                                       \
	       WBTraceRmGrpWatch(wbtrc_functname,gwbtrc_indent2);       \
	     }                                                          \
	   WBTraceSPrintF format_args;                                  \
	   WBTracePrintFD("return(%s);\n",gwbtrc_strbuf);               \
	   gwbtrc_indent2-=gwbtrc_indentsize;                           \
	   WBTracePrintFD("%*s} /* %s, %s:%d */\n",                     \
					     gwbtrc_bracesize,          \
					     "",                        \
					     wbtrc_functname,           \
					     __FILE__,                  \
					     __LINE__);                 \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
       WBTrc__Return(errorlvl, val, format_args)                        \
       return(val);                                                     \
     }

#  define WBTrcVReturn(errorlvl, format_args)                           \
     {                                                                  \
       if (gwbtrc_tracing == 2                                          \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   gwbtrc_errorlevel = errorlvl;                                \
	   if (gwbtrc_basewatch)                                        \
	     {                                                          \
	       WBTraceChkWatch();                                       \
	       WBTraceRmGrpWatch(wbtrc_functname,gwbtrc_indent2);       \
	     }                                                          \
	   WBTraceSPrintF format_args;                                  \
	   WBTracePrintFD("return; /* fmt_args(%s) */\n",gwbtrc_strbuf); \
	   gwbtrc_indent2-=gwbtrc_indentsize;                           \
	   WBTracePrintFD("%*s} /* %s, %s:%d */\n",                     \
					     gwbtrc_bracesize,          \
					     "",                        \
					     wbtrc_functname,           \
					     __FILE__,                  \
					     __LINE__);                 \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
       WBTrc__VReturn(errorlvl, format_args)                            \
       return;                                                          \
     }

#  define WBTrcExit(errorlvl, val, format_args)                         \
     {                                                                  \
       if (gwbtrc_tracing == 2                                          \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   gwbtrc_errorlevel = errorlvl;                                \
	   if (gwbtrc_basewatch)                                        \
	     WBTraceChkWatch();                                         \
	   WBTraceSPrintF format_args;                                  \
	   WBTracePrintFD("exit(%s) /* %s, %s:%d */\n",                 \
					     gwbtrc_strbuf,             \
					     wbtrc_functname,           \
					     __FILE__,                  \
					     __LINE__);                 \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
       WBTrc__Exit(errorlvl, val, format_args)                          \
       exit(val);                                                       \
     }

#  define WBTrcAbort(errorlvl, format_args)                             \
     {                                                                  \
       if (gwbtrc_tracing == 2                                          \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   gwbtrc_errorlevel = errorlvl;                                \
	   if (gwbtrc_basewatch)                                        \
	     WBTraceChkWatch();                                         \
	   WBTraceSPrintF format_args;                                  \
	   WBTracePrintFD("abort(%s) /* %s, %s:%d */\n",                \
				      gwbtrc_strbuf,                    \
				      wbtrc_functname,                  \
				      __FILE__,                         \
				      __LINE__);                        \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
       WBTrc__Abort(errorlvl, format_args)                              \
       abort();                                                         \
     }

#  define WBTrcAssert(errorlvl, test, format_args)                      \
     {                                                                  \
       if (gwbtrc_tracing == 2                                          \
	 && test == 0 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   gwbtrc_errorlevel = errorlvl;                                \
	   if (gwbtrc_basewatch)                                        \
	     WBTraceChkWatch();                                         \
	   WBTraceSPrintF format_args;                                  \
	   WBTracePrintFD("*** Assertion failed *** %s, assert(%s), /* %s:%d */\n", \
				      wbtrc_functname,                  \
				      gwbtrc_strbuf,                    \
				      __FILE__,                         \
				      __LINE__);                        \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
       WBTrc__Assert(errorlvl, test, format_args)                       \
       assert(test);                                                    \
     }

#  define WBTrcWatch(errorlvl, watchname, watchaddr, watchlen)          \
     {                                                                  \
       if (gwbtrc_tracing == 2 && gwbtrc_tracewatch                     \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   WBTraceWatch(wbtrc_functname,gwbtrc_indent2,                 \
	      __FILE__,__LINE__,watchname,(char *)watchaddr,watchlen);  \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
     }

#  define WBTrcRmWatch(errorlvl, watchaddr)                             \
     {                                                                  \
       if (gwbtrc_tracing == 2 && gwbtrc_tracewatch                     \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   WBTraceRmWatch(watchaddr);                                   \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
     }

#  define WBTrcPrintFD(errorlvl, format_args)                           \
     {                                                                  \
       if (gwbtrc_tracing == 2                                          \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   gwbtrc_errorlevel = errorlvl;                                \
	   WBTracePrintFD format_args;                                  \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
     }

#  define WBTrcPrintF(errorlvl, format_args)                            \
     {                                                                  \
       if (gwbtrc_tracing == 2                                          \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   gwbtrc_errorlevel = errorlvl;                                \
	   WBTracePrintF format_args;                                   \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
     }

#  define WBTrcHexDump(errorlvl, dumpname, addr, len)                   \
     {                                                                  \
       if (gwbtrc_tracing == 2                                          \
	 && errorlvl >= gwbtrc_tracerange1 && errorlvl <= gwbtrc_tracerange2) \
	 {                                                              \
	   WBTRC_LOCK();                                                \
	   gwbtrc_errorlevel = errorlvl;                                \
	   WBTraceHexDump(wbtrc_functname,dumpname,__FILE__,__LINE__,addr,len); \
	   WBTRC_UNLOCK();                                              \
	 }                                                              \
     }

#else

#  if defined(WBTRC_LEVEL1)

#    define WBTrcFrcMainEntry(forcetrc)                                 \
       char *wbtrc_functname="main";                                    \
	   WBTraceInit(1,forcetrc,argv[0]);                             \
	   WBTrc__MainEntry();

#    define WBTrcMainEntry()  WBTrcFrcMainEntry(0)

#    define WBTrcEntry(errorlvl, pname, format_args)                    \
       char *wbtrc_functname=pname;                                     \
	   WBTrc__Entry(errorlvl, pname, format_args);

#    define WBTrcReturn(errorlvl, val, format_args)                     \
       {                                                                \
	 WBTrc__Return(errorlvl, val, format_args);                     \
	 return(val);                                                   \
       }

#    define WBTrcVReturn(errorlvl, format_args)                         \
       {                                                                \
	 WBTrc__VReturn(errorlvl, format_args);                         \
	 return;                                                        \
       }

#    define WBTrcExit(errorlvl, val, format_args)                       \
       {                                                                \
	 WBTrc__Exit(errorlvl, val, format_args);                       \
	 exit(val);                                                     \
       }

#    define WBTrcAbort(errorlvl, format_args)                           \
       {                                                                \
	 WBTrc__Abort(errorlvl, format_args);                           \
	 abort();                                                       \
       }

#    define WBTrcAssert(errorlvl, test, format_args)                    \
       {                                                                \
	 WBTrc_Assert(errorlvl, test, format_args);                     \
	 assert(test);                                                  \
       }

#    define WBTrcPrintFD(errorlvl, format_args)
#    define WBTrcPrintF(errorlvl, format_args)
#    define WBTrcHexDump(errorlvl, dumpname, addr, len)
#    define WBTrcWatch(errorlvl, watchname, watchaddr, watchlen)
#    define WBTrcRmWatch(errorlvl, watchaddr)

#  else

#    define WBTrcMainEntry()
#    define WBTrcEntry(errorlvl, pname, format_args)
#    define WBTrcReturn(errorlvl, val, format_args)       return(val)
#    define WBTrcVReturn(errorlvl, format_args)           return
#    define WBTrcExit(errorlvl, val, format_args)         exit(val)
#    define WBTrcAbort(errorlvl, format_args)             abort()
#    define WBTrcAssert(errorlvl, test, format_args)      assert(test)
#    define WBTrcPrintFD(errorlvl, format_args)
#    define WBTrcPrintF(errorlvl, format_args)
#    define WBTrcHexDump(errorlvl, dumpname, addr, len)
#    define WBTrcWatch(errorlvl, watchname, watchaddr, watchlen)
#    define WBTrcRmWatch(errorlvl, watchaddr)

#  endif

#endif

#define wbtrcmainentry WBTrcMainEntry
#define wbtrcentry WBTrcEntry
#define wbtrcreturn WBTrcReturn
#define wbtrcvreturn WBTrcVReturn
#define wbtrcexit WBTrcExit
#define wbtrcabort WBTrcAbort
#define wbtrcassert WBTrcAssert
#define wbtrcprintfd WBTrcPrintFD
#define wbtrcprintf WBTrcPrintF
#define wbtrchexdump WBTrcHexDump
#define wbtrcwatch WBTrcWatch
#define wbtrcrmwatch WBTrcRmWatch
#define wbtrclockf WBTraceLockF
#define wbtrcunlockf WBTraceUnlockF
#define WBTrcLockF WBTraceLockF
#define WBTrcUnlockF WBTraceUnlockF


/*************************************************************************\
**  Error handling Structure and Prototypes
\*************************************************************************/

struct error_hdls
  {
    int error;
    void *errparam;
#ifdef WBSTDC
    int (*errfunct)(void *, int, int, char *);
#else
    int (*errfunct)();
#endif
    struct error_hdls *next;
  };

struct errlvl_hdls
  {
    int errlvl;
    void *lvlparam;
#ifdef WBSTDC
    void (*lvlfunct)(void *, int, int, char *);
#else
    void (*lvlfunct)();
#endif
    struct errlvl_hdls *next;
  };

struct error_hdl
  {
    int errlvl,
        lerrlvl,
#ifdef WBTHREADS
	lockerr,
#endif
        lerror;
    char *filename;
#ifdef WBTHREADS
    void *logparam,
	 *lockparam,
         *unlockparam,
#else
    void *logparam,
#endif
         *file;  /* file is void so we don't have to include stdio.h */
#ifdef WBSTDC
    void (*logfunct)(void *, int, int, char *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *);
# endif
#else
    void (*logfunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)();
# endif
#endif
    struct error_hdls *errorhdls;
    struct errlvl_hdls *errlvlhdls;
  };
typedef struct error_hdl WBERROR;

#ifdef WBPROTOS
  WBERROR *WBErrorOpen(int,
                       char *);

  void WBErrorClose(WBERROR *);

#ifdef WBTHREADS
  void WBErrorLockF(WBERROR *,
		    void *,
                    int (*)(void *));

  void WBErrorUnlockF(WBERROR *,
		      void *,
		      int (*)(void *));
#endif

  int WBErrorErrorF(WBERROR *,
                    int,
                    void *,
                    int (*)(void *, int, int, char *));

  int WBErrorLevelF(WBERROR *,
                    int,
                    void *,
                    void (*)(void *, int, int, char *));

  void WBErrorLogF(WBERROR *,
		   void *,
                   void (*)(void *, int, int, char *));

  int WBErrorLog(WBERROR *,
		 int,
		 int,
                 char *, ...);

  void WBErrorLevel(WBERROR *,
                    int);

  int WBErrorLastError(WBERROR *);

  int WBErrorLastLevel(WBERROR *);
#else
  WBERROR *WBErrorOpen();
  void WBErrorClose();
#ifdef WBTHREADS
  void WBErrorLockF();
  void WBErrorUnlockF();
#endif
  int WBErrorErrorF();
  int WBErrorLevelF();
  void WBErrorLogF();
  int WBErrorLog();
  void WBErrorLevel();
  int WBErrorLastError();
  int WBErrorLastLevel();
#endif

#define wberroropen WBErrorOpen
#define wberrorclose WBErrorClose
#define wberrorlockf WBErrorLockF
#define wberrorunlockf WBErrorUnlockF
#define wberrorerrorf WBErrorErrorF
#define wberrorlevelf WBErrorLevelF
#define wberrorlogf WBErrorLogF
#define wberrorlog WBErrorLog
#define wberrorlevel WBErrorLevel
#define wberrorlasterror WBErrorLastError
#define wberrorlastlevel WBErrorLastLevel

#if defined(__FILE__) && defined(__LINE__) && !defined(WBCOMMENTS)
#  define WBError(aa,bb,cc) \
          WBErrorLog((aa), (bb), (cc), "File: %s, %d\n",__FILE__,__LINE__);
#endif


/*************************************************************************\
**  Exit Functions and Prototypes
**  (For systems that do not support on_exit() or atexit()).
\*************************************************************************/

#ifdef WBSTDC
  void wbexit(int exstatus);
  int wbon_exit(void (*exitproc)(int, void *), void *);
  int wbat_exit(void (*exitproc)(void));
#else
  void wbexit();
  int wbon_exit();
  int wbat_exit();
#endif

#ifdef HAS_NOEXIT
# define exit(a) wbexit(a)
# define on_exit(a,b) wbon_exit(a,b)
# define atexit(a) wbat_exit(a)
#endif


/*************************************************************************\
**  Memory Sructure and Prototypes
\*************************************************************************/

#define WB_OVRRIGHT 0x01
#define WB_OVRLEFT  0x02

struct WBmem_tree
  {
    struct WBmem_tree *left,
		      *right;
    int bal;
    unsigned int flags;
    char *item;
  };

struct mem_struct
  {
    WBERROR *errhdl;
    int logall,
        ovrchk,
#ifdef WBTHREADS
        lockerr,
#endif
        trace;
    void *ovrcheck;
#ifdef WBSTDC
    void *(*alloc)(void *, unsigned int),
         (*free)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    void *(*alloc)(),
         (*free)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
    void *allocparam,
# ifdef WBTHREADS
	 *freeparam,
	 *lockparam,
	 *unlockparam,
         *rlockparam,
         *runlockparam;
# else
         *freeparam;
# endif
    struct WBmem_tree *memtree;
  };
typedef struct mem_struct WBMEM;

#ifdef WBPROTOS
  WBMEM *WBMemoryOpen(WBERROR *,
                      void *,
                      void *(*)(void *, unsigned int),
		      void *,
                      void (*)(void *, void *));

  void WBMemoryClose(WBMEM *, int);

# ifdef WBTHREADS
  void WBMemoryLockF(WBMEM *,
                    void *,
                    int (*)(void *));

  void WBMemoryUnlockF(WBMEM *,
                      void *,
                      int (*)(void *));

  void WBMemoryRLockF(WBMEM *,
                    void *,
                    int (*)(void *));

  void WBMemoryRUnlockF(WBMEM *,
		      void *,
                      int (*)(void *));
# endif

  void *WBMemoryAlloc(WBMEM *,
		      unsigned int,
                      int,
                      char *, ...);

  void WBMemoryFree(WBMEM *,
                    void *,
                    char *, ...);

  int WBMemoryReport(WBMEM *,
                     int);

  void *WBMemorySearch(WBMEM *,
                       void *);

  unsigned int WBMemorySize(WBMEM *,
                            void *);

  void WBMemoryTracing(WBMEM *,
		       int);

  void WBMemoryLogging(WBMEM *,
                       int);

  void *WBMalloc(unsigned int,
                 int,
                 char *, ...);

  void *WBRealloc(void *,
                  unsigned int,
                  int,
                  char *, ...);

  void WBFree(void *,
	      char *, ...);

  int WBMallocReport(int);

  void WBMallocMemH(WBMEM *);

  void WBMallocErrH(WBERROR *);

  void WBMallocLogging(int);

  void WBMallocTracing(int);

# ifdef WBTHREADS
  void WBMallocLockF(void *,
                    int (*)(void *));

  void WBMallocUnlockF(void *,
                      int (*)(void *));

  void WBMallocRLockF(void *,
                    int (*)(void *));

  void WBMallocRUnlockF(void *,
		      int (*)(void *));
# endif

#else
  WBMEM *WBMemoryOpen();
  void WBMemoryClose();
# ifdef WBTHREADS
  void WBMemoryLockF();
  void WBMemoryUnlockF();
  void WBMemoryRLockF();
  void WBMemoryRUnlockF();
# endif
  void *WBMemoryAlloc();
  void WBMemoryFree();
  int WBMemoryReport();
  void *WBMemorySearch();
  unsigned int WBMemorySize();
  void WBMemoryTracing();
  void WBMemoryLogging();
  void *WBMalloc();
  void *WBRealloc();
  void WBFree();
  int WBMallocReport();
  void WBMallocMemH();
  void WBMallocErrH();
  void WBMallocLogging();
  void WBMallocTracing();
# ifdef WBTHREADS
  void WBMallocLockF();
  void WBMallocUnlockF();
  void WBMallocRLockF();
  void WBMallocRUnlockF();
# endif
#endif

#define wbmemoryopen WBMemoryOpen
#define wbmemoryclose WBMemoryClose
#define wbmemorylockf WBMemoryLockF
#define wbmemoryunlockf WBMemoryUnlockF
#define wbmemoryrlockf WBMemoryRLockF
#define wbmemoryrunlockf WBMemoryRUnlockF
#define wbmemoryalloc WBMemoryAlloc
#define wbmemoryfree WBMemoryFree
#define wbmemoryreport WBMemoryReport
#define wbmemorysearch WBMemorySearch
#define wbmemorysize WBMemorySize
#define wbmemorytracing WBMemoryTracing
#define wbmemorylogging WBMemoryLogging

#define wbmalloc WBMalloc
#define wbrealloc WBRealloc
#define wbfree WBFree
#define wbmallocreport WBMallocReport
#define wbmallocmemh WBMallocMemH
#define wbmallocerrh WBMallocErrH
#define wbmalloclogging WBMallocLogging
#define wbmalloctracing WBMallocTracing
#define wbmalloclockf WBMallocLockF
#define wbmallocunlockf WBMallocUnlockF
#define wbmallocrlockf WBMallocRLockF
#define wbmallocrunlockf WBMallocRUnlockF

#if defined(WBMEMTRACE) || defined(WBUSERMEM)
#  define malloc(aa) \
	WBMalloc((aa),0,"malloc(%d): %s, %d\n",(aa),__FILE__,__LINE__)

#  define calloc(aa,bb) \
	WBMalloc(((aa)*(bb)),1,"calloc(%d,%d): %s, %d\n",(aa),(bb),__FILE__,__LINE__)

#    define realloc(aa,bb) \
	  WBRealloc((aa),(bb),0,"realloc(%p,%d): %s, %d\n",(aa),(bb),__FILE__,__LINE__)

#    define free(aa) \
	  WBFree((aa),"free(%p): %s, %d\n",(aa),__FILE__,__LINE__)
#endif

/*************************************************************************\
**  Single Linked List Structure and Prototypes
\*************************************************************************/

struct single_list
  {
    WBERROR *errhdl;
    int isptr,
#ifdef WBTHREADS
	lockerr,
#endif
	update,
	circular;
    long numitems;
    void *copyparam,
	 *cmpparam,
	 *delparam,
# ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
	 *unlockparam,
	 *rlockparam,
	 *runlockparam;
# else
	 *exeparam;
# endif

#ifdef WBSTDC
    int (*cmpfunct)(void *, void *, void *);
    void *(*copyfunct)(void *, void *),
	 (*delfunct)(void *, void *),
	 (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
	(*unlockfunct)(void *),
	(*rlockfunct)(void *),
	(*runlockfunct)(void *);
# endif
#else
    int (*cmpfunct)();
    void *(*copyfunct)(),
	 (*delfunct)(),
	 (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
	(*unlockfunct)(),
	(*rlockfunct)(),
	(*runlockfunct)();
# endif
#endif
    struct single_link *base,
		       *cptr,
		       *lcptr,
		       *endptr,
		       *lendptr,
		       *ptrlist;
    struct single_list *mainlist;
  };
typedef struct single_list WBSLIST;

#ifdef WBPROTOS
  WBSLIST *WBSListOpen(WBERROR *);

  void WBSListClose(WBSLIST *);

#ifdef WBTHREADS
  void WBSListLockF(WBSLIST *,
		    void *,
		    int (*)(void *));

  void WBSListUnlockF(WBSLIST *,
		      void *,
		      int (*)(void *));

  void WBSListRLockF(WBSLIST *,
		    void *,
		    int (*)(void *));

  void WBSListRUnlockF(WBSLIST *,
		      void *,
		      int (*)(void *));
#endif

  void WBSListCopyF(WBSLIST *,
		    void *,
		    void *(*)(void *, void *));

  void WBSListCompareF(WBSLIST *,
		       void *,
		       int (*)(void *, void *, void *));

  void WBSListDeleteF(WBSLIST *,
		      void *,
		      void (*)(void *, void *));

  void WBSListExecuteF(WBSLIST *,
		       void *,
		       void (*)(void *, void *));

  int WBSListAdd(WBSLIST *,
		 void *,
		 int);

  int WBSListDelete(WBSLIST *,
		    int);

  void WBSListCircular(WBSLIST *,
		       int);

  void WBSListExecute(WBSLIST *);

  void *WBSListSearch(WBSLIST *,
		      void *);

  void *WBSListGet(WBSLIST *,
		   int);

  long WBSListNumItems(WBSLIST *);

  WBSLIST *WBSListPointer(WBSLIST *);

  unsigned int WBSListIs(WBSLIST *);

  int WBSListIsPtrEqual(WBSLIST *,
			WBSLIST *);

  void WBSListSetPtrEqual(WBSLIST *,
			  WBSLIST *);

  long WBSListCopy(WBSLIST *,
		   WBSLIST *,
		   WBSLIST *);

  long WBSListCut(WBSLIST *,
		  WBSLIST *,
		  WBSLIST *);

  long WBSListAddList(WBSLIST *,
		      WBSLIST *,
		      int,
		      int);
#else
  WBSLIST *WBSListOpen();
  void WBSListClose();
#ifdef WBTHREADS
  void WBSListLockF();
  void WBSListUnlockF();
  void WBSListRLockF();
  void WBSListRUnlockF();
#endif
  void WBSListCopyF();
  void WBSListCompareF();
  void WBSListDeleteF();
  void WBSListExecuteF();
  int WBSListAdd();
  int WBSListDelete();
  void WBSListCircular();
  void WBSListExecute();
  void *WBSListSearch();
  void *WBSListGet();
  long WBSListNumItems();
  WBSLIST *WBSListPointer();
  unsigned int WBSListIs();
  int WBSListIsPtrEqual();
  void WBSListSetPtrEqual();
  long WBSListCopy();
  long WBSListCut();
  long WBSListAddList();
#endif

#define wbslistopen WBSListOpen
#define wbslistclose WBSListClose
#define wbslistlockf WBSListLockF
#define wbslistunlockf WBSListUnlockF
#define wbslistrlockf WBSListRLockF
#define wbslistrunlockf WBSListRUnlockF
#define wbslistcopyf WBSListCopyF
#define wbslistcomparef WBSListCompareF
#define wbslistdeletef WBSListDeleteF
#define wbslistexecutef WBSListExecuteF
#define wbslistadd WBSListAdd
#define wbslistdelete WBSListDelete
#define wbslistcircular WBSListCircular
#define wbslistexecute WBSListExecute
#define wbslistsearch WBSListSearch
#define wbslistget WBSListGet
#define wbslistnumitems WBSListNumItems
#define wbslistpointer WBSListPointer
#define wbslistis WBSListIs
#define wbslistisptrequal WBSListIsPtrEqual
#define wbslistsetptrequal WBSListSetPtrEqual
#define wbslistcopy WBSListCopy
#define wbslistcut WBSListCut
#define wbslistaddlist WBSListAddList

#define WBSListAddCurrent(a, b) WBSListAdd(a, b, WB_CURRENT)
#define wbslistaddcurrent(a, b) WBSListAdd(a, b, WB_CURRENT)
#define WBSListAddSorted(a,b) WBSListAdd(a,b,WB_SORTED)
#define wbslistaddsorted(a,b) WBSListAdd(a,b,WB_SORTED)
#define WBSListAddFirst(a,b) WBSListAdd(a,b,WB_FIRST)
#define wbslistaddfirst(a,b) WBSListAdd(a,b,WB_FIRST)
#define WBSListAddLast(a,b) WBSListAdd(a,b,WB_LAST)
#define wbslistaddlast(a,b) WBSListAdd(a,b,WB_LAST)
#define WBSListAddPrevious(a,b) WBSListAdd(a,b,WB_PREV)
#define wbslistaddprevious(a,b) WBSListAdd(a,b,WB_PREV)
#define WBSListAddPrev(a,b) WBSListAdd(a,b,WB_PREV)
#define wbslistaddprev(a,b) WBSListAdd(a,b,WB_PREV)
#define WBSListInsert(a,b) WBSListAdd(a,b,WB_PREV)
#define wbslistinsert(a,b) WBSListAdd(a,b,WB_PREV)
#define WBSListAddNext(a,b) WBSListAdd(a,b,WB_NEXT)
#define wbslistaddnext(a,b) WBSListAdd(a,b,WB_NEXT)
#define WBSListAppend(a,b) WBSListAdd(a,b,WB_NEXT)
#define wbslistappend(a,b) WBSListAdd(a,b,WB_NEXT)

#define WBSListIsCircular(a) (a->circular)
#define wbslistiscircular(a) (a->circular)

#define WBSListDelNode(a) WBSListDelete(a,WB_CURRENT)
#define wbslistdelnode(a) WBSListDelete(a,WB_CURRENT)
#define WBSListDelCurrent(a) WBSListDelete(a,WB_CURRENT)
#define wbslistdelcurrent(a) WBSListDelete(a,WB_CURRENT)
#define WBSListDelFirst(a) WBSListDelete(a,WB_FIRST)
#define wbslistdelfirst(a) WBSListDelete(a,WB_FIRST)
#define WBSListDelLast(a) WBSListDelete(a,WB_LAST)
#define wbslistdellast(a) WBSListDelete(a,WB_LAST)
#define WBSListDelNext(a) WBSListDelete(a,WB_NEXT)
#define wbslistdelnext(a) WBSListDelete(a,WB_NEXT)
#define WBSListClear(a) WBSListDelete(a,WB_CLEAR)
#define wbslistclear(a) WBSListDelete(a,WB_CLEAR)

#define WBSListGetEqual(a,b) WBSListSearch(a,b)
#define wbslistgetequal(a,b) WBSListSearch(a,b)
#define WBSListIsMember(a,b) WBSListSearch(a,b)
#define wbslistismember(a,b) WBSListSearch(a,b)
#define WBSListGetCurrent(a) WBSListGet(a,WB_CURRENT)
#define wbslistgetcurrent(a) WBSListGet(a,WB_CURRENT)
#define WBSListGetFirst(a) WBSListGet(a,WB_FIRST)
#define wbslistgetfirst(a) WBSListGet(a,WB_FIRST)
#define WBSListGetLast(a) WBSListGet(a,WB_LAST)
#define wbslistgetlast(a) WBSListGet(a,WB_LAST)
#define WBSListGetNext(a) WBSListGet(a,WB_NEXT)
#define wbslistgetnext(a) WBSListGet(a,WB_NEXT)

#define WBSListIsEmpty(a) (WBSListIS(a) == 0 ? 1 : 0)
#define wbslistisempty(a) (WBSListIS(a) == 0 ? 1 : 0)
#define WBSListIsFirst(a) (ISBITON(WBSListIS(a),WB_FIRST) ? 1 : 0)
#define wbslistisfirst(a) (ISBITON(WBSListIS(a),WB_FIRST) ? 1 : 0)
#define WBSListIsLast(a) (ISBITON(WBSListIS(a),WB_LAST) ? 1 : 0)
#define wbslistislast(a) (ISBITON(WBSListIS(a),WB_LAST) ? 1 : 0)
#define WBSListIsOnBase(a) (ISBITON(WBSListIS(a),WB_FIRST) ? 1 : 0)
#define wbslistisonbase(a) (ISBITON(WBSListIS(a),WB_FIRST) ? 1 : 0)

#define WBSListPaste(a, b, c, d) WBSListAddList(a, b, c, d)
#define wbslistpaste(a, b, c, d) WBSListAddList(a, b, c, d)
#define WBSListPasteFirst(a, b, c) WBSListAddList(a, b, WB_FIRST, c)
#define wbslistpastefirst(a, b, c) WBSListAddList(a, b, WB_FIRST, c)
#define WBSListPasteLast(a, b, c) WBSListAddList(a, b, WB_LAST, c)
#define wbslistpastelast(a, b, c) WBSListAddList(a, b, WB_LAST, c)
#define WBSListPasteSorted(a, b, c) WBSListAddList(a, b, WB_SORTED, c)
#define wbslistpastesorted(a, b, c) WBSListAddList(a, b, WB_SORTED, c)
#define WBSListPastePrevious(a, b, c) WBSListAddList(a, b, WB_PREV, c)
#define wbslistpasteprevious(a, b, c) WBSListAddList(a, b, WB_PREV, c)
#define WBSListPastePrev(a, b, c) WBSListAddList(a, b, WB_PREV, c)
#define wbslistpasteprev(a, b, c) WBSListAddList(a, b, WB_PREV, c)
#define WBSListPasteNext(a, b, c) WBSListAddList(a, b, WB_NEXT, c)
#define wbslistpastenext(a, b, c) WBSListAddList(a, b, WB_NEXT, c)


/*************************************************************************\
**  Double Linked List Structure and Prototypes
\*************************************************************************/

struct double_list
  {
    WBERROR *errhdl;
    int isptr,
#ifdef WBTHREADS
	lockerr,
#endif
	update,
	circular;
    long numitems;
    void *copyparam,
	 *cmpparam,
	 *delparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
	 *unlockparam,
	 *rlockparam,
	 *runlockparam;
#else
	 *exeparam;
#endif

#ifdef WBSTDC
    int (*cmpfunct)(void *, void *, void *);
    void *(*copyfunct)(void *, void *),
	 (*delfunct)(void *, void *),
	 (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
	(*unlockfunct)(void *),
	(*rlockfunct)(void *),
	(*runlockfunct)(void *);
# endif
#else
    int (*cmpfunct)();
    void *(*copyfunct)(),
	 (*delfunct)(),
	 (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
	(*unlockfunct)(),
	(*rlockfunct)(),
	(*runlockfunct)();
# endif
#endif
    struct double_link *base,
		       *cptr,
		       *endptr;
    struct single_link *ptrlist;
    struct double_list *mainlist;
  };
typedef struct double_list WBDLIST;

#ifdef WBPROTOS
  WBDLIST *WBDListOpen(WBERROR *);

  void WBDListClose(WBDLIST *);

#ifdef WBTHREADS
  void WBDListLockF(WBDLIST *,
		    void *,
		    int (*)(void *));

  void WBDListUnlockF(WBDLIST *,
		      void *,
		      int (*)(void *));

  void WBDListRLockF(WBDLIST *,
		    void *,
		    int (*)(void *));

  void WBDListRUnlockF(WBDLIST *,
		      void *,
		      int (*)(void *));
#endif

  void WBDListCopyF(WBDLIST *,
		    void *,
		    void *(*)(void *, void *));

  void WBDListCompareF(WBDLIST *,
		       void *,
		       int (*)(void *, void *, void *));

  void WBDListDeleteF(WBDLIST *,
		      void *,
		      void (*)(void *, void *));

  void WBDListExecuteF(WBDLIST *,
		       void *,
		       void (*)(void *, void *));

  int WBDListAdd(WBDLIST *,
		 void *,
		 int);

  int WBDListDelete(WBDLIST *,
		    int);

  void WBDListCircular(WBDLIST *,
		       int);

  void WBDListExecute(WBDLIST *);

  void *WBDListSearch(WBDLIST *,
		      void *);

  void *WBDListGet(WBDLIST *,
		   int);

  long WBDListNumItems(WBDLIST *);

  WBDLIST *WBDListPointer(WBDLIST *);

  unsigned int WBDListIs(WBDLIST *);

  int WBDListIsPtrEqual(WBDLIST *,
			WBDLIST *);

  void WBDListSetPtrEqual(WBDLIST *,
			  WBDLIST *);

  long WBDListCopy(WBDLIST *,
		   WBDLIST *,
		   WBDLIST *);

  long WBDListCut(WBDLIST *,
		  WBDLIST *,
		  WBDLIST *);

  long WBDListAddList(WBDLIST *,
		      WBDLIST *,
		      int,
		      int);
#else
  WBDLIST *WBDListOpen();
  void WBDListClose();
#ifdef WBTHREADS
  void WBDListLockF();
  void WBDListUnlockF();
  void WBDListRLockF();
  void WBDListRUnlockF();
#endif
  void WBDListCopyF();
  void WBDListCompareF();
  void WBDListDeleteF();
  void WBDListExecuteF();
  int WBDListAdd();
  int WBDListDelete();
  void WBDListCircular();
  void WBDListExecute();
  void *WBDListSearch();
  void *WBDListGet();
  long WBDListNumItems();
  WBDLIST *WBDListPointer();
  unsigned int WBDListIs();
  int WBDListIsPtrEqual();
  void WBDListSetPtrEqual();
  long WBDListCopy();
  long WBDListCut();
  long WBDListAddList();
#endif

#define wbdlistopen WBDListOpen
#define wbdlistclose WBDListClose
#define wbdlistlockf WBDListLockF
#define wbdlistunlockf WBDListUnlockF
#define wbdlistrlockf WBDListRLockF
#define wbdlistrunlockf WBDListRUnlockF
#define wbdlistcopyf WBDListCopyF
#define wbdlistcomparef WBDListCompareF
#define wbdlistdeletef WBDListDeleteF
#define wbdlistexecutef WBDListExecuteF
#define wbdlistadd WBDListAdd
#define wbdlistdelete WBDListDelete
#define wbdlistcircular WBDListCircular
#define wbdlistexecute WBDListExecute
#define wbdlistsearch WBDListSearch
#define wbdlistget WBDListGet
#define wbdlistnumitems WBDListNumItems
#define wbdlistpointer WBDListPointer
#define wbdlistis WBDListIs
#define wbdlistisptrequal WBDListIsPtrEqual
#define wbdlistsetptrequal WBDListSetPtrEqual
#define wbdlistcopy WBDListCopy
#define wbdlistcut WBDListCut
#define wbdlistaddlist WBDListAddList

#define WBDListAddCurrent(a, b) WBDListAdd(a, b, WB_CURRENT)
#define wbdlistaddcurrent(a, b) WBDListAdd(a, b, WB_CURRENT)
#define WBDListAddSorted(a, b) WBDListAdd(a, b, WB_SORTED)
#define wbdlistaddsorted(a, b) WBDListAdd(a, b, WB_SORTED)
#define WBDListAddFirst(a, b) WBDListAdd(a, b, WB_FIRST)
#define wbdlistaddfirst(a, b) WBDListAdd(a, b, WB_FIRST)
#define WBDListAddLast(a, b) WBDListAdd(a, b, WB_LAST)
#define wbdlistaddlast(a, b) WBDListAdd(a, b, WB_LAST)
#define WBDListAddPrevious(a, b) WBDListAdd(a, b, WB_PREV)
#define wbdlistaddprevious(a, b) WBDListAdd(a, b, WB_PREV)
#define WBDListAddPrev(a, b) WBDListAdd(a, b, WB_PREV)
#define wbdlistaddprev(a, b) WBDListAdd(a, b, WB_PREV)
#define WBDListInsert(a, b) WBDListAdd(a, b, WB_PREV)
#define wbdlistinsert(a, b) WBDListAdd(a, b, WB_PREV)
#define WBDListAddNext(a, b) WBDListAdd(a, b, WB_NEXT)
#define wbdlistaddnext(a, b) WBDListAdd(a, b, WB_NEXT)
#define WBDListAppend(a, b) WBDListAdd(a, b, WB_NEXT)
#define wbdlistappend(a, b) WBDListAdd(a, b, WB_NEXT)

#define WBDListIsCircular(a) (a->circular)
#define wbdlistiscircular(a) (a->circular)

#define WBDListDelNode(a) WBDListDelete(a,WB_CURRENT)
#define wbdlistdelnode(a) WBDListDelete(a,WB_CURRENT)
#define WBDListDelCurrent(a) WBDListDelete(a,WB_CURRENT)
#define wbdlistdelcurrent(a) WBDListDelete(a,WB_CURRENT)
#define WBDListDelFirst(a) WBDListDelete(a,WB_FIRST)
#define wbdlistdelfirst(a) WBDListDelete(a,WB_FIRST)
#define WBDListDelLast(a) WBDListDelete(a,WB_LAST)
#define wbdlistdellast(a) WBDListDelete(a,WB_LAST)
#define WBDListDelPrevious(a) WBDListDelete(a,WB_PREV)
#define wbdlistdelprevious(a) WBDListDelete(a,WB_PREV)
#define WBDListDelPrev(a) WBDListDelete(a,WB_PREV)
#define wbdlistdelprev(a) WBDListDelete(a,WB_PREV)
#define WBDListDelNext(a) WBDListDelete(a,WB_NEXT)
#define wbdlistdelnext(a) WBDListDelete(a,WB_NEXT)
#define WBDListClear(a) WBDListDelete(a,WB_CLEAR)
#define wbdlistclear(a) WBDListDelete(a,WB_CLEAR)

#define WBDListGetEqual(a,b) WBDListSearch(a,b)
#define wbdlistgetequal(a,b) WBDListSearch(a,b)
#define WBDListIsMember(a,b) WBDListSearch(a,b)
#define wbdlistismember(a,b) WBDListSearch(a,b)
#define WBDListGetCurrent(a) WBDListGet(a,WB_CURRENT)
#define wbdlistgetcurrent(a) WBDListGet(a,WB_CURRENT)
#define WBDListGetFirst(a) WBDListGet(a,WB_FIRST)
#define wbdlistgetfirst(a) WBDListGet(a,WB_FIRST)
#define WBDListGetLast(a) WBDListGet(a,WB_LAST)
#define wbdlistgetlast(a) WBDListGet(a,WB_LAST)
#define WBDListGetPrevious(a) WBDListGet(a,WB_PREV)
#define wbdlistgetprevious(a) WBDListGet(a,WB_PREV)
#define WBDListGetPrev(a) WBDListGet(a,WB_PREV)
#define wbdlistgetprev(a) WBDListGet(a,WB_PREV)
#define WBDListGetNext(a) WBDListGet(a,WB_NEXT)
#define wbdlistgetnext(a) WBDListGet(a,WB_NEXT)

#define WBDListIsEmpty(a) (WBDListIS(a) == 0 ? 1 : 0)
#define wbdlistisempty(a) (WBDListIS(a) == 0 ? 1 : 0)
#define WBDListIsFirst(a) (ISBITON(WBDListIS(a),WB_FIRST) ? 1 : 0)
#define wbdlistisfirst(a) (ISBITON(WBDListIS(a),WB_FIRST) ? 1 : 0)
#define WBDListIsLast(a) (ISBITON(WBDListIS(a),WB_LAST) ? 1 : 0)
#define wbdlistislast(a) (ISBITON(WBDListIS(a),WB_LAST) ? 1 : 0)
#define WBDListIsOnBase(a) (ISBITON(WBDListIS(a),WB_FIRST) ? 1 : 0)
#define wbdlistisonbase(a) (ISBITON(WBDListIS(a),WB_FIRST) ? 1 : 0)

#define WBDListPaste(a, b, c, d) WBDListAddList(a, b, c, d)
#define wbdlistpaste(a, b, c, d) WBDListAddList(a, b, c, d)
#define WBDListPasteFirst(a, b, c) WBDListAddList(a, b, WB_FIRST, c)
#define wbdlistpastefirst(a, b, c) WBDListAddList(a, b, WB_FIRST, c)
#define WBDListPasteLast(a, b, c) WBDListAddList(a, b, WB_LAST, c)
#define wbdlistpastelast(a, b, c) WBDListAddList(a, b, WB_LAST, c)
#define WBDListPasteSorted(a, b, c) WBDListAddList(a, b, WB_SORTED, c)
#define wbdlistpastesorted(a, b, c) WBDListAddList(a, b, WB_SORTED, c)
#define WBDListPastePrevious(a, b, c) WBDListAddList(a, b, WB_PREV, c)
#define wbdlistpasteprevious(a, b, c) WBDListAddList(a, b, WB_PREV, c)
#define WBDListPastePrev(a, b, c) WBDListAddList(a, b, WB_PREV, c)
#define wbdlistpasteprev(a, b, c) WBDListAddList(a, b, WB_PREV, c)
#define WBDListPasteNext(a, b, c) WBDListAddList(a, b, WB_NEXT, c)
#define wbdlistpastenext(a, b, c) WBDListAddList(a, b, WB_NEXT, c)


/*************************************************************************\
**  Multi-Dimensional Array Prototypes
\*************************************************************************/

struct array_struct
  {
    WBERROR *errhdl;
    char *dimensions;
    void *array;
  };
typedef struct array_struct WBARRAY;

#ifdef WBPROTOS
  WBARRAY *WBArrayOpen(WBERROR *,
                       char *);

  void WBArrayClose(WBARRAY *);

  void *WBArrayArray(WBARRAY *);
#else
  WBARRAY *WBArrayOpen();
  void WBArrayClose();
  void *WBArrayArray();
#endif

#define wbarrayopen WBArrayOpen
#define wbarrayclose WBArrayClose
#define wbarrayarray WBArrayArray


/*************************************************************************\
**  Stack Structure and Prototypes
\*************************************************************************/

struct stack_struct
  {
    WBERROR *errhdl;
    unsigned int itemsize;
    int linked,
#ifdef WBTHREADS
        lockerr,
#endif
        copyitem;
#ifdef WBTHREADS
    void *delparam,
         *lockparam,
         *unlockparam,
         *rlockparam,
         *runlockparam;
#else
    void *delparam;
#endif

#ifdef WBSTDC
    void (*delfunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    void (*delfunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
    long numitems;
    union stk_union
      {
       struct stk_array
	 {
           unsigned int arraysize,
                        stkptr;
           void *array;
         } stkarray;
       struct stk_list
         {
           struct single_link *baseptr;
         } stklist;
      } stkunion;
  };
typedef struct stack_struct WBSTACK;

#ifdef WBPROTOS
  WBSTACK *WBStackOpen(WBERROR *,
                       unsigned int,
                       unsigned int,
                       int);

  void WBStackClose(WBSTACK *);

#ifdef WBTHREADS
  void WBStackLockF(WBSTACK *,
                    void *,
                    int (*)(void *));

  void WBStackUnlockF(WBSTACK *,
                      void *,
                      int (*)(void *));

  void WBStackRLockF(WBSTACK *,
                    void *,
                    int (*)(void *));

  void WBStackRUnlockF(WBSTACK *,
                      void *,
                      int (*)(void *));
#endif

  void WBStackDeleteF(WBSTACK *,
                      void *,
                      void (*)(void *, void *));

  int WBStackPush(WBSTACK *,
                  void *);

  void *WBStackPop(WBSTACK *,
                   void *);

  void *WBStackPeek(WBSTACK *,
		    void *);

  long WBStackNumItems(WBSTACK *);

  long WBStackMaxItems(WBSTACK *);

  void WBStackClear(WBSTACK *);

  int WBStackIsEmpty(WBSTACK *);
#else
  WBSTACK *WBStackOpen();
  void WBStackClose();
#ifdef WBTHREADS
  void WBStackLockF();
  void WBStackUnlockF();
  void WBStackRLockF();
  void WBStackRUnlockF();
#endif
  void WBStackDeleteF();
  int WBStackPush();
  void *WBStackPop();
  void *WBStackPeek();
  long WBStackNumItems();
  long WBStackMaxItems();
  void WBStackClear();
  int WBStackIsEmpty();
#endif

#define wbstackopen WBStackOpen
#define wbstackclose WBStackClose
#define wbstacklockf WBStackLockF
#define wbstackunlockf WBStackUnlockF
#define wbstackrlockf WBStackRLockF
#define wbstackrunlockf WBStackRUnlockF
#define wbstackdeletef WBStackDeleteF
#define wbstackpush WBStackPush
#define wbstackpop WBStackPop
#define wbstackpeek WBStackPeek
#define wbstacknumitems WBStackNumItems
#define wbstackmaxitems WBStackMaxItems
#define wbstackclear WBStackClear
#define wbstackisempty WBStackIsEmpty


/*************************************************************************\
**  Queue Structure and Prototypes
\*************************************************************************/

struct queue_struct
  {
    WBERROR *errhdl;
    unsigned int itemsize;
    int linked,
#ifdef WBTHREADS
        lockerr,
#endif
	copyitem;
#ifdef WBTHREADS
    void *delparam,
         *lockparam,
	 *unlockparam,
         *rlockparam,
         *runlockparam;
#else
    void *delparam;
#endif

#ifdef WBSTDC
    void (*delfunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    void (*delfunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
    long numitems;
    union que_union
      {
       struct que_array
         {
           unsigned int arraysize,
                        numitems,
                        bqueptr,
                        equeptr;
           void *array;
	 } quearray;
       struct que_list
         {
           struct single_link *baseptr,
                               *endptr;
         } quelist;
      } queunion;
  };
typedef struct queue_struct WBQUEUE;

#ifdef WBPROTOS
  WBQUEUE *WBQueueOpen(WBERROR *,
                       unsigned int,
                       unsigned int,
                       int);

  void WBQueueClose(WBQUEUE *);

#ifdef WBTHREADS
  void WBQueueLockF(WBQUEUE *,
                    void *,
                    int (*)(void *));

  void WBQueueUnlockF(WBQUEUE *,
                      void *,
                      int (*)(void *));

  void WBQueueRLockF(WBQUEUE *,
                    void *,
                    int (*)(void *));

  void WBQueueRUnlockF(WBQUEUE *,
                      void *,
                      int (*)(void *));
#endif

  void WBQueueDeleteF(WBQUEUE *,
                      void *,
		      void (*)(void *, void *));

  int WBQueuePush(WBQUEUE *,
		  void *);

  void *WBQueuePop(WBQUEUE *,
                   void *);

  void *WBQueuePeek(WBQUEUE *,
		    void *);

  long WBQueueNumItems(WBQUEUE *);

  long WBQueueMaxItems(WBQUEUE *);

  void WBQueueClear(WBQUEUE *);

  int WBQueueIsEmpty(WBQUEUE *);
#else
  WBQUEUE *WBQueueOpen();
  void WBQueueClose();
#ifdef WBTHREADS
  void WBQueueLockF();
  void WBQueueUnlockF();
  void WBQueueRLockF();
  void WBQueueRUnlockF();
#endif
  void WBQueueDeleteF();
  int WBQueuePush();
  void *WBQueuePop();
  void *WBQueuePeek();
  long WBQueueNumItems();
  long WBQueueMaxItems();
  void WBQueueClear();
  int WBQueueIsEmpty();
#endif

#define wbqueueopen WBQueueOpen
#define wbqueueclose WBQueueClose
#define wbqueuelockf WBQueueLockF
#define wbqueueunlockf WBQueueUnlockF
#define wbqueuerlockf WBQueueRLockF
#define wbqueuerunlockf WBQueueRUnlockF
#define wbqueuedeletef WBQueueDeleteF
#define wbqueuepush WBQueuePush
#define wbqueuepop WBQueuePop
#define wbqueuepeek WBQueuePeek
#define wbqueuenumitems WBQueueNumItems
#define wbqueuemaxitems WBQueueMaxItems
#define wbqueueclear WBQueueClear
#define wbqueueisempty WBQueueIsEmpty


/*************************************************************************\
**  Dequeue Structure and Prototypes
\*************************************************************************/

struct dqueue_struct
  {
    WBERROR *errhdl;
    unsigned int itemsize;
    int linked,
#ifdef WBTHREADS
        lockerr,
#endif
	copyitem;
#ifdef WBTHREADS
    void *delparam,
         *lockparam,
         *unlockparam,
         *rlockparam,
         *runlockparam;
#else
    void *delparam;
#endif

#ifdef WBSTDC
    void (*delfunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    void (*delfunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
    long numitems;
    union dque_union
      {
       struct dque_array
         {
           unsigned int arraysize,
                        numitems,
                        bdqueptr,
                        edqueptr;
	   void *array;
	 } dquearray;
       struct dque_list
         {
           struct double_link *baseptr,
                              *endptr;
         } dquelist;
      } dqueunion;
  };
typedef struct dqueue_struct WBDQUEUE;

#ifdef WBPROTOS
  WBDQUEUE *WBDQueueOpen(WBERROR *,
                         unsigned int,
                         unsigned int,
                         int);

  void WBDQueueClose(WBDQUEUE *);

#ifdef WBTHREADS
  void WBDQueueLockF(WBDQUEUE *,
                     void *,
                     int (*)(void *));

  void WBDQueueUnlockF(WBDQUEUE *,
                       void *,
                       int (*)(void *));

  void WBDQueueRLockF(WBDQUEUE *,
                     void *,
                     int (*)(void *));

  void WBDQueueRUnlockF(WBDQUEUE *,
                       void *,
                       int (*)(void *));
#endif

  void WBDQueueDeleteF(WBDQUEUE *,
		       void *,
		       void (*)(void *, void *));

  int WBDQueuePushL(WBDQUEUE *,
		    void *);

  int WBDQueuePushR(WBDQUEUE *,
		    void *);

  void *WBDQueuePopL(WBDQUEUE *,
		     void *);

  void *WBDQueuePopR(WBDQUEUE *,
                     void *);

  void *WBDQueuePeekL(WBDQUEUE *,
		     void *);

  void *WBDQueuePeekR(WBDQUEUE *,
		     void *);

  void WBDQueueClear(WBDQUEUE *);

  long WBDQueueNumItems(WBDQUEUE *);

  long WBDQueueMaxItems(WBDQUEUE *);

  int WBDQueueIsEmpty(WBDQUEUE *);
#else
  WBDQUEUE *WBDQueueOpen();
  void WBDQueueClose();
#ifdef WBTHREADS
  void WBDQueueLockF();
  void WBDQueueUnlockF();
  void WBDQueueRLockF();
  void WBDQueueRUnlockF();
#endif
  void WBDQueueDeleteF();
  int WBDQueuePushL();
  int WBDQueuePushR();
  void *WBDQueuePopL();
  void *WBDQueuePopR();
  void *WBDQueuePeekL();
  void *WBDQueuePeekR();
  void WBDQueueClear();
  long WBDQueueNumItems();
  long WBDQueueMaxItems();
  int WBDQueueIsEmpty();
#endif

#define wbdqueueopen WBDQueueOpen
#define wbdqueueclose WBDQueueClose
#define wbdqueuelockf WBDQueueLockF
#define wbdqueueunlockf WBDQueueUnlockF
#define wbdqueuerlockf WBDQueueRLockF
#define wbdqueuerunlockf WBDQueueRUnlockF
#define wbdqueuedeletef WBDQueueDeleteF
#define wbdqueuepushl WBDQueuePushL
#define wbdqueuepushr WBDQueuePushR
#define wbdqueuepopl WBDQueuePopL
#define wbdqueuepopr WBDQueuePopR
#define wbdqueuepeekl WBDQueuePeekL
#define wbdqueuepeekr WBDQueuePeekR
#define wbdqueueclear WBDQueueClear
#define wbdqueuenumitems WBDQueueNumItems
#define wbdqueuemaxitems WBDQueueMaxItems
#define wbdqueueisempty WBDQueueIsEmpty


/*************************************************************************\
**  Binary Tree Structure and Prototypes
\*************************************************************************/

struct tree_struct
  {
    WBERROR *errhdl;
#ifdef WBTHREADS
    int duplicates,
        lockerr;
#else
    int duplicates;
#endif
    long numitems;
    void *cmpparam,
         *delparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
         *unlockparam,
         *rlockparam,
         *runlockparam;
#else
         *exeparam;
#endif

#ifdef WBSTDC
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
         (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    int (*cmpfunct)();
    void (*delfunct)(),
         (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
    struct binary_tree *root;
  };
typedef struct tree_struct WBTREE;

#ifdef WBPROTOS
  WBTREE *WBTreeOpen(WBERROR *);

  void WBTreeClose(WBTREE *);

  void WBTreeDuplicates(WBTREE *,
                        int);

#ifdef WBTHREADS
  void WBTreeLockF(WBTREE *,
                   void *,
                   int (*)(void *));

  void WBTreeUnlockF(WBTREE *,
                     void *,
                     int (*)(void *));

  void WBTreeRLockF(WBTREE *,
                   void *,
                   int (*)(void *));

  void WBTreeRUnlockF(WBTREE *,
		     void *,
                     int (*)(void *));
#endif

  void WBTreeCompareF(WBTREE *,
                      void *,
                      int (*)(void *, void *, void *));

  void WBTreeDeleteF(WBTREE *,
                     void *,
		     void (*)(void *, void *));

  void WBTreeExecuteF(WBTREE *,
                      void *,
                      void (*)(void *, void *));

  int WBTreeAdd(WBTREE *,
                void *);

  int WBTreeDelete(WBTREE *,
                   void *);

  void WBTreeClear(WBTREE *);

  void *WBTreeSearch(WBTREE *,
                     void *);

  void WBTreePreOrder(WBTREE *);

  void WBTreeInOrder(WBTREE *);

  void WBTreePostOrder(WBTREE *);

  long WBTreeNumItems(WBTREE *);

  int WBTreeIsEmpty(WBTREE *);
#else
  WBTREE *WBTreeOpen();
  void WBTreeClose();
  void WBTreeDuplicates();
#ifdef WBTHREADS
  void WBTreeLockF();
  void WBTreeUnlockF();
  void WBTreeRLockF();
  void WBTreeRUnlockF();
#endif
  void WBTreeCompareF();
  void WBTreeDeleteF();
  void WBTreeExecuteF();
  int WBTreeAdd();
  int WBTreeDelete();
  void WBTreeClear();
  void *WBTreeSearch();
  void WBTreePreOrder();
  void WBTreeInOrder();
  void WBTreePostOrder();
  long WBTreeNumItems();
  int WBTreeIsEmpty();
#endif

#define wbtreeopen WBTreeOpen
#define wbtreeclose WBTreeClose
#define wbtreeduplicates WBTreeDuplicates
#define wbtreelockf WBTreeLockF
#define wbtreeunlockf WBTreeUnlockF
#define wbtreerlockf WBTreeRLockF
#define wbtreerunlockf WBTreeRUnlockF
#define wbtreecomparef WBTreeCompareF
#define wbtreedeletef WBTreeDeleteF
#define wbtreeexecutef WBTreeExecuteF
#define wbtreeadd WBTreeAdd
#define wbtreedelete WBTreeDelete
#define wbtreeclear WBTreeClear
#define wbtreesearch WBTreeSearch
#define wbtreepreorder WBTreePreOrder
#define wbtreeinorder WBTreeInOrder
#define wbtreepostorder WBTreePostOrder
#define wbtreenumitems WBTreeNumItems
#define wbtreeisempty WBTreeIsEmpty


/*************************************************************************\
**  Threaded Binary Tree Structure and Prototypes
\*************************************************************************/

struct ttree_struct
  {
    WBERROR *errhdl;
#ifdef WBTHREADS
    int duplicates,
        lockerr;
#else
    int duplicates;
#endif
    long numitems;
    void *cmpparam,
         *delparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
         *unlockparam,
         *rlockparam,
         *runlockparam;
#else
         *exeparam;
#endif

#ifdef WBSTDC
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
         (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
	(*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    int (*cmpfunct)();
    void (*delfunct)(),
         (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
	(*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
    struct threaded_tree *root,
                         *cptr;
  };
typedef struct ttree_struct WBTTREE;

#ifdef WBPROTOS
  WBTTREE *WBTTreeOpen(WBERROR *);

  void WBTTreeClose(WBTTREE *);

  void WBTTreeDuplicates(WBTTREE *,
                         int);

#ifdef WBTHREADS
  void WBTTreeLockF(WBTTREE *,
		    void *,
                    int (*)(void *));

  void WBTTreeUnlockF(WBTTREE *,
                      void *,
                      int (*)(void *));

  void WBTTreeRLockF(WBTTREE *,
                    void *,
		    int (*)(void *));

  void WBTTreeRUnlockF(WBTTREE *,
                      void *,
                      int (*)(void *));
#endif

  void WBTTreeCompareF(WBTTREE *,
                       void *,
                       int (*)(void *, void *, void *));

  void WBTTreeDeleteF(WBTTREE *,
                      void *,
                      void (*)(void *, void *));

  void WBTTreeExecuteF(WBTTREE *,
                       void *,
                       void (*)(void *, void *));

  int WBTTreeAdd(WBTTREE *,
                 void *);

  int WBTTreeDelete(WBTTREE *,
                    void *);

  void WBTTreeClear(WBTTREE *);

  void *WBTTreeGetCurrent(WBTTREE *);

  void *WBTTreeGetFirst(WBTTREE *);

  void *WBTTreeGetLast(WBTTREE *);

  void *WBTTreeGetNext(WBTTREE *);

  void *WBTTreeGetPrev(WBTTREE *);

  void *WBTTreeGetEqual(WBTTREE *,
			void *);

  void *WBTTreeGetEqGr(WBTTREE *,
                       void *);

  void *WBTTreeGetGr(WBTTREE *,
                     void *);

  void *WBTTreeGetEqLs(WBTTREE *,
                       void *);

  void *WBTTreeGetLs(WBTTREE *,
                     void *);

  void WBTTreeExecute(WBTTREE *);

  long WBTTreeNumItems(WBTTREE *);

  int WBTTreeIsEmpty(WBTTREE *);
#else
  WBTTREE *WBTTreeOpen();
  void WBTTreeClose();
  void WBTTreeDuplicates();
#ifdef WBTHREADS
  void WBTTreeLockF();
  void WBTTreeUnlockF();
  void WBTTreeRLockF();
  void WBTTreeRUnlockF();
#endif
  void WBTTreeCompareF();
  void WBTTreeDeleteF();
  void WBTTreeExecuteF();
  int WBTTreeAdd();
  int WBTTreeDelete();
  void WBTTreeClear();
  void *WBTTreeGetCurrent();
  void *WBTTreeGetFirst();
  void *WBTTreeGetLast();
  void *WBTTreeGetNext();
  void *WBTTreeGetPrev();
  void *WBTTreeGetEqual();
  void *WBTTreeGetEqGr();
  void *WBTTreeGetGr();
  void *WBTTreeGetEqLs();
  void *WBTTreeGetLs();
  void WBTTreeExecute();
  long WBTTreeNumItems();
  int WBTTreeIsEmpty();
#endif

#define wbttreeopen WBTTreeOpen
#define wbttreeclose WBTTreeClose
#define wbttreeduplicates WBTTreeDuplicates
#define wbttreelockf WBTTreeLockF
#define wbttreeunlockf WBTTreeUnlockF
#define wbttreerlockf WBTTreeRLockF
#define wbttreerunlockf WBTTreeRUnlockF
#define wbttreecomparef WBTTreeCompareF
#define wbttreedeletef WBTTreeDeleteF
#define wbttreeexecutef WBTTreeExecuteF
#define wbttreeadd WBTTreeAdd
#define wbttreedelete WBTTreeDelete
#define wbttreeclear WBTTreeClear
#define wbttreegetcurrent WBTTreeGetCurrent
#define wbttreegetfirst WBTTreeGetFirst
#define wbttreegetlast WBTTreeGetLast
#define wbttreegetnext WBTTreeGetNext
#define wbttreegetprev WBTTreeGetPrev
#define wbttreegetequal WBTTreeGetEqual
#define wbttreegeteqgr WBTTreeGetEqGr
#define wbttreegetgr WBTTreeGetGr
#define wbttreegeteqls WBTTreeGetEqLs
#define wbttreegetls WBTTreeGetLs
#define wbttreeexecute WBTTreeExecute
#define wbttreenumitems WBTTreeNumItems
#define wbttreeisempty WBTTreeIsEmpty


/*************************************************************************\
**  AVL Tree Structure and Prototypes
\*************************************************************************/

struct avltree_st
  {
    WBERROR *errhdl;
#ifdef WBTHREADS
    int duplicates,
        lockerr;
#else
    int duplicates;
#endif
    long numitems;
    void *cmpparam,
         *delparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
	 *unlockparam,
	 *rlockparam,
	 *runlockparam;
#else
	 *exeparam;
#endif

#ifdef WBSTDC
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
         (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    int (*cmpfunct)();
    void (*delfunct)(),
         (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
    struct avl_tree *root;
  };
typedef struct avltree_st WBAVL;

#ifdef WBPROTOS
  WBAVL *WBAVLOpen(WBERROR *);

  void WBAVLClose(WBAVL *);

  void WBAVLDuplicates(WBAVL *,
                       int);

#ifdef WBTHREADS
  void WBAVLLockF(WBAVL *,
                  void *,
                  int (*)(void *));

  void WBAVLUnlockF(WBAVL *,
		    void *,
                    int (*)(void *));

  void WBAVLRLockF(WBAVL *,
                  void *,
                  int (*)(void *));

  void WBAVLRUnlockF(WBAVL *,
                    void *,
		    int (*)(void *));
#endif

  void WBAVLCompareF(WBAVL *,
                     void *,
                     int (*)(void *, void *, void *));

  void WBAVLDeleteF(WBAVL *,
                    void *,
                    void (*)(void *, void *));

  void WBAVLExecuteF(WBAVL *,
                     void *,
                     void (*)(void *, void *));

  int WBAVLAdd(WBAVL *,
               void *);

  int WBAVLDelete(WBAVL *,
		  void *);

  void WBAVLClear(WBAVL *);

  void *WBAVLSearch(WBAVL *,
                    void *);

  void WBAVLPreOrder(WBAVL *);

  void WBAVLInOrder(WBAVL *);

  void WBAVLPostOrder(WBAVL *);

  long WBAVLNumItems(WBAVL *);

  int WBAVLIsEmpty(WBAVL *);
#else
  WBAVL *WBAVLOpen();
  void WBAVLClose();
  void WBAVLDuplicates();
#ifdef WBTHREADS
  void WBAVLLockF();
  void WBAVLUnlockF();
  void WBAVLRLockF();
  void WBAVLRUnlockF();
#endif
  void WBAVLCompareF();
  void WBAVLDeleteF();
  void WBAVLExecuteF();
  int WBAVLAdd();
  int WBAVLDelete();
  void WBAVLClear();
  void *WBAVLSearch();
  void WBAVLPreOrder();
  void WBAVLInOrder();
  void WBAVLPostOrder();
  long WBAVLNumItems();
  int WBAVLIsEmpty();
#endif

#define wbavlopen WBAVLOpen
#define wbavlclose WBAVLClose
#define wbavlduplicates WBAVLDuplicates
#define wbavllockf WBAVLLockF
#define wbavlunlockf WBAVLUnlockF
#define wbavlrlockf WBAVLRLockF
#define wbavlrunlockf WBAVLRUnlockF
#define wbavlcomparef WBAVLCompareF
#define wbavldeletef WBAVLDeleteF
#define wbavlexecutef WBAVLExecuteF
#define wbavladd WBAVLAdd
#define wbavldelete WBAVLDelete
#define wbavlclear WBAVLClear
#define wbavlsearch WBAVLSearch
#define wbavlpreorder WBAVLPreOrder
#define wbavlinorder WBAVLInOrder
#define wbavlpostorder WBAVLPostOrder
#define wbavlnumitems WBAVLNumItems
#define wbavlisempty WBAVLIsEmpty


/*************************************************************************\
**  Threaded AVL Tree Structure and Prototypes
\*************************************************************************/

struct tavltree_st
  {
    WBERROR *errhdl;
#ifdef WBTHREADS
    int duplicates,
        lockerr;
#else
    int duplicates;
#endif
    long numitems;
    void *cmpparam,
	 *delparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
         *unlockparam,
         *rlockparam,
         *runlockparam;
#else
         *exeparam;
#endif

#ifdef WBSTDC
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
         (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
	(*runlockfunct)(void *);
# endif
#else
    int (*cmpfunct)();
    void (*delfunct)(),
         (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
	(*runlockfunct)();
# endif
#endif
    struct tavl_tree *root,
                     *cptr;
  };
typedef struct tavltree_st WBTAVL;

#ifdef WBPROTOS
  WBTAVL *WBTAVLOpen(WBERROR *);

  void WBTAVLClose(WBTAVL *);

  void WBTAVLDuplicates(WBTAVL *,
                        int);

#ifdef WBTHREADS
  void WBTAVLLockF(WBTAVL *,
                   void *,
		   int (*)(void *));

  void WBTAVLUnlockF(WBTAVL *,
                     void *,
                     int (*)(void *));

  void WBTAVLRLockF(WBTAVL *,
                   void *,
                   int (*)(void *));

  void WBTAVLRUnlockF(WBTAVL *,
                     void *,
                     int (*)(void *));
#endif

  void WBTAVLCompareF(WBTAVL *,
                      void *,
                      int (*)(void *, void *, void *));

  void WBTAVLDeleteF(WBTAVL *,
                     void *,
                     void (*)(void *, void *));

  void WBTAVLExecuteF(WBTAVL *,
                      void *,
                      void (*)(void *, void *));

  int WBTAVLAdd(WBTAVL *,
		void *);

  int WBTAVLDelete(WBTAVL *,
                   void *);

  void WBTAVLClear(WBTAVL *);

  void *WBTAVLGetCurrent(WBTAVL *);

  void *WBTAVLGetFirst(WBTAVL *);

  void *WBTAVLGetLast(WBTAVL *);

  void *WBTAVLGetNext(WBTAVL *);

  void *WBTAVLGetPrev(WBTAVL *);

  void *WBTAVLGetEqual(WBTAVL *,
                       void *);

  void *WBTAVLGetEqGr(WBTAVL *,
                      void *);

  void *WBTAVLGetGr(WBTAVL *,
                    void *);

  void *WBTAVLGetEqLs(WBTAVL *,
                      void *);

  void *WBTAVLGetLs(WBTAVL *,
                    void *);

  void WBTAVLExecute(WBTAVL *);

  long WBTAVLNumItems(WBTAVL *);

  int WBTAVLIsEmpty(WBTAVL *);
#else
  WBTAVL *WBTAVLOpen();
  void WBTAVLClose();
  void WBTAVLDuplicates();
#ifdef WBTHREADS
  void WBTAVLLockF();
  void WBTAVLUnlockF();
  void WBTAVLRLockF();
  void WBTAVLRUnlockF();
#endif
  void WBTAVLCompareF();
  void WBTAVLDeleteF();
  void WBTAVLExecuteF();
  int WBTAVLAdd();
  int WBTAVLDelete();
  void WBTAVLClear();
  void *WBTAVLGetCurrent();
  void *WBTAVLGetFirst();
  void *WBTAVLGetLast();
  void *WBTAVLGetNext();
  void *WBTAVLGetPrev();
  void *WBTAVLGetEqual();
  void *WBTAVLGetEqGr();
  void *WBTAVLGetGr();
  void *WBTAVLGetEqLs();
  void *WBTAVLGetLs();
  void WBTAVLExecute();
  long WBTAVLNumItems();
  int WBTAVLIsEmpty();
#endif

#define wbtavlopen WBTAVLOpen
#define wbtavlclose WBTAVLClose
#define wbtavlduplicates WBTAVLDuplicates
#define wbtavllockf WBTAVLLockF
#define wbtavlunlockf WBTAVLUnlockF
#define wbtavlrlockf WBTAVLRLockF
#define wbtavlrunlockf WBTAVLRUnlockF
#define wbtavlcomparef WBTAVLCompareF
#define wbtavldeletef WBTAVLDeleteF
#define wbtavlexecutef WBTAVLExecuteF
#define wbtavladd WBTAVLAdd
#define wbtavldelete WBTAVLDelete
#define wbtavlclear WBTAVLClear
#define wbtavlgetcurrent WBTAVLGetCurrent
#define wbtavlgetfirst WBTAVLGetFirst
#define wbtavlgetlast WBTAVLGetLast
#define wbtavlgetnext WBTAVLGetNext
#define wbtavlgetprev WBTAVLGetPrev
#define wbtavlgetequal WBTAVLGetEqual
#define wbtavlgeteqgr WBTAVLGetEqGr
#define wbtavlgetgr WBTAVLGetGr
#define wbtavlgeteqls WBTAVLGetEqLs
#define wbtavlgetls WBTAVLGetLs
#define wbtavlexecute WBTAVLExecute
#define wbtavlnumitems WBTAVLNumItems
#define wbtavlisempty WBTAVLIsEmpty


/*************************************************************************\
**  Seperate Chaining HASH Table Structure and Prototypes
\*************************************************************************/

struct sc_hash
  {
    WBERROR *errhdl;
#ifdef WBTHREADS
    int lockerr;
#endif
    unsigned int tablesize;
    long numitems;
    struct single_link **hasharray;
    void *hashparam,
         *delparam,
         *cmpparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
         *unlockparam,
         *rlockparam,
         *runlockparam;
#else
         *exeparam;
#endif

#ifdef WBSTDC
    unsigned int (*hashfunct)(void *, void *);
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
         (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    unsigned int (*hashfunct)();
    int (*cmpfunct)();
    void (*delfunct)(),
         (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
  };
typedef struct sc_hash WBHASH;

#ifdef WBPROTOS
  WBHASH *WBHashOpen(WBERROR *,
                     unsigned int);

  void WBHashClose(WBHASH *);

#ifdef WBTHREADS
  void WBHashLockF(WBHASH *,
                   void *,
                   int (*)(void *));

  void WBHashUnlockF(WBHASH *,
                     void *,
                     int (*)(void *));

  void WBHashRLockF(WBHASH *,
                   void *,
                   int (*)(void *));

  void WBHashRUnlockF(WBHASH *,
                     void *,
                     int (*)(void *));
#endif

  void WBHashHashingF(WBHASH *,
                      void *,
                      unsigned int (*)(void *, void *));

  void WBHashCompareF(WBHASH *,
                      void *,
                      int (*)(void *, void *, void *));

  void WBHashDeleteF(WBHASH *,
		     void *,
                     void (*)(void *, void *));

  void WBHashExecuteF(WBHASH *,
                      void *,
                      void (*)(void *, void *));

  int WBHashAdd(WBHASH *,
                void *);

  int WBHashDelete(WBHASH *,
                   void *);

  void WBHashClear(WBHASH *);

  void *WBHashSearch(WBHASH *,
                     void *);

  void WBHashExecute(WBHASH *);

  unsigned int WBHashFunct(void *,
                           void *);

  void WBHashStats(WBHASH *, char *);

  long WBHashNumItems(WBHASH *);

  int WBHashIsEmpty(WBHASH *);
#else
  WBHASH *WBHashOpen();
  void WBHashClose();
#ifdef WBTHREADS
  void WBHashLockF();
  void WBHashUnlockF();
  void WBHashRLockF();
  void WBHashRUnlockF();
#endif
  void WBHashHashingF();
  void WBHashCompareF();
  void WBHashDeleteF();
  void WBHashExecuteF();
  int WBHashAdd();
  int WBHashDelete();
  void WBHashClear();
  void *WBHashSearch();
  void WBHashExecute();
  unsigned int WBHashFunct();
  void WBHashStats();
  long WBHashNumItems();
  int WBHashIsEmpty();
#endif

#define wbhashopen WBHashOpen
#define wbhashclose WBHashClose
#define wbhashlockf WBHashLockF
#define wbhashunlockf WBHashUnlockF
#define wbhashrlockf WBHashRLockF
#define wbhashrunlockf WBHashRUnlockF
#define wbhashhashingf WBHashHashingF
#define wbhashcomparef WBHashCompareF
#define wbhashdeletef WBHashDeleteF
#define wbhashexecutef WBHashExecuteF
#define wbhashadd WBHashAdd
#define wbhashdelete WBHashDelete
#define wbhashclear WBHashClear
#define wbhashsearch WBHashSearch
#define wbhashexecute WBHashExecute
#define wbhashfunct WBHashFunct
#define wbhashstats WBHashStats
#define wbhashnumitems WBHashNumItems
#define wbhashisempty WBHashIsEmpty


/*************************************************************************\
**  Dynamic Seperate Chaining HASH Table Structure and Prototypes
\*************************************************************************/

struct dl_hash
  {
    WBERROR *errhdl;
#ifdef WBTHREADS
    int lockerr;
#endif
    unsigned int splitbucket,
                 ctablesize,
                 otablesize,
                 numpages,
                 avgitems;
    long numitems;
    struct single_link ***hasharray;
    void *hashparam,
         *delparam,
         *cmpparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
	 *unlockparam,
         *rlockparam,
	 *runlockparam;
#else
         *exeparam;
#endif

#ifdef WBSTDC
    unsigned int (*hashfunct)(void *, void *);
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
         (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    unsigned int (*hashfunct)();
    int (*cmpfunct)();
    void (*delfunct)(),
         (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
  };
typedef struct dl_hash WBDHASH;

#ifdef WBPROTOS
  WBDHASH *WBDHashOpen(WBERROR *,
                       int);

  void WBDHashClose(WBDHASH *);

#ifdef WBTHREADS
  void WBDHashLockF(WBDHASH *,
                   void *,
                   int (*)(void *));

  void WBDHashUnlockF(WBDHASH *,
                     void *,
                     int (*)(void *));

  void WBDHashRLockF(WBDHASH *,
                   void *,
                   int (*)(void *));

  void WBDHashRUnlockF(WBDHASH *,
                     void *,
                     int (*)(void *));
#endif

  void WBDHashHashingF(WBDHASH *,
                       void *,
		       unsigned int (*)(void *, void *));

  void WBDHashCompareF(WBDHASH *,
                       void *,
                       int (*)(void *, void *, void *));

  void WBDHashDeleteF(WBDHASH *,
                      void *,
                      void (*)(void *, void *));

  void WBDHashExecuteF(WBDHASH *,
                       void *,
                       void (*)(void *, void *));

  int WBDHashAdd(WBDHASH *,
                 void *);

  int WBDHashDelete(WBDHASH *,
                    void *);

  void WBDHashClear(WBDHASH *);

  void *WBDHashSearch(WBDHASH *,
                      void *);

  void WBDHashExecute(WBDHASH *);

  unsigned int WBDHashFunct(void *,
                            void *);

  void WBDHashStats(WBDHASH *, char *);

  long WBDHashNumItems(WBDHASH *);

  int WBDHashIsEmpty(WBDHASH *);
#else
  WBDHASH *WBDHashOpen();
  void WBDHashClose();
#ifdef WBTHREADS
  void WBDHashLockF();
  void WBDHashUnlockF();
  void WBDHashRLockF();
  void WBDHashRUnlockF();
#endif
  void WBDHashHashingF();
  void WBDHashCompareF();
  void WBDHashDeleteF();
  void WBDHashExecuteF();
  int WBDHashAdd();
  int WBDHashDelete();
  void WBDHashClear();
  void *WBDHashSearch();
  void WBDHashExecute();
  unsigned int WBDHashFunct();
  void WBDHashStats();
  long WBDHashNumItems();
  int WBDHashIsEmpty();
#endif

#define wbdhashopen WBDHashOpen
#define wbdhashclose WBDHashClose
#define wbdhashlockf WBDHashLockF
#define wbdhashunlockf WBDHashUnlockF
#define wbdhashrlockf WBDHashRLockF
#define wbdhashrunlockf WBDHashRUnlockF
#define wbdhashhashingf WBDHashHashingF
#define wbdhashcomparef WBDHashCompareF
#define wbdhashdeletef WBDHashDeleteF
#define wbdhashexecutef WBDHashExecuteF
#define wbdhashadd WBDHashAdd
#define wbdhashdelete WBDHashDelete
#define wbdhashclear WBDHashClear
#define wbdhashsearch WBDHashSearch
#define wbdhashexecute WBDHashExecute
#define wbdhashfunct WBDHashFunct
#define wbdhashstats WBDHashStats
#define wbdhashnumitems WBDHashNumItems
#define wbdhashisempty WBDHashIsEmpty


/*************************************************************************\
**  Paging Seperate Chaining HASH Table Structure and Prototypes
\*************************************************************************/

struct pl_hash
  {
    WBERROR *errhdl;
#ifdef WBTHREADS
    int lockerr;
#endif
    unsigned int splitbucket,
                 ctablesize,
		 otablesize,
                 pagesize,
                 numpages,
                 avgitems;
    long numitems;
    struct single_link ***hasharray;
    void *hashparam,
         *delparam,
         *cmpparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
         *unlockparam,
	 *rlockparam,
         *runlockparam;
#else
         *exeparam;
#endif

#ifdef WBSTDC
    unsigned int (*hashfunct)(void *, void *);
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
         (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    unsigned int (*hashfunct)();
    int (*cmpfunct)();
    void (*delfunct)(),
         (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
	(*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
  };
typedef struct pl_hash WBPHASH;

#ifdef WBPROTOS
  WBPHASH *WBPHashOpen(WBERROR *,
                       unsigned int,
                       int);

  void WBPHashClose(WBPHASH *);

#ifdef WBTHREADS
  void WBPHashLockF(WBPHASH *,
                   void *,
                   int (*)(void *));

  void WBPHashUnlockF(WBPHASH *,
                     void *,
                     int (*)(void *));

  void WBPHashRLockF(WBPHASH *,
                   void *,
                   int (*)(void *));

  void WBPHashRUnlockF(WBPHASH *,
                     void *,
                     int (*)(void *));
#endif

  void WBPHashHashingF(WBPHASH *,
                       void *,
                       unsigned int (*)(void *, void *));

  void WBPHashCompareF(WBPHASH *,
                       void *,
		       int (*)(void *, void *, void *));

  void WBPHashDeleteF(WBPHASH *,
                      void *,
                      void (*)(void *, void *));

  void WBPHashExecuteF(WBPHASH *,
                       void *,
                       void (*)(void *, void *));

  int WBPHashAdd(WBPHASH *,
                 void *);

  int WBPHashDelete(WBPHASH *,
                    void *);

  void WBPHashClear(WBPHASH *);

  void *WBPHashSearch(WBPHASH *,
		      void *);

  void WBPHashExecute(WBPHASH *);

  unsigned int WBPHashFunct(void *,
                            void *);

  void WBPHashStats(WBPHASH *, char *);

  long WBPHashNumItems(WBPHASH *);

  int WBPHashIsEmpty(WBPHASH *);
#else
  WBPHASH *WBPHashOpen();
  void WBPHashClose();
#ifdef WBTHREADS
  void WBPHashLockF();
  void WBPHashUnlockF();
  void WBPHashRLockF();
  void WBPHashRUnlockF();
#endif
  void WBPHashHashingF();
  void WBPHashCompareF();
  void WBPHashDeleteF();
  void WBPHashExecuteF();
  int WBPHashAdd();
  int WBPHashDelete();
  void WBPHashClear();
  void *WBPHashSearch();
  void WBPHashExecute();
  unsigned int WBPHashFunct();
  void WBPHashStats();
  long WBPHashNumItems();
  int WBPHashIsEmpty();
#endif

#define wbphashopen WBPHashOpen
#define wbphashclose WBPHashClose
#define wbphashlockf WBPHashLockF
#define wbphashunlockf WBPHashUnlockF
#define wbphashrlockf WBPHashRLockF
#define wbphashrunlockf WBPHashRUnlockF
#define wbphashhashingf WBPHashHashingF
#define wbphashcomparef WBPHashCompareF
#define wbphashdeletef WBPHashDeleteF
#define wbphashexecutef WBPHashExecuteF
#define wbphashadd WBPHashAdd
#define wbphashdelete WBPHashDelete
#define wbphashclear WBPHashClear
#define wbphashsearch WBPHashSearch
#define wbphashexecute WBPHashExecute
#define wbphashfunct WBPHashFunct
#define wbphashstats WBPHashstats
#define wbphashnumitems WBPHashNumItems
#define wbphashisempty WBPHashIsEmpty


/*************************************************************************\
**  Set Structure and Prototypes
\*************************************************************************/

struct setKDB
  {
    WBERROR *errhdl;
    WBTAVL *settree;
#ifdef WBTHREADS
    int lockerr;
#endif
    void *copyparam,
#ifdef WBTHREADS
	 *delparam,
	 *lockparam,
         *unlockparam,
         *rlockparam,
         *runlockparam;
#else
	 *delparam;
#endif

#ifdef WBSTDC
    void *(*copyfunct)(void *, void *),
         (*delfunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# endif
#else
    void *(*copyfunct)(),
         (*delfunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
	(*rlockfunct)(),
	(*runlockfunct)();
# endif
#endif
  };
typedef struct setKDB WBSET;

#ifdef WBPROTOS
  WBSET *WBSetOpen(WBERROR *);

  void WBSetClose(WBSET *);

#ifdef WBTHREADS
  void WBSetLockF(WBSET *,
                   void *,
                   int (*)(void *));

  void WBSetUnlockF(WBSET *,
                     void *,
		     int (*)(void *));

  void WBSetRLockF(WBSET *,
                   void *,
                   int (*)(void *));

  void WBSetRUnlockF(WBSET *,
		     void *,
                     int (*)(void *));
#endif

  void WBSetCopyF(WBSET *,
                  void *,
                  void *(*)(void *, void *));

  void WBSetCompareF(WBSET *,
                     void *,
                     int (*)(void *, void *, void *));

  void WBSetDeleteF(WBSET *,
		    void *,
                    void (*)(void *, void *));

  void WBSetExecuteF(WBSET *,
                     void *,
                     void (*)(void *, void *));

  int WBSetAdd(WBSET *,
               void *);

  int WBSetDelete(WBSET *,
                  void *);

  void WBSetClear(WBSET *);

  void *WBSetIsMember(WBSET *,
                      void *);

  long WBSetUnion(WBSET *,
		  WBSET *,
                  WBSET *);

  long WBSetIntersection(WBSET *,
                         WBSET *,
                         WBSET *);

  long WBSetDifference(WBSET *,
                       WBSET *,
                       WBSET *);

  int WBSetIsSubset(WBSET *, WBSET *);

  int WBSetIsEqual(WBSET *, WBSET *);

  void WBSetExecute(WBSET *);

  long WBSetNumItems(WBSET *);

  int WBSetIsEmpty(WBSET *);
#else
  WBSET *WBSetOpen();
  void WBSetClose();
#ifdef WBTHREADS
  void WBSetLockF();
  void WBSetUnlockF();
  void WBSetRLockF();
  void WBSetRUnlockF();
#endif
  void WBSetCopyF();
  void WBSetCompareF();
  void WBSetDeleteF();
  void WBSetExecuteF();
  int WBSetAdd();
  int WBSetDelete();
  void WBSetClear();
  void *WBSetIsMember();
  long WBSetUnion();
  long WBSetIntersection();
  long WBSetDifference();
  int WBSetIsSubset();
  int WBSetIsEqual();
  void WBSetExecute();
  long WBSetNumItems();
  int WBSetIsEmpty();
#endif

#define wbsetopen WBSetOpen
#define wbsetclose WBSetClose
#define wbsetlockf WBSetLockF
#define wbsetunlockf WBSetUnlockF
#define wbsetrlockf WBSetRLockF
#define wbsetrunlockf WBSetRUnlockF
#define wbsetcopyf WBSetCopyF
#define wbsetcomparef WBSetCompareF
#define wbsetdeletef WBSetDeleteF
#define wbsetexecutef WBSetExecuteF
#define wbsetadd WBSetAdd
#define wbsetdelete WBSetDelete
#define wbsetclear WBSetClear
#define wbsetismember WBSetIsMember
#define wbsetunion WBSetUnion
#define wbsetintersection WBSetIntersection
#define wbsetdifference WBSetDifference
#define wbsetissubset WBSetIsSubset
#define wbsetisequal WBSetIsEqual
#define wbsetexecute WBSetExecute
#define wbsetnumitems WBSetNumItems
#define wbsetisempty WBSetIsEmpty


/*************************************************************************\
**  Table Structure and Prototypes
\*************************************************************************/

struct table
  {
    WBERROR *errhdl;
    WBTAVL *tabletree;
#ifdef WBTHREADS
    int lockerr;
    void *lockparam,
	 *unlockparam,
         *rlockparam,
         *runlockparam;
# ifdef WBSTDC
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
        (*runlockfunct)(void *);
# else
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
	(*runlockfunct)();
# endif
#endif
  };
typedef struct table WBTABLE;
typedef struct table WBBAG;

#ifdef WBPROTOS
  WBTABLE *WBTableOpen(WBERROR *);

  void WBTableClose(WBTABLE *);

  void WBTableCompareF(WBTABLE *,
                       void *,
                       int (*)(void *, void *, void *));

  void WBTableDeleteF(WBTABLE *,
                      void *,
                      void (*)(void *, void *));

  void WBTableExecuteF(WBTABLE *,
                       void *,
                       void (*)(void *, void *));

#ifdef WBTHREADS
  void WBTableLockF(WBTABLE *,
		   void *,
                   int (*)(void *));

  void WBTableUnlockF(WBTABLE *,
                     void *,
                     int (*)(void *));

  void WBTableRLockF(WBTABLE *,
                   void *,
                   int (*)(void *));

  void WBTableRUnlockF(WBTABLE *,
		     void *,
		     int (*)(void *));
#endif

  int WBTableAdd(WBTABLE *,
                 void *);

  int WBTableDelete(WBTABLE *,
                    void *);

  void WBTableClear(WBTABLE *);

  void *WBTableIsMember(WBTABLE *,
                        void *);

  void WBTableExecute(WBTABLE *);

  int WBTableIsEmpty(WBTABLE *);

  long WBTableNumItems(WBTABLE *);
#else
  WBTABLE *WBTableOpen();
  void WBTableClose();
  void WBTableCompareF();
  void WBTableDeleteF();
  void WBTableExecuteF();
#ifdef WBTHREADS
  void WBTableLockF();
  void WBTableUnlockF();
  void WBTableRLockF();
  void WBTableRUnlockF();
#endif
  int WBTableAdd();
  int WBTableDelete();
  void WBTableClear();
  void *WBTableIsMember();
  void WBTableExecute();
  int WBTableIsEmpty();
  long WBTableNumItems();
#endif

#define wbtableopen WBTableOpen
#define wbtableclose WBTableClose
#define wbtablelockf WBTableLockF
#define wbtableunlockf WBTableUnlockF
#define wbtablerlockf WBTableRLockF
#define wbtablerunlockf WBTableRUnlockF
#define wbtablecomparef WBTableCompareF
#define wbtabledeletef WBTableDeleteF
#define wbtableexecutef WBTableExecuteF
#define wbtableadd WBTableAdd
#define wbtabledelete WBTableDelete
#define wbtableclear WBTableClear
#define wbtableismember WBTableIsMember
#define wbtableexecute WBTableExecute
#define wbtableisempty WBTableIsEmpty
#define wbtablenumitems WBTableNumItems

#define wbbagopen WBTableOpen
#define wbbagclose WBTableClose
#define wbbaglockf WBTableLockF
#define wbbagunlockf WBTableUnlockF
#define wbbagrlockf WBTableRLockF
#define wbbagrunlockf WBTableRUnlockF
#define wbbagcomparef WBTableCompareF
#define wbbagdeletef WBTableDeleteF
#define wbbagexecutef WBTableExecuteF
#define wbbagadd WBTableAdd
#define wbbagdelete WBTableDelete
#define wbbagclear WBTableClear
#define wbbagismember WBTableIsMember
#define wbbagexecute WBTableExecute
#define wbbagisempty WBTableIsEmpty
#define wbbagnumitems WBTableNumItems

#define WBBagOpen WBTableOpen
#define WBBagClose WBTableClose
#define WBBagLockF WBTableLockF
#define WBBagUnlockF WBTableUnlockF
#define WBBagRLockF WBTableRLockF
#define WBBagRUnlockF WBTableRUnlockF
#define WBBagCompareF WBTableCompareF
#define WBBagDeleteF WBTableDeleteF
#define WBBagExecuteF WBTableExecuteF
#define WBBagAdd WBTableAdd
#define WBBagDelete WBTableDelete
#define WBBagClear WBTableClear
#define WBBagIsMember WBTableIsMember
#define WBBagExecute WBTableExecute
#define WBBagIsEmpty WBTableIsEmpty
#define WBBagNumItems WBTableNumItems


/*************************************************************************\
**  Dictionary Structure and Prototypes
\*************************************************************************/

struct dict
  {
    WBERROR *errhdl;
    WBTAVL *dicttree;
#ifdef WBTHREADS
    int lockerr;
#endif

#ifdef WBTHREADS
    void *copyparam,
	 *lockparam,
         *unlockparam,
         *rlockparam,
         *runlockparam;
#else
    void *copyparam;
#endif

#ifdef WBSTDC
    void *(*copyfunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
	(*runlockfunct)(void *);
# endif
#else
    void *(*copyfunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
        (*unlockfunct)(),
        (*rlockfunct)(),
        (*runlockfunct)();
# endif
#endif
  };
typedef struct dict WBDICT;

#ifdef WBPROTOS
  WBDICT *WBDictOpen(WBERROR *);

  void WBDictClose(WBDICT *);

#ifdef WBTHREADS
  void WBDictLockF(WBDICT *,
                   void *,
                   int (*)(void *));

  void WBDictUnlockF(WBDICT *,
                     void *,
                     int (*)(void *));

  void WBDictRLockF(WBDICT *,
                   void *,
                   int (*)(void *));

  void WBDictRUnlockF(WBDICT *,
                     void *,
                     int (*)(void *));
#endif

  void WBDictCopyF(WBDICT *,
		   void *,
                   void *(*)(void *, void *));

  void WBDictCompareF(WBDICT *,
                      void *,
                      int (*)(void *, void *, void *));

  void WBDictDeleteF(WBDICT *,
                     void *,
                     void (*)(void *, void *));

  void WBDictExecuteF(WBDICT *,
		      void *,
                      void (*)(void *, void *));

  int WBDictAdd(WBDICT *,
                void *);

  int WBDictDelete(WBDICT *,
		   void *);

  void WBDictClear(WBDICT *);

  void *WBDictIsMember(WBDICT *,
                       void *);

  int WBDictIsPred(WBDICT *,
                   void *);

  int WBDictIsSucc(WBDICT *,
                   void *);

  void *WBDictGetPred(WBDICT *,
                      void *);

  void *WBDictGetSucc(WBDICT *,
                      void *);

  long WBDictGetRange(WBDICT *,
                      WBDICT *,
                      void *,
                      void *);

  void WBDictExecute(WBDICT *);

  long WBDictNumItems(WBDICT *);

  int WBDictIsEmpty(WBDICT *);
#else
  WBDICT *WBDictOpen();
  void WBDictClose();
#ifdef WBTHREADS
  void WBDictLockF();
  void WBDictUnlockF();
  void WBDictRLockF();
  void WBDictRUnlockF();
#endif
  void WBDictCopyF();
  void WBDictCompareF();
  void WBDictDeleteF();
  void WBDictExecuteF();
  int WBDictAdd();
  int WBDictDelete();
  void WBDictClear();
  void *WBDictIsMember();
  int WBDictIsPred();
  int WBDictIsSucc();
  void *WBDictGetPred();
  void *WBDictGetSucc();
  long WBDictGetRange();
  void WBDictExecute();
  long WBDictNumItems();
  int WBDictIsEmpty();
#endif

#define wbdictopen WBDictOpen
#define wbdictclose WBDictClose
#define wbdictlockf WBDictLockF
#define wbdictunlockf WBDictUnlockF
#define wbdictrlockf WBDictRLockF
#define wbdictrunlockf WBDictRUnlockF
#define wbdictcopyf WBDictCopyF
#define wbdictcomparef WBDictCompareF
#define wbdictdeletef WBDictDeleteF
#define wbdictexecutef WBDictExecuteF
#define wbdictadd WBDictAdd
#define wbdictdelete WBDictDelete
#define wbdictclear WBDictClear
#define wbdictismember WBDictIsMember
#define wbdictispred WBDictIsPred
#define wbdictissucc WBDictIsSucc
#define wbdictgetpred WBDictGetPred
#define wbdictgetsucc WBDictGetSucc
#define wbdictgetrange WBDictGetRange
#define wbdictexecute WBDictExecute
#define wbdictnumitems WBDictNumItems
#define wbdictisempty WBDictIsEmpty


/*************************************************************************\
**  Priority Heap Structure and Prototypes
\*************************************************************************/

#define WBHEAP_OPENENTRIES(heapsize, maxheap) heapsize < maxheap ? maxheap - heapsize : 0

#define WBHEAP_ADD(heap, heapsize, maxheap, prifunct, priparam, item) \
   {                                                                  \
     unsigned int mi_index;                                           \
								      \
     if (heapsize < maxheap)                                          \
       {                                                              \
	 mi_index = heapsize + 1;                                     \
	 heapsize++;                                                  \
	 while (mi_index > 1 && (*prifunct)(priparam,item,heap[(mi_index/2)-1]) > 0) \
	   {                                                          \
	     heap[mi_index-1] = heap[(mi_index/2)-1];                 \
	     mi_index /= 2;                                           \
	   }                                                          \
	 heap[mi_index-1] = item;                                     \
       }                                                              \
   }

#define WBHEAP_DELETE(heap, heapsize, prifunct, priparam, tmpitem, in_dex) \
   {                                                                  \
     unsigned int mi_tmpindex = in_dex + 1, mi_left, mi_right, mi_largest; \
								      \
     if (heapsize)                                                    \
       {                                                              \
	 heap[in_dex] = heap[--heapsize];                             \
	 do                                                           \
	   {                                                          \
	     mi_left = mi_tmpindex*2;                                 \
	     mi_right = mi_tmpindex*2+1;                              \
	     if (mi_right <= heapsize                                 \
	       && (*prifunct)(priparam,heap[mi_right-1],heap[mi_tmpindex-1]) > 0) \
	       mi_largest = mi_right;                                 \
	     else                                                     \
	       mi_largest = mi_tmpindex;                              \
	     if (mi_left <= heapsize                                  \
	       && (*prifunct)(priparam,heap[mi_left-1],heap[mi_largest-1]) > 0) \
	       mi_largest = mi_left;                                  \
             if (mi_largest != mi_tmpindex)                           \
               {                                                      \
                 tmpitem = heap[mi_tmpindex-1];                       \
                 heap[mi_tmpindex-1] = heap[mi_largest-1];            \
                 heap[mi_largest-1] = tmpitem;                        \
		 mi_tmpindex = mi_largest;                            \
	       }                                                      \
             else mi_tmpindex = 0;                                    \
           }                                                          \
         while (mi_tmpindex);                                         \
       }                                                              \
   }

#define WBHEAP_VALIDITEM(heapsize, index) index < heapsize ? 1 : 0

#define WBHEAP_GETITEM(heap, index) heap[index]

struct priority_heap
  {
    WBERROR *errhdl;
    int duplicates;
    void **heap,
	 *priparam,
	 *cmpparam,
	 *delparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
	 *unlockparam,
	 *rlockparam,
	 *runlockparam;
#else
	 *exeparam;
#endif

#ifdef WBSTDC
    int (*prifunct)(void *, void *, void *);
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
         (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
        (*unlockfunct)(void *),
        (*rlockfunct)(void *),
	(*runlockfunct)(void *);
# endif
#else
    int (*prifunct)();
    int (*cmpfunct)();
    void (*delfunct)(),
         (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
	(*unlockfunct)(),
	(*rlockfunct)(),
	(*runlockfunct)();
# endif
#endif
#ifdef WBTHREADS
    int lockerr;
#endif
    unsigned int heapsize,
		 maxheap;
  };
typedef struct priority_heap WBPHEAP;

#ifdef WBPROTOS
  WBPHEAP *WBPHeapOpen(WBERROR *, unsigned int);
  void WBPHeapClose(WBPHEAP *);
#ifdef WBTHREADS
  void WBPHeapLockF(WBPHEAP *, void *, int (*)(void *));
  void WBPHeapUnlockF(WBPHEAP *, void *, int (*)(void *));
  void WBPHeapRLockF(WBPHEAP *, void *, int (*)(void *));
  void WBPHeapRUnlockF(WBPHEAP *, void *, int (*)(void *));
#endif
  void WBPHeapDuplicates(WBPHEAP *, int);
  void WBPHeapPriorityF(WBPHEAP *, void *, int (*)(void *, void *, void *));
  void WBPHeapCompareF(WBPHEAP *, void *, int (*)(void *, void *, void *));
  void WBPHeapDeleteF(WBPHEAP *, void *, void (*)(void *, void *));
  void WBPHeapExecuteF(WBPHEAP *, void *, void (*)(void *, void *));
  int WBPHeapAdd(WBPHEAP *, void *);
  int WBPHeapDelete(WBPHEAP *, void *);
  void WBPHeapClear(WBPHEAP *);
  void *WBPHeapSearch(WBPHEAP *, void *);
  void *WBPHeapGet(WBPHEAP *);
  void *WBPHeapPeek(WBPHEAP *);
  void WBPHeapExecute(WBPHEAP *);
  long WBPHeapNumItems(WBPHEAP *);
  int WBPHeapIsEmpty(WBPHEAP *);
#else
  WBPHEAP *WBPHeapOpen();
  void WBPHeapClose();
#ifdef WBTHREADS
  void WBPHeapLockF();
  void WBPHeapUnlockF();
  void WBPHeapRLockF();
  void WBPHeapRUnlockF();
#endif
  void WBPHeapDuplicates();
  void WBPHeapPriorityF();
  void WBPHeapCompareF();
  void WBPHeapDeleteF();
  void WBPHeapExecuteF();
  int WBPHeapAdd();
  int WBPHeapDelete();
  void WBPHeapClear();
  void *WBPHeapSearch();
  void *WBPHeapGet();
  void *WBPHeapPeek();
  void WBPHeapExecute();
  long WBPHeapNumItems();
  int WBPHeapIsEmpty();
#endif

#define wbpheapopen WBPHeapOpen
#define wbpheapclose WBPHeapClose
#define wbpheaplockf WBPHeapLockF
#define wbpheapunlockf WBPHeapUnlockF
#define wbpheaprlockf WBPHeapRLockF
#define wbpheaprunlockf WBPHeapRUnlockF
#define wbpheappriorityf WBPHeapPriorityF
#define wbpheapduplicates WBPHeapDuplicates
#define wbpheapcomparef WBPHeapCompareF
#define wbpheapdeletef WBPHeapDeleteF
#define wbpheapexecutef WBPHeapExecuteF
#define wbpheapadd WBPHeapAdd
#define wbpheapdelete WBPHeapDelete
#define wbpheapclear WBPHeapClear
#define wbpheapsearch WBPHeapSearch
#define wbpheapget WBPHeapGet
#define wbpheappeek WBPHeapPeek
#define wbpheapexecute WBPHeapExecute
#define wbpheapnumitems WBPHeapNumItems
#define wbpheapisempty WBPHeapIsEmpty


/*************************************************************************\
**  Priority Queue Structure and Prototypes
\*************************************************************************/

struct wb_priority_queue
  {
    WBERROR *errhdl;
    WBSLIST *ovrbuff;
    WBARRAY *array;
    WBAVL *ptree;
    void **heap,
	 *pparam,
	 *cmpparam,
	 *delparam,
#ifdef WBTHREADS
	 *exeparam,
	 *lockparam,
	 *unlockparam,
	 *rlockparam,
	 *runlockparam;
#else
	 *exeparam;
#endif

#ifdef WBSTDC
    int (*pfunct)(void *, void *, void *);
    int (*cmpfunct)(void *, void *, void *);
    void (*delfunct)(void *, void *),
	 (*exefunct)(void *, void *);
# ifdef WBTHREADS
    int (*lockfunct)(void *),
	(*unlockfunct)(void *),
	(*rlockfunct)(void *),
	(*runlockfunct)(void *);
# endif
#else
    int (*pfunct)();
    int (*cmpfunct)();
    void (*delfunct)(),
	 (*exefunct)();
# ifdef WBTHREADS
    int (*lockfunct)(),
	(*unlockfunct)(),
	(*rlockfunct)(),
	(*runlockfunct)();
# endif
#endif
#ifdef WBTHREADS
    int lockerr;
#endif
    unsigned int heapsize,
		 maxheap;
  };
typedef struct wb_priority_queue WBPQUEUE;

#ifdef WBPROTOS
  WBPQUEUE *WBPQueueOpen(WBERROR *,
			 unsigned int,
			 int);

  void WBPQueueClose(WBPQUEUE *);

  void WBPQueueDuplicates(WBPQUEUE *,
			  int);

#ifdef WBTHREADS
  void WBPQueueLockF(WBPQUEUE *,
		   void *,
		   int (*)(void *));

  void WBPQueueUnlockF(WBPQUEUE *,
		     void *,
		     int (*)(void *));

  void WBPQueueRLockF(WBPQUEUE *,
		   void *,
		   int (*)(void *));

  void WBPQueueRUnlockF(WBPQUEUE *,
		     void *,
		     int (*)(void *));
#endif

  void WBPQueuePriorityF(WBPQUEUE *,
			 void *,
			 int (*)(void *, void *, void *));

  void WBPQueueCompareF(WBPQUEUE *,
			void *,
			int (*)(void *, void *, void *));

  void WBPQueueDeleteF(WBPQUEUE *,
		       void *,
		       void (*)(void *, void *));

  void WBPQueueExecuteF(WBPQUEUE *,
			void *,
			void (*)(void *, void *));

  int WBPQueueAdd(WBPQUEUE *,
		  void *);

  int WBPQueueDelete(WBPQUEUE *,
		     void *);

  void WBPQueueClear(WBPQUEUE *);

  void *WBPQueueSearch(WBPQUEUE *,
		       void *);

  void *WBPQueueGet(WBPQUEUE *);

  void *WBPQueuePeek(WBPQUEUE *);

  void WBPQueueExecute(WBPQUEUE *);

  long WBPQueueNumItems(WBPQUEUE *);

  int WBPQueueIsEmpty(WBPQUEUE *);
#else
  WBPQUEUE *WBPQueueOpen();
  void WBPQueueClose();
  void WBPQueueDuplicates();
#ifdef WBTHREADS
  void WBPQueueLockF();
  void WBPQueueUnlockF();
  void WBPQueueRLockF();
  void WBPQueueRUnlockF();
#endif
  void WBPQueuePriorityF();
  void WBPQueueCompareF();
  void WBPQueueDeleteF();
  void WBPQueueExecuteF();
  int WBPQueueAdd();
  int WBPQueueDelete();
  void WBPQueueClear();
  void *WBPQueueSearch();
  void *WBPQueueGet();
  void *WBPQueuePeek();
  void WBPQueueExecute();
  long WBPQueueNumItems();
  int WBPQueueIsEmpty();
#endif

#define wbpqueueopen WBPQueueOpen
#define wbpqueueclose WBPQueueClose
#define wbpqueueduplicates WBPQueueDuplicates
#define wbpqueuelockf WBPQueueLockF
#define wbpqueueunlockf WBPQueueUnlockF
#define wbpqueuerlockf WBPQueueRLockF
#define wbpqueuerunlockf WBPQueueRUnlockF
#define wbpqueuepriorityf WBPQueuePriorityF
#define wbpqueuecomparef WBPQueueCompareF
#define wbpqueuedeletef WBPQueueDeleteF
#define wbpqueueexecutef WBPQueueExecuteF
#define wbpqueueadd WBPQueueAdd
#define wbpqueuedelete WBPQueueDelete
#define wbpqueueclear WBPQueueClear
#define wbpqueuesearch WBPQueueSearch
#define wbpqueueget WBPQueueGet
#define wbpqueuepeek WBPQueuePeek
#define wbpqueueexecute WBPQueueExecute
#define wbpqueuenumitems WBPQueueNumItems
#define wbpqueueisempty WBPQueueIsEmpty

#endif
