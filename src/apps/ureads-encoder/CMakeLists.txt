project("ureads-encoder")

set(source_files
        encoding.cc
        main.cc
        program-options.cc
        )

add_executable(ureads-encoder ${source_files})

target_link_libraries(ureads-encoder PRIVATE spring)
target_link_libraries(ureads-encoder PRIVATE coding)
target_link_libraries(ureads-encoder PRIVATE format)
target_link_libraries(ureads-encoder PRIVATE util)
target_link_libraries(ureads-encoder PRIVATE gabac)
