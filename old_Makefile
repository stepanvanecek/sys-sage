
CC=g++

CFLAGS=-Wall -std=c++11
CU_FLAGS=-Wall -std=c++11

INC=$(shell xml2-config --cflags)
LIBS=$(shell xml2-config --libs)

SYSTOPO_INC=-I/Users/stepan/Documents/phd/sys-topo/src
SYSTOPO_LIBS=-L/Users/stepan/Documents/phd/sys-topo/lib -lsys-topo


LIB_DEPS=src/hwloc.o src/Topology.o
EXAMPLES_DEPS=examples/main.o


all: lib examples

lib: $(LIB_DEPS)
	$(CC) -shared $(CFLAGS) $(INC) $(LIBS) $^ -o lib/libsys-topo.so

examples: $(EXAMPLES_DEPS)
	$(CC) $(CFLAGS) $(INC) $(SYSTOPO_INC) $(LIBS) $(SYSTOPO_LIBS) $^ -o examples/main

%.o:%.cpp
	$(CC) $(CU_FLAGS) $(INC) $(SYSTOPO_INC) $< -c -o $@

clean:
	rm -rf src/*.o examples/*.o lib/*.so examples/main
