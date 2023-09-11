/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <utility>

#include "apps/genie/annotation/code.h"
#include "codecs/api/mpegg_utils.h"
#include "codecs/include/mpegg-codecs.h"
#include "filesystem/filesystem.hpp"
#include "genie/core/writer.h"
#include "genie/util/runtime-exception.h"
#include "genie/util/string-helpers.h"

#include "genie/core/record/data_unit/record.h"
#include "genie/variantsite/VariantSiteParser.h"
//#include "genie/entropy/bsc/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#define SYSERROR() GetLastError()
#else
#include <errno.h>
#define SYSERROR() errno
#endif

namespace genieapp {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName)
    : Code(_inputFileName, _outputFileName, genie::core::AlgoID::BSC, false) {
}  // TODO @Stefanie: change default AlgoID

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput)
    : Code(_inputFileName, _outputFileName, genie::core::AlgoID::BSC, testOutput) {
}  // TODO @Stefanie: change default AlgoID

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, std::string encodeString,
           bool testOutput)
    : Code(_inputFileName, _outputFileName, convertStringToALgoID(encodeString), testOutput) {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, genie::core::AlgoID encodeMode,
           bool testOutput, std::string& rec)
    : Code(_inputFileName, _outputFileName, encodeMode, testOutput, "", "", rec) {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, genie::core::AlgoID encodeMode,
           bool testOutput, const std::string& _infoFieldsFileName, const std::string& _configFileName,
           std::string& rec)
    : inputFileName(_inputFileName),
      outputFileName(_outputFileName),
      infoFieldsFileName(_infoFieldsFileName),
      configFileName(_configFileName),
      compressedData{} {
    rec = rec;
    if (encodeMode != genie::core::AlgoID::BSC) UTILS_DIE("No Valid codec selected ");
    if (inputFileName.empty()) {
        std::cerr << ("No Valid Inputs ") << std::endl;
        return;
    }

    std::map<genie::core::AnnotDesc, std::stringstream> output;
    std::map<std::string, genie::core::record::variant_site::AttributeData> info;
    std::stringstream infoFields;
    if (!infoFieldsFileName.empty()) {
        std::ifstream infoFieldsFile;
        infoFieldsFile.open(infoFieldsFileName, std::ios::in);
        if (infoFieldsFile.is_open()) {
            infoFields << infoFieldsFile.rdbuf();
            infoFieldsFile.close();
        }
    }
    std::ifstream site_MGrecs(inputFileName, std::ios::binary);
    std::map<std::string, std::stringstream> attributeStream;
    genie::variant_site::VariantSiteParser variantSiteParser(site_MGrecs, output, info, attributeStream,
                                                                           infoFields);

    fillAnnotationParameterSet();
    encodeData();
    fillAnnotationAccessUnit();

    std::ofstream txtFile;
    if (testOutput) {
        std::string txtName = outputFileName + ".txt";
        txtFile.open(txtName, std::ios::out);
    }

    std::ofstream outputFile;
    outputFile.open(outputFileName, std::ios::binary | std::ios::out);

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    if (outputFile.is_open()) {
        genie::core::Writer dataUnitWriter(&outputFile);
        APS_dataUnit.write(dataUnitWriter);
        AAU_dataUnit.write(dataUnitWriter);

        std::cerr << "bytes written: " << std::to_string(dataUnitWriter.getBitsWritten() / 8) << std::endl;
        outputFile.close();
        if (testOutput) {
            genie::core::Writer txtWriter(&txtFile, true);
            APS_dataUnit.write(txtWriter);
            AAU_dataUnit.write(txtWriter);
            txtWriter.flush();
            txtFile.close();
        }
    } else {
        std::cerr << "Failed to open file : " << SYSERROR() << std::endl;
    }
}

genie::core::AlgoID Code::convertStringToALgoID(std::string algoString) const {
    const std::map<std::string, genie::core::AlgoID> algoIds = {{"GABAC", genie::core::AlgoID::CABAC},
                                                                {"LZMA", genie::core::AlgoID::LZMA},
                                                                {"ZSTD", genie::core::AlgoID::ZSTD},
                                                                {"BSC", genie::core::AlgoID::BSC},
                                                                {"PROCRUSTES", genie::core::AlgoID::PROCRUSTES},
                                                                {"JBIG", genie::core::AlgoID::JBIG},
                                                                {"LZW", genie::core::AlgoID::LZW},
                                                                {"BIN", genie::core::AlgoID::BIN},
                                                                {"SPARSE", genie::core::AlgoID::SPARSE},
                                                                {"DEL", genie::core::AlgoID::DEL},
                                                                {"RLE", genie::core::AlgoID::RLE},
                                                                {"SER", genie::core::AlgoID::SER}};

    return algoIds.at(algoString);
}

// ---------------------------------------------------------------------------------------------------------------------
void Code::fillAnnotationParameterSet() {
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
    std::vector<genie::core::DataType> par_type(n_pars, genie::core::DataType::UINT8);
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    par_num_array_dims = {0};
    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val(
        n_pars, std::vector<std::vector<std::vector<std::vector<uint8_t>>>>(0));
    genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);

    return {descriptorID,       encodingMode, genotypeParameters, likelihoodParameters, contactMatrixParameters,
            algorithmParameters};
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

    return {n_filter,
            filter_ID_len,
            filter_ID,
            desc_len,
            description,
            n_features_names,
            feature_name_len,
            feature_name,
            n_ontology_terms,
            ontology_term_name_len,
            ontology_term_name,
            n_descriptors,
            descriptor_configuration,
            n_compressors,
            compressor_parameter_set,
            n_attributes,
            attribute_parameter_set};
}

