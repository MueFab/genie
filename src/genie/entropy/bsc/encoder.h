/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_ENCODER_H_
#define SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#define SYSERROR() GetLastError()
#else
#include <errno.h>
#define SYSERROR() errno
#endif

#include <sstream>

#include "genie/core/access-unit.h"
#include "genie/core/entropy-encoder.h"
#include "genie/util/make_unique.h"
#include "genie/util/stop_watch.h"

#include "apps/genie/annotation/code.h"
#include "codecs/include/mpegg-codecs.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace bsc {

class BSCParameters {
 public:
    uint8_t lzpHashSize;
    uint8_t lzpMinLen;
    uint8_t blockSorter;
    uint8_t coder;
    uint16_t features;
    BSCParameters()
        : lzpHashSize(MPEGG_BSC_DEFAULT_LZPHASHSIZE),
          lzpMinLen(MPEGG_BSC_DEFAULT_LZPMINLEN),
          blockSorter(MPEGG_BSC_BLOCKSORTER_BWT),
          coder(MPEGG_BSC_CODER_QLFC_STATIC) {}
    BSCParameters(uint8_t _lzpHashSize, uint8_t _lzpMinLen, uint8_t _blockSorter, uint8_t _coder,
                  uint16_t _features = 0)
        : lzpHashSize(_lzpHashSize),
          lzpMinLen(_lzpMinLen),
          blockSorter(_blockSorter),
          coder(_coder),
          features(_features) {}

    genie::core::record::annotation_parameter_set::AlgorithmParameters convertToAlgorithmParameters() const;
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSet(
        uint8_t compressor_ID) const;

    bool parsAreDefault() const {
        return lzpHashSize == MPEGG_BSC_DEFAULT_LZPHASHSIZE && lzpMinLen == MPEGG_BSC_DEFAULT_LZPMINLEN &&
               blockSorter == MPEGG_BSC_BLOCKSORTER_BWT && coder == MPEGG_BSC_CODER_QLFC_STATIC;
    }
};

class BSCEncoder {
 public:
    BSCEncoder();

    void encode(std::stringstream &input, std::stringstream &output);

    void decode(std::stringstream &input, std::stringstream &output);

    void configure(const BSCParameters bscParameters) {
        lzpHashSize = bscParameters.lzpHashSize;
        lzpMinLen = bscParameters.lzpMinLen;
        blockSorter = bscParameters.blockSorter;
        coder = bscParameters.coder;
    }

 private:
    uint8_t lzpHashSize;
    uint8_t lzpMinLen;
    uint8_t blockSorter;
    uint8_t coder;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace bsc
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
