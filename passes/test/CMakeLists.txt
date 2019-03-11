# Shared object version
add_library(TestPass MODULE test.cpp)

# Static version
# add_library(LLVMTest test.cpp)

target_compile_features(TestPass PRIVATE cxx_range_for cxx_auto_type)
