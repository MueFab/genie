project("local-assembly-encoder")

set(source_files
        encoding.cc
        main.cc
        program-options.cc
        )

add_executable(local-assembly-encoder ${source_files})

target_link_libraries(local-assembly-encoder PRIVATE gabac)
target_link_libraries(local-assembly-encoder PRIVATE mpegg_p2)
target_link_libraries(local-assembly-encoder PRIVATE sam)
target_link_libraries(local-assembly-encoder PRIVATE local_assembly)
