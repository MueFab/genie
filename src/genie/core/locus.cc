/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/locus.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"
#include "genie/util/string_helpers.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

const std::string& Locus::GetRef() const { return ref_name_; }

// -----------------------------------------------------------------------------

uint32_t Locus::GetStart() const { return start_; }

// -----------------------------------------------------------------------------

uint32_t Locus::GetEnd() const { return end_; }

// -----------------------------------------------------------------------------

bool Locus::PositionPresent() const { return pos_present_; }

// -----------------------------------------------------------------------------

Locus::Locus(std::string ref)
    : ref_name_(std::move(ref)), pos_present_(false), start_(0), end_(0) {}

// -----------------------------------------------------------------------------

Locus::Locus(std::string ref, const uint32_t start, const uint32_t end)
    : ref_name_(std::move(ref)), pos_present_(true), start_(start), end_(end) {
  UTILS_DIE_IF(end_ < start_, "Locus ends before start");
}

// -----------------------------------------------------------------------------

Locus Locus::FromString(const std::string& string) {
  auto tok = util::Tokenize(string, ':');
  if (tok.size() == 1) {
    return Locus(tok.front());  // Sequence name only
  }
  if (constexpr size_t num_start_end = 2; tok.size() == num_start_end) {
    const auto pos = util::Tokenize(tok.back(), '-');  // Sequence + position
    UTILS_DIE_IF(pos.size() != num_start_end, "Invalid locus");
    return {tok.front(), static_cast<uint32_t>(std::stoi(pos[0])),
            static_cast<uint32_t>(std::stoi(pos[1]))};
  }
  UTILS_DIE("Invalid locus");
}

// -----------------------------------------------------------------------------

std::string Locus::ToString() const {
  return pos_present_ ? ref_name_ + ':' + std::to_string(start_) + '-' +
                            std::to_string(end_)
                      : ref_name_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
