/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#define __STDC_WANT_LIB_EXT1__ 1
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include "genie/util/make-unique.h"
#include "genie/util/watch.h"

//#include "codecs/api/mpegg_utils.h"
#include "codecs/include/mpegg-codecs.h"
#include "genie/core/arrayType.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/entropy/lzma/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace lzma {

// ---------------------------------------------------------------------------------------------------------------------

LZMAEncoder::LZMAEncoder()
    : level(MPEGG_LZMA_DEFAULT_LEVEL),
      dictSize(MPEGG_LZMA_DEFAULT_DIC_SIZE),
      lc(MPEGG_LZMA_DEFAULT_LC),
      lp(MPEGG_LZMA_DEFAULT_LP),
      pb(MPEGG_LZMA_DEFAULT_PB),
      fb(MPEGG_LZMA_DEFAULT_FB),
      numThreads(MPEGG_LZMA_DEFAULT_THREADS) {}

void LZMAEncoder::encode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    unsigned char *inputBuffer = NULL;

    inputBuffer = (unsigned char *)malloc(sizeof(*inputBuffer) * srcLen);
    memcpy(inputBuffer, input.str().c_str(), srcLen);

    unsigned char *compressedBuffer = NULL;
    size_t inputSize = srcLen;
    size_t compSize = 0;

    int ret = mpegg_lzma_compress(&compressedBuffer, &compSize, inputBuffer, inputSize, level, dictSize, lc, lp, pb, fb,
                                  numThreads);
    if (ret != 0) {
        std::cerr << "error with lzma compression\n";
    }
    for (size_t i = 0; i < compSize; ++i) output << compressedBuffer[i];
    if (compressedBuffer) free(compressedBuffer);
    if (inputBuffer) free(inputBuffer);
}

void LZMAEncoder::decode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    unsigned char *destination;
    size_t destLen = srcLen;
    int ret = mpegg_lzma_decompress(&destination, &destLen, (const unsigned char *)input.str().c_str(), srcLen);
    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    output.write((const char *)destination, destLen);
    if (destination) free(destination);
}

genie::core::record::annotation_parameter_set::AlgorithmParameters LZMAParameters::convertToAlgorithmParameters()
    const {
    uint8_t n_pars = 7;
    std::vector<uint8_t> par_ID{1, 2, 3, 4, 5, 6, 7};
    std::vector<genie::core::DataType> par_type{genie::core::DataType::UINT8, genie::core::DataType::UINT32,
                                                genie::core::DataType::UINT8, genie::core::DataType::UINT8,
                                                genie::core::DataType::UINT8, genie::core::DataType::UINT16,
                                                genie::core::DataType::UINT8};
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<uint32_t> values{MPEGG_LZMA_DEFAULT_LEVEL,  MPEGG_LZMA_DEFAULT_DIC_SIZE, MPEGG_LZMA_DEFAULT_LC,
                                 MPEGG_LZMA_DEFAULT_LP,     MPEGG_LZMA_DEFAULT_PB,       MPEGG_LZMA_DEFAULT_FB,
                                 MPEGG_LZMA_DEFAULT_THREADS};

    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

    for (auto i = 0; i < n_pars; ++i) {
        if (par_type.at(i) == core::DataType::UINT8) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<uint8_t>(
                {static_cast<uint8_t>(values.at(i))}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
        if (par_type.at(i) == core::DataType::UINT16) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<uint16_t>(
                {static_cast<uint16_t>(values.at(i))}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
        if (par_type.at(i) == core::DataType::UINT32) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<uint32_t>(
                {static_cast<uint32_t>(values.at(i))}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
    }

    return genie::core::record::annotation_parameter_set::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

genie::core::record::annotation_parameter_set::CompressorParameterSet LZMAParameters::compressorParameterSet(
    uint8_t compressor_ID) const {
    std::vector<genie::core::AlgoID> LZMAalgorithm_ID{genie::core::AlgoID::LZMA};
    uint8_t n_compressor_steps = 1;
    std::vector<uint8_t> compressor_step_ID{0};
    std::vector<bool> use_default_pars{true};
    std::vector<genie::core::record::annotation_parameter_set::AlgorithmParameters> algorithm_parameters;
    std::vector<uint8_t> n_in_vars{0};
    std::vector<std::vector<uint8_t>> in_var_ID{{0}};
    std::vector<std::vector<uint8_t>> prev_step_ID;
    std::vector<std::vector<uint8_t>> prev_out_var_ID;
    std::vector<uint8_t> n_completed_out_vars{0};
    std::vector<std::vector<uint8_t>> completed_out_var_ID;

    return genie::core::record::annotation_parameter_set::CompressorParameterSet(
        compressor_ID, n_compressor_steps, compressor_step_ID, LZMAalgorithm_ID, use_default_pars, algorithm_parameters,
        n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID);
}

}  // namespace lzma
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
