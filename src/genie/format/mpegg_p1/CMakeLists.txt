project("genie-mpegg_p1")

set(source_files
        mpegg_file.cc
        file_header.cc
        dataset_group.cc
        dataset_group_header.cc
        #        dataset/access_unit_p1.cc
        #        dataset.cc
        #        dataset_header.cc
        util.cc
        dataset/access_unit/access_unit.cc
        dataset/access_unit/access_unit_header.cc
        dataset/access_unit/signature_cfg.cc
        dataset/access_unit/block.cc
        dataset/u_access_unit_info.cc
        #        dataset/master_index_table/access_unit_info/extended_au_info.cc
        #        dataset/master_index_table/access_unit_info/access_unit_info.cc
        #        dataset/master_index_table/u_access_unit_info/u_access_unit_info.cc
        #        dataset/master_index_table/master_index_table.cc
        #        dataset/master_index_table/ref_seq_info.cpp
        dataset/dataset.cc
        dataset/dataset_header.cc
        dataset/dataset_parameter_set.cc
        dataset/access_unit/block_header.cc
        dataset/descriptor_stream.cc
        dataset/descriptor_stream_header.cc

        reference/reference.cc

        reference/reference_location/reference_location.cc
        reference/reference_location/external.cc
        reference/reference_location/internal.cc

        # ExternalReference
        reference/reference_location/external_reference/external_reference.cc
        reference/reference_location/external_reference/mpegg_ref.cc
        reference/reference_location/external_reference/fasta_ref.cc
        reference/reference_location/external_reference/raw_ref.cc

        reference/reference_location/external_reference/checksum.cc
        reference/reference_location/external_reference/sha256.cc
        reference/reference_location/external_reference/md5.cc

        reference_metadata/reference_metadata.cc
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
        dataset/class_description.cc
        )

add_library(genie-mpegg_p1 ${source_files})

target_link_libraries(genie-mpegg_p1 PUBLIC genie-mgb)
