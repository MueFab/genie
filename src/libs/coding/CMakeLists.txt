project("coding")

set(source_files
        gabac-compressor.cc
        gabac-decompressor.cc
        local-assembly-decoder.cc
        local-assembly-encoder.cc
        mpegg-raw-au.cc
        reference-based-decoder.cc
        reference-based-encoder.cc
        spring-decoder.cc
        spring-encoder.cc
        full-local-assembly-encoder.cc
        reference-encoder.cc
        read-encoder.cc
        )

add_library(coding ${source_files})
target_include_directories(coding PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

