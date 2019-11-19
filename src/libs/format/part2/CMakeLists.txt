project("format")

set(source_files
        access_unit.cc
        clutter.cc
        access_unit/block.cc
        access_unit/extended_au.cc
        access_unit/signature_cfg.cc
        access_unit/mm_cfg.cc
        access_unit/ref_cfg.cc
        access_unit/au_type_cfg.cc
        data_unit.cc
        raw_reference_sequence.cc
        raw_reference.cc
        parameter_set.cc
        parameter_set/crps_info.cc
        parameter_set/descriptor_configuration_container.cc
        parameter_set/parameter_set_crps.cc
        parameter_set/descriptor_configuration.cc
        parameter_set/qv_coding_config_1/parameter_set_qvps.cc
        parameter_set/qv_coding_config_1/qv_codebook.cc
        parameter_set/qv_coding_config_1/qv_coding_config_1.cc
        parameter_set/descriptor_configuration_present/decoder_configuration.cc
        parameter_set/descriptor_configuration_present/descriptor_configuration_present.cc
        parameter_set/descriptor_configuration_present/cabac/cabac_context_parameters.cc
        parameter_set/descriptor_configuration_present/cabac/cabac_binarization_parameters.cc
        parameter_set/descriptor_configuration_present/cabac/cabac_binarization.cc
        parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.cc
        parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.cc
        parameter_set/descriptor_configuration_present/cabac/support_values.cc
        parameter_set/descriptor_configuration_present/cabac/transform_subseq_parameters.cc
        parameter_set/descriptor_configuration_present/cabac/transform_subseq_cfg.cc
        parameter_set/qv_coding_config.cc
)

add_library(format ${source_files})
target_link_libraries(format util)
target_link_libraries(format gabac)
