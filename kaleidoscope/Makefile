# LIBS=core orcjit native analysis executionengine instcombine object runtimedyld scalaropts support
LIBS=core orcjit native # aarch64
# ldflags creates dynamic dependency to my own copy of llvm
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LIBS)`
# HOME=/home/accts/emb99
# ARM_TRIP=aarch64-linux-gnu
# SYSROOT=${HOME}/sproj/fresh-arm/gcc-${ARM_TRIP}/${ARM_TRIP}
# LDFLAGS=-static-libstdc++
VERBOSE=-v -Xlinker --verbose
CXX=clang++
# HELLO=-l:LLVMHello.so
HELLO=/home/eli/llvm-project/llvm/lib/Transforms/Hello/hello.so

%: %.cpp libkld.o KaleidoscopeJIT.h 
	$(CXX) $(VERBOSE) -g3 -O0 $< libkld.o $(HELLO) -o $@ $(FLAGS) -rdynamic

%-static: %.cpp libkld.o KaleidoscopeJIT.h
	$(CXX) $(VERBOSE) -g3 -O0 -static $< libkld.o -o $@ $(FLAGS)

%.ll: %.cpp
	clang++ -g3 -S -emit-llvm $^ -o $@ $(FLAGS)

clean:
	rm core.* vgcore.*
