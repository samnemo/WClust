
INC = ../include
LIB = ../lib

AR = ar
RANLIB = ranlib
CC = g++ 
#CC = insure g++ -g

CFLAGS = -O1 -g -Wno-deprecated  -DFOPEN

OBJS= Node.o DataNode.o KDBNode.o KDBTree.o Rect.o Point.o Queue.o KDBTree_C.o

.C.o:
	$(CC) -c $(CFLAGS) -I$(INC) $<


all: $(OBJS)
	$(AR) cr ../lib/libkdbtree.a $(OBJS)

clean:
	rm -f *.o

