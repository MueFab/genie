project("genie-mpegg_p1")

set(source_files
#        dataset/access_unit_p1.cc
#        dataset.cc
#        dataset_header.cc
        dataset_group.cc
        dataset/dataset_parameter_set.cc
        file_header.cc
        mpegg_file.cc
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
        reference_metadata/reference_metadata.cpp
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
        dataset/dataset.cc
        dataset/dataset_header.cc
        dataset/fields/block_header_flags/block_header_flags.cpp
        dataset/fields/block_header_flags/with_header.cpp
        dataset/fields/block_header_flags/without_header.cpp
        dataset/fields/seq_info/seq_info.cpp
        dataset/fields/block_header_flags/class_info/class_info.cpp
        dataset/fields/u_access_unit_info/u_access_unit_info.cpp
        dataset/fields/seq_additinal_info/seq_additional_info.cpp
        access_unit/access_unit.h
        master_index_table/master_index_table.h
        master_index_table/access_unit_info/access_unit_info.h
        master_index_table/u_access_unit_info/u_access_unit_info.h
        master_index_table/ref_seq_info.h
        master_index_table/access_unit_info/extended_au_info.h
        master_index_table/ref_seq_info.cpp
        master_index_table/access_unit_info/extended_au_info.cpp
        master_index_table/access_unit_info/access_unit_info.cpp access_unit/access_unit.cc)

add_library(genie-mpegg_p1 ${source_files})

target_link_libraries(genie-mpegg_p1 PUBLIC genie-mgb)
