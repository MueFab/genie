/**
 * Copyright 2018-2024 The Genie Authors.
 * @file tokenizer.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the TokenState class for name tokenization in the GENIE
 * framework.
 *
 * This file contains the declaration of the `TokenState` structure, which
 * provides functionalities for breaking down strings into tokens for encoding
 * and decoding in the GENIE framework. The `TokenState` class is designed to
 * handle various types of tokens such as alphabetic sequences, numbers, special
 * characters, and zero sequences.
 *
 * @details The `TokenState` class supports various tokenization operations for
 * encoding genomic record names. It tracks the state of the input string, the
 * tokens being generated, and the tokens from previous records for
 * reference-based compression. It also includes static methods for encoding
 * tokens into an `AccessUnit::Descriptor` object.
 */

#ifndef SRC_GENIE_NAME_TOKENIZER_TOKENIZER_H_
#define SRC_GENIE_NAME_TOKENIZER_TOKENIZER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/name/tokenizer/token.h"

// -----------------------------------------------------------------------------

namespace genie::name::tokenizer {

/**
 * @brief Manages the state for tokenizing and encoding genomic record names.
 *
 * The `TokenState` structure is responsible for managing the process of
 * breaking down input strings into individual tokens for genomic name encoding.
 * It supports multiple types of tokens, such as alphabetic characters, numbers,
 * and special sequences, and provides methods to traverse the input string and
 * generate the appropriate tokens.
 */
class TokenState {
  uint32_t token_pos_;  //!< @brief Current position in the list of old tokens.
  const std::vector<SingleToken>&
      old_rec_;  //!< @brief Reference to the old record's tokens for
               //!< reference-based compression.
  std::vector<SingleToken>
      cur_rec_;  //!< @brief List of tokens generated for the current record.

  std::string::const_iterator
      cur_it_;  //!< @brief Iterator pointing to the current position in the
               //!< input string.
  std::string::const_iterator
      end_it_;  //!< @brief Iterator pointing to the end of the input string.

 protected:
  /**
   * @brief Checks if there are more characters to be processed in the input
   * string.
   *
   * @return True if there are more characters to process, false otherwise.
   */
  [[nodiscard]] bool more() const;

  /**
   * @brief Advances the iterator to the next character in the input string.
   */
  void step();

  /**
   * @brief Retrieves the current character pointed to by the iterator.
   *
   * @return The current character.
   */
  [[nodiscard]] char get() const;

  /**
   * @brief Pushes a new token with a specified type and parameter into the
   * token list.
   *
   * @param t The type of the token.
   * @param param An optional parameter associated with the token.
   */
  void PushToken(Tokens t, uint32_t param = 0);

  /**
   * @brief Pushes an existing `SingleToken` object into the token list.
   *
   * @param t The token to push.
   */
  void PushToken(const SingleToken& t);

  /**
   * @brief Creates a string token from the given parameter and pushes it into
   * the token list.
   *
   * @param param The string value to create a token from.
   */
  void PushTokenString(const std::string& param);

  /**
   * @brief Retrieves the next old token for reference-based encoding.
   *
   * This function returns the next token from the old record's token list for
   * reference-based encoding. It advances the internal position pointer after
   * retrieving the token.
   *
   * @return A reference to the next old token.
   */
  [[nodiscard]] const SingleToken& GetOldToken() const;

  /**
   * @brief Processes alphabetic characters in the input string.
   *
   * This function processes a contiguous sequence of alphabetic characters in
   * the input string, generating a corresponding token for the sequence.
   */
  void alphabetic();

  /**
   * @brief Processes a sequence of zeros in the input string.
   *
   * This function handles a sequence of '0' characters in the input string,
   * generating a corresponding token to represent the zero sequence.
   */
  void zeros();

  /**
   * @brief Processes numeric characters in the input string.
   *
   * This function processes a contiguous sequence of numeric characters,
   * generating a corresponding token for the number sequence.
   */
  void number();

  /**
   * @brief Processes individual special characters in the input string.
   *
   * This function handles any special or non-alphanumeric characters,
   * generating a separate token for each character.
   */
  void character();

 public:
  /**
   * @brief Constructs a `TokenState` object for the given old tokens and input
   * string.
   *
   * @param old The list of tokens from the previous record (for reference-based
   * compression).
   * @param input The input string to be tokenized.
   */
  TokenState(const std::vector<SingleToken>& old, const std::string& input);

  /**
   * @brief Executes the tokenization process and returns the generated tokens.
   *
   * This function performs the tokenization process on the input string, using
   * the old tokens for reference-based compression when applicable.
   *
   * @return A list of tokens generated from the input string.
   */
  std::vector<SingleToken>&& run();

  /**
   * @brief Encodes a list of tokens into the specified descriptor streams.
   *
   * This static function takes a list of tokens and encodes them into the
   * specified `core::AccessUnit::Descriptor` object, organizing them into the
   * appropriate streams.
   *
   * @param tokens The list of tokens to encode.
   * @param streams The descriptor to store the encoded token streams.
   */
  static void encode(const std::vector<SingleToken>& tokens,
                     core::AccessUnit::Descriptor& streams);
};

// -----------------------------------------------------------------------------

}  // namespace genie::name::tokenizer

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_TOKENIZER_TOKENIZER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
