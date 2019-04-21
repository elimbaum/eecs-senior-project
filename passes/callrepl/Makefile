CC=clang
CXX=clang++
CFLAGS=-g3

MMIOFU_DIR=~/sproj-git/mmiofu

CPPFLAGS=`llvm-config --cxxflags` -I $(MMIOFU_DIR)
COMPILE_LIB=$(CC) $(MMIOFU_DIR)/fu_mmap.o $(MMIOFU_DIR)/blop.o $< $(CFLAGS) $(CPPFLAGS) -o $@

all: libCallReplPass.so libCallReplPass.o

libCallReplPass.so: EXTRA=-fPIC -shared
libCallReplPass.o:  EXTRA=-static -c

libCallReplPass.%: callrepl.cpp
	$(COMPILE_LIB) $(EXTRA)
