/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <sstream>

#include "code.h"
#include "codecs/api/mpegg_utils.h"
#include "filesystem/filesystem.hpp"
#include "genie/util/runtime-exception.h"
#include "genie/util/string-helpers.h"
//#include "codecs/libbsc/libbsc.h"
#include "codecs/include/mpegg-codecs.h"
//#include "mpegg-codecs.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------

genieapp::annotation::Code::Code(std::string inputFileName, std::string OutputFileName, uint8_t encodeMode)
    : inputFileName(inputFileName), outputFileName(outputFileName), compressedData{} {
    if (encodeMode != 3) return;

    fillAnnotationParameterSet();
    encodeData();
    fillAnntationAccessUnit();
}

// ---------------------------------------------------------------------------------------------------------------------
void genieapp::annotation::Code::fillAnnotationParameterSet() {
    uint64_t n_tiles = 1;
    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size;
    tile_size.resize(n_tiles, fileSize);

    genie::core::record::annotation_parameter_set::TileStructure defaultTileStructure(
        AT_coord_size, false, variable_size_tiles, n_tiles, start_index, end_index, tile_size);

    uint8_t symmetry_mode = 0;
    bool symmetry_minor_diagonal = false;

    bool attribute_dependent_tiles = false;

    uint16_t n_add_tile_structures = 0;
    std::vector<uint16_t> nAttributes{};
    std::vector<std::vector<uint16_t>> attribute_IDs;
    std::vector<uint8_t> nDescriptors{};
    std::vector<std::vector<uint8_t>> descriptor_IDs;
    std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure;

    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration(
        AT_coord_size, AG_class, attribute_contiguity, two_dimensional, column_major_tile_order, symmetry_mode,
        symmetry_minor_diagonal, attribute_dependent_tiles, defaultTileStructure, n_add_tile_structures, nAttributes,
        attribute_IDs, nDescriptors, descriptor_IDs, additional_tile_structure);

    AnnotationEncodingParameters annotationEncodingParameters = fillAnnotationEncodingParameters();

    uint8_t parameter_set_ID = 1;
    uint8_t AT_alphabet_ID = 0;
    bool AT_pos_40_bits_flag = false;
    uint8_t n_aux_attribute_groups = 0;
    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configuration(1,
                                                                                                     tileConfiguration);

    genie::core::record::annotation_parameter_set::Record record(parameter_set_ID, AT_ID, AT_alphabet_ID, AT_coord_size,
                                                                 AT_pos_40_bits_flag, n_aux_attribute_groups,
                                                                 tile_configuration, annotationEncodingParameters);
    annotationParameterSet = record;
}

genie::core::record::annotation_parameter_set::DescriptorConfiguration Code::fillDescriptorConfiguration() {
    genie::core::record::annotation_parameter_set::ContactMatrixParameters contactMatrixParameters;
    genie::core::record::annotation_parameter_set::LikelihoodParameters likelihoodParameters;
    genie::core::record::annotation_parameter_set::GenotypeParameters genotypeParameters;

    uint8_t n_pars = 1;
    std::vector<uint8_t> par_ID(n_pars, 0);
    std::vector<uint8_t> par_type(n_pars, 4);
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    par_num_array_dims = {0};
    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val(
        n_pars, std::vector<std::vector<std::vector<std::vector<uint8_t>>>>(0));
    genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);

    return genie::core::record::annotation_parameter_set::DescriptorConfiguration(
        descriptorID, encodingMode, genotypeParameters, likelihoodParameters, contactMatrixParameters,
        algorithmParameters);
}

genie::core::record::annotation_parameter_set::AnnotationEncodingParameters Code::fillAnnotationEncodingParameters() {
    DescriptorConfiguration descriptorConfigration = fillDescriptorConfiguration();
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSet;
    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet;

    uint8_t n_filter = 0;
    std::vector<uint8_t> filter_ID_len{};
    std::vector<std::string> filter_ID{};
    std::vector<uint16_t> desc_len{};
    std::vector<std::string> description{};

    uint8_t n_features_names = 0;
    std::vector<uint8_t> feature_name_len{};
    std::vector<std::string> feature_name{};

    uint8_t n_ontology_terms = 0;
    std::vector<uint8_t> ontology_term_name_len{};
    std::vector<std::string> ontology_term_name{};

    uint8_t n_descriptors = 1;
    std::vector<DescriptorConfiguration> descriptor_configuration(n_descriptors, descriptorConfigration);

    uint8_t n_compressors = 0;
    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set(
        n_compressors, genie::core::record::annotation_parameter_set::CompressorParameterSet());

    uint8_t n_attributes = 0;
    std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set(
        n_attributes, genie::core::record::annotation_parameter_set::AttributeParameterSet());

    return AnnotationEncodingParameters(n_filter, filter_ID_len, filter_ID, desc_len, description, n_features_names,
                                        feature_name_len, feature_name, n_ontology_terms, ontology_term_name_len,
                                        ontology_term_name, n_descriptors, descriptor_configuration, n_compressors,
                                        compressor_parameter_set, n_attributes, attribute_parameter_set);
}

