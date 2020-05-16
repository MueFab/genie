project("genie-core")

set(source_files
        parameter/descriptor_present/decoder.cc
        parameter/descriptor_present/decoder-regular.cc
        parameter/descriptor_present/decoder-tokentype.cc
        parameter/descriptor_present/descriptor_present.cc

        parameter/computed_ref.cc
        parameter/computed_ref_extended.cc
        parameter/data_unit.cc
        parameter/descriptor.cc
        parameter/descriptor_subseq_cfg.cc
        parameter/parameter_set.cc
        parameter/quality-values.cc

        record/alignment_external/none.cc
        record/alignment_external/other-rec.cc
        record/alignment_split/other-rec.cc
        record/alignment_split/same-rec.cc
        record/alignment_split/unpaired.cc
        record/segment.cc
        record/record.cc
        record/alignment-split.cc
        record/alignment-shared-data.cc
        record/alignment-external.cc
        record/alignment-box.cc
        record/alignment.cc

        stats/perf-stats.cc

        access-unit-payload.cc
        access-unit-raw.cc
        classifier-bypass.cc
        classifier-regroup.cc
        constants.cc
        format-importer.cc
        format-exporter.cc
        format-exporter-compressed.cc
        global-cfg.cc
        locus.cc
        name-encoder-none.cc
        read-encoder.cc
        read-decoder.cc
        )

add_library(genie-core ${source_files})


target_link_libraries(genie-core PUBLIC genie-util)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-core PUBLIC "${THIS_DIR}")
target_include_directories(genie-core PUBLIC "${TOP_DIR}")
