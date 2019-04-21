CC=clang
CXX=clang++
LDLIBS=-lopenblas
CFLAGS=-g3 -pedantic -Wall -lm -fPIC
CXXFLAGS=-g3 -pedantic -Wall -lm -fPIC

%: %.c
	$(CC) $< $(CFLAGS) -o $@

fu_mmap.ll: fu_mmap.c
	$(CC) $< -g3 -S -emit-llvm -o $@

fu_mmap.c: fu_mmap.h

macc.h: reg.h

blop.o: blop.cc blop.hh blop_impl.cc

macc_test: macc_test.cpp macc.h
	$(CXX) $< $(LDLIBS) -o $@

blasf_test: blasf_test.o fu_mmap.o
	$(CXX) $^ -o $@

blas_operation.o: blas_operation.cc blas_operation.h
	$(CXX) -fPIC $< $(LDLIBS) -c -o $@
