project("genie-mpegg_p1")

set(source_files
#        dataset/access_unit_p1.cc
#        dataset.cc
#        dataset_header.cc
        util.cc
        dataset_group.cc
        file_header.cc
        mpegg_file.cc
        dataset/access_unit/access_unit.cc
        dataset/access_unit/block.cc
        dataset/fields/block_header_flags/class_info/class_info.cc
        dataset/fields/block_header_flags/block_header_flags.cc
        dataset/fields/block_header_flags/with_header.cc
        dataset/fields/block_header_flags/without_header.cc
#        dataset/fields/seq_additional_info/seq_additional_info.cc
        dataset/fields/seq_info/seq_info.cc
        dataset/fields/u_access_unit_info/u_access_unit_info.cc
#        dataset/master_index_table/access_unit_info/extended_au_info.cc
#        dataset/master_index_table/access_unit_info/access_unit_info.cc
#        dataset/master_index_table/u_access_unit_info/u_access_unit_info.cc
#        dataset/master_index_table/master_index_table.cc
#        dataset/master_index_table/ref_seq_info.cpp
        dataset/dataset.cc
        dataset/dataset_header.cc
        dataset/dataset_parameter_set.cc
        dataset/descriptor_stream.cc
        dataset/descriptor_stream_header.cc

        dataset/master_index_table/u_access_unit_info/u_access_unit_info.cc

        reference/reference.cc

        reference/reference_location/reference_location.cc
        reference/reference_location/external.cc
        reference/reference_location/internal.cc

        reference/reference_location/external_reference/external_reference.cc
        reference/reference_location/external_reference/mpegg_ref.cc
        reference/reference_location/external_reference/fasta_ref.cc
        reference/reference_location/external_reference/raw_ref.cc

        reference/reference_location/external_reference/checksum.cc
        reference/reference_location/external_reference/sha256.cc
        reference/reference_location/external_reference/md5.cc

#        reference_metadata/reference_metadata.cc
        reference/reference_location/internal.cc
        reference/reference_location/external.cc
        reference/reference_location/reference_location.cc
        reference/reference_location/external_reference/external_reference.cc
        reference/reference_location/external_reference/mpegg_ref.cc
        reference/reference_location/external_reference/raw_ref.cc
        reference/reference_location/external_reference/fasta_ref.cc
        reference/reference_location/external_reference/checksum.cc
        reference/reference_location/external_reference/md5.cc
        reference/reference_location/external_reference/sha256.cc

        label_list/label_list.cc
        label_list/label/label.cc
        label_list/label/dataset_info.cc
        label_list/label/dataset_region.cc
        )

add_library(genie-mpegg_p1 ${source_files})

target_link_libraries(genie-mpegg_p1 PUBLIC genie-mgb)
