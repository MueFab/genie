/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/location/internal.h"

#include <memory>

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference::location {

// -----------------------------------------------------------------------------

Internal::Internal(const uint8_t reserved,
                   const uint8_t internal_dataset_group_id,
                   const uint16_t internal_dataset_id)
    : Location(reserved, false),
      internal_dataset_group_id_(internal_dataset_group_id),
      internal_dataset_id_(internal_dataset_id) {}

// -----------------------------------------------------------------------------

Internal::Internal(util::BitReader& reader) : Location(reader) {
  internal_dataset_group_id_ = reader.ReadAlignedInt<uint8_t>();
  internal_dataset_id_ = reader.ReadAlignedInt<uint16_t>();
}

// -----------------------------------------------------------------------------

Internal::Internal(util::BitReader& reader, const uint8_t reserved)
    : Location(reserved, false) {
  internal_dataset_group_id_ = reader.ReadAlignedInt<uint8_t>();
  internal_dataset_id_ = reader.ReadAlignedInt<uint16_t>();
}

// -----------------------------------------------------------------------------

uint8_t Internal::GetDatasetGroupId() const {
  return internal_dataset_group_id_;
}

// -----------------------------------------------------------------------------

uint16_t Internal::GetDatasetId() const { return internal_dataset_id_; }

// -----------------------------------------------------------------------------

void Internal::Write(util::BitWriter& writer) {
  Location::Write(writer);
  writer.WriteAlignedInt(internal_dataset_group_id_);
  writer.WriteAlignedInt(internal_dataset_id_);
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::meta::RefBase> Internal::decapsulate() {
  auto ret = std::make_unique<core::meta::InternalRef>(
      internal_dataset_group_id_, internal_dataset_id_);
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
