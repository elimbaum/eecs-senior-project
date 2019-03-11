FLAGS=`llvm-config --cxxflags`


all: libTestPass.so

libTestPass.so: test.cpp
	clang $< $(FLAGS) -c -fPIC -o $@

