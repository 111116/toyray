test: rgbe.h rgbe.c testrgbe.cpp
	gcc rgbe.c -c -o rgbe.o &&\
	g++ testrgbe.cpp -c -o test.o &&\
	g++ test.o rgbe.o -o test

clean:
	rm test test.o rgbe.o
