# LIBS=core orcjit native analysis executionengine instcombine object runtimedyld scalaropts support
LIBS=core orcjit native # aarch64
# ldflags creates dynamic dependency to my own copy of llvm
FLAGS=`llvm-config --cxxflags --ldflags --system-libs --libs $(LIBS)`
# HOME=/home/accts/emb99
# ARM_TRIP=aarch64-linux-gnu
# SYSROOT=${HOME}/sproj/fresh-arm/gcc-${ARM_TRIP}/${ARM_TRIP}
# LDFLAGS=-static-libstdc++

%: %.cpp KaleidoscopeJIT.h
	clang++ -v -g3 -O0 $< -o $@ $(FLAGS) -rdynamic

%-static: %.cpp KaleidoscopeJIT.h
	clang++ -v -g3 -O0 -static $< -o $@ $(FLAGS) -rdynamic -Xlinker --verbose \
	  -L/home/accts/emb99/sproj/lib \
	  -L/usr/lib/debug/usr/lib64 \
	  -L/usr/lib/x86_64-linux-gnu \
	  -L/usr/local/anaconda/lib \
	  -L/usr/lib64/gcc/x86_64-redhat-linux/7.3.1

%-arm: %.cpp KaleidoscopeJIT.h
	${SYSROOT}/../bin/${ARM_TRIP}-g++ \
	  -v -g3 -O0 $< -o $@ $(FLAGS) \
	  -L${HOME}/sproj/fresh-arm/llvm-project/build/lib \
	  -L${SYSROOT}/../lib \
	  -I${SYSROOT}/include/c++/7.4.1 \
	  -I${SYSROOT}/include/c++/7.4.1/${ARM_TRIP} \
	  -L${SYSROOT}/lib/gcc/${ARM_TRIP}/8.2.1 \
	  -B${SYSROOT}/bin

%.ll: %.cpp
	clang++ -g3 -S -emit-llvm $^ -o $@ $(FLAGS)

clean:
	rm core.* vgcore.*
