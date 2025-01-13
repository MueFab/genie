/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/descriptor_stream_protection.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

DescriptorStreamProtection::DescriptorStreamProtection(
    std::string ds_protection_value)
    : DSProtectionValue(std::move(ds_protection_value)) {}

// -----------------------------------------------------------------------------

DescriptorStreamProtection::DescriptorStreamProtection(
    util::BitReader& reader) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  DSProtectionValue.resize(length);
  reader.ReadAlignedBytes(DSProtectionValue.data(), DSProtectionValue.length());
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

const std::string& DescriptorStreamProtection::GetProtectionValue() const {
  return DSProtectionValue;
}

// -----------------------------------------------------------------------------

const std::string& DescriptorStreamProtection::GetKey() const {
  static const std::string key = "dspr";
  return key;
}

// -----------------------------------------------------------------------------

void DescriptorStreamProtection::BoxWrite(util::BitWriter& bit_writer) const {
  bit_writer.WriteAlignedBytes(DSProtectionValue.data(),
                              DSProtectionValue.size());
}

// -----------------------------------------------------------------------------

bool DescriptorStreamProtection::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DescriptorStreamProtection&>(info);
  return DSProtectionValue == other.DSProtectionValue;
}

// -----------------------------------------------------------------------------

std::string DescriptorStreamProtection::decapsulate() {
  return std::move(DSProtectionValue);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
