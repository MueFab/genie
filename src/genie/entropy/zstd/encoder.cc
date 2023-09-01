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
#include "genie/entropy/zstd/encoder.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"

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
    std::vector<uint8_t> par_num_array_dims(3, 0);
    std::vector<uint8_t> values{use_dictionary_flag, 0, 0};

    std::vector<std::vector<uint8_t>> par_array_dims(0, std::vector<uint8_t>(1, 0));
    std::vector<std::vector<std::vector<std::vector<uint8_t>>>> temp;
    std::vector<std::vector<std::vector<uint8_t>>> temp2;
    std::vector<std::vector<uint8_t>> temp3;
    std::vector<uint8_t> temp4(1, 0);
    temp3.resize(n_pars, temp4);
    temp2.resize(1, temp3);
    temp.resize(1, temp2);
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val(1, temp);

    for (auto &l : par_val) {
        for (auto &k : l)
            for (auto &j : k)
                for (size_t i = 0; i < j.size(); ++i) {
                    j[i][0] = values[i];
                }
    }

    genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
    return genie::core::record::annotation_parameter_set::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

}  // namespace zstd
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
