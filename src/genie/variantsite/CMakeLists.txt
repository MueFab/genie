project("genie-vsite")



set(source_files
        VariantSiteParser.cc
        JsonInfoFieldParser.cc
        ParameterSetComposer.cc
        AccessUnitComposer.cc
)

add_library(genie-vsite ${source_files})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-core)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-bsc)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-lzma)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-zstd)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
