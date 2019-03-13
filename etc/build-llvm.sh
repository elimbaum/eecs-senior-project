ROOT=/home/eli/
# TRIPLE=aarch64-linux-gnu
# 
# export CC=$ROOT/gcc-$TRIPLE/bin/$TRIPLE-gcc
# export CXX=$ROOT/gcc-$TRIPLE/bin/$TRIPLE-g++
# 
# echo CC is `ls $CC`
# echo CXX is `ls $CXX`
VERBOSE=Off

cmake -DCMAKE_BUILD_TYPE="Release" \
  -DLLVM_ENABLE_PROJECTS=clang \
  -DCMAKE_INSTALL_PREFIX=$ROOT/llvm \
  -DLLVM_TARGET_ARCH=X86 \
  -DLLVM_TARGETS_TO_BUILD=X86 \
  -DCMAKE_VERBOSE_MAKEFILE=$VERBOSE \
  -G Ninja ../llvm &&
cmake --build . &&
cmake --build . --target install

