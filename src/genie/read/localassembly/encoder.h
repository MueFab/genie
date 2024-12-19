/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 * @brief Header file for the `Encoder` class in the `localassembly` namespace.
 *
 * This file defines the `Encoder` class for handling the encoding of records
 * using the Local Assembly coding scheme. It extends the basecoder
 * `EncoderStub` class and provides additional functionality for managing local
 * references during the encoding process.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_
#define SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <utility>

#include "genie/core/read_encoder.h"
#include "genie/read/basecoder/encoder_stub.h"
#include "genie/read/localassembly/local_reference.h"

// -----------------------------------------------------------------------------

namespace genie::read::localassembly {

/**
 * @brief Encoder class for handling local assembly based encoding.
 *
 * This class extends the `basecoder::EncoderStub` to implement the local
 * assembly encoding process. It manages the local reference sequences used to
 * correctly encode the record data in the GENIE framework.
 */
class Encoder final : public basecoder::EncoderStub {
  /// Flag for enabling debug output during encoding.
  bool debug_;

  /**
   * @brief Internal encoding state (not exposed publicly).
   */
  struct LaEncodingState final : EncodingState {
    /**
     * @brief Construct from raw data.
     * @param data Chunk of data to construct from.
     * @param cr_buf_max_size  LA parameter.
     */
    explicit LaEncodingState(const core::record::Chunk& data,
                             uint32_t cr_buf_max_size);

    /// Local assembly handler.
    LocalReference ref_coder;
  };

  /**
   * @brief Print current local assembly state.
   * @param state Current state of the encoder.
   * @param ref1 Reference sequence of the first segment.
   * @param ref2 Reference sequence of the second segment.
   * @param r Current record being processed.
   */
  void PrintDebug(const LaEncodingState& state, const std::string& ref1,
                  const std::string& ref2, const core::record::Record& r) const;

  /**
   * @brief Pack encoded data into an access unit.
   * @param id Multithreading sync ID.
   * @param qv Encoded quality values.
   * @param read_name Encoded record names.
   * @param state Encoding state.
   * @return Assembled access unit with parameters.
   */
  core::AccessUnit Pack(size_t id, core::QvEncoder::qv_coded qv,
                        core::AccessUnit::Descriptor read_name,
                        EncodingState& state) override;

  /**
   * @brief Calculate references for record from local assembly.
   * @param r Record to calculate for.
   * @param state Encoding state.
   * @return Pair of references. If unpaired, second reference remains empty.
   */
  std::pair<std::string, std::string> GetReferences(
      const core::record::Record& r, EncodingState& state) override;

  /**
   * @brief Create a new encoding state from a chunk of data.
   * @param data Chunk to create from.
   * @return A unique pointer to an `EncodingState` object.
   */
  [[nodiscard]] std::unique_ptr<EncodingState> CreateState(
      const core::record::Chunk& data) const override;

 public:
  /**
   * @brief Create an encoder with specified parameters.
   * @param debug If true, debug information is printed.
   * @param write_raw Flag for enabling raw write mode.
   */
  Encoder(bool debug, bool write_raw);

  /**
   * @brief Downgrade class N to class M and then proceed with encoding.
   * @param t Data chunk to process.
   * @param id Multithreading sync ID.
   */
  void FlowIn(core::record::Chunk&& t, const util::Section& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
