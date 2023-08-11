cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(jbigkit
        VERSION 2.1 # JBIGKIT version
        DESCRIPTION "JBIGKIT library version"
)

set(source_files
        jbig.c
        jbig85.c
        jbig_ar.c
)

add_library(${PROJECT_NAME} ${source_files})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# Declare public API of your library
get_filename_component(TOP_DIR ../ ABSOLUTE)
target_include_directories(${PROJECT_NAME} PUBLIC ${TOP_DIR})