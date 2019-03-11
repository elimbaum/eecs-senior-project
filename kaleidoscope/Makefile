# LIBS=core orcjit native analysis executionengine instcombine object runtimedyld scalaropts support
LLVM_LIBS=core orcjit native # aarch64
# ldflags creates dynamic dependency to my own copy of llvm
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LLVM_LIBS)`
ROOT=$(HOME)/sproj-git
# ARM_TRIP=aarch64-linux-gnu
# SYSROOT=${HOME}/sproj/fresh-arm/gcc-${ARM_TRIP}/${ARM_TRIP}
# LDFLAGS=-static-libstdc++
VERBOSE=#-v -Xlinker --verbose
CXX=clang++ -fuse-ld=gold
PASS=$(ROOT)/passes/test/libTestPass.so
LIBS=libkld.o -l:$(PASS) #-lLLVMHello

default: kld

%: %.cpp libkld.o $(PASS) KaleidoscopeJIT.h 
	$(CXX) $(VERBOSE) -g3 -O0 $< $(LIBS) -o $@ $(FLAGS) -rdynamic

%-static: %.cpp libkld.o KaleidoscopeJIT.h
	$(CXX) $(VERBOSE) -g3 -O0 $< $(LIBS) -o $@ $(FLAGS) -static

%.ll: %.cpp
	clang++ -g3 -S -emit-llvm $^ -o $@ $(FLAGS)

clean:
	rm core.* vgcore.*
