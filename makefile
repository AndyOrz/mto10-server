CXX=c++ -std=c++11
Projs=main.o ConfigManager.o TCP.o GameEngine.o Tools.o DatabaseAccess.o

SUBDIRS:=$(shell find . -maxdepth 1 -type d)
SUBDIRS:=$(basename $(patsubst ./%,%,$(SUBDIRS)))

RECURSIVE_MAKE= @for subdir in $(SUBDIRS); \
do \
	echo "making in $(subdir)"; \
	$(MAKE) -C $$subdir || exit 1; \
done

RECURSIVE_CLEAN= @for subdir in $(SUBDIRS); \
do \
	echo "cleaning in $(subdir)"; \
	$(MAKE) -C $$subdir clean || exit 1; \
done

all: main

main: Proj main.o
	$(CXX) main.o ConfigManager.o TCP.o GameEngine.o Tools.o DatabaseAccess.o \
	`mysql_config --cflags --libs` -lpthread -o main

main.o: main.cpp main.h common.h
	$(CXX) -c main.cpp -o main.o

Proj:
	$(RECURSIVE_MAKE)
	cp ./*/*.o .
.PHONY: clean
clean:
	$(RECURSIVE_CLEAN)
	rm -rf *.o main main_test my-mto10.log