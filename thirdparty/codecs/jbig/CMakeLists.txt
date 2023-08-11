cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(jbig-interface
        VERSION 1.0
        DESCRIPTION "JBIG-interface version"
)

option(JBIG_INTERFACE_STATIC_LIB "Build static library" ON)

set(source_files
        jbig_interface.c
)

if(JBIG_INTERFACE_STATIC_LIB)
    add_library(${PROJECT_NAME} ${source_files})
else()
    add_library(${PROJECT_NAME} SHARED ${source_files})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

get_filename_component(TOP_DIR ../ ABSOLUTE)
target_include_directories(${PROJECT_NAME} PUBLIC ${TOP_DIR})

add_subdirectory(jbigkit)
target_link_libraries(${PROJECT_NAME} PUBLIC jbigkit)

#install(TARGETS ${PROJECT_NAME}
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})