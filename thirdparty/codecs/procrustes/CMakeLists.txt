cmake_minimum_required(VERSION 3.5)
project(Procrustes)

set(INC_LIBRARIES
        )

include(apply_dependencies NO_POLICY_SCOPE)

file(GLOB_RECURSE Procrustes_sources "*.c")

add_library(Procrustes OBJECT ${Procrustes_sources})


