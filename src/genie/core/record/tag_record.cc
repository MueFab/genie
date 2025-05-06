/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/tag_record.h"

#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------

void TagRecord::Set(const Tag& tag) {
  if (const auto it = FindInsertPosition(tag.key());
      it != tags_.end() && it->key() == tag.key()) {
    *it = tag;
  } else {
    tags_.insert(it, tag);
  }
}

// -----------------------------------------------------------------------------

[[nodiscard]] const Tag* TagRecord::Get(const std::string& key) const {
  const auto it = FindByKey(key);
  return it != tags_.end() && KeyToString(it->key()) == key ? &*it : nullptr;
}

// -----------------------------------------------------------------------------

[[nodiscard]] bool TagRecord::Contains(const std::string& key) const {
  return Get(key) != nullptr;
}

// -----------------------------------------------------------------------------

void TagRecord::Erase(const std::string& key) {
  if (const auto it = FindByKey(key);
      it != tags_.end() && KeyToString(it->key()) == key) {
    tags_.erase(it);
  }
}

// -----------------------------------------------------------------------------

[[nodiscard]] const std::vector<Tag>& TagRecord::All() const { return tags_; }

// -----------------------------------------------------------------------------

std::string TagRecord::KeyToString(const std::array<char, 2>& key) {
  return std::string{key[0], key[1]};
}

// -----------------------------------------------------------------------------

std::vector<Tag>::iterator TagRecord::FindByKey(const std::string& key) {
  return std::lower_bound(tags_.begin(), tags_.end(), key,
                          [](const Tag& tag, const std::string& k) {
                            return KeyToString(tag.key()) < k;
                          });
}

// -----------------------------------------------------------------------------

[[nodiscard]] std::vector<Tag>::const_iterator TagRecord::FindByKey(
    const std::string& key) const {
  return std::lower_bound(tags_.begin(), tags_.end(), key,
                          [](const Tag& tag, const std::string& k) {
                            return KeyToString(tag.key()) < k;
                          });
}

// -----------------------------------------------------------------------------

std::vector<Tag>::iterator TagRecord::FindInsertPosition(
    const std::array<char, 2>& key) {
  return std::lower_bound(tags_.begin(), tags_.end(), key,
                          [](const Tag& tag, const std::array<char, 2>& k) {
                            return tag.key() < k;
                          });
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
