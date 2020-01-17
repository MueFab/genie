project("genie-core")

set(source_files
        parameter/descriptor_present/decoder.cc
        parameter/descriptor_present/descriptor_present.cc

        parameter/computed_ref.cc
        parameter/computed_ref_extended.cc
        parameter/data_unit.cc
        parameter/descriptor.cc
        parameter/descriptor_box.cc
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

        access-unit-payload.cc
        access-unit-raw.cc
        constants.cc
        global-cfg.cc
        locus.cc
        )

add_library(genie-core ${source_files})


target_link_libraries(genie-core PUBLIC genie-util)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-core PUBLIC "${THIS_DIR}")
target_include_directories(genie-core PUBLIC "${TOP_DIR}")