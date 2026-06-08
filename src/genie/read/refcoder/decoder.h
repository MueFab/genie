/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.h
 * @brief Header file for the `Decoder` class in the `read::refcoder` namespace.
 *
 * This file defines the `Decoder` class used for decoding genomic records that
 * are encoded using reference-based encoding. It provides functionality to
 * manage decoding states and reference excerpts, enabling the reconstruction of
 * sequences from encoded streams using reference information.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_REFCODER_DECODER_H_
#define SRC_GENIE_READ_REFCODER_DECODER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/core/read_decoder.h"
#include "genie/read/basecoder/decoder_stub.h"

// -----------------------------------------------------------------------------

namespace genie::read::refcoder {

/**
 * @brief Reference-based decoder for genomic records.
 *
 * The `Decoder` class implements the decoding of genomic records that have been
 * encoded using a reference-based approach. It manages reference excerpts,
 * decodes segment metadata, and constructs the decoded genomic records. This
 * class extends the `basecoder::DecoderStub` to handle specific reference-based
 * decoding logic.
 */
class Decoder final : public basecoder::DecoderStub {
  /**
   * @brief Creates a new decoding state based on the provided access unit.
   *
   * The `CreateDecodingState` method initializes the `RefDecodingState` for the
   * current decoding process based on the incoming access unit. It prepares the
   * necessary reference data for subsequent decoding operations.
   *
   * @param t The access unit containing encoded genomic data.
   * @return A unique pointer to the newly created decoding state.
   */
  std::unique_ptr<DecodingState> CreateDecodingState(
      core::AccessUnit& t) override;

  /**
   * @brief Retrieves reference sequences for the specified segment metadata.
   *
   * The `GetReferences` method extracts the relevant reference sequences for
   * the given segment metadata, using the reference excerpt stored in the
   * current decoding state.
   *
   * @param meta The segment metadata containing information on the reference.
   * @param state The current decoding state.
   * @return A vector of reference sequences used for decoding the segment.
   */
  std::vector<std::string> GetReferences(
      const basecoder::Decoder::SegmentMeta& meta,
      DecodingState& state) override;

  /**
   * @brief Internal state for managing reference-based decoding.
   */
  struct RefDecodingState final : DecodingState {
    /// Reference excerpt for managing sequence reconstruction.
    core::ReferenceManager::ReferenceExcerpt ref_excerpt;

    /**
     * @brief Constructs the state from the provided access unit.
     *
     * Initializes the reference excerpt and other state variables from the
     * given access unit, preparing for the decoding of records.
     *
     * @param t_data The access unit containing encoded data streams.
     */
    explicit RefDecodingState(core::AccessUnit& t_data);
  };

 public:
  /**
   * @brief Default constructor for the `Decoder` class.
   */
  Decoder() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::refcoder

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_REFCODER_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
