.PHONY=all clean

CC=g++

CFLAGS=-std=c++2a -DNDEBUG -w -I inc
LDFLAGS=-r -b binary
OPTFLAGS=-s -O3 -g0

EXE=dino.exe
SOURCES=$(shell dir /b src)
RESOURSES=$(shell dir /b res)

RES_OBJECTS=$(RESOURSES:.bmp=.bmp.o)
SRC_OBJECTS=$(SOURCES:.cpp=.cpp.o)

OBJECTS=$(SRC_OBJECTS) $(RES_OBJECTS)

all: $(EXE) clean

$(EXE): $(OBJECTS)
	$(CC) $(OPTFLAGS) $(OBJECTS) -o $@

%.cpp.o:
	$(CC) $(CFLAGS) $(OPTFLAGS) src/$(@:.cpp.o=.cpp) -c -o $(@)

%.bmp.o:
	ld $(LDFLAGS) -o $@ res/$(@:.bmp.o=.bmp) 

clean:
	del *.o