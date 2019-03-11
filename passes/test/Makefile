FLAGS=`llvm-config --cxxflags`


all: libTestPass.so

libTestPass.so: test.cpp
	clang $< $(FLAGS) -g3 -c -fPIC -o $@

