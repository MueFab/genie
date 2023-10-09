/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder.h"
#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace jbig {

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

JBIGEncoder::JBIGEncoder()
    : num_lines_per_stripe(-1),
      deterministic_pred(false),
      typical_pred(false),
      diff_layer_typical_pred(false),
      two_line_template(false) {}

void JBIGEncoder::encode(std::stringstream& input, std::stringstream& output, uint32_t ncols, uint32_t nrows) {
    const size_t srcLen = input.str().size();
    unsigned char* inputBuffer = NULL;
    inputBuffer = (unsigned char*)malloc(sizeof(*inputBuffer) * srcLen);
    memcpy(inputBuffer, input.str().c_str(), srcLen);

    unsigned char* compressedBuffer = NULL;
    size_t compSize = 3*srcLen;


    int ret = mpegg_jbig_compress_default(&compressedBuffer, &compSize, inputBuffer, srcLen, nrows,
                                ncols);
    if (ret != 0) {
        std::cerr << "error with compression\n";
    }
    for (size_t i = 0; i < compSize; ++i) output << compressedBuffer[i];
 

}

void JBIGEncoder::encode(std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint32_t ncols, uint32_t nrows) {
    const size_t srcLen = input.size();
    unsigned char* inputBuffer = NULL;
    inputBuffer = (unsigned char*)malloc(sizeof(*inputBuffer) * srcLen);
    memcpy(inputBuffer, &input[0], srcLen);

    unsigned char* decompressedBuffer = NULL;
    size_t dest_data_len;

    int ret = mpegg_jbig_decompress_default(&decompressedBuffer, &dest_data_len, inputBuffer, srcLen,
                                            (unsigned long*)&nrows, (unsigned long*)&ncols);
    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    for (size_t i = 0; i < dest_data_len; ++i) output.push_back(decompressedBuffer[i]);
}

void JBIGEncoder::decode(std::stringstream& input, std::stringstream& output, uint32_t& ncols, uint32_t& nrows) {
    const size_t srcLen = input.str().size();
    unsigned char* inputBuffer = NULL;
    inputBuffer = (unsigned char*)malloc(sizeof(*inputBuffer) * srcLen);
    memcpy(inputBuffer, input.str().c_str(), srcLen);

    unsigned char* decompressedBuffer = NULL;
    size_t dest_data_len;

    int ret = mpegg_jbig_decompress_default(&decompressedBuffer, &dest_data_len, inputBuffer, srcLen, (unsigned long*) &nrows,
                                  (unsigned long*)&ncols);
    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    for (size_t i = 0; i < dest_data_len; ++i) output << decompressedBuffer[i];
}

genie::core::record::annotation_parameter_set::AlgorithmParameters JBIGparameters::convertToAlgorithmParameters()
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
    for (uint8_t i = 0; i < n_pars; ++i) {
        if (par_type.at(i) == core::DataType::BOOL) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<bool>(
                {static_cast<bool>(values.at(i))}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
        if (par_type.at(i) == core::DataType::INT32) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<int32_t>(
                {static_cast<int32_t>(values.at(i))}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
    }

    return genie::core::record::annotation_parameter_set::AlgorithmParameters();
}

}  // namespace jbig
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------