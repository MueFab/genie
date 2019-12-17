project("coding")

set(source_files
        gabac-compressor.cc
        gabac-decompressor.cc
        local-assembly-encoder.cc
        mpegg-raw-au.cc
        reference-based-encoder.cc
        spring-encoder.cc
        reference-encoder.cc
        read-encoder.cc
        gabac-compressor.cc
        genie-gabac-output-stream.cc
        spring-encoder.cc
        gabac-seq-conf-set.cc
        ../../../test/libs/coding/gabac-compressor.h ../../../test/libs/coding/gabac-compressor.cc)

add_library(coding ${source_files})
target_include_directories(coding PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(coding format)
