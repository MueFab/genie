cmake_minimum_required(VERSION 2.9.0)
project(Procrustes)

set(INC_LIBRARIES
        )

include(apply_dependencies NO_POLICY_SCOPE)

file(GLOB_RECURSE Procrustes_sources "*.c")

add_library(Procrustes OBJECT ${Procrustes_sources})


