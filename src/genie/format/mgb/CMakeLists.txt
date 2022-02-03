project("genie-mgb")

set(source_files
        au_type_cfg.cc
        block.cc
        extended_au.cc
        mm_cfg.cc
        ref_cfg.cc
        signature_cfg.cc

        access_unit.cc
        raw_reference_seq.cc
        raw_reference.cc

        data-unit-factory.cc

        exporter.cc
        importer.cc

        reference.cc
        mgb_file.h mgb_file.cc)

add_library(genie-mgb ${source_files})

target_link_libraries(genie-mgb PUBLIC genie-core)