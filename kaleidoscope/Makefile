# LIBS=core orcjit native analysis executionengine instcombine object runtimedyld scalaropts support
LIBS=core mcjit native
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LIBS)`

%: %.cpp
	clang++ -g3 -O0 $^ -o $@ $(FLAGS) -rdynamic

%.ll: %.cpp
	clang++ -g3 -S -emit-llvm $^ -o $@ $(FLAGS)
