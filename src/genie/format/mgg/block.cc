/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/block.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

Block::Block(util::BitReader& reader)
    : header_(reader), payload_(reader, header_.GetPayloadSize()) {}

// -----------------------------------------------------------------------------

uint32_t Block::GetPayloadSize() const { return header_.GetPayloadSize(); }

// -----------------------------------------------------------------------------

uint64_t Block::GetLength() const {
  const uint64_t len = BlockHeader::GetLength() + header_.GetPayloadSize();

  /// block_payload[] u(8)
  //    len += block_payload.Size() * sizeof(uint8_t);

  return len;
}

// -----------------------------------------------------------------------------

void Block::Write(util::BitWriter& writer) const {
  header_.Write(writer);
  payload_.Write(writer);
}

// -----------------------------------------------------------------------------

bool Block::operator==(const Block& other) const {
  return header_ == other.header_ &&
         payload_.GetPayload() == other.GetPayload();
}

// -----------------------------------------------------------------------------

Block::Block(const core::GenDesc desc_id, util::DataBlock payload)
    : header_(false, desc_id, 0, static_cast<uint32_t>(payload.GetRawSize())),
      payload_(std::move(payload)) {}

// -----------------------------------------------------------------------------

const util::DataBlock& Block::GetPayload() const {
  return payload_.GetPayload();
}

// -----------------------------------------------------------------------------

core::GenDesc Block::GetDescId() const { return header_.GetDescriptorId(); }

// -----------------------------------------------------------------------------

void Block::PrintDebug(std::ostream& output, const uint8_t depth,
                        const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Block");
  print_offset(
      output, depth + 1, max_depth,
      "Block descriptor ID: " + GetDescriptor(header_.GetDescriptorId()).name);
  print_offset(
      output, depth + 1, max_depth,
      "Block payload Size: " + std::to_string(header_.GetPayloadSize()));
}

// -----------------------------------------------------------------------------

Block::Block(mgb::Block b)
    : header_(false, static_cast<core::GenDesc>(b.GetDescriptorId()), 0,
              static_cast<uint32_t>(b.GetPayloadUnparsed().GetPayloadSize())),
      payload_(std::move(b.GetPayloadUnparsed())) {}

// -----------------------------------------------------------------------------

core::Payload Block::MovePayload() { return std::move(payload_); }

// -----------------------------------------------------------------------------

mgb::Block Block::decapsulate() {
  return {header_.GetDescriptorId(), MovePayload()};
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
