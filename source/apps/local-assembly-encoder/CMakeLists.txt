project("local-assembly-encoder")

set(source_files
        encoding.cc
        main.cc
        program-options.cc
)

add_executable(local-assembly-encoder ${source_files})

target_link_libraries(local-assembly-encoder PRIVATE utils)
