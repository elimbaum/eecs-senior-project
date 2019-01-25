# LIBS=core orcjit native analysis executionengine instcombine object runtimedyld scalaropts support
LIBS=core orcjit native
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LIBS)`

%: %.cpp CustomJIT.h
	clang++ -g3 -O0 $< -o $@ $(FLAGS) -rdynamic

%.ll: %.cpp
	clang++ -g3 -S -emit-llvm $^ -o $@ $(FLAGS)

clean:
	rm core.* vgcore.*
