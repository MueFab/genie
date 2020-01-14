project("backbone")

set(source_files
        block-payload.cc
        constants.cc
        mpegg-raw-au.cc
        )

add_library(backbone ${source_files})

add_subdirectory("mpegg_rec")

target_link_libraries(backbone PUBLIC backbone_mpegg_rec)
target_link_libraries(backbone PUBLIC util)