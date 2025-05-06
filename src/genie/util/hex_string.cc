/**
 * Copyright 2018-2024 The Genie Authors.
 * @file hex_string.cc
 * @brief
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/hex_string.h"

#include <algorithm>
#include <string>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

HexString::HexString(const std::string& input) { set(input); }

// -----------------------------------------------------------------------------

const std::string& HexString::str() const { return value_; }

// -----------------------------------------------------------------------------

void HexString::set(const std::string& input) {
  UTILS_DIE_IF(!IsValidHex(input), "Invalid hex string: \"" + input + "\"");
  value_ = input;
}

// -----------------------------------------------------------------------------

bool HexString::operator==(const HexString& other) const {
  return value_ == other.value_;
}

// -----------------------------------------------------------------------------

bool HexString::operator!=(const HexString& other) const {
  return !(*this == other);
}

// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const HexString& h) {
  return os << h.value_;
}

// -----------------------------------------------------------------------------

bool HexString::IsValidHex(const std::string& s) {
  if (s.empty()) return false;
  if (s.length() % 2 != 0) return false;

  return std::all_of(s.begin(), s.end(), [](const char c) {
    return std::isxdigit(static_cast<unsigned char>(c));
  });
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
