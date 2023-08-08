project("genie-gvc")

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

set(XTENSOR_USE_TBB 1)

set(source_files
        genotype_coder.cc
        genotype_parameters.cc
)

add_library(genie-gvc ${source_files})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

#target_link_libraries(genie-gvc PUBLIC xtensor)
#target_include_directories(genie-gvc PUBLIC ${XTENSOR_INCLUDE_DIRS})
target_link_libraries(genie-gvc PUBLIC xtensor xtensor::optimize)
target_link_libraries(genie-gvc PUBLIC genie-core)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