// ---------------------------------------------------------------------------------------------------------------------
void genieapp::annotation::Code::fillAnntationAccessUnit() {
    bool is_attribute = false;
    uint16_t attribute_ID = false;
    uint64_t n_tiles_per_col = 0;
    uint64_t n_tiles_per_row = 0;
    uint64_t n_blocks = 1;
    uint64_t tile_index_1 = 0;
    bool tile_index_2_exists = false;
    uint64_t tile_index_2 = 0;
    genie::core::record::annotation_access_unit::AnnotationAccessUnitHeader annotation_access_unit_header(
        attribute_contiguity, two_dimensional, column_major_tile_order, variable_size_tiles, AT_coord_size,
        is_attribute, attribute_ID, descriptorID, n_tiles_per_col, n_tiles_per_row, n_blocks, tile_index_1,
        tile_index_2_exists, tile_index_2);

    bool indexed = false;
    uint32_t block_payload_size = static_cast<uint32_t>(fileSize);

    genie::core::record::annotation_access_unit::BlockHeader block_header(attribute_contiguity, descriptorID,
                                                                          attribute_ID, indexed, block_payload_size);

    genie::core::record::annotation_access_unit::GenotypePayload genotype_payload;
    genie::core::record::annotation_access_unit::LikelihoodPayload likelihood_payload;
    genie::core::record::annotation_access_unit::ContactMatrixMatPayload cm_mat_payload;
    std::vector<genie::core::record::annotation_access_unit::ContactMatrixBinPayload> cm_bin_payload;
    std::string filename = "testFiles/exampleMGrecs/ALL.chrX.10000_site.compressed";
    std::ifstream compressedFile;
    compressedFile.open(filename, std::ios::binary | std::ios::in);

    std::stringstream stringPayload;
    stringPayload << compressedFile.rdbuf();
    std::string generic_payload = stringPayload.str();
    uint8_t num_chrs = 0;

    genie::core::record::annotation_access_unit::BlockPayload block_payload(
        descriptorID, num_chrs, genotype_payload, likelihood_payload, cm_bin_payload, cm_mat_payload, generic_payload);

    std::vector<genie::core::record::annotation_access_unit::Block> blocks;
    genie::core::record::annotation_access_unit::Block block(block_header, block_payload, num_chrs);
    blocks.push_back(block);

    genie::core::record::annotation_access_unit::AnnotationType AT_type =
        genie::core::record::annotation_access_unit::AnnotationType::VARIANTS;
    uint8_t AT_subtype = 1;

    genie::core::record::annotation_access_unit::Record record(
        AT_ID, AT_type, AT_subtype, AG_class, annotation_access_unit_header, blocks, attribute_contiguity,
        two_dimensional, column_major_tile_order, AT_coord_size, n_blocks, num_chrs);
    annotationAccessUnit = record;
}

// ---------------------------------------------------------------------------------------------------------------------
void genieapp::annotation::Code::encodeData() {
    std::ofstream infile;
    infile.open(inputFileName, std::ios::binary | std::ios::out);

    std::stringstream wholeTestFile;
    if (infile.is_open()) {
        wholeTestFile << infile.rdbuf();
        infile.close();
    }

    unsigned char *dest = NULL;
    size_t destLen = 0;
    size_t srcLen = wholeTestFile.str().size();
    // const char src[] = wholeTestFile.str().c_str();

    mpegg_bsc_compress(&dest, &destLen, (const unsigned char *)wholeTestFile.str().c_str(), srcLen,
                       MPEGG_BSC_DEFAULT_LZPHASHSIZE, MPEGG_BSC_DEFAULT_LZPMINLEN, MPEGG_BSC_BLOCKSORTER_BWT,
                       MPEGG_BSC_CODER_QLFC_STATIC);
    compressedData << dest;
}

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
