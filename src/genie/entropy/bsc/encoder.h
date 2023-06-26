/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_ENCODER_H_
#define SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access-unit.h"
#include "genie/core/entropy-encoder.h"
#include "genie/util/make-unique.h"
#include "genie/util/watch.h"

#include "codecs/api/mpegg_utils.h"
#include "codecs/include/mpegg-codecs.h"

#ifdef _WIN32
#include <windows.h>
#define SYSERROR() GetLastError()
#else
#include <errno.h>
#define SYSERROR() errno
#endif
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
};

class BSCEncoder {
 public:
    BSCEncoder()
        : lzpHashSize(MPEGG_BSC_DEFAULT_LZPHASHSIZE),
          lzpMinLen(MPEGG_BSC_DEFAULT_LZPMINLEN),
          blockSorter(MPEGG_BSC_BLOCKSORTER_BWT),
          coder(MPEGG_BSC_CODER_QLFC_STATIC) {}

    void encode(const std::stringstream &input, std::stringstream &output) {
        unsigned char *dest = NULL;
        size_t destLen = 0;
        size_t srcLen = input.str().size();
        std::cout << "source length: " << std::to_string(srcLen) << std::endl;

        mpegg_bsc_compress(&dest, &destLen, (const unsigned char *)input.str().c_str(), srcLen, lzpHashSize, lzpMinLen,
                           blockSorter, coder);
        output.write((const char *)dest, destLen);
        std::cout << "compressedData length: " << std::to_string(output.str().size()) << std::endl;
    }

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