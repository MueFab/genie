project("ureads-encoder")

set(source_files
        encoding.cc
        main.cc
        program-options.cc
        )

add_executable(ureads-encoder ${source_files})

target_link_libraries(ureads-encoder PUBLIC genie-sam)
target_link_libraries(ureads-encoder PUBLIC genie-localassembly)
target_link_libraries(ureads-encoder PUBLIC genie-mgb)
target_link_libraries(ureads-encoder PUBLIC genie-gabac)
