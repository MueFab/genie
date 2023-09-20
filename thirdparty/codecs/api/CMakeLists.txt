cmake_minimum_required(VERSION 2.9.0)
project(Api)

set(INC_LIBRARIES
        Libbsc
        Lzma
        Zstd
        jbigkit
        jbig-interface
        )

include(apply_dependencies NO_POLICY_SCOPE)

file(GLOB_RECURSE Api_sources "*.c")

add_library(Api OBJECT ${Api_sources})


#target_link_libraries(${PROJECT_NAME} PUBLIC jbigkit)
#target_link_libraries(${PROJECT_NAME} PUBLIC jbig-interface)


