/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the GABAC (Genomic Arithmetic Binary Adaptive Coding)
 * decoder class.
 *
 * This file contains the declaration of the `Decoder` class, which is
 * responsible for decompressing block payloads back into raw access units using
 * the GABAC module. The decoder handles MPEG-G compliant decompression and
 * transformation of encoded genomic data. It leverages CABAC (Context-Adaptive
 * Binary Arithmetic Coding) for entropy decoding and supports various
 * transformation operations.
 *
 * @details The `Decoder` class inherits from the `core::EntropyDecoder` class
 * and provides an implementation for the `process` method, which decodes
 * subsequences using a specified GABAC configuration. The primary method in
 * this class, `Decompress`, performs the actual decoding and transformation of
 * compressed subsequences based on the provided configuration and handles
 * optional mismatch coding.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODER_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODER_H_

// -----------------------------------------------------------------------------

#include <tuple>   // For std::tuple return type in process function.
#include <vector>  // For using std::vector container type.

#include "genie/core/access_unit.h"
#include "genie/core/entropy_decoder.h"
#include "genie/entropy/gabac/gabac_seq_conf_set.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Module to Decompress a block payload into a raw access unit using the
 * GABAC module.
 *
 * The `Decoder` class provides functionalities to Decompress genomic block
 * payloads encoded using the GABAC module. It extends the
 * `core::EntropyDecoder` class and overrides its `process` method to Decode and
 * transform subsequence data back into its original raw format.
 *
 * @details This class supports decoding of various MPEG-G compliant descriptor
 * subsequences using the GABAC library. It handles transformation, CABAC
 * decompression, and mismatch decoding operations. The `Decompress` function
 * performs the main decoding operation using the provided
 * `EncodingConfiguration`.
 */
class Decoder : public core::EntropyDecoder {
 private:
  /**
   * @brief Decompress a subsequence payload using the specified GABAC
   * configuration.
   *
   * This function decodes the input subsequence payload using the
   * configuration specified in the `conf` parameter. It applies the necessary
   * CABAC decompression and optional transformations, depending on the
   * settings defined in the `EncodingConfiguration`. The function also
   * supports mismatch decoding if enabled via the `mm_coder_enabled` flag.
   *
   * @param conf GABAC configuration for the decompression process.
   * @param data The compressed subsequence payload to be decoded.
   * @param mm_coder_enabled Boolean flag indicating if mismatch decoding is
   * enabled.
   * @return The decompressed subsequence in raw format.
   *
   * @details The function uses the `EncodingConfiguration` to apply
   * transformations and Decompress the subsequence payload using CABAC
   * decoding. If mismatch coding is enabled, it decodes the mismatch
   * subsequences accordingly.
   */
  static core::AccessUnit::Subsequence Decompress(
      const EncodingConfiguration& conf, core::AccessUnit::Subsequence&& data,
      bool mm_coder_enabled);

 public:
  /**
   * @brief Processes and decodes a descriptor using the specified subsequence
   * configuration.
   *
   * This function takes a descriptor subsequence configuration (`param`), an
   * encoded descriptor (`d`), and a flag indicating whether mismatch coding
   * is enabled (`mm_coder_enabled`). It returns a tuple containing the decoded
   * descriptor and performance statistics.
   *
   * @param param The configuration for the subsequence descriptor.
   * @param d The descriptor containing encoded subsequence data.
   * @param mm_coder_enabled Boolean flag to enable or disable mismatch coding.
   * @return A tuple containing the decoded descriptor and performance
   * statistics.
   *
   * @details The function overrides the `process` method in the
   * `core::EntropyDecoder` base class. It utilizes the GABAC library to
   * Decode the subsequence data based on the configurations in `param` and
   * `d`. The mismatch coding is optionally enabled using the `mm_coder_enabled`
   * flag, allowing for selective mismatch subsequence decoding.
   */
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Process(
      const core::parameter::DescriptorSubSequenceCfg& param,
      core::AccessUnit::Descriptor& d, bool mm_coder_enabled) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------