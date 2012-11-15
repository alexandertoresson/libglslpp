CXXFLAGS:=-g -O2 -std=c++11 -Wno-narrowing

test: test.cpp

clean:
	rm -f test
