project("genie")

set(source_files
        dsg.cc
        generation.cc
        program-options.cc
        stream-saver.cc
        utilities.cc
)

add_executable(genie ${source_files})

target_link_libraries(genie PRIVATE gabac)
target_link_libraries(genie PRIVATE spring)
target_link_libraries(genie PRIVATE util)
target_link_libraries(genie PRIVATE pthread)

if(${GENIE_USE_OPENMP})
    target_compile_definitions(genie PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(genie PRIVATE ${OpenMP_CXX_LIBRARIES})
endif()
