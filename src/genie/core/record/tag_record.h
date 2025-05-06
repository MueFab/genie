/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_TAG_RECORD_H_
#define SRC_GENIE_CORE_RECORD_TAG_RECORD_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/core/record/tag.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief Class representing a collection of tags.
 */
class TagRecord {
 public:
  /**
   * @brief Sets a tag in the collection.
   */
  void Set(const Tag& tag);

  /**
   * @brief Retrieves a tag by its key.
   * @param key The key of the tag to retrieve.
   * @return A pointer to the tag if found, nullptr otherwise.
   */
  [[nodiscard]] const Tag* Get(const std::string& key) const;

  /**
   * @brief Checks if a tag with the given key exists in the collection.
   * @param key The key to check.
   * @return True if the tag exists, false otherwise.
   */
  [[nodiscard]] bool Contains(const std::string& key) const;

  /**
   * @brief Erases a tag with the given key from the collection.
   * @param key The key of the tag to erase.
   */
  void Erase(const std::string& key);

  /**
   * @brief Retrieves all tags in the collection.
   * @return A vector containing all tags.
   */
  [[nodiscard]] const std::vector<Tag>& All() const;

 private:
  /// @brief Vector of tags.
  std::vector<Tag> tags_;

  /**
   * @brief Converts a key to a string.
   * @param key The key to convert.
   * @return The string representation of the key.
   */
  [[nodiscard]]
  static std::string KeyToString(const std::array<char, 2>& key);

  /**
   * @brief Finds a tag by its key.
   * @param key The key to search for.
   * @return An iterator to the found tag, or end() if not found.
   */
  std::vector<Tag>::iterator FindByKey(const std::string& key);

  /**
   * @brief Finds a tag by its key (const version).
   * @param key The key to search for.
   * @return A const iterator to the found tag, or end() if not found.
   */
  [[nodiscard]] std::vector<Tag>::const_iterator FindByKey(
      const std::string& key) const;

  /**
   * @brief Finds the position to insert a tag based on its key.
   * @param key The key to search for.
   * @return An iterator to the position where the tag can be inserted.
   */
  std::vector<Tag>::iterator FindInsertPosition(const std::array<char, 2>& key);
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_TAG_RECORD_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
