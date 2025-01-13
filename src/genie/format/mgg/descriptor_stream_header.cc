/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/descriptor_stream_header.h"

#include <string>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

bool DescriptorStreamHeader::GetReserved() const { return reserved_; }

// -----------------------------------------------------------------------------

core::GenDesc DescriptorStreamHeader::GetDescriptorId() const {
  return descriptor_id_;
}

// -----------------------------------------------------------------------------

core::record::ClassType DescriptorStreamHeader::GetClassType() const {
  return class_id_;
}

// -----------------------------------------------------------------------------

uint32_t DescriptorStreamHeader::GetNumBlocks() const { return num_blocks_; }

// -----------------------------------------------------------------------------

void DescriptorStreamHeader::AddBlock() { num_blocks_++; }

// -----------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader()
    : DescriptorStreamHeader(false, static_cast<core::GenDesc>(0),
                             core::record::ClassType::kNone, 0) {}

// -----------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader(
    const bool reserved, const core::GenDesc gen_desc,
    const core::record::ClassType class_id, const uint32_t num_blocks)
    : reserved_(reserved),
      descriptor_id_(gen_desc),
      class_id_(class_id),
      num_blocks_(num_blocks) {}

// -----------------------------------------------------------------------------

void DescriptorStreamHeader::BoxWrite(util::BitWriter& bit_writer) const {
  bit_writer.WriteBits(reserved_, 1);
  bit_writer.WriteBits(static_cast<uint8_t>(descriptor_id_), 7);
  bit_writer.WriteBits(static_cast<uint8_t>(class_id_), 4);
  bit_writer.WriteBits(num_blocks_, 32);
  bit_writer.FlushBits();
}

// -----------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader(util::BitReader& reader) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  reserved_ = reader.Read<bool>(1);
  descriptor_id_ = reader.Read<core::GenDesc>(7);
  class_id_ = reader.Read<core::record::ClassType>(4);
  num_blocks_ = reader.Read<uint32_t>(32);
  reader.FlushHeldBits();
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

const std::string& DescriptorStreamHeader::GetKey() const {
  static const std::string key = "dshd";  // NOLINT
  return key;
}

// -----------------------------------------------------------------------------

bool DescriptorStreamHeader::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DescriptorStreamHeader&>(info);
  return reserved_ == other.reserved_ &&
         descriptor_id_ == other.descriptor_id_ &&
         class_id_ == other.class_id_ && num_blocks_ == other.num_blocks_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
