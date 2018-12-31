FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs core mcjit native`

%: %.cpp
	clang++ -g3 $^ -o $@ $(FLAGS) -rdynamic
