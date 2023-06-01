cmake_minimum_required(VERSION 2.9.0)
project(Api)

set(INC_LIBRARIES
        Libbsc
        Lzma
        Zstd
        )

include(apply_dependencies NO_POLICY_SCOPE)

file(GLOB_RECURSE Api_sources "*.c")

add_library(Api OBJECT ${Api_sources})


