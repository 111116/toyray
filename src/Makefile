
test: main.cpp *.h *.hpp saveexr.o lib/json.hpp
	gcc lib/rgbe/rgbe.c -O2 -c -o rgbe.o
	g++ main.cpp rgbe.o saveexr.o -o test -g -std=c++14 -fopenmp

saveexr.o: lib/saveexr.h lib/saveexr.cpp lib/tinyexr.h
	g++ lib/saveexr.cpp -O2 -c -o saveexr.o

clean:
	rm test *.o
