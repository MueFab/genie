
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "RandomRecordFillIn.h"

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::annotation_encoding_parameters::LikelihoodParameters
RandomAnnotationEncodingParameters::randomLikelihood() {
    uint8_t num_gl_per_sample = randomU8();
    bool transform_flag = randomBool();
    uint8_t dtype_id = randomU8();
    return genie::core::record::annotation_encoding_parameters::LikelihoodParameters(num_gl_per_sample, transform_flag,
                                                                                     dtype_id);
}

genie::core::record::annotation_encoding_parameters::GenotypeParameters
RandomAnnotationEncodingParameters::randomGenotypeParameters() {
    uint8_t max_ploidy = randomU8();
    bool no_reference_flag = randomBool();
    bool not_available_flag = randomBool();
    genie::core::record::annotation_encoding_parameters::BinarizationID binarization_ID =
        static_cast<genie::core::record::annotation_encoding_parameters::BinarizationID>(randomBool());
    uint8_t num_bit_plane = 0;
    genie::core::record::annotation_encoding_parameters::ConcatAxis concat_axis =
        genie::core::record::annotation_encoding_parameters::ConcatAxis::DO_NOT_CONCAT;
    if (binarization_ID == genie::core::record::annotation_encoding_parameters::BinarizationID::BIT_PLANE) {
        num_bit_plane = randomU8();
        concat_axis = static_cast<genie::core::record::annotation_encoding_parameters::ConcatAxis>(rand() % 3);
    }

    uint8_t num_variants_payloads = 1;
    if (concat_axis == genie::core::record::annotation_encoding_parameters::ConcatAxis::DO_NOT_CONCAT &&
        binarization_ID == genie::core::record::annotation_encoding_parameters::BinarizationID::BIT_PLANE)
        num_variants_payloads = num_bit_plane;

    std::vector<bool> sort_variants_rows_flag{};
    std::vector<bool> sort_variants_cols_flag{};
    std::vector<bool> transpose_variants_mat_flag{};
    std::vector<uint8_t> variants_codec_ID{};
    for (auto i = 0; i < num_variants_payloads; ++i) {
        sort_variants_rows_flag.push_back(randomBool());
        sort_variants_cols_flag.push_back(randomBool());
        transpose_variants_mat_flag.push_back(randomBool());
        variants_codec_ID.push_back(static_cast<uint8_t>(rand() % 16) + 16);
    }

    bool sort_phases_rows_flag = false;
    bool sort_phases_cols_flag = false;
    bool transpose_phases_mat_flag = false;
    bool phases_codec_ID = false;
    bool phases_value = false;
    bool encode_phases_data_flag = false;
    if (encode_phases_data_flag) {
        sort_phases_rows_flag = randomBool();
        sort_phases_cols_flag = randomBool();
        transpose_phases_mat_flag = randomBool();
        phases_codec_ID = randomBool();
        phases_value = randomBool();
    }

    return genie::core::record::annotation_encoding_parameters::GenotypeParameters(
        max_ploidy, no_reference_flag, not_available_flag, binarization_ID, num_bit_plane, concat_axis,
        sort_variants_rows_flag, sort_variants_cols_flag, transpose_variants_mat_flag, variants_codec_ID,
        encode_phases_data_flag, sort_phases_rows_flag, sort_phases_cols_flag, transpose_phases_mat_flag,
        phases_codec_ID, phases_value);
}

genie::core::record::annotation_encoding_parameters::AttributeParameterSet
RandomAnnotationEncodingParameters::randomAttributeParameterSet() {
    uint16_t attribute_ID = randomU16();
    uint8_t attribute_name_len = randomU8();
    if (attribute_name_len < 3) attribute_name_len = 3;
    std::string attribute_name = randomString(attribute_name_len);
    uint8_t attribute_type = randomType();
    uint8_t attribute_num_array_dims = randomU2();

    std::vector<uint8_t> attribute_array_dims{};
    for (auto i = 0; i < attribute_num_array_dims; ++i) attribute_array_dims.push_back(randomU8());
    
    std::vector<uint8_t> attribute_default_val = randomValForType(attribute_type);

    bool attribute_miss_val_flag = randomBool();
    bool attribute_miss_default_flag = randomBool();

    std::vector<uint8_t> attribute_miss_val = randomValForType(attribute_type);

    for (auto& byte : attribute_miss_val) byte = randomU8();

    uint8_t strLen = randomU4() + 3;
    std::string attribute_miss_str = randomString(strLen);

    uint8_t compressor_ID = randomU8();

    uint8_t n_steps_with_dependencies = randomU8();
    std::vector<uint8_t> dependency_step_ID(n_steps_with_dependencies, 0);
    for (auto& byte : dependency_step_ID) byte = randomU8();
    std::vector<uint8_t> n_dependencies(n_steps_with_dependencies, 0);
    for (auto& byte : n_dependencies) byte = randomU4();
    std::vector<std::vector<uint8_t>> dependency_var_ID(n_steps_with_dependencies, std::vector<uint8_t>());
    std::vector<std::vector<bool>> dependency_is_attribute(n_steps_with_dependencies, std::vector<bool>());
    std::vector<std::vector<uint16_t>> dependency_ID(n_steps_with_dependencies, std::vector<uint16_t>());
    for (auto i = 0; i < n_steps_with_dependencies; ++i) {
        dependency_var_ID[i].resize(n_dependencies[i]);
        dependency_is_attribute[i].resize(n_dependencies[i]);
        dependency_ID[i].resize(n_dependencies[i]);
        for (auto j = 0; j < n_dependencies[i]; ++j) {
            dependency_var_ID[i][j] = randomU4();
            dependency_is_attribute[i][j] = (randomBool());
            if (dependency_is_attribute[i][j])
                dependency_ID[i][j] = (randomU16());
            else
                dependency_ID[i][j] = (randomU8() / 2);
        }
    }
    genie::core::record::annotation_encoding_parameters::AttributeParameterSet attr(
        attribute_ID, attribute_name_len, attribute_name, attribute_type, attribute_num_array_dims,
        attribute_array_dims, attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag,
        attribute_miss_val, attribute_miss_str, compressor_ID, n_steps_with_dependencies, dependency_step_ID,
        n_dependencies, dependency_var_ID, dependency_is_attribute, dependency_ID);
    return attr;
}

genie::core::record::annotation_encoding_parameters::AlgorithmParameters
RandomAnnotationEncodingParameters::randomAlgorithmParameters() {
    uint8_t n_pars = randomU8();
    std::vector<uint8_t> par_ID(n_pars, 0);
    std::vector<uint8_t> par_type(n_pars, 0);
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val(
        n_pars, std::vector<std::vector<std::vector<std::vector<uint8_t>>>>(0));
    for (auto i = 0; i < n_pars; ++i) {
        par_ID[i] = randomU4();
    }

    return genie::core::record::annotation_encoding_parameters::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}