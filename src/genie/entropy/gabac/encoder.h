/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the GABAC Encoder class for compressing raw access
 * units into block payloads.
 *
 * This file contains the declaration of the `Encoder` class, which extends the
 * `EntropyEncoder` class. It is responsible for encoding genomic data using the
 * GABAC (Genomic Adaptive Binary Arithmetic Coding) compression technique. The
 * encoder performs the actual compression based on the configurations specified
 * and generates the compressed payloads that can be used for efficient storage
 * and transmission.
 *
 * @details The `Encoder` class offers two main functions, `Compress` and
 * `CompressTokens`, for transforming genomic data based on the
 * `EncodingConfiguration` provided. Additionally, the `process` method is
 * overridden to provide the entry point for compression operations when working
 * with `AccessUnit::Descriptor` objects.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODER_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODER_H_

// -----------------------------------------------------------------------------

#include "genie/core/access_unit.h"
#include "genie/core/entropy_encoder.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/entropy/gabac/gabac_seq_conf_set.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Module to Compress raw access units into block payloads using GABAC.
 *
 * The `Encoder` class is a part of the GABAC library that provides
 * functionalities to Compress genomic data using GABAC encoding techniques. It
 * operates on raw access units, processes descriptors, and utilizes predefined
 * configurations to generate compressed block payloads that are suitable for
 * storage and transmission.
 *
 * @details This class inherits from `core::EntropyEncoder` and implements the
 * main interface method `process` for compression. It also provides two static
 * helper methods, `Compress` and `CompressTokens`, to perform transformation on
 * individual subsequences or descriptor tokens respectively.
 */
class Encoder : public core::EntropyEncoder {
 private:
  /**
   * @brief Perform GABAC compression on a single subsequence.
   *
   * This function takes a raw uncompressed subsequence and applies GABAC
   * compression using the provided configuration. The output is a transformed
   * `AccessUnit::Subsequence` containing the compressed representation.
   *
   * @param conf GABAC configuration to use for compression.
   * @param in The input uncompressed subsequence.
   * @return A compressed `AccessUnit::Subsequence` that can be stored or
   * transmitted.
   */
  static core::AccessUnit::Subsequence Compress(
      const EncodingConfiguration& conf, core::AccessUnit::Subsequence&& in);

  /**
   * @brief Perform GABAC compression on descriptor tokens.
   *
   * This function compresses the descriptor tokens of an access unit
   * descriptor using the provided GABAC configuration. It is primarily used
   * for compressing specific fields or descriptors within the access unit.
   *
   * @param conf0 The configuration to use for token compression.
   * @param in The input uncompressed descriptor.
   * @return A compressed `AccessUnit::Descriptor` containing the transformed
   * representation of the tokens.
   */
  static core::AccessUnit::Descriptor CompressTokens(
      const EncodingConfiguration& conf0, core::AccessUnit::Descriptor&& in);

 public:
  GabacSeqConfSet
      config_set_;  //!< @brief Configuration set for GABAC compression. This
                    //!< set remains static over time.
  [[maybe_unused]] bool
      write_out_streams_{};  //!< @brief Flag to indicate whether to write
                             //!< out streams for debugging or analysis.

  /**
   * @brief Process and Compress the given descriptor using GABAC encoding.
   *
   * This method overrides the `process` function from `core::EntropyEncoder`
   * and provides the main interface for compressing access unit descriptors.
   * It applies the GABAC compression based on the internal configuration set
   * and returns the encoded result as an `EntropyCoded` object.
   *
   * @param desc The input descriptor to be compressed.
   * @return The compressed `EntropyCoded` representation of the input
   * descriptor.
   */
  entropy_coded Process(core::AccessUnit::Descriptor& desc) override;

  /**
   * @brief Constructs an Encoder object with the option to enable or disable
   * stream writing.
   *
   * This constructor initializes the encoder with a flag indicating whether
   * the transformed streams should be written out for debugging or analysis
   * purposes.
   *
   * @param write_out_streams Flag to enable or disable writing out streams.
   */
  explicit Encoder(bool write_out_streams);
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
