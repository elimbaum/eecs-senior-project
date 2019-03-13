LLVM_LIBS=core orcjit native support analysis # aarch64
# ldflags creates dynamic dependency to my own copy of llvm
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LLVM_LIBS)` -fuse-ld=gold -g3
ROOT=$(HOME)/sproj-git
VERBOSE=#-v #-Xlinker --verbose
CXX=clang++
PASS=$(ROOT)/passes/test/libTestPass.o
LIBS=$(ROOT)/kaleidoscope/libkld.o

EXE=hello

default: $(EXE)

$(EXE) $(EXE)_s $(EXE).ll: $(EXE).cpp

$(EXE): EXTRA_FLAG=-rdynamic
$(EXE)_s: EXTRA_FLAG=-static

$(EXE) $(EXE)_s:
	$(CXX) $(VERBOSE) $(EXE).cpp $(LIBS) $(FLAGS) -o $@ $(EXTRA_FLAG)

%.ll:
	clang++ -g3 -S -emit-llvm $< -o $@

clean:
	rm core.* vgcore.*
