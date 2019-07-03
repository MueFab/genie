project("genie")

set(source_files
        dsg.cc
        exceptions.cc
        gabac_integration.cc
        generation.cc
        genie_file_format.cc
        ProgramOptions.cc
        thread_pool.cc
        utilities.cc
)

add_executable(genie ${source_files})

if(${GENIE_USE_OPENMP})
    target_compile_definitions(genie PRIVATE GENIE_USE_OPENMP)
endif()

target_link_libraries(genie format)
target_link_libraries(genie alico)
target_link_libraries(genie fileio)
target_link_libraries(genie calq)
target_link_libraries(genie gabac)
target_link_libraries(genie spring)
