CXX=c++ -std=c++11
fname=DatabaseAccess

all: $(fname).o

$(fname).o: $(fname).cpp $(fname).h
	$(CXX) -c $(fname).cpp `mysql_config --cflags --libs` -o $(fname).o 

test: test.cpp $(fname).cpp $(fname).h
	$(CXX) test.cpp $(fname).cpp `mysql_config --cflags --libs` -o test

.PHONY: clean
clean:
	rm -rf *.o test