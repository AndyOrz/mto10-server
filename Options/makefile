CXX=c++ -std=c++11
fname=Options

all: $(fname).o

$(fname).o: $(fname).cpp $(fname).h cmdline.h
	$(CXX) -c $(fname).cpp -o $(fname).o

test: test.cpp $(fname).cpp $(fname).h cmdline.h
	$(CXX) test.cpp $(fname).cpp -o test

.PHONY: clean
clean:
	rm -rf *.o test