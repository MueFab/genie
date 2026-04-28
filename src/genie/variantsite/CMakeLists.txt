project("genie-variantsite")

set(source_files
    accessunit_composer.cc
    parameterset_composer.cc
)

add_library(${PROJECT_NAME} ${source_files})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-core)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-annotation)