/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference_metadata.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
bool ReferenceMetadata::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const ReferenceMetadata&>(info);
  return dataset_group_id_ == other.dataset_group_id_ &&
         reference_id_ == other.reference_id_ &&
         reference_metadata_value_ == other.reference_metadata_value_;
}

// -----------------------------------------------------------------------------
const std::string& ReferenceMetadata::GetKey() const {
  static const std::string key = "rfmd";
  return key;
}

// -----------------------------------------------------------------------------
ReferenceMetadata::ReferenceMetadata(const uint8_t dataset_group_id,
                                     const uint8_t reference_id,
                                     std::string reference_metadata_value)
    : dataset_group_id_(dataset_group_id),
      reference_id_(reference_id),
      reference_metadata_value_(std::move(reference_metadata_value)) {}

// -----------------------------------------------------------------------------
ReferenceMetadata::ReferenceMetadata(util::BitReader& bitreader) {
  const auto start_pos = bitreader.GetStreamPosition() - 4;
  const auto length = bitreader.ReadAlignedInt<uint64_t>();
  const auto len_value = length - sizeof(uint8_t) * 2;
  reference_metadata_value_.resize(len_value);
  dataset_group_id_ = bitreader.ReadAlignedInt<uint8_t>();
  reference_id_ = bitreader.ReadAlignedInt<uint8_t>();
  bitreader.ReadAlignedBytes(reference_metadata_value_.data(),
                             reference_metadata_value_.length());
  UTILS_DIE_IF(start_pos + length != bitreader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------
void ReferenceMetadata::BoxWrite(util::BitWriter& bit_writer) const {
  bit_writer.WriteAlignedInt<uint8_t>(dataset_group_id_);
  bit_writer.WriteAlignedInt<uint8_t>(reference_id_);
  bit_writer.WriteAlignedBytes(reference_metadata_value_.data(),
                              reference_metadata_value_.length());
}

// -----------------------------------------------------------------------------
uint8_t ReferenceMetadata::GetDatasetGroupId() const {
  return dataset_group_id_;
}

// -----------------------------------------------------------------------------
uint8_t ReferenceMetadata::GetReferenceId() const { return reference_id_; }

// -----------------------------------------------------------------------------
const std::string& ReferenceMetadata::GetReferenceMetadataValue() const {
  return reference_metadata_value_;
}

// -----------------------------------------------------------------------------
void ReferenceMetadata::PatchId(const uint8_t group_id) {
  dataset_group_id_ = group_id;
}

// -----------------------------------------------------------------------------
void ReferenceMetadata::PatchRefId(const uint8_t old, const uint8_t _new) {
  if (reference_id_ == old) {
    reference_id_ = _new;
  }
}

// -----------------------------------------------------------------------------
std::string ReferenceMetadata::decapsulate() {
  return std::move(reference_metadata_value_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
