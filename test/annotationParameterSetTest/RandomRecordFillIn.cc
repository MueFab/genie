
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "RandomRecordFillIn.h"

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::annotation_parameter_set::LikelihoodParameters
RandomAnnotationEncodingParameters::randomLikelihood() {
    uint8_t num_gl_per_sample = randomU8();
    bool transform_flag = randomBool();
    uint8_t dtype_id = randomU8();
    return genie::core::record::annotation_parameter_set::LikelihoodParameters(num_gl_per_sample, transform_flag,
                                                                                     dtype_id);
}

genie::core::record::annotation_parameter_set::GenotypeParameters
RandomAnnotationEncodingParameters::randomGenotypeParameters() {
    uint8_t max_ploidy = randomU8();
    bool no_reference_flag = randomBool();
    bool not_available_flag = randomBool();
    genie::core::record::annotation_parameter_set::BinarizationID binarization_ID =
        static_cast<genie::core::record::annotation_parameter_set::BinarizationID>(randomBool());
    uint8_t num_bit_plane = 0;
    genie::core::record::annotation_parameter_set::ConcatAxis concat_axis =
        genie::core::record::annotation_parameter_set::ConcatAxis::DO_NOT_CONCAT;
    if (binarization_ID == genie::core::record::annotation_parameter_set::BinarizationID::BIT_PLANE) {
        num_bit_plane = randomU8();
        concat_axis = static_cast<genie::core::record::annotation_parameter_set::ConcatAxis>(rand() % 3);
    }

    uint8_t num_variants_payloads = 1;
    if (concat_axis == genie::core::record::annotation_parameter_set::ConcatAxis::DO_NOT_CONCAT &&
        binarization_ID == genie::core::record::annotation_parameter_set::BinarizationID::BIT_PLANE)
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

    return genie::core::record::annotation_parameter_set::GenotypeParameters(
        max_ploidy, no_reference_flag, not_available_flag, binarization_ID, num_bit_plane, concat_axis,
        sort_variants_rows_flag, sort_variants_cols_flag, transpose_variants_mat_flag, variants_codec_ID,
        encode_phases_data_flag, sort_phases_rows_flag, sort_phases_cols_flag, transpose_phases_mat_flag,
        phases_codec_ID, phases_value);
}

genie::core::record::annotation_parameter_set::AttributeParameterSet
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
    if (!attribute_miss_val_flag) attribute_miss_default_flag = false;
    std::vector<uint8_t> attribute_miss_val = randomValForType(attribute_type);
    if (!attribute_miss_val_flag || attribute_miss_default_flag) attribute_miss_val.clear();
    uint8_t strLen = randomU4() + 3;
    std::string attribute_miss_str = randomString(strLen);
    if (!attribute_miss_val_flag) attribute_miss_str = "";

    uint8_t compressor_ID = randomU8();

    uint8_t n_steps_with_dependencies = randomU4();
    std::vector<uint8_t> dependency_step_ID(n_steps_with_dependencies, 0);
    for (auto& byte : dependency_step_ID) byte = randomU4();
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
    genie::core::record::annotation_parameter_set::AttributeParameterSet attr(
        attribute_ID, attribute_name_len, attribute_name, attribute_type, attribute_num_array_dims,
        attribute_array_dims, attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag,
        attribute_miss_val, attribute_miss_str, compressor_ID, n_steps_with_dependencies, dependency_step_ID,
        n_dependencies, dependency_var_ID, dependency_is_attribute, dependency_ID);
    return attr;
}

