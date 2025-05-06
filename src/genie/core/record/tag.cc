/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/tag.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------

Tag::Tag(const char k1, const char k2, TagData val)
    : key_({k1, k2}), value_(std::move(val)) {
  if (!IsValidChar(key_[0]) || !IsValidChar(key_[1])) {
    throw std::invalid_argument(
        "Tag key must be two alphanumeric ASCII characters.");
  }
}

// -----------------------------------------------------------------------------

Tag::Tag(const std::array<char, 2>& key, TagData val)
    : key_(key), value_(std::move(val)) {
  if (!IsValidKey(key_)) {
    throw std::invalid_argument(
        "Tag key must be two alphanumeric ASCII characters.");
  }
}

// -----------------------------------------------------------------------------

[[nodiscard]] const std::array<char, 2>& Tag::key() const { return key_; }

// -----------------------------------------------------------------------------

[[nodiscard]] const TagData& Tag::value() const { return value_; }

// -----------------------------------------------------------------------------

bool Tag::operator==(const Tag& other) const {
  return key_ == other.key_ && value_ == other.value_;
}

// -----------------------------------------------------------------------------

bool Tag::operator!=(const Tag& other) const { return !(*this == other); }

// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Tag& tag) {
  os << tag.key_[0] << tag.key_[1];
  return os;
}

// -----------------------------------------------------------------------------

bool Tag::IsValidChar(const char c) {
  return std::isalnum(static_cast<unsigned char>(c));
}

// -----------------------------------------------------------------------------

bool Tag::IsValidKey(const std::array<char, 2>& key) {
  return IsValidChar(key[0]) && IsValidChar(key[1]);
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
