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

target_include_directories(genie-gvc PUBLIC ${XTENSOR_INCLUDE_DIRS})
#target_link_libraries(genie-gvc PUBLIC xtl)
#target_link_libraries(genie-gvc PUBLIC xtensor)
target_link_libraries(genie-gvc PUBLIC xtensor xtensor::optimize)
target_link_libraries(genie-gvc PUBLIC genie-core)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