// ---------------------------------------------------------------------------------------------------------------------
void Code::fillAnnotationAccessUnit() {
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
    auto block_payload_size = static_cast<uint32_t>(fileSize);

    genie::core::record::annotation_access_unit::BlockHeader block_header(attribute_contiguity, descriptorID,
                                                                          attribute_ID, indexed, block_payload_size);

    genie::core::record::annotation_access_unit::GenotypePayload genotype_payload;
    genie::core::record::annotation_access_unit::LikelihoodPayload likelihood_payload;
    genie::core::record::annotation_access_unit::ContactMatrixMatPayload cm_mat_payload;
    std::vector<genie::core::record::annotation_access_unit::ContactMatrixBinPayload> cm_bin_payload;

    uint8_t readByte;
    std::vector<uint8_t> payloadVector;
    while (compressedData >> readByte) payloadVector.push_back(readByte);
    //  std::string generic_payload = compressedData.str();
    uint8_t num_chrs = 0;

    genie::core::record::annotation_access_unit::BlockPayload block_payload(
        descriptorID, num_chrs, genotype_payload, likelihood_payload, cm_bin_payload, cm_mat_payload,
        static_cast<uint16_t>(payloadVector.size()), payloadVector);

    std::vector<genie::core::record::annotation_access_unit::Block> blocks;
    genie::core::record::annotation_access_unit::Block block(block_header, block_payload, num_chrs);
    blocks.push_back(block);

    genie::core::record::annotation_access_unit::AnnotationType AT_type =
        genie::core::record::annotation_access_unit::AnnotationType::VARIANTS;
    uint8_t AT_subtype = 1;

    genie::core::record::annotation_access_unit::Record record(
        AT_ID, AT_type, AT_subtype, AG_class, annotation_access_unit_header, blocks, attribute_contiguity,
        two_dimensional, column_major_tile_order, AT_coord_size, variable_size_tiles, n_blocks, num_chrs);
    annotationAccessUnit = record;
}

// ---------------------------------------------------------------------------------------------------------------------
void Code::encodeData() {
    std::ifstream infile;
    infile.open(inputFileName, std::ios::binary);

    std::stringstream wholeTestFile;
    if (infile.is_open()) {
        wholeTestFile << infile.rdbuf();
        infile.close();
    }
    /* genie::entropy::bsc::BSCParameters bscParameters;
    bscParameters.blockSorter = MPEGG_BSC_BLOCKSORTER_BWT;
    bscParameters.coder = MPEGG_BSC_CODER_QLFC_STATIC;
    bscParameters.lzpHashSize = MPEGG_BSC_DEFAULT_LZPHASHSIZE;
    bscParameters.lzpMinLen = MPEGG_BSC_DEFAULT_LZPMINLEN;

    genie::entropy::bsc::BSCEncoder encoder;
    encoder.configure(bscParameters);
    encoder.encode(wholeTestFile, compressedData);

    unsigned char *dest = NULL;
    size_t destLen = 0;
    size_t srcLen = wholeTestFile.str().size();
    std::cout << "source length: " << std::to_string(srcLen) << std::endl;

    mpegg_bsc_compress(&dest, &destLen, (const unsigned char *)wholeTestFile.str().c_str(), srcLen,
                       MPEGG_BSC_DEFAULT_LZPHASHSIZE, MPEGG_BSC_DEFAULT_LZPMINLEN, MPEGG_BSC_BLOCKSORTER_BWT,
                       MPEGG_BSC_CODER_QLFC_STATIC);
    compressedData.write((const char *)dest, destLen);
    std::cout << "compressedData length: " << std::to_string(compressedData.str().size()) << std::endl;
    */
}

void TempEncoder::encode(const std::stringstream& input, std::stringstream& output) {
    // uint8_t lzpHashSize = MPEGG_BSC_DEFAULT_LZPHASHSIZE;
    //  uint8_t lzpMinLen = MPEGG_BSC_DEFAULT_LZPMINLEN;
    //  uint8_t blockSorter = MPEGG_BSC_BLOCKSORTER_BWT;
    //  uint16_t coder = MPEGG_BSC_CODER_QLFC_STATIC;

    unsigned char* dest = NULL;
    size_t destLen = 0;
    size_t srcLen = input.str().size();
    //   std::cout << "source length: " << std::to_string(srcLen) << std::endl;

    mpegg_bsc_compress(&dest, &destLen, (const unsigned char*)input.str().c_str(), srcLen,
                       MPEGG_BSC_DEFAULT_LZPHASHSIZE, MPEGG_BSC_DEFAULT_LZPMINLEN, MPEGG_BSC_BLOCKSORTER_BWT,
                       MPEGG_BSC_CODER_QLFC_STATIC);
    output.write((const char*)dest, destLen);
    //   std::cout << "compressedData length: " << std::to_string(output.str().size()) << std::endl;

    /*
            unsigned char *dest = NULL;
            size_t destLen = 0;
            size_t srcLen = input.str().size();
            std::cout << "source length: " << std::to_string(srcLen) << std::endl;

            mpegg_bsc_compress(&dest, &destLen, (const unsigned char *)input.str().c_str(), srcLen, lzpHashSize,
       lzpMinLen, blockSorter, coder); output.write((const char *)dest, destLen);
        }*/
}

void encodeVariantGenotype(std::string& _input_fpath, std::string& _output_fpath) {
    if (_input_fpath == _output_fpath) {
    }
}

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
