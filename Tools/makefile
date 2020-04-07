CXX=c++ -std=c++11
fname=Tools

all: $(fname).o

$(fname).o: $(fname).cpp $(fname).h
	$(CXX) -c $(fname).cpp -o $(fname).o

test-1: test.cpp
	$(CXX) test.cpp -o test

.PHONY: clean
clean:
	rm -rf *.o test