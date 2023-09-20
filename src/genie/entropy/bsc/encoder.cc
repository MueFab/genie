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

//#include "codecs/api/mpegg_utils.h"
#include "codecs/include/mpegg-codecs.h"
#include "genie/core/constants.h"
#include "genie/entropy/bsc/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace bsc {

// ---------------------------------------------------------------------------------------------------------------------

BSCEncoder::BSCEncoder()
    : lzpHashSize(MPEGG_BSC_DEFAULT_LZPHASHSIZE),
      lzpMinLen(MPEGG_BSC_DEFAULT_LZPMINLEN),
      blockSorter(MPEGG_BSC_BLOCKSORTER_BWT),
      coder(MPEGG_BSC_CODER_QLFC_STATIC) {}

void BSCEncoder::encode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    unsigned char *destination;
    size_t destLen = 0;

    int ret = mpegg_bsc_compress(&destination, &destLen, (const unsigned char *)input.str().c_str(), srcLen,
                                 lzpHashSize, lzpMinLen, blockSorter, coder);
    if (ret != 0) {
        std::cerr << "error with compression\n";
    }
    output.write((const char *)destination, destLen);
    if (destination) free(destination);
}

void BSCEncoder::decode(std::stringstream &input, std::stringstream &output) {
    const size_t srcLen = input.str().size();
    unsigned char *destination;
    size_t destLen = srcLen;
    int ret = mpegg_bsc_decompress(&destination, &destLen, (const unsigned char *)input.str().c_str(), srcLen);
    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    output.write((const char *)destination, destLen);
    if (destination) free(destination);
}

#include <type_traits>

genie::core::record::annotation_parameter_set::AlgorithmParameters BSCParameters::convertToAlgorithmParameters() const {
    uint8_t n_pars = 4;
    std::vector<uint8_t> par_ID{1, 2, 3, 4};
    const std::vector<genie::core::DataType> par_type{genie::core::DataType::UINT8, genie::core::DataType::UINT8,
                                                      genie::core::DataType::UINT8, genie::core::DataType::UINT8};
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<uint8_t> values{MPEGG_BSC_DEFAULT_LZPHASHSIZE, MPEGG_BSC_DEFAULT_LZPMINLEN, MPEGG_BSC_BLOCKSORTER_BWT,
                                MPEGG_BSC_CODER_QLFC_STATIC};

    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));

    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

    for (auto i = 0; i < n_pars; ++i) {
        if (par_type.at(i) == core::DataType::UINT8) {
            par_val.push_back( core::record::annotation_parameter_set::parameterToVector<uint8_t>(
                {values.at(i)}, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
    }

    return genie::core::record::annotation_parameter_set::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

}  // namespace bsc
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
