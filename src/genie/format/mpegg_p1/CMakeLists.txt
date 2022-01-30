project("genie-mpegg_p1")

set(source_files
        file_header.cc
        dataset_group.cc
        dataset_group_header.cc
        #        dataset/access_unit_p1.cc
        #        dataset.cc
        #        dataset_header.cc
        signature_cfg.cc
        block.cc
        #        dataset/master_index_table/access_unit_info/extended_au_info.cc
        #        dataset/master_index_table/access_unit_info/access_unit_info.cc
        #        dataset/master_index_table/u_access_unit_info/u_access_unit_info.cc
        #        dataset/master_index_table/master_index_table.cc
        #        dataset/master_index_table/ref_seq_info.cpp
        block_header.cc
        reference_location.cc

        reference_location.cc

        label_list.cc
        label.cc
        label_dataset.cc
        label_region.cc
        gen_info.h reference_metadata.h dataset_group_metadata.h master_index_table.h offset.h)

add_library(genie-mpegg_p1 ${source_files})

target_link_libraries(genie-mpegg_p1 PUBLIC genie-mgb)
