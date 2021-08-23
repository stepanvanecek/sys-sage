
CC=g++

CFLAGS=-Wall -std=c++11
CU_FLAGS=-Wall -std=c++11

INC=$(shell xml2-config --cflags)
LIBS=$(shell xml2-config --libs)

LIB_DEPS=hwloc.o Topology.o
MAIN_DEPS=$(LIB_DEPS) main.o


all: main

lib: $(LIB_DEPS)
	$(CC) -shared $(CFLAGS) $(INC) $(LIBS) $^ -o sys-topo.so

main: $(MAIN_DEPS)
	#echo $(LIBS)
	#echo $(INC)
	$(CC) $(CFLAGS) $(INC) $(LIBS) $^ -o main

%.o:%.cpp
	$(CC) $(CU_FLAGS) $(INC) $< -c -o $@

clean:
	rm -f *.o main
