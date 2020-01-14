project("ureads-encoder")

set(source_files
        encoding.cc
        main.cc
        program-options.cc
        )

add_executable(ureads-encoder ${source_files})

target_link_libraries(ureads-encoder PUBLIC sam)
target_link_libraries(ureads-encoder PUBLIC local_assembly)
target_link_libraries(ureads-encoder PUBLIC mpegg_p2)
target_link_libraries(ureads-encoder PUBLIC gabac)
