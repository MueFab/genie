/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/descriptor_stream.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------

DescriptorStream::DescriptorStream(const size_t descriptor_id,
                                   std::string ds_protection_value)
    : descriptor_id_(descriptor_id),
      ds_protection_value_(std::move(ds_protection_value)) {}

// -----------------------------------------------------------------------------

DescriptorStream::DescriptorStream(const nlohmann::json& obj)
    : descriptor_id_(obj["descriptor_ID"]),
      ds_protection_value_(obj["DS_protection_value"]) {}

// -----------------------------------------------------------------------------

size_t DescriptorStream::GetId() const { return descriptor_id_; }

// -----------------------------------------------------------------------------

nlohmann::json DescriptorStream::ToJson() const {
  nlohmann::json ret;
  ret["descriptor_ID"] = descriptor_id_;
  ret["DS_protection_value"] = ds_protection_value_;
  return ret;
}

// -----------------------------------------------------------------------------

const std::string& DescriptorStream::GetProtection() const {
  return ds_protection_value_;
}

// -----------------------------------------------------------------------------

std::string& DescriptorStream::GetProtection() { return ds_protection_value_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
