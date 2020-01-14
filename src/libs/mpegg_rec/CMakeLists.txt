project("mpegg_rec")

set(source_files
        mgrecs-exporter.cc
        mgrecs-importer.cc
        )

add_library(mpegg_rec ${source_files})

target_link_libraries(mpegg_rec PUBLIC backbone)