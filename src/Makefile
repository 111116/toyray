test: main.cpp geometry.h la.h writebmp.h envmap.h saveexr.o
	gcc rgbe/rgbe.c -c -o rgbe.o
	g++ main.cpp rgbe.o saveexr.o -o test -O2 -std=c++14
	rm rgbe.o

saveexr.o: saveexr.h saveexr.cpp lib/tinyexr.h
	g++ saveexr.cpp -O2 -c -o saveexr.o

clean:
	rm test
