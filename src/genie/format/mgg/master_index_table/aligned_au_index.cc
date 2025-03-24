/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/master_index_table/aligned_au_index.h"

#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::master_index_table {

// -----------------------------------------------------------------------------

bool AlignedAuIndex::operator==(const AlignedAuIndex& other) const {
  return au_byte_offset_ == other.au_byte_offset_ &&
         au_start_position_ == other.au_start_position_ &&
         au_end_position_ == other.au_end_position_ &&
         ref_cfg_ == other.ref_cfg_ && extended_cfg_ == other.extended_cfg_ &&
         block_byte_offset_ == other.block_byte_offset_ &&
         byte_offset_size_ == other.byte_offset_size_ &&
         position_size_ == other.position_size_;
}

// -----------------------------------------------------------------------------

AlignedAuIndex::AlignedAuIndex(const uint64_t au_byte_offset,
                               const uint64_t au_start_position,
                               const uint64_t au_end_position,
                               const uint8_t byte_offset_size,
                               const uint8_t position_size)
    : au_byte_offset_(au_byte_offset),
      au_start_position_(au_start_position),
      au_end_position_(au_end_position),
      byte_offset_size_(byte_offset_size),
      position_size_(position_size) {}

// -----------------------------------------------------------------------------

AlignedAuIndex::AlignedAuIndex(
    util::BitReader& reader, const uint8_t byte_offset_size,
    const uint8_t position_size,
    const core::parameter::DataUnit::DatasetType dataset_type,
    const bool multiple_alignment, const bool block_header_flag,
    const std::vector<core::GenDesc>& descriptors)
    : byte_offset_size_(byte_offset_size), position_size_(position_size) {
  au_byte_offset_ = reader.Read<uint64_t>(byte_offset_size_);
  au_start_position_ = reader.Read<uint64_t>(position_size_);
  au_end_position_ = reader.Read<uint64_t>(position_size_);
  if (dataset_type == core::parameter::DataUnit::DatasetType::kReference) {
    ref_cfg_ = mgb::RefCfg(position_size_, reader);
  }
  if (multiple_alignment) {
    extended_cfg_ = mgb::ExtendedAu(position_size_, reader);
  }
  if (!block_header_flag) {
    for (const auto& d : descriptors) {
      (void)d;
      block_byte_offset_.emplace_back(reader.Read<uint64_t>(byte_offset_size_));
    }
  }
}

// -----------------------------------------------------------------------------

void AlignedAuIndex::Write(util::BitWriter& writer) const {
  writer.WriteBits(au_byte_offset_, byte_offset_size_);
  writer.WriteBits(au_start_position_, position_size_);
  writer.WriteBits(au_end_position_, position_size_);
  if (ref_cfg_ != std::nullopt) {
    ref_cfg_->Write(writer);
  }
  if (extended_cfg_ != std::nullopt) {
    extended_cfg_->Write(writer);
  }
  for (const auto& b : block_byte_offset_) {
    writer.WriteBits(b, byte_offset_size_);
  }
}

// -----------------------------------------------------------------------------

uint64_t AlignedAuIndex::GetByteOffset() const { return au_byte_offset_; }

// -----------------------------------------------------------------------------

uint64_t AlignedAuIndex::GetAuStartPos() const { return au_start_position_; }

// -----------------------------------------------------------------------------

uint64_t AlignedAuIndex::GetAuEndPos() const { return au_end_position_; }

// -----------------------------------------------------------------------------

bool AlignedAuIndex::IsReference() const { return ref_cfg_ != std::nullopt; }

// -----------------------------------------------------------------------------

const mgb::RefCfg& AlignedAuIndex::GetReferenceInfo() const {
  return *ref_cfg_;
}

// -----------------------------------------------------------------------------

void AlignedAuIndex::SetReferenceInfo(const mgb::RefCfg& ref_cfg) {
  ref_cfg_ = ref_cfg;
}

// -----------------------------------------------------------------------------

void AlignedAuIndex::SetExtended(const mgb::ExtendedAu& ext_au) {
  extended_cfg_ = ext_au;
}

// -----------------------------------------------------------------------------

const mgb::ExtendedAu& AlignedAuIndex::GetExtension() const {
  return *extended_cfg_;
}

// -----------------------------------------------------------------------------

bool AlignedAuIndex::IsExtended() const {
  return extended_cfg_ != std::nullopt;
}

// -----------------------------------------------------------------------------

const std::vector<uint64_t>& AlignedAuIndex::GetBlockOffsets() const {
  return block_byte_offset_;
}

// -----------------------------------------------------------------------------

void AlignedAuIndex::AddBlockOffset(uint64_t offset) {
  UTILS_DIE_IF(
      !block_byte_offset_.empty() && block_byte_offset_.back() > offset,
      "Blocks unordered");
  block_byte_offset_.emplace_back(offset);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::master_index_table

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
