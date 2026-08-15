/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 * @brief Header file for the ZSTD entropy encoder module.
 * @details This file declares the `Encoder` class, which provides functionality
 * for compressing raw access units into block payloads using the ZSTD
 * compression algorithm. The class is part of the GENIE MPEG-G toolkit and
 * implements the required functionality to process MPEG-G descriptors and
 * generate compressed data blocks for efficient storage.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_
#define SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/access_unit.h"
#include "genie/core/entropy_encoder.h"
#include "genie/core/parameter/descriptor_present/decoder_regular.h"
#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/core/parameter/annotation/compressor_parameter_set.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::zstd {

class ZSTDParameters {
 public:
    bool use_dictionary_flag;
    uint16_t dictionary_size;
    std::string dictionary;
    ZSTDParameters() : use_dictionary_flag(false), dictionary_size(0), dictionary{} {}
    ZSTDParameters(uint8_t _use_dictionary_flag, uint8_t _dictionary_size, std::string _dictionary)
        : use_dictionary_flag(_use_dictionary_flag), dictionary_size(_dictionary_size), dictionary(_dictionary) {}

    genie::core::parameter::annotation::AlgorithmParameters convertToAlgorithmParameters() const;
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSet(
        uint8_t compressor_ID) const;

    bool parsAreDefault() const { return use_dictionary_flag == false && dictionary_size == 0 && dictionary.empty(); }
};

class ZSTDEncoder {
 public:
    ZSTDEncoder();

    void encode(std::stringstream &input, std::stringstream &output);

    void decode(std::stringstream &input, std::stringstream &output);

    void configure(const ZSTDParameters zstdParameters) {
        use_dictionary_flag = zstdParameters.use_dictionary_flag;
        dictionary_size = zstdParameters.dictionary_size;
        dictionary = zstdParameters.dictionary;
    }

 private:
    uint8_t use_dictionary_flag;
    uint16_t dictionary_size;
    std::string dictionary;
};

/**
 * @brief Encoder class for the ZSTD compression algorithm.
 * @details This class handles the compression of raw access units into block
 * payloads using the ZSTD algorithm. It converts the raw MPEG-G descriptors
 * into compressed representations, following the MPEG-G specifications and
 * utilizing the GENIE core infrastructure.
 */
class Encoder final : public core::EntropyEncoder {
 public:
  bool write_out_streams_{};  //!< @brief Flag to enable writing output streams
                              //!< for debugging purposes.

  /**
   * @brief Compress a given descriptor using the ZSTD algorithm.
   * @param desc Reference to the descriptor to be compressed.
   * @return The compressed entropy-coded data, encapsulated in the
   * `EntropyCoded` structure.
   */
  entropy_coded Process(core::AccessUnit::Descriptor& desc) override;

  /**
   * @brief Construct a new Encoder object.
   * @param write_out_streams Flag to enable or disable writing out streams for
   * debugging.
   */
  explicit Encoder(bool write_out_streams);
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
