cmake_minimum_required(VERSION 2.9.0)
project(Zstd)

set(INC_LIBRARIES
        )

include(apply_dependencies NO_POLICY_SCOPE)

file(GLOB_RECURSE Zstd_sources "*.c")

add_compile_definitions(ZSTD_DISABLE_ASM)
add_library(Zstd OBJECT ${Zstd_sources})


#set_property(TARGET Libbsc PROPERTY POSITION_INDEPENDENT_CODE ON)

