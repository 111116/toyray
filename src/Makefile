SHELL := /bin/bash
CC := gcc-9
CXX := g++-9

OPT_FOPENMP = $(shell cat /dev/null | $(CXX) -x c++ - -c -o /dev/null -fopenmp &> /dev/null && echo '-fopenmp')
# uncomment this line to force disable OpenMP
OPT_FOPENMP = 

# $(info $(OPT_FOPENMP))
ifeq ($(OPT_FOPENMP), -fopenmp)
$(info OpenMP is enabled)
OPT_FOPENMP += -DTHREADED
else
$(info OpenMP is disabled)
endif

test: main.cpp *.h *.hpp saveexr.o */*.hpp
# 	$(CC) lib/rgbe/rgbe.c -O2 -c -o rgbe.o
	$(CXX) main.cpp saveexr.o -o test -O2 -std=c++14 $(OPT_FOPENMP)

saveexr.o: lib/saveexr.h lib/saveexr.cpp lib/tinyexr.h
	$(CXX) lib/saveexr.cpp -O2 -c -o saveexr.o

clean:
	rm test *.o
