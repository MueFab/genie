/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.h
 * @brief Header file for the `Decoder` class in the `localassembly` namespace.
 *
 * This file defines the `Decoder` class for handling the decoding of records
 * using the Local Assembly coding scheme. It extends the basecoder
 * `DecoderStub` class and provides additional functionality for managing local
 * references during the decoding process.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_
#define SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/read/basecoder/decoder_stub.h"
#include "genie/read/localassembly/local_reference.h"

// -----------------------------------------------------------------------------

namespace genie::read::localassembly {

/**
 * @brief Decoder class for handling local assembly based decoding.
 *
 * This class extends the `basecoder::DecoderStub` to implement the local
 * assembly decoding process. It manages the local reference sequences used to
 * correctly Decode the record data in the GENIE framework.
 */
class Decoder final : public basecoder::DecoderStub {
  /**
   * @brief Create a specialized decoding state for the local assembly decoder.
   * @param t Incoming access unit data.
   * @return A unique pointer to the decoding state.
   */
  std::unique_ptr<DecodingState> CreateDecodingState(
      core::AccessUnit& t) override;

  /**
   * @brief Get reference sequences for a given segment.
   * @param meta Metadata of the segment.
   * @param state Current decoding state.
   * @return Vector of reference sequences.
   */
  std::vector<std::string> GetReferences(
      const basecoder::Decoder::SegmentMeta& meta,
      DecodingState& state) override;

  /**
   * @brief Hook called after each record is decoded.
   * @param state Current decoding state.
   * @param rec Decoded record.
   */
  void RecordDecodedHook(DecodingState& state,
                         const core::record::Record& rec) override;

  /**
   * @brief Specialized state structure for local assembly decoding.
   */
  struct LaDecodingState final : DecodingState {
    /// Handles local reference sequence management.
    LocalReference ref_encoder;

    /**
     * @brief Constructs the decoding state with the provided access unit.
     * @param t_data Access unit to initialize the state.
     */
    explicit LaDecodingState(core::AccessUnit& t_data);
  };

 public:
  /**
   * @brief Default constructor for the `Decoder` class.
   */
  Decoder() = default;

  /**
   * @brief Default destructor for the `Decoder` class.
   */
  ~Decoder() override = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
