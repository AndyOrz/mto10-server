CXX=c++ -std=c++11

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

all: mto10_server_d mto10_read

mto10_server_d: Proj mto10_server_d.o
	$(CXX) mto10_server_d.o ConfigManager.o TCP.o GameEngine.o Tools.o DatabaseAccess.o \
	`mysql_config --cflags --libs` -lpthread -o mto10_server_d

mto10_server_d.o: mto10_server_d.cpp mto10_server_d.h common.h
	$(CXX) -c mto10_server_d.cpp -o mto10_server_d.o

mto10_read: Proj mto10_read.o
	$(CXX) mto10_read.o ConfigManager.o Tools.o DatabaseAccess.o Options.o \
	`mysql_config --cflags --libs` -o mto10_read

mto10_read.o: mto10_read.cpp mto10_read.h common.h
	$(CXX) -c mto10_read.cpp -o mto10_read.o

Proj:
	$(RECURSIVE_MAKE)
	cp ./*/*.o .
.PHONY: clean
clean:
	$(RECURSIVE_CLEAN)
	rm -rf *.o mto10_server_d mto10_read mto10_server_d.log