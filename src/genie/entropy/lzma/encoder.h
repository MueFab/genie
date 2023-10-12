/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_LZMA_ENCODER_H_
#define SRC_GENIE_ENTROPY_LZMA_ENCODER_H_

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
#include "genie/util/make-unique.h"
#include "genie/util/watch.h"

#include "codecs/include/mpegg-codecs.h"
#include "apps/genie/annotation/code.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace lzma {

class LZMAParameters {
 public:
    uint8_t level;      /* 0 <= level <= 9, default = 5 */
    uint32_t dictSize;  /* default = (1 << 24) */
    uint8_t lc;         /* 0 <= lc <= 8, default = 3  */
    uint8_t lp;         /* 0 <= lp <= 4, default = 0  */
    uint8_t pb;         /* 0 <= pb <= 4, default = 2  */
    uint16_t fb;        /* 5 <= fb <= 273, default = 32 */
    uint8_t numThreads; /* 1 or 2, default = 2 */

    LZMAParameters()
        : level(MPEGG_LZMA_DEFAULT_LEVEL),
          dictSize(MPEGG_LZMA_DEFAULT_DIC_SIZE),
          lc(MPEGG_LZMA_DEFAULT_LC),
          lp(MPEGG_LZMA_DEFAULT_LP),
          pb(MPEGG_LZMA_DEFAULT_PB),
          fb(MPEGG_LZMA_DEFAULT_FB),
          numThreads(MPEGG_LZMA_DEFAULT_THREADS) {}

    LZMAParameters(uint8_t _level,
                   uint32_t _dictSize,
                   uint8_t _lc,
                   uint8_t _lp,
                   uint8_t _pb,
                   uint16_t _fb,
                   uint8_t _numThreads)
        : level(_level),
          dictSize(_dictSize),
          lc(_lc),
          lp(_lp),
          pb(_pb),
          fb(_fb),
          numThreads(_numThreads) {}

    genie::core::record::annotation_parameter_set::AlgorithmParameters convertToAlgorithmParameters() const;
};

class LZMAEncoder {
 public:
    LZMAEncoder();

    void encode(std::stringstream &input, std::stringstream &output);

    void decode(std::stringstream &input, std::stringstream &output);

    void configure(const LZMAParameters lzmaParameters) {
        level = lzmaParameters.level;
        dictSize = lzmaParameters.dictSize;
        lc = lzmaParameters.lc;
        lp = lzmaParameters.lp;
        pb = lzmaParameters.pb;
        fb = lzmaParameters.fb;
        numThreads = lzmaParameters.numThreads;
    }

 private:
    uint8_t level;      /* 0 <= level <= 9, default = 5 */
    uint32_t dictSize;  /* default = (1 << 24) */
    uint8_t lc;         /* 0 <= lc <= 8, default = 3  */
    uint8_t lp;         /* 0 <= lp <= 4, default = 0  */
    uint8_t pb;         /* 0 <= pb <= 4, default = 2  */
    uint16_t fb;        /* 5 <= fb <= 273, default = 32 */
    uint8_t numThreads; /* 1 or 2, default = 2 */
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lzma
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_LZMA_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
