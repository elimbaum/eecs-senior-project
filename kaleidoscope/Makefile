# LIBS=core orcjit native analysis executionengine instcombine object runtimedyld scalaropts support
LIBS=core orcjit native
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LIBS)`
HOME=/home/accts/emb99
ARM_TRIP=arm-linux-gnueabihf
SYSROOT=${HOME}/sproj/gcc-${ARM_TRIP}/${ARM_TRIP}

%: %.cpp KaleidoscopeJIT.h
	clang++ -v -g3 -O0 $< -o $@ $(FLAGS) -rdynamic

%-arm: %.cpp KaleidoscopeJIT.h
	clang++ -v -g3 -O0 $< -o $@ $(FLAGS) -rdynamic --target=${ARM_TRIP} \
		--sysroot=${SYSROOT}/libc \
		-I${SYSROOT}/include/c++/8.2.1 \
		-I${SYSROOT}/include/c++/8.2.1/${ARM_TRIP} \
		-L${SYSROOT}/lib/gcc/${ARM_TRIP}/8.2.1 \
		-B${SYSROOT}/bin

%.ll: %.cpp
	clang++ -g3 -S -emit-llvm $^ -o $@ $(FLAGS)

clean:
	rm core.* vgcore.*
