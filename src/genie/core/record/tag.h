/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_TAG_H_
#define SRC_GENIE_CORE_RECORD_TAG_H_

// -----------------------------------------------------------------------------

#include <array>
#include <variant>

#include "genie/core/access_unit.h"
#include "genie/core/record/tag_value.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------

/**
 * @brief Class representing a tag with a 2-character key and associated value.
 */
class Tag {
 public:
  /**
   * @brief Constructs a Tag with the given key and value.
   * @param k1 First character of the key.
   * @param k2 Second character of the key.
   * @param val Value associated with the tag.
   */
  Tag(char k1, char k2, TagData val);

  /**
   * @brief Constructs a Tag with the given key and value.
   * @param key Array of two characters representing the key.
   * @param val Value associated with the tag.
   */
  Tag(const std::array<char, 2>& key, TagData val);

  /**
   * @brief Returns the key of the tag.
   * @return The key of the tag as an array of two characters.
   */
  [[nodiscard]] const std::array<char, 2>& key() const;

  /**
   * @brief Returns the value of the tag.
   * @return The value of the tag as a TagData variant.
   */
  [[nodiscard]] const TagData& value() const;

  /**
   * @brief Compares this tag with another tag for equality.
   * @param other The other tag to compare with.
   * @return True if the tags are equal, false otherwise.
   */
  [[nodiscard]] bool operator==(const Tag& other) const;

  /**
   * @brief Compares this tag with another tag for inequality.
   * @param other The other tag to compare with.
   * @return True if the tags are not equal, false otherwise.
   */
  [[nodiscard]] bool operator!=(const Tag& other) const;

  /**
   * @brief Outputs the tag to an output stream.
   * @param os The output stream.
   * @param tag The tag to output.
   * @return The output stream.
   */
  friend std::ostream& operator<<(std::ostream& os, const Tag& tag);

 private:
  /// @brief Key of the tag, represented as an array of two characters.
  std::array<char, 2> key_;

  /// @brief Value of the tag, represented as a variant of different types.
  TagData value_;

  /**
   * @brief Checks if a character is valid for a tag key.
   * @param c The character to check.
   * @return True if the character is valid, false otherwise.
   */
  static bool IsValidChar(char c);

  /**
   * @brief Checks if a key is valid for a tag.
   * @param key The key to check.
   * @return True if the key is valid, false otherwise.
   */
  static bool IsValidKey(const std::array<char, 2>& key);
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_TAG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
