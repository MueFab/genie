project("local-assembly-encoder")

set(source_files
        encoding.cc
        exceptions.cc
        main.cc
        program-options.cc
        reference-encoder.h reference-encoder.cc read-encoder.h read-encoder.cc read-decoder.h stream-container.h read-decoder.cc)

add_executable(local-assembly-encoder ${source_files})

target_link_libraries(local-assembly-encoder PRIVATE util)
