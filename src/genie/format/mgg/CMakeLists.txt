project("genie-mgg")

set(source_files
        file_header.cc
        dataset_group.cc
        dataset_group_header.cc
        #        dataset/access_unit_p1.cc
        #        dataset.cc
        #        dataset_header.cc
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
        access_unit.cc
        access_unit_header.cc
        au_information.cc
        au_protection.cc
        dataset.cc
        dataset_group_metadata.cc
        dataset_group_protection.cc
        dataset_header.cc
        dataset_mapping_table.cc
        dataset_mapping_table_list.cc
        dataset_metadata.cc
        dataset_parameter_set.cc
        dataset_protection.cc
        descriptor_stream.cc
        descriptor_stream_header.cc
        gen_info.cc
        master_index_table.cc
        offset.cc
        packet.cc
        packet_header.cc
        reference.cc
        reference_metadata.cc
        box.cc
        mgg_file.cc)

add_library(genie-mgg ${source_files})

target_link_libraries(genie-mgg PUBLIC genie-mgb)
