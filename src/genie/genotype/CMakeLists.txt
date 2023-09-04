project("genie-gvc")

set(XTENSOR_USE_XSIMD 0)
set(XTENSOR_USE_TBB 0)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

set(source_files
        genotype_coder.cc
        genotype_parameters.cc
)

add_library(genie-gvc ${source_files})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # GNU Compiler Collection
    #
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wundef")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common")
    target_compile_options(genie-gvc PUBLIC "-Wshadow")
    target_compile_options(genie-gvc PUBLIC "-pedantic")
    target_compile_options(genie-gvc PUBLIC "-Wall")
    target_compile_options(genie-gvc PUBLIC "-Wextra")
    target_compile_options(genie-gvc PUBLIC "-Wshadow")
    target_compile_options(genie-gvc PUBLIC "-Wundef")
    target_compile_options(genie-gvc PUBLIC "-Wconversion")
    target_compile_options(genie-gvc PUBLIC "-Wdouble-promotion")
    target_compile_options(genie-gvc PUBLIC "-fno-common")
endif ()


#target_include_directories(genie-gvc PUBLIC ${XTENSOR_INCLUDE_DIRS})
#target_link_libraries(genie-gvc PUBLIC xtl)
#target_link_libraries(genie-gvc PUBLIC xtensor)
target_link_libraries(${PROJECT_NAME} PUBLIC xtensor xtensor::optimize)
target_link_libraries(${PROJECT_NAME} PUBLIC mpeggCodecs-shared)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-core)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
