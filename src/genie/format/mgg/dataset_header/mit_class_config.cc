/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/mit_class_config.h"

#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

// -----------------------------------------------------------------------------

bool MitClassConfig::operator==(const MitClassConfig& other) const {
  return id_ == other.id_ && descriptor_ids_ == other.descriptor_ids_;
}

// -----------------------------------------------------------------------------

MitClassConfig::MitClassConfig(const core::record::ClassType id) : id_(id) {}

// -----------------------------------------------------------------------------

MitClassConfig::MitClassConfig(util::BitReader& reader,
                               const bool block_header_flag) {
  id_ = reader.Read<core::record::ClassType>(4);
  if (!block_header_flag) {
    const auto num_descriptors = reader.Read<uint8_t>(5);
    for (size_t i = 0; i < num_descriptors; ++i) {
      descriptor_ids_.emplace_back(reader.Read<core::GenDesc>(7));
    }
  }
}

// -----------------------------------------------------------------------------

void MitClassConfig::Write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint8_t>(id_), 4);
  if (!descriptor_ids_.empty()) {
    writer.WriteBits(descriptor_ids_.size(), 5);
    for (const auto& d : descriptor_ids_) {
      writer.WriteBits(static_cast<uint8_t>(d), 7);
    }
  }
}

// -----------------------------------------------------------------------------

void MitClassConfig::AddDescriptorId(core::GenDesc desc) {
  descriptor_ids_.emplace_back(desc);
}

// -----------------------------------------------------------------------------

core::record::ClassType MitClassConfig::GetClassId() const { return id_; }

// -----------------------------------------------------------------------------

const std::vector<core::GenDesc>& MitClassConfig::GetDescriptorIDs() const {
  return descriptor_ids_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
