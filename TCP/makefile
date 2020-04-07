CXX=c++ -std=c++11
fname=TCP

all: $(fname).o

$(fname).o: $(fname).cpp $(fname).h
	$(CXX) -c $(fname).cpp -o $(fname).o

test: test.cpp $(fname).cpp $(fname).h md5.h
	$(CXX) test.cpp $(fname).cpp -o test

test-1: test.cpp
	$(CXX) test.cpp -o test

.PHONY: clean
clean:
	rm -rf *.o test