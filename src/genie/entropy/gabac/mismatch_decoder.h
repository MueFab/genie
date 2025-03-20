/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the MismatchDecoder class for decoding mismatch
 * sequences using GABAC.
 *
 * This file contains the declaration of the MismatchDecoder class, which
 * extends the core MismatchDecoder class to handle the decoding of mismatch
 * sequences. The class utilizes the GABAC (Genomic Adaptive Binary Arithmetic
 * Coding) engine for efficient decoding of transformed symbols, handling
 * various configurations and transformations.
 *
 * @details The MismatchDecoder class is designed to work with encoded genomic
 * sequences, where mismatches against a reference sequence need to be decoded.
 * The class ensures that mismatched bases are decoded correctly while excluding
 * a specified reference base.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_MISMATCH_DECODER_H_
#define SRC_GENIE_ENTROPY_GABAC_MISMATCH_DECODER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "genie/core/mismatch_decoder.h"
#include "genie/entropy/gabac/decode_transformed_symbols.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Class for decoding mismatch subsequences using GABAC.
 *
 * This class extends the core MismatchDecoder to support decoding of mismatch
 * subsequences. It uses GABAC for entropy decoding and manages the state of
 * multiple transformed symbol decoders.
 */
class MismatchDecoder : public core::MismatchDecoder {
 public:
  /**
   * @brief Constructs a MismatchDecoder with the specified DataBlock and
   * encoding configuration.
   *
   * This constructor initializes the MismatchDecoder with a DataBlock
   * containing the encoded mismatch subsequences and a configuration that
   * controls the decoding process.
   *
   * @param d DataBlock containing the encoded mismatch data.
   * @param c The encoding configuration specifying GABAC parameters and
   * transformations.
   */
  explicit MismatchDecoder(util::DataBlock&& d, const EncodingConfiguration& c);

  /**
   * @brief Decodes a base from the mismatch subsequence, excluding the
   * specified reference base.
   *
   * This function reads one base from the encoded mismatch subsequence,
   * ensuring that the specified reference base is not included in the
   * possible decoded bases.
   *
   * @param ref Base to exclude during decoding (reference base).
   * @return The decoded base as a 64-bit integer.
   */
  uint64_t DecodeMismatch(uint64_t ref) override;

  /**
   * @brief Checks if there are more mismatch symbols left in the subsequence
   * to be decoded.
   *
   * @return True if there are remaining symbols to Decode, false otherwise.
   */
  [[nodiscard]] bool DataLeft() const override;

  /**
   * @brief Retrieves the total number of symbols in the mismatch subsequence.
   *
   * @return The total number of symbols in the current subsequence.
   */
  [[maybe_unused]] [[nodiscard]] uint64_t GetSubseqSymbolsTotal() const;

  /**
   * @brief Creates a copy of the current MismatchDecoder object.
   *
   * This function generates a new MismatchDecoder object that is a copy of
   * the current one, preserving all internal states and configurations.
   *
   * @return A unique pointer to the newly created MismatchDecoder object.
   */
  [[nodiscard]] std::unique_ptr<core::MismatchDecoder> Copy() const override;

  /**
   * @brief A vector storing transformed subsequence data blocks.
   */
  std::vector<util::DataBlock> trnsf_subseq_data_;

 private:
  uint64_t num_subseq_symbols_total_;    //!< @brief Total number of symbols in
                                         //!< the subsequence.
  uint64_t num_subseq_symbols_decoded_;  //!< @brief Number of symbols decoded
                                         //!< so far.

  std::vector<TransformedSymbolsDecoder>
      trnsf_symbols_decoder_;  //!< @brief Decoders for transformed symbols.
  size_t num_trnsf_subseqs_;   //!< @brief Number of transformed subsequences.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_MISMATCH_DECODER_H_

// -----------------------------------------------------------------------------

// -------------------