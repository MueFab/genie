project("format")

set(source_files
        fasta/fasta-file-reader.cc
        fasta/fasta-record.cc

        fastq/fastq-exporter.cc
        fastq/fastq-importer.cc
        fastq/fastq-file-reader.cc
        fastq/fastq-record.cc

        mpegg_rec/mgrecs-exporter.cc
        mpegg_rec/mgrecs-importer.cc
        mpegg_rec/alignment.cc
        mpegg_rec/alignment-container.cc
        mpegg_rec/external-alignment.cc
        mpegg_rec/external_alignment/external-alignment-none.cc
        mpegg_rec/external_alignment/external-alignment-other-rec.cc
        mpegg_rec/meta-alignment.cc
        mpegg_rec/mpegg-record.cc
        mpegg_rec/segment.cc
        mpegg_rec/split-alignment.cc
        mpegg_rec/split_alignment/split-alignment-other-rec.cc
        mpegg_rec/split_alignment/split-alignment-same-rec.cc
        mpegg_rec/split_alignment/split-alignment-unpaired.cc

        mpegg_p2/access_unit.cc
        mpegg_p2/access_unit/au_type_cfg.cc
        mpegg_p2/access_unit/block.cc
        mpegg_p2/access_unit/extended_au.cc
        mpegg_p2/access_unit/mm_cfg.cc
        mpegg_p2/access_unit/ref_cfg.cc
        mpegg_p2/access_unit/signature_cfg.cc
        mpegg_p2/clutter.cc
        mpegg_p2/data_unit.cc
        mpegg_p2/mpegg-p-2-exporter.cc
        mpegg_p2/parameter_set.cc
        mpegg_p2/parameter_set/crps_info.cc
        mpegg_p2/parameter_set/descriptor_configuration.cc
        mpegg_p2/parameter_set/descriptor_configuration_container.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/cabac/cabac_binarization.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/cabac/cabac_binarization_parameters.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/cabac/cabac_context_parameters.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/cabac/support_values.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/cabac/transform_subseq_cfg.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/cabac/transform_subseq_parameters.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/decoder_configuration.cc
        mpegg_p2/parameter_set/descriptor_configuration_present/descriptor_configuration_present.cc
        mpegg_p2/parameter_set/parameter_set_crps.cc
        mpegg_p2/parameter_set/qv_coding_config.cc
        mpegg_p2/parameter_set/qv_coding_config_1/parameter_set_qvps.cc
        mpegg_p2/parameter_set/qv_coding_config_1/qv_codebook.cc
        mpegg_p2/parameter_set/qv_coding_config_1/qv_coding_config_1.cc
        mpegg_p2/raw_reference.cc
        mpegg_p2/raw_reference_sequence.cc

        sam/sam-exporter.cc
        sam/sam-importer.cc
        sam/sam-file-reader.cc
        sam/sam-line.cc
        sam/sam-record.cc
        sam/sam-tag.cc
        )

add_library(format ${source_files})
target_include_directories(format PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/mpegg_rec")
target_link_libraries(format util)
target_link_libraries(format gabac)

