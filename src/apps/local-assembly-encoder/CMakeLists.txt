project("local-assembly-encoder")

set(source_files
        encoding.cc
        main.cc
        program-options.cc
        reference-encoder.cc
        read-encoder.cc
        read-decoder.cc
        full-local-assembly-encoder.cc
        full-local-assembly-decoder.cc
        genie-gabac-output-stream.cc)

add_executable(local-assembly-encoder ${source_files})

target_link_libraries(local-assembly-encoder PRIVATE util)
target_link_libraries(local-assembly-encoder PRIVATE gabac)
target_link_libraries(local-assembly-encoder PRIVATE format)
