LLVM_LIBS=core orcjit native support analysis # aarch64
# ldflags creates dynamic dependency to my own copy of llvm
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LLVM_LIBS)` -fuse-ld=gold -g3
ROOT=$(HOME)/sproj-git
VERBOSE=#-v -Xlinker --verbose
CXX=clang++
PASS=$(ROOT)/passes/test/libTestPass.o
LIBS=libkld.o $(PASS)

default: kld

kld kld_s kld.ll: kld.cpp $(LIBS) KaleidoscopeJIT.h

kld: EXTRA_FLAG=-rdynamic
kld_s: EXTRA_FLAG=-static

kld kld_s:
	$(CXX) $(VERBOSE) kld.cpp $(LIBS) $(FLAGS) -o $@ $(EXTRA_FLAG)

%.ll:
	clang++ -g3 -S -emit-llvm $< -o $@

clean:
	rm core.* vgcore.*
