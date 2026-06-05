cmake_minimum_required(VERSION 3.5)
project(Lzma)

# disable 7ZIP dependencies
add_definitions(-D_7ZIP_ST)

set(INC_LIBRARIES

        )

include(apply_dependencies NO_POLICY_SCOPE)

file(GLOB Lzma_sources "*.c")
add_library(Lzma OBJECT ${Lzma_sources})
