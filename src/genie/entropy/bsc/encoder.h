/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines a BSC-specific encoder for entropy compression using the GABAC
 * algorithm.
 * @details This file contains the implementation of the `Encoder` class, which
 * performs compression of raw access units into entropy-coded block payloads
 * using the GABAC library. The class inherits from the base `EntropyEncoder`
 *          interface and implements the necessary functionality to process
 * MPEG-G descriptors.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_ENCODER_H_
#define SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// -----------------------------------------------------------------------------

#include <libbsc.h>
#include "genie/core/access_unit/access_unit.h"
#include "genie/core/entropy_encoder.h"
#include "genie/core/parameter/descriptor_present/decoder_regular.h"
#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/core/parameter/annotation/compressor_parameter_set.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::bsc {

class BSCParameters {
 public:
    uint8_t lzpHashSize;
    uint8_t lzpMinLen;
    uint8_t blockSorter;
    uint8_t coder;
    uint16_t features;
    BSCParameters()
        : lzpHashSize(LIBBSC_DEFAULT_LZPHASHSIZE),
          lzpMinLen(LIBBSC_DEFAULT_LZPMINLEN),
          blockSorter(LIBBSC_DEFAULT_BLOCKSORTER),
          coder(LIBBSC_DEFAULT_CODER),
          features(0) {}
    BSCParameters(uint8_t _lzpHashSize, uint8_t _lzpMinLen, uint8_t _blockSorter, uint8_t _coder,
                  uint16_t _features = 0)
        : lzpHashSize(_lzpHashSize),
          lzpMinLen(_lzpMinLen),
          blockSorter(_blockSorter),
          coder(_coder),
          features(_features) {}

    genie::core::parameter::annotation::AlgorithmParameters convertToAlgorithmParameters() const;
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSet(
        uint8_t compressor_ID) const;

    bool parsAreDefault() const {
        return lzpHashSize == LIBBSC_DEFAULT_LZPHASHSIZE && lzpMinLen == LIBBSC_DEFAULT_LZPMINLEN &&
               blockSorter == LIBBSC_DEFAULT_BLOCKSORTER && coder == LIBBSC_DEFAULT_CODER;
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

/**
 * @brief Module to Compress raw access units into block payloads using GABAC.
 * @details The `Encoder` class is responsible for handling entropy compression
 * of raw MPEG-G access units. It implements the core entropy encoding interface
 * and integrates GABAC-specific algorithms to achieve high compression
 * efficiency.
 */
class Encoder : public core::EntropyEncoder {
 public:
  bool writeOutStreams{};  //!< @brief Flag to enable or disable writing out
                           //!< streams for debugging.

  /**
   * @brief Compress the given descriptor using GABAC.
   * @param desc Reference to the descriptor to be compressed.
   * @return The compressed entropy-coded data.
   * @details The function compresses an MPEG-G descriptor using the GABAC
   * library, returning the entropy-coded representation. The resulting data is
   * stored in a structure that encapsulates all necessary metadata and
   * bitstream information.
   */
  entropy_coded Process(core::AccessUnit::Descriptor& desc) override;

  /**
   * @brief Construct a new Encoder object.
   * @param write_out_streams Flag to enable or disable writing out streams for
   * debugging purposes.
   * @details This constructor initializes the encoder with the specified stream
   * writing configuration.
   */
  explicit Encoder(bool write_out_streams);
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
