CC=clang
CXX=clang++
CFLAGS=-g3 
CPPFLAGS=`llvm-config --cxxflags` 
COMPILE_LIB=$(CC) $< $(CFLAGS) $(CPPFLAGS) -o $@

all: libCallReplPass.so libCallReplPass.o

libCallReplPass.so: EXTRA=-fPIC -shared
libCallReplPass.o:  EXTRA=-static -c

libCallReplPass.%: callrepl.cpp
	$(COMPILE_LIB) $(EXTRA)
