LLVM_LIBS=core orcjit native # aarch64
# ldflags creates dynamic dependency to my own copy of llvm
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LLVM_LIBS)` -fuse-ld=gold -g3
ROOT=$(HOME)/sproj-git
VERBOSE=#-v -Xlinker --verbose
CXX=clang++
PASS=$(ROOT)/passes/test/libTestPass.o
LIBS=libkld.o $(PASS)

default: kld

kld kld_s kld.ll: kld.cpp $(LIBS) KaleidoscopeJIT.h

COMPILE=$(CXX) $(VERBOSE) kld.cpp $(LIBS) $(FLAGS) -o $@

kld:
	$(COMPILE) -rdynamic

kld_s:
	$(COMPILE) -static

%.ll:
	clang++ -g3 -S -emit-llvm $< -o $@

clean:
	rm core.* vgcore.*
