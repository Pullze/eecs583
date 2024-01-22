# add_llvm_library(samplePass MODULE
#     Pass.cpp
#     )
add_llvm_pass_plugin(samplePass Pass.cpp)