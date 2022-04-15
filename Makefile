CC=g++
SRCS=$(shell dir /b /s *.cpp)
LIBS=src/asciiImgLib.cpp src/screen.cpp src/engine.cpp
CFLAGS= -std=c++2a -DNDEBUG -w -I inc
OPTFLAGS= -s -O3 -g0
DINORES=obj/clouds.o obj/ground.o obj/jump.o obj/kamen.o obj/run1.o obj/run2.o
OES=dino.o engine.o screen.o asciiImgLib.o
all:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(SRCS) -o dino
dino:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(LIBS) src/dino.cpp -c 
	$(CC) $(OES) $(OPTFLAGS) $(DINORES) -o dino
	del $(OES)
test2:
	$(CC) -std=c++2a -w -I inc -DNDEBUG src/asciiImgLib.cpp src/screen.cpp src/engine.cpp src/test2.cxx -o test
gnd:
	ld -r -b binary -o ground.o res/ground.bmp
ttt:
	$(CC) -c -std=c++2a -w -I inc -DNDEBUG src/asciiImgLib.cpp src/screen.cpp src/engine.cpp src/test.cxx
