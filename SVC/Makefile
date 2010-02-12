CXX = g++
CXXFLAGS = -g -Wall -I./ -I../
DEPEND = g++ -MM
DEL = rm -rf

LD = $(CXX)
LDFLAGS =  -lm

SOURCES           := $(wildcard *.cpp)
OBJECTS           := $(SOURCES:.cpp=.o)
EXESRC            := $(wildcard *.c)
EXECUTABLES       := $(EXESRC:.c=)

all: $(EXECUTABLES)

% : %.c $(OBJECTS); ${LD} $(CXXFLAGS) $< $(OBJECTS) $(LDFLAGS) -o $@

%.o : %.cpp; $(CXX) $(CXXFLAGS) -c $< -o $@;

%.d : %.cpp; $(DEPEND) $(CXXFLAGS) $< | grep -v /usr/include > $@; 

clean: ; $(DEL) *.d; $(DEL) $(OBJECTS); $(DEL) $(EXECUTABLES)

clear: ; $(DEL) *~

include $(SOURCES:.cpp=.d)
