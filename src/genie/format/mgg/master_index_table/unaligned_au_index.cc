/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/master_index_table/unaligned_au_index.h"

#include <vector>

#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::master_index_table {

// -----------------------------------------------------------------------------

bool UnalignedAuIndex::operator==(const UnalignedAuIndex& other) const {
  return au_byte_offset_ == other.au_byte_offset_ &&
         ref_cfg_ == other.ref_cfg_ && sig_cfg_ == other.sig_cfg_ &&
         block_byte_offset_ == other.block_byte_offset_ &&
         byte_offset_size_ == other.byte_offset_size_ &&
         position_size_ == other.position_size_ &&
         signature_size_ == other.signature_size_;
}

// -----------------------------------------------------------------------------

const std::vector<uint64_t>& UnalignedAuIndex::GetBlockOffsets() const {
  return block_byte_offset_;
}

// -----------------------------------------------------------------------------

UnalignedAuIndex::UnalignedAuIndex(
    util::BitReader& reader, const uint8_t byte_offset_size,
    const uint8_t position_size,
    const core::parameter::DataUnit::DatasetType dataset_type,
    const bool signature_flag, const bool signature_const_flag,
    const uint8_t signature_size, const bool block_header_flag,
    const std::vector<core::GenDesc>& descriptors,
    const core::AlphabetId alphabet)
    : byte_offset_size_(byte_offset_size),
      position_size_(position_size),
      signature_size_(signature_size) {
  au_byte_offset_ = reader.Read<uint64_t>(byte_offset_size_);
  if (dataset_type == core::parameter::DataUnit::DatasetType::kReference) {
    ref_cfg_ = mgb::RefCfg(position_size_, reader);
  } else if (signature_flag) {
    sig_cfg_ =
        mgb::SignatureCfg(reader, signature_const_flag ? signature_size_ : 0,
                          GetAlphabetProperties(alphabet).base_bits);
    reader.FlushHeldBits();
  }
  if (!block_header_flag) {
    for (const auto& d : descriptors) {
      (void)d;
      block_byte_offset_.emplace_back(reader.Read<uint64_t>(byte_offset_size_));
    }
  }
}

// -----------------------------------------------------------------------------

void UnalignedAuIndex::Write(util::BitWriter& writer) const {
  writer.WriteBits(au_byte_offset_, byte_offset_size_);
  if (ref_cfg_ != std::nullopt) {
    ref_cfg_->Write(writer);
  }
  if (sig_cfg_ != std::nullopt) {
    sig_cfg_->Write(writer);
    writer.FlushBits();
  }
  for (const auto& b : block_byte_offset_) {
    writer.WriteBits(b, byte_offset_size_);
  }
}

// -----------------------------------------------------------------------------

uint64_t UnalignedAuIndex::GetAuOffset() const { return au_byte_offset_; }

// -----------------------------------------------------------------------------

void UnalignedAuIndex::AddBlockOffset(uint64_t offset) {
  UTILS_DIE_IF(
      !block_byte_offset_.empty() && block_byte_offset_.back() > offset,
      "Blocks unordered");
  block_byte_offset_.emplace_back(offset);
}

// -----------------------------------------------------------------------------

UnalignedAuIndex::UnalignedAuIndex(const uint64_t au_byte_offset,
                                   const uint8_t byte_offset_size,
                                   const int8_t position_size,
                                   const int8_t signature_size)
    : au_byte_offset_(au_byte_offset),
      byte_offset_size_(byte_offset_size),
      position_size_(position_size),
      signature_size_(signature_size) {}

// -----------------------------------------------------------------------------

bool UnalignedAuIndex::IsReference() const { return ref_cfg_ != std::nullopt; }

// -----------------------------------------------------------------------------

const mgb::RefCfg& UnalignedAuIndex::GetReferenceInfo() const {
  return *ref_cfg_;
}

// -----------------------------------------------------------------------------

void UnalignedAuIndex::SetReferenceInfo(const mgb::RefCfg& ref_cfg) {
  ref_cfg_ = ref_cfg;
}

// -----------------------------------------------------------------------------

bool UnalignedAuIndex::HasSignature() const { return sig_cfg_ != std::nullopt; }

// -----------------------------------------------------------------------------

const mgb::SignatureCfg& UnalignedAuIndex::GetSignatureInfo() const {
  return *sig_cfg_;
}

// -----------------------------------------------------------------------------

void UnalignedAuIndex::SetSignatureInfo(
    const mgb::SignatureCfg& signature_cfg) {
  sig_cfg_ = signature_cfg;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::master_index_table

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
