LIBS=core mcjit native orcjit
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LIBS)`

%: %.cpp
	clang++ -v -g3 -O0 $^ -o $@ $(FLAGS) -rdynamic
