/**
 * Copyright 2018-2024 The Genie Authors.
 * @file token.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of token structures and enumerations for name tokenization
 * in the GENIE framework.
 *
 * This file contains the declarations of various token-related structures and
 * enumerations used in the GENIE framework. It provides the `Tokens`
 * enumeration, which defines different token types, and the `SingleToken`
 * structure, which represents individual tokens with their associated
 * parameters. The file also includes utility functions for working with tokens,
 * such as retrieving token metadata and generating patches between token
 * sequences.
 *
 * @details The tokens defined in this file are used for encoding and decoding
 * genomic record names in the GENIE framework. They enable efficient
 * representation of name changes and allow for both direct encoding and
 * differential encoding relative to previous records.
 */

#ifndef SRC_GENIE_NAME_TOKENIZER_TOKEN_H_
#define SRC_GENIE_NAME_TOKENIZER_TOKEN_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::name::tokenizer {

// -----------------------------------------------------------------------------

constexpr uint8_t kTypeSeq =
    0;  /// Constant representing a sequence type for encoding.

/**
 * @brief Enumeration of token types used for name encoding and decoding.
 *
 * The `Tokens` enumeration defines the various types of tokens used in the
 * GENIE framework for representing different segments of a genomic record name.
 * These tokens can represent direct strings, numeric values, characters, and
 * delta-encoded differences.
 */
enum class Tokens : uint8_t {
  /// Duplicate token indicating repetition of a previous pattern.
  DUP = 0,

  /// Difference token indicating changes relative to a previous token.
  DIFF = 1,

  /// A literal string token.
  STRING = 2,

  /// A single character token.
  CHAR = 3,

  /// Numeric digits token.
  DIGITS = 4,

  /// Delta-encoded numeric value.
  DELTA = 5,

  /// Numeric digits token with leading zeros.
  DIGITS0 = 6,

  /// Delta-encoded numeric value with leading zeros.
  DELTA0 = 7,

  /// Match token indicating alignment with a previous sequence.
  MATCH = 8,

  /// Delta-encoded zero-length value.
  DZLEN = 9,

  /// End-of-sequence token.
  END = 10,

  /// No-operation token.
  NONE = 11
};

/**
 * @brief Stores metadata for each type of token.
 *
 * The `TokenInfo` structure provides metadata about each token type, including
 * its name and any associated sequence parameters. It is used for debugging and
 * descriptive purposes.
 */
struct TokenInfo {
  /// The name of the token type.
  std::string name;

  /// Indicates the presence of a sequence parameter (if applicable).
  int8_t param_seq;
};

/**
 * @brief Represents a single token with its type and associated parameters.
 *
 * The `SingleToken` structure encapsulates an individual token, storing its
 * type, an optional numeric parameter, and an optional string parameter. It
 * supports comparison operations for equality and inequality, making it
 * suitable for use in differential encoding.
 */
struct SingleToken {
  /// The type of the token.
  Tokens token;

  /// Optional numeric parameter associated with the token.
  uint32_t param;

  /// Optional string parameter associated with the token.
  std::string param_string;

  /**
   * @brief Constructs a `SingleToken` object with the specified type,
   * parameter, and string value.
   *
   * @param t The type of the token.
   * @param p The numeric parameter associated with the token.
   * @param ps The string parameter associated with the token.
   */
  SingleToken(Tokens t, uint32_t p, std::string ps);

  /**
   * @brief Compares two `SingleToken` objects for equality.
   *
   * @param t The other `SingleToken` to compare with.
   * @return True if the tokens are equal, false otherwise.
   */
  bool operator==(const SingleToken& t) const;

  /**
   * @brief Compares two `SingleToken` objects for inequality.
   *
   * @param t The other `SingleToken` to compare with.
   * @return True if the tokens are not equal, false otherwise.
   */
  bool operator!=(const SingleToken& t) const;
};

/**
 * @brief Retrieves metadata for a given token type.
 *
 * This function returns a `TokenInfo` structure containing metadata such as the
 * name and parameter sequence type for the specified token.
 *
 * @param t The token type to retrieve metadata for.
 * @return The `TokenInfo` structure associated with the token type.
 */
const TokenInfo& GetTokenInfo(Tokens t);

/**
 * @brief Generates a differential patch between two token sequences.
 *
 * This function compares an old and a new token sequence and generates a patch
 * representing the changes needed to transform the old sequence into the new
 * one.
 *
 * @param old_string The old token sequence.
 * @param new_string The new token sequence.
 * @return A vector of `SingleToken` objects representing the patch.
 */
std::vector<SingleToken> patch(const std::vector<SingleToken>& old_string,
                               const std::vector<SingleToken>& new_string);

// -----------------------------------------------------------------------------

}  // namespace genie::name::tokenizer

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_TOKENIZER_TOKEN_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
