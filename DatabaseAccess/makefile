CXX=c++ -std=c++11
fname=DatabaseAccess

all: $(fname).o

$(fname).o: $(fname).cpp $(fname).h
	$(CXX) -c $(fname).cpp -o $(fname).o 

test: test.cpp $(fname).cpp $(fname).h
	$(CXX) test.cpp $(fname).cpp -o test -g

.PHONY: clean
clean:
	rm -rf *.o test