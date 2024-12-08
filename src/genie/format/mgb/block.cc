/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/block.h"

#include <memory>
#include <sstream>
#include <utility>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
Block::Block(const uint8_t descriptor_id,
             core::AccessUnit::Descriptor&& payload)
    : descriptor_id_(descriptor_id),
      block_payload_size_(0),
      count_(0),
      payload_(std::move(payload)) {
  count_ = static_cast<uint8_t>(
      std::get<core::AccessUnit::Descriptor>(payload_).GetSize());
}

// -----------------------------------------------------------------------------
Block::Block()
    : descriptor_id_(0),
      block_payload_size_(0),
      count_(0),
      payload_(core::AccessUnit::Descriptor(static_cast<core::GenDesc>(0))) {}

// -----------------------------------------------------------------------------
Block::Block(const size_t qv_count, util::BitReader& reader)
    : payload_(core::AccessUnit::Descriptor()) {
  reader.ReadBits(1);
  descriptor_id_ = reader.Read<uint8_t>(7);
  reader.ReadBits(3);
  block_payload_size_ = reader.Read<uint32_t>(29);

  /*   for(size_t i = 0; i < block_payload_size; ++i) {
         reader.read(8);
     } */

  count_ = static_cast<uint8_t>(
      static_cast<core::GenDesc>(descriptor_id_) == core::GenDesc::kQv
          ? qv_count
          : GetDescriptor(static_cast<core::GenDesc>(descriptor_id_))
                .sub_seqs.size());
  // payload = core::AccessUnit::Descriptor(core::GenDesc(descriptor_ID), count,
  // block_payload_size, reader);
  payload_ = core::Payload(reader, block_payload_size_);

  reader.FlushHeldBits();
}

// -----------------------------------------------------------------------------
void Block::Write(util::BitWriter& writer) const {
  writer.WriteBits(0, 1);
  writer.WriteBits(descriptor_id_, 7);
  writer.WriteBits(0, 3);

  if (std::holds_alternative<core::Payload>(payload_)) {
    writer.WriteBits(std::get<core::Payload>(payload_).GetPayloadSize(), 29);
    std::get<core::Payload>(payload_).Write(writer);
  } else {
    writer.WriteBits(
        std::get<core::AccessUnit::Descriptor>(payload_).GetWrittenSize(), 29);
    std::get<core::AccessUnit::Descriptor>(payload_).Write(writer);
  }
  writer.FlushBits();
}

// -----------------------------------------------------------------------------
core::AccessUnit::Descriptor&& Block::MovePayload() {
  return std::move(std::get<core::AccessUnit::Descriptor>(payload_));
}

// -----------------------------------------------------------------------------
uint8_t Block::GetDescriptorId() const { return descriptor_id_; }

// -----------------------------------------------------------------------------
size_t Block::GetWrittenSize() const {
  if (std::holds_alternative<core::Payload>(payload_)) {
    return std::get<core::Payload>(payload_).GetPayloadSize() +
           sizeof(uint32_t) + sizeof(uint8_t);
  }
  return std::get<core::AccessUnit::Descriptor>(payload_).GetWrittenSize() +
         sizeof(uint32_t) + sizeof(uint8_t);
}

// -----------------------------------------------------------------------------
bool Block::IsLoaded() const {
  return (std::holds_alternative<core::Payload>(payload_) &&
          std::get<core::Payload>(payload_).IsPayloadLoaded()) ||
         std::holds_alternative<core::AccessUnit::Descriptor>(payload_);
}

// -----------------------------------------------------------------------------
bool Block::IsParsed() const {
  return std::holds_alternative<core::AccessUnit::Descriptor>(payload_);
}

// -----------------------------------------------------------------------------
void Block::load() {
  if (IsLoaded()) {
    return;
  }
  std::get<core::Payload>(payload_).LoadPayload();
}

// -----------------------------------------------------------------------------
void Block::unload() {
  if (!IsLoaded()) {
    return;
  }
  if (!IsParsed()) {
    std::get<core::Payload>(payload_).UnloadPayload();
  }
}

// -----------------------------------------------------------------------------
void Block::parse() {
  if (!IsLoaded()) {
    load();
  }
  std::stringstream ss;

  ss.write(static_cast<const char*>(
               std::get<core::Payload>(payload_).GetPayload().GetData()),
           static_cast<std::streamsize>(
               std::get<core::Payload>(payload_).GetPayload().GetRawSize()));

  util::BitReader reader(ss);

  payload_ =
      core::AccessUnit::Descriptor(static_cast<core::GenDesc>(descriptor_id_),
                                   count_, block_payload_size_, reader);
}

// -----------------------------------------------------------------------------
void Block::pack() {
  if (!IsParsed()) {
    return;
  }
  std::stringstream ss;
  util::BitWriter writer(ss);

  std::get<core::AccessUnit::Descriptor>(payload_).Write(writer);

  util::BitReader reader(ss);

  payload_ = core::Payload(reader, ss.str().length());
}

// -----------------------------------------------------------------------------
Block::Block(core::GenDesc descriptor_id, core::Payload payload)
    : descriptor_id_(static_cast<uint8_t>(descriptor_id)),
      block_payload_size_(static_cast<uint32_t>(payload.GetPayloadSize())),
      count_(0),
      payload_(std::move(payload)) {}

// -----------------------------------------------------------------------------
core::Payload Block::MovePayloadUnparsed() {
  return std::move(std::get<core::Payload>(payload_));
}

// -----------------------------------------------------------------------------
core::Payload& Block::GetPayloadUnparsed() {
  return std::get<core::Payload>(payload_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