genie::core::record::annotation_parameter_set::AlgorithmParameters
RandomAnnotationEncodingParameters::randomAlgorithmParameters() {
    uint8_t n_pars = randomU4();
    // randomU8();
    std::vector<uint8_t> par_ID(n_pars, 0);
    std::vector<uint8_t> par_type(n_pars, 0);
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val(
        n_pars, std::vector<std::vector<std::vector<std::vector<uint8_t>>>>(0));
    for (auto i = 0; i < n_pars; ++i) {
        par_ID[i] = randomU4();
        par_type[i] = randomType();
        par_num_array_dims[i] = randomU2();
        par_array_dims[i].resize(par_num_array_dims[i]);
        for (auto j = 0; j < par_num_array_dims[i]; ++j) par_array_dims[i][j] = randomU4();  // randomU8();
        if (par_num_array_dims[i] == 0)
            par_val[i].resize(1,
                              std::vector<std::vector<std::vector<uint8_t>>>(1, std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims[i] == 1)
            par_val[i].resize(par_array_dims[i][0],
                              std::vector<std::vector<std::vector<uint8_t>>>(1, std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims[i] == 2)
            par_val[i].resize(par_array_dims[i][0], std::vector<std::vector<std::vector<uint8_t>>>(
                                                        par_array_dims[i][1], std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims[i] == 3)
            par_val[i].resize(par_array_dims[i][0],
                              std::vector<std::vector<std::vector<uint8_t>>>(
                                  par_array_dims[i][1], std::vector<std::vector<uint8_t>>(par_array_dims[i][2])));

        for (auto& d1 : par_val)
            for (auto& d2 : d1)
                for (auto& d3 : d2)
                    for (auto& d4 : d3) d4 = randomValForType(par_type[i]);
    }

    return genie::core::record::annotation_parameter_set::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

genie::core::record::annotation_parameter_set::TileStructure
RandomAnnotationEncodingParameters::randomTileStructure() {
    uint8_t ATCoordSize = randomU2();
    bool two_dimensional = randomBool();
    return randomTileStructure(ATCoordSize, two_dimensional);
}
genie::core::record::annotation_parameter_set::TileStructure
RandomAnnotationEncodingParameters::simpleTileStructure(uint8_t ATCoordSize, bool two_dimensional) {
    bool variable_size_tiles = false;
    uint64_t n_tiles = 0;
    uint8_t dimensions = two_dimensional ? 2 : 1;
    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size;
    start_index.resize(n_tiles, std::vector<uint64_t>(dimensions, 0));
    end_index.resize(n_tiles, std::vector<uint64_t>(dimensions, 0));
    tile_size.resize(dimensions, 0);

    if (variable_size_tiles)
        for (auto i = 0; i < n_tiles; ++i) {
            for (auto j = 0; j < dimensions; ++j) {
                start_index[i][j] = 0;
                end_index[i][j] = 1;
            }
        }
    else
        for (auto j = 0; j < dimensions; ++j) tile_size[j] = 2;

    return genie::core::record::annotation_parameter_set::TileStructure(
        ATCoordSize, two_dimensional, variable_size_tiles, n_tiles, start_index, end_index, tile_size);
}

genie::core::record::annotation_parameter_set::TileStructure
RandomAnnotationEncodingParameters::randomTileStructure(uint8_t ATCoordSize, bool two_dimensional) {
    bool variable_size_tiles = randomBool();
    uint64_t n_tiles = randomAtCoordSize(ATCoordSize);
    if (n_tiles > 4096) n_tiles = n_tiles % 4096;
    uint8_t dimensions = two_dimensional ? 2 : 1;
    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size;
    start_index.resize(n_tiles, std::vector<uint64_t>(dimensions, 0));
    end_index.resize(n_tiles, std::vector<uint64_t>(dimensions, 0));
    tile_size.resize(n_tiles, 0);
    for (auto i = 0; i < n_tiles; ++i) {
        if (variable_size_tiles)
            for (auto j = 0; j < dimensions; ++j) {
                start_index[i][j] = randomAtCoordSize(ATCoordSize);
                end_index[i][j] = randomAtCoordSize(ATCoordSize);
            }
        else
            tile_size[i] = randomAtCoordSize(ATCoordSize);
    }

    return genie::core::record::annotation_parameter_set::TileStructure(
        ATCoordSize, two_dimensional, variable_size_tiles, n_tiles, start_index, end_index, tile_size);
}

genie::core::record::annotation_parameter_set::TileConfiguration
RandomAnnotationEncodingParameters::randomTileConfiguration(uint8_t AT_coord_size) {
    uint8_t AG_class = randomU3();
    bool attribute_contiguity = randomBool();

    bool two_dimensional = randomBool();

    bool column_major_tile_order = randomBool();
    uint8_t symmetry_mode = randomU3();
    bool symmetry_minor_diagonal = randomBool();

    bool attribute_dependent_tiles = randomBool();

    genie::core::record::annotation_parameter_set::TileStructure default_tile_structure =
        simpleTileStructure(AT_coord_size, two_dimensional);

    uint16_t n_add_tile_structures = 0;
    std::vector<uint16_t> n_attributes;
    std::vector<std::vector<uint16_t>> attribute_ID;
    std::vector<uint8_t> n_descriptors;
    std::vector<std::vector<uint8_t>> descriptor_ID;
    std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure;

    if (attribute_dependent_tiles) {
        n_add_tile_structures = randomU8();  // randomU16
        n_attributes.resize(n_add_tile_structures);
        descriptor_ID.resize(n_add_tile_structures, std::vector<uint8_t>(0));
        attribute_ID.resize(n_add_tile_structures, std::vector<uint16_t>(0));
        for (auto i = 0; i < n_add_tile_structures; ++i) {
            n_attributes[i] = randomU8();  // randomU16
            for (auto j = 0; j < n_attributes[i]; ++j) {
                attribute_ID[i].push_back(randomU16());
            }
            n_descriptors.push_back(randomU8() / 2);
            for (auto j = 0; j < n_descriptors[i]; ++j) {
                descriptor_ID[i].push_back(randomU8() / 2);
            }
            additional_tile_structure.push_back(simpleTileStructure(AT_coord_size, two_dimensional));
        }
    }
    return genie::core::record::annotation_parameter_set::TileConfiguration(
        AT_coord_size, AG_class, attribute_contiguity, two_dimensional, column_major_tile_order, symmetry_mode,
        symmetry_minor_diagonal, attribute_dependent_tiles, default_tile_structure, n_add_tile_structures, n_attributes,
        attribute_ID, n_descriptors, descriptor_ID, additional_tile_structure);
}

genie::core::record::annotation_parameter_set::ContactMatrixParameters
RandomAnnotationEncodingParameters::randomContactMatrixParameters() {
    uint8_t num_samples = randomU8();
    std::vector<uint8_t> sample_ID(num_samples, 0);
    for (auto& sampleID : sample_ID) sampleID = randomU8();
    std::vector<std::string> sample_name(num_samples, "");
    for (auto& sampleName : sample_name) sampleName = randomString(randomU4() + 2);
    uint8_t num_chrs = randomU8();
    std::vector<uint8_t> chr_ID(num_chrs, 0);
    for (auto& chrID : chr_ID) chrID = randomU8();
    std::vector<std::string> chr_name(num_chrs, "");
    for (auto& chrName : chr_name) chrName = randomString(randomU4() + 2);
    std::vector<uint64_t> chr_length(num_chrs, 0);
    for (auto& length : chr_length) length = randomU64();

    uint32_t interval = randomU32();
    uint32_t tile_size = randomU32();
    uint8_t num_interval_multipliers = randomU8();
    std::vector<uint32_t> interval_multiplier(num_interval_multipliers, 0);
    for (auto& intMult : interval_multiplier) intMult = randomU32();
    uint8_t num_norm_methods = randomU8();
    std::vector<uint8_t> norm_method_ID(num_norm_methods, 0);
    for (auto& normMethodID : norm_method_ID) normMethodID = randomU8();
    std::vector<std::string> norm_method_name(num_norm_methods, "");
    for (auto& normMethodName : norm_method_name) normMethodName = randomString(randomU4() + 2);
    std::vector<bool> norm_method_mult_flag(num_norm_methods, false);
    for (auto i = 0; i < num_norm_methods; ++i) norm_method_mult_flag[i] = randomBool();

    uint8_t num_norm_matrices = randomU8();
    std::vector<uint8_t> norm_matrix_ID(num_norm_matrices, 0);
    for (auto& ID : norm_matrix_ID) ID = randomU8();
    std::vector<std::string> norm_matrix_name(num_norm_matrices, "");
    for (auto& name : norm_matrix_name) name = randomString(randomU4() + 2);

    return genie::core::record::annotation_parameter_set::ContactMatrixParameters(
        num_samples, sample_ID, sample_name, num_chrs, chr_ID, chr_name, chr_length, interval, tile_size,
        num_interval_multipliers, interval_multiplier, num_norm_methods, norm_method_ID, norm_method_name,
        norm_method_mult_flag, num_norm_matrices, norm_matrix_ID, norm_matrix_name);
}

genie::core::record::annotation_parameter_set::ContactMatrixParameters
RandomAnnotationEncodingParameters::simpleContactMatrixParameters() {
    return genie::core::record::annotation_parameter_set::ContactMatrixParameters();
}

genie::core::record::annotation_parameter_set::TileConfiguration
RandomAnnotationEncodingParameters::randomTileConfiguration() {
    uint8_t AT_coord_size = randomU8();
    return randomTileConfiguration(AT_coord_size);
}

genie::core::record::annotation_parameter_set::DescriptorConfiguration
RandomAnnotationEncodingParameters::randomDescriptorConfiguration() {
    genie::core::record::annotation_parameter_set::DescriptorID descriptorID =
        static_cast<genie::core::record::annotation_parameter_set::DescriptorID>(randomU2());
    uint8_t encoding_mode_ID = randomU8();

    return genie::core::record::annotation_parameter_set::DescriptorConfiguration(
        descriptorID, encoding_mode_ID, genie::core::record::annotation_parameter_set::GenotypeParameters(),
        genie::core::record::annotation_parameter_set::LikelihoodParameters(), simpleContactMatrixParameters(),
        genie::core::record::annotation_parameter_set::AlgorithmParameters());
}

genie::core::record::annotation_parameter_set::CompressorParameterSet
RandomAnnotationEncodingParameters::randomCompressorParameterSet() {
    uint8_t compressor_ID = randomU8();
    uint8_t n_compressor_steps = randomU4();
    std::vector<uint8_t> compressor_step_ID(n_compressor_steps, 0);
    for (auto& ID : compressor_step_ID) ID = randomU4();
    std::vector<uint8_t> algorithm_ID(n_compressor_steps, 0);
    for (auto& ID : algorithm_ID) ID = randomU4() + randomU4();
    std::vector<bool> use_default_pars(n_compressor_steps);
    std::vector<genie::core::record::annotation_parameter_set::AlgorithmParameters> algorithm_parameters;
    for (auto i = 0; i < n_compressor_steps;++i) {
        use_default_pars[i] = true;
        // temporary set al to true, randomBool();
            if (!use_default_pars[i])
            algorithm_parameters.push_back(genie::core::record::annotation_parameter_set::AlgorithmParameters());
    }
    std::vector<uint8_t> n_in_vars(n_compressor_steps, 0);
    std::vector<std::vector<uint8_t>> in_var_ID(n_compressor_steps);
    std::vector<std::vector<uint8_t>> prev_step_ID(n_compressor_steps);
    std::vector<std::vector<uint8_t>> prev_out_var_ID(n_compressor_steps);
    std::vector<uint8_t> n_completed_out_vars(n_compressor_steps);
    std::vector<std::vector<uint8_t>> completed_out_var_ID(n_compressor_steps);
    for (auto i = 0; i < n_compressor_steps; ++i) {
        n_in_vars[i] = randomU4();
        for (auto j = 0; j < n_in_vars[i]; ++j) {
            in_var_ID[i].push_back(randomU4());
            prev_step_ID[i].push_back(randomU4());
            prev_out_var_ID[i].push_back(randomU4());
        }
        n_completed_out_vars[i] = randomU4();
        for (auto j = 0; j < n_completed_out_vars[i]; ++j) completed_out_var_ID[i].push_back(randomU4());
    }

    return genie::core::record::annotation_parameter_set::CompressorParameterSet(
        compressor_ID, n_compressor_steps, compressor_step_ID, algorithm_ID, use_default_pars, algorithm_parameters,
        n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID);
}
