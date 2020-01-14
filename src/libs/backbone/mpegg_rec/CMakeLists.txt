project("backbone_mpegg_rec")

set(source_files
        external_alignment/external-alignment-none.cc
        external_alignment/external-alignment-other-rec.cc

        split_alignment/split-alignment-other-rec.cc
        split_alignment/split-alignment-same-rec.cc
        split_alignment/split-alignment-unpaired.cc

        alignment.cc
        alignment-container.cc
        external-alignment.cc
        meta-alignment.cc
        ../../mpegg_rec/mgrecs-exporter.cc
        ../../mpegg_rec/mgrecs-importer.cc
        mpegg-record.cc
        segment.cc
        split-alignment.cc
        )

add_library(backbone_mpegg_rec ${source_files})
get_filename_component(THIS_DIR ./ ABSOLUTE)
target_include_directories(backbone_mpegg_rec PRIVATE "${THIS_DIR}")
