test: main.cpp geometry.h la.h writebmp.h
	g++ main.cpp -o test -O2

clean:
	rm test
