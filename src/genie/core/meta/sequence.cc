/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/sequence.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------

Sequence::Sequence(std::string name, const uint64_t length, const uint16_t id)
    : name_(std::move(name)), length_(length), id_(id) {}

// -----------------------------------------------------------------------------

Sequence::Sequence(const nlohmann::json& json)
    : name_(json["name"]), length_(json["length"]), id_(json["id"]) {}

// -----------------------------------------------------------------------------

nlohmann::json Sequence::ToJson() const {
  nlohmann::json ret;
  ret["name"] = name_;
  ret["length"] = length_;
  ret["id"] = id_;
  return ret;
}

// -----------------------------------------------------------------------------

const std::string& Sequence::GetName() const { return name_; }

// -----------------------------------------------------------------------------

uint64_t Sequence::GetLength() const { return length_; }

// -----------------------------------------------------------------------------

uint16_t Sequence::GetId() const { return id_; }

// -----------------------------------------------------------------------------

std::string& Sequence::GetName() { return name_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
