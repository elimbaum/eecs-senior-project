CC=clang
CXX=clang++
CFLAGS=-g3 
CPPFLAGS=`llvm-config --cxxflags` 
COMPILE_LIB=$(CC) $< $(CFLAGS) $(CPPFLAGS) -o $@

all: libTestPass.so libTestPass.o

libTestPass.so: EXTRA=-fPIC -shared
libTestPass.o:  EXTRA=-static -c

libTestPass.*: test.cpp
	$(COMPILE_LIB) $(EXTRA)
