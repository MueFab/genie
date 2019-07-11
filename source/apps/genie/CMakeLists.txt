project("genie")

set(source_files
        dsg.cc
        exceptions.cc
        generation.cc
        ProgramOptions.cc
        StreamSaver.cc
        utilities.cc
)

add_executable(genie ${source_files})

target_link_libraries(genie PRIVATE gabac)
target_link_libraries(genie PRIVATE spring)
target_link_libraries(genie PRIVATE utils)

target_link_libraries(genie PRIVATE pthread)
