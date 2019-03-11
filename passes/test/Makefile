CC=clang
CXX=clang++
CFLAGS=-g3 -c
CPPFLAGS=`llvm-config --cxxflags` 
COMPILE_LIB=$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

all: libTestPass.so libTestPass.o

libTestPass.so: EXTRA=-fPIC

libTestPass.*: test.cpp
	$(COMPILE_LIB) $(EXTRA)
