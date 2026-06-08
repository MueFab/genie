/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Header file for the LZMA-based entropy encoder class.
 * @details This file contains the definition of the `Encoder` class for
 * compressing raw access units using the LZMA algorithm. The `Encoder` class
 * implements the `EntropyEncoder` interface and provides functionality for
 * converting MPEG-G descriptors into their compressed representations.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_LZMA_ENCODER_H_
#define SRC_GENIE_ENTROPY_LZMA_ENCODER_H_

// -----------------------------------------------------------------------------

#include "genie/core/access_unit.h"
#include "genie/core/entropy_encoder.h"
#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/core/parameter/annotation/compressor_parameter_set.h"
#include <lzma.h>

// -----------------------------------------------------------------------------

namespace genie::entropy::lzma {

#define LZMA_DEFAULT_LEVEL 5
#define LZMA_DEFAULT_DIC_SIZE (1 << 24)
#define LZMA_DEFAULT_LC 3
#define LZMA_DEFAULT_LP 0
#define LZMA_DEFAULT_PB 2
#define LZMA_DEFAULT_FB 32
#define LZMA_DEFAULT_THREADS 2


/**
 * @brief Class for compressing raw access units using the LZMA algorithm.
 * @details This encoder class inherits from the `EntropyEncoder` interface and
 * implements the `process` method to encode and Compress access units into
 * LZMA-compressed blocks. It also provides a configurable option to enable
 * writing out intermediate streams.
 */
class Encoder final : public core::EntropyEncoder {
 public:
  bool write_out_streams_{};  //!< @brief Flag to enable or disable writing out
                              //!< intermediate streams

  /**
   * @brief Compress a given descriptor using the LZMA algorithm.
   * @param desc Reference to the descriptor to be compressed.
   * @return The compressed entropy-coded data.
   * @details This method takes a raw MPEG-G descriptor and performs LZMA
   * compression, producing a compressed version of the descriptor in a standard
   * format.
   */
  entropy_coded Process(core::AccessUnit::Descriptor& desc) override;

  /**
   * @brief Construct a new Encoder object.
   * @param write_out_streams Flag to enable or disable writing out intermediate
   * streams.
   * @details If `write_out_streams_` is set to `true`, additional intermediate
   * data will be stored for debugging or analysis purposes.
   */
  explicit Encoder(bool write_out_streams);
};

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
        : level(LZMA_DEFAULT_LEVEL),
          dictSize(LZMA_DEFAULT_DIC_SIZE),
          lc(LZMA_DEFAULT_LC),
          lp(LZMA_DEFAULT_LP),
          pb(LZMA_DEFAULT_PB),
          fb(LZMA_DEFAULT_FB),
          numThreads(LZMA_DEFAULT_THREADS) {}

    LZMAParameters(uint8_t _level, uint32_t _dictSize, uint8_t _lc, uint8_t _lp, uint8_t _pb, uint16_t _fb,
                   uint8_t _numThreads)
        : level(_level), dictSize(_dictSize), lc(_lc), lp(_lp), pb(_pb), fb(_fb), numThreads(_numThreads) {}

    genie::core::parameter::annotation::AlgorithmParameters convertToAlgorithmParameters() const;
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSet(
        uint8_t compressor_ID) const;

    bool parsAreDefault() const {
        return level == LZMA_DEFAULT_LEVEL && dictSize == LZMA_DEFAULT_DIC_SIZE &&
               lc == LZMA_DEFAULT_LC && lp == LZMA_DEFAULT_LP && pb == LZMA_DEFAULT_PB &&
               fb == LZMA_DEFAULT_FB && numThreads == LZMA_DEFAULT_THREADS;
    }
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

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

#endif  // SRC_GENIE_ENTROPY_LZMA_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
