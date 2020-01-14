project("mpegg_read")

set(source_files
        read-decoder.cc
        read-encoder.cc)

add_library(mpegg_read ${source_files})

target_link_libraries(mpegg_read PUBLIC backbone)