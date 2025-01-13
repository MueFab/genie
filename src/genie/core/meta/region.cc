/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/region.h"

#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------

Region::Region(const uint16_t seq_id, const uint64_t start, const uint64_t end,
               std::vector<record::ClassType> classes)
    : seq_id_(seq_id),
      classes_(std::move(classes)),
      start_pos_(start),
      end_pos_(end) {}

// -----------------------------------------------------------------------------

Region::Region(const nlohmann::json& json)
    : seq_id_(json["seq_ID"]),
      start_pos_(json["start_pos"]),
      end_pos_(json["end_pos"]) {
  std::vector<uint8_t> tmp = json["classes"];
  for (const auto& t : tmp) {
    classes_.push_back(static_cast<record::ClassType>(t));
  }
}

// -----------------------------------------------------------------------------

nlohmann::json Region::ToJson() const {
  nlohmann::json ret;
  ret["seq_ID"] = seq_id_;
  ret["start_pos"] = start_pos_;
  ret["end_pos"] = end_pos_;
  std::vector<uint8_t> tmp;
  for (const auto& t : classes_) {
    tmp.push_back(static_cast<uint8_t>(t));
    ret["classes"] = tmp;
  }
  return ret;
}

// -----------------------------------------------------------------------------

uint16_t Region::GetSeqId() const { return seq_id_; }

// -----------------------------------------------------------------------------

const std::vector<record::ClassType>& Region::GetClasses() const {
  return classes_;
}

// -----------------------------------------------------------------------------

uint64_t Region::GetStartPos() const { return start_pos_; }

// -----------------------------------------------------------------------------

uint64_t Region::GetEndPos() const { return end_pos_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
