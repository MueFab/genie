/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Decoder class for handling token-based name
 * decoding in the GENIE framework.
 *
 * This file contains the declaration of the `Decoder` class, which is a
 * specialized implementation of the `NameDecoder` interface. The `Decoder`
 * class is responsible for decoding encoded name tokens from genomic records
 * back into readable strings using the GENIE framework's tokenizer.
 *
 * @details The `Decoder` class provides a token-based approach to name
 * decoding, reconstructing genomic record names from token sequences that were
 * previously encoded. It is designed to work within the `tokenizer` module of
 * the GENIE library.
 */

#ifndef SRC_GENIE_NAME_TOKENIZER_DECODER_H_
#define SRC_GENIE_NAME_TOKENIZER_DECODER_H_

// -----------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>

#include "genie/core/name_decoder.h"
#include "genie/name/tokenizer/token.h"

// -----------------------------------------------------------------------------

namespace genie::name::tokenizer {

/**
 * @brief Implements the token-based name decoding functionality for Genie
 *
 * The `Decoder` class is responsible for decoding the names of genomic records
 * using a token-based approach. It processes encoded `AccessUnit::Descriptor`
 * objects, extracts the name tokens, and reconstructs the original record
 * names. The class is designed to work within the `tokenizer` module of the
 * GENIE library.
 */
class Decoder final : public core::NameDecoder {
  /**
   * @brief Converts a list of tokens into a readable string format.
   *
   * This static method takes a vector of `SingleToken` objects and combines
   * them to form a complete readable string, reconstructing the original name
   * from its tokenized representation.
   *
   * @param rec The vector of `SingleToken` objects representing the encoded
   * name.
   * @return The reconstructed name as a `std::string`.
   */
  static std::string inflate(const std::vector<SingleToken>& rec);

 public:
  /**
   * @brief Processes an encoded descriptor and decodes the names of the
   * records.
   *
   * This method takes an encoded `core::AccessUnit::Descriptor` object as
   * input, performs the necessary decoding operations, and returns the decoded
   * names along with performance statistics. The decoding is performed
   * according to the GENIE format specifications.
   *
   * @param desc The descriptor of the encoded access unit to be processed.
   * @return A tuple containing:
   *  - `std::vector<std::string>`: A vector of decoded record names.
   *  - `core::stats::PerfStats`: The performance statistics for the decoding
   * process.
   */
  std::tuple<std::vector<std::string>, core::stats::PerfStats> Process(
      core::AccessUnit::Descriptor& desc) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::name::tokenizer

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_TOKENIZER_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
