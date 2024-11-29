/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/block_header.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
BlockHeader::BlockHeader(util::BitReader& reader) {
  reserved1_ = reader.Read<bool>(1);
  descriptor_id_ = reader.Read<core::GenDesc>(7);
  reserved2_ = reader.Read<uint8_t>(3);
  block_payload_size_ = reader.Read<uint32_t>(29);
}

// -----------------------------------------------------------------------------
core::GenDesc BlockHeader::GetDescriptorId() const { return descriptor_id_; }

// -----------------------------------------------------------------------------
uint32_t BlockHeader::GetPayloadSize() const { return block_payload_size_; }

// -----------------------------------------------------------------------------
uint64_t BlockHeader::GetLength() { return 1 + 4; }  /// 1 + 7 + 3 + 29}

// -----------------------------------------------------------------------------
void BlockHeader::Write(util::BitWriter& writer) const {
  writer.WriteBits(reserved1_, 1);
  writer.WriteBits(static_cast<uint8_t>(descriptor_id_), 7);
  writer.WriteBits(reserved2_, 3);
  writer.WriteBits(block_payload_size_, 29);
}

// -----------------------------------------------------------------------------
BlockHeader::BlockHeader(const bool reserved1, const core::GenDesc desc_id,
                         const uint8_t reserved2, const uint32_t payload_size)
    : reserved1_(reserved1),
      descriptor_id_(desc_id),
      reserved2_(reserved2),
      block_payload_size_(payload_size) {}

// -----------------------------------------------------------------------------
bool BlockHeader::GetReserved1() const { return reserved1_; }

// -----------------------------------------------------------------------------
uint8_t BlockHeader::GetReserved2() const { return reserved2_; }

// -----------------------------------------------------------------------------
bool BlockHeader::operator==(const BlockHeader& other) const {
  return reserved1_ == other.reserved1_ &&
         descriptor_id_ == other.descriptor_id_ &&
         reserved2_ == other.reserved2_ &&
         block_payload_size_ == other.block_payload_size_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
