test: main.cpp geometry.h la.h writebmp.h envmap.h
	gcc rgbe/rgbe.c -c -o rgbe.o
	g++ main.cpp rgbe.o -o test -O2 -std=c++14

clean:
	rm test
