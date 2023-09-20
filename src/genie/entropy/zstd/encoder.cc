/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include "genie/util/make-unique.h"
#include "genie/util/watch.h"

#include "codecs/api/mpegg_utils.h"
#include "codecs/include/mpegg-codecs.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/entropy/zstd/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace zstd {

// ---------------------------------------------------------------------------------------------------------------------

ZSTDEncoder::ZSTDEncoder() : use_dictionary_flag(false), dictionary_size(0), dictionary{} {}

void ZSTDEncoder::encode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    unsigned char *destination;
    size_t destLen = srcLen;

    int ret = mpegg_zstd_compress(&destination, &destLen, (const unsigned char *)input.str().c_str(), srcLen, 0);

    if (ret != 0) {
        std::cerr << "error with compression\n";
    }
    output.write((const char *)destination, destLen);
    if (destination) free(destination);
}

void ZSTDEncoder::decode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    char *dest = new char[srcLen];
    unsigned char *destination;
    size_t destLen = srcLen;
    int ret = mpegg_zstd_decompress(&destination, &destLen, (const unsigned char *)input.str().c_str(), srcLen);
    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    output.write((const char *)destination, destLen);
    delete[] dest;
}

genie::core::record::annotation_parameter_set::AlgorithmParameters ZSTDParameters::convertToAlgorithmParameters()
    const {
    uint8_t n_pars = 3;
    std::vector<uint8_t> par_ID{1, 2, 3};
    std::vector<genie::core::DataType> par_type{genie::core::DataType::BOOL, genie::core::DataType::UINT16,
                                                genie::core::DataType::STRING};
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<uint16_t> values{use_dictionary_flag, 0, 0};

    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

    for (auto i = 0; i < n_pars; ++i) {
        if (par_type.at(i) == core::DataType::BOOL) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<bool>(
                {static_cast<bool>(values.at(i))}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
        if (par_type.at(i) == core::DataType::UINT16) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<uint16_t>(
                {static_cast<uint16_t>(values.at(i))}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
        if (par_type.at(i) == core::DataType::STRING) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<uint8_t>(
                {0}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
    }

    return genie::core::record::annotation_parameter_set::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

}  // namespace zstd
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
