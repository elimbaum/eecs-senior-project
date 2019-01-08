LIBS=core orcjit native analysis executionengine instcombine object runtimedyld scalaropts support
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LIBS)`

%: %.cpp
	clang++ -v -g3 -O0 $^ -o $@ $(FLAGS) -rdynamic

%.ll: %.cpp
	clang++ -v -g3 -S -emit-llvm $^ -o $@ $(FLAGS)
