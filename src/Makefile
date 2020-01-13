
test: main.cpp *.h *.hpp saveexr.o lib/json.hpp
	gcc rgbe/rgbe.c -c -o rgbe.o
	g++ main.cpp rgbe.o saveexr.o -o test -g -std=c++14
	rm rgbe.o

saveexr.o: saveexr.h saveexr.cpp lib/tinyexr.h
	g++ saveexr.cpp -O2 -c -o saveexr.o

clean:
	rm test

legacy: legacy.cpp *.h *.hpp saveexr.o lib/json.hpp
	gcc rgbe/rgbe.c -c -o rgbe.o
	g++ legacy.cpp rgbe.o saveexr.o -o test -g -std=c++14
	rm rgbe.o
