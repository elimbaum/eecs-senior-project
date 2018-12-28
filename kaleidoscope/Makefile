FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs core`

kld: kld.cpp
	clang++ -g3 $^ -o $@ $(FLAGS)
