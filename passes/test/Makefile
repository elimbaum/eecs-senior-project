CC=clang
CXX=clang++
CFLAGS=-g3 -c
CPPFLAGS=`llvm-config --cxxflags` 
COMPILE_LIB=$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

all: libTestPass.so libTestPass.o

libTestPass.so: test.cpp
	$(COMPILE_LIB) -fPIC

libTestPass.o: test.cpp
	$(COMPILE_LIB)
