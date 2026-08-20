/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/jbig/encoder.h"
#include <cstring>
#include <iostream>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::jbig {

// ---------------------------------------------------------------------------------------------------------------------

JBIGEncoder::JBIGEncoder()
    : num_lines_per_stripe(-1),
      deterministic_pred(false),
      typical_pred(false),
      diff_layer_typical_pred(false),
      two_line_template(false) {
}

// ---------------------------------------------------------------------------------------------------------------------

void JBIGEncoder::encode(
    std::stringstream& input,
    std::stringstream& output,
    uint32_t ncols,
    uint32_t nrows) {
    const size_t srcLen = input.str().size();
    unsigned char* compressedBuffer;
    size_t compSize;

    auto buf_nrows = (unsigned long) nrows;  // NOLINT(runtime/int)
    auto buf_ncols = (unsigned long) ncols;  // NOLINT(runtime/int)

    int ret = mpegg_jbig_compress_default(
        &compressedBuffer,
        &compSize,
        (const unsigned char*)input.str().c_str(),
        srcLen,
        buf_nrows,
        buf_ncols);
    if (ret != 0) {
        std::cerr << "error with jbig compression\n";
    }
    for (size_t idx_i = 0; idx_i < compSize; ++idx_i)
        output << compressedBuffer[idx_i];
    free(compressedBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void JBIGEncoder::encode(
    std::vector<uint8_t>& input,
    std::vector<uint8_t>& output,
    uint32_t ncols,
    uint32_t nrows) {
    const size_t srcLen = input.size();
    unsigned char* inputBuffer = NULL;
    inputBuffer = (unsigned char*)malloc(sizeof(*inputBuffer) * srcLen);
    memcpy(inputBuffer, &input[0], srcLen);

    unsigned char* compressedBuffer = NULL;
    size_t dest_data_len;

    auto buf_nrows = (unsigned long) ncols;  // NOLINT(runtime/int)
    auto buf_ncols = (unsigned long) nrows;  // NOLINT(runtime/int)

    int ret = mpegg_jbig_compress_default(
        &compressedBuffer,
        &dest_data_len,
        inputBuffer,
        srcLen,
        buf_nrows,
        buf_ncols);
    if (ret != 0) {
        std::cerr << "error with compression\n";
        free(inputBuffer);
        return;
    }
    for (size_t idx_i = 0; idx_i < dest_data_len; ++idx_i)
        output.push_back(compressedBuffer[idx_i]);
    free(inputBuffer);
    free(compressedBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void JBIGEncoder::decode(
    std::stringstream& input,
    std::stringstream& output,
    uint32_t& ncols,
    uint32_t& nrows) {
    const size_t srcLen = input.str().size();
    unsigned char* decompressedBuffer;
    size_t dest_data_len;

    unsigned long buf_nrows, buf_ncols;  // NOLINT(runtime/int)

    int ret = mpegg_jbig_decompress_default(
        &decompressedBuffer,
        &dest_data_len,
        (const unsigned char*)input.str().c_str(),
        srcLen,
        &buf_nrows,
        &buf_ncols);

    nrows = static_cast<uint32_t>(buf_nrows);
    ncols = static_cast<uint32_t>(buf_ncols);

    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    for (size_t idx_i = 0; idx_i < dest_data_len; ++idx_i) {
        output << decompressedBuffer[idx_i];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::parameter::annotation::AlgorithmParameters JBIGparameters::convertToAlgorithmParameters()
    const {
    uint8_t n_pars = 5;
    std::vector<uint8_t> par_ID = {1, 2, 3, 4, 5};
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<genie::core::DataType> par_type{genie::core::DataType::INT32, genie::core::DataType::BOOL,
                                                genie::core::DataType::BOOL, genie::core::DataType::BOOL,
                                                genie::core::DataType::BOOL};
    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;
    std::vector<int32_t> values{num_lines_per_stripe, deterministic_pred, typical_pred, diff_layer_typical_pred,
                                two_line_template};
    for (uint8_t idx_i = 0; idx_i < n_pars; ++idx_i) {
        if (par_type.at(idx_i) == core::DataType::BOOL) {
            par_val.push_back(core::parameter::annotation::parameterToVector<bool>(
                {static_cast<bool>(values.at(idx_i))}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
        if (par_type.at(idx_i) == core::DataType::INT32) {
            par_val.push_back(core::parameter::annotation::parameterToVector<int32_t>(
                {static_cast<int32_t>(values.at(idx_i))}, par_type.at(idx_i), par_num_array_dims.at(idx_i), par_array_dims.at(idx_i)));
        }
    }

    return genie::core::parameter::annotation::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::parameter::annotation::CompressorParameterSet JBIGparameters::compressorParameterSet(
    uint8_t compressor_ID) const {
    std::vector<genie::core::AlgoID> JBIGalgorithm_ID{genie::core::AlgoID::JBIG};
    uint8_t n_compressor_steps = 1;
    std::vector<uint8_t> compressor_step_ID{0};
    std::vector<bool> use_default_pars{true};
    std::vector<genie::core::parameter::annotation::AlgorithmParameters> algorithm_parameters;
    std::vector<uint8_t> n_in_vars{0};
    std::vector<std::vector<uint8_t>> in_var_ID{{0}};
    std::vector<std::vector<uint8_t>> prev_step_ID;
    std::vector<std::vector<uint8_t>> prev_out_var_ID;
    std::vector<uint8_t> n_completed_out_vars{0};
    std::vector<std::vector<uint8_t>> completed_out_var_ID;

    return genie::core::parameter::annotation::CompressorParameterSet(
        compressor_ID, n_compressor_steps, compressor_step_ID, JBIGalgorithm_ID, use_default_pars, algorithm_parameters,
        n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::jbig

// ---------------------------------------------------------------------------------------------------------------------
