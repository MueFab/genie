/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/payload.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

util::DataBlock Payload::InternalLoadPayload(util::BitReader& reader) const {
  const auto pos = reader.GetStreamPosition();
  reader.SetStreamPosition(payload_position_);
  util::DataBlock tmp;
  tmp.Resize(payload_size_);
  reader.ReadAlignedBytes(tmp.GetData(), payload_size_);
  reader.SetStreamPosition(pos);
  return tmp;
}

// -----------------------------------------------------------------------------

uint64_t Payload::GetPayloadSize() const {
  if (IsPayloadLoaded()) {
    return block_payload_.GetRawSize();
  }
  return payload_size_;
}

// -----------------------------------------------------------------------------

void Payload::LoadPayload() {
  block_payload_ = InternalLoadPayload(*internal_reader_);
  payload_loaded_ = true;
}

// -----------------------------------------------------------------------------

void Payload::UnloadPayload() {
  payload_loaded_ = false;
  block_payload_.Clear();
}

// -----------------------------------------------------------------------------

bool Payload::IsPayloadLoaded() const { return payload_loaded_; }

// -----------------------------------------------------------------------------

const util::DataBlock& Payload::GetPayload() const { return block_payload_; }

// -----------------------------------------------------------------------------

util::DataBlock&& Payload::MovePayload() {
  payload_loaded_ = false;
  return std::move(block_payload_);
}

// -----------------------------------------------------------------------------

Payload::Payload(util::DataBlock payload)
    : payload_loaded_(true),
      payload_position_(0),
      payload_size_(payload.GetRawSize()),
      internal_reader_(nullptr) {
  block_payload_ = std::move(payload);
}

// -----------------------------------------------------------------------------

Payload::Payload(util::BitReader& reader, const uint64_t size)
    : payload_loaded_(false),
      payload_position_(reader.GetStreamPosition()),
      payload_size_(size),
      internal_reader_(&reader) {
  reader.SkipAlignedBytes(size);
}

// -----------------------------------------------------------------------------

void Payload::Write(util::BitWriter& writer) const {
  if (!IsPayloadLoaded() && internal_reader_) {
    auto tmp = InternalLoadPayload(*internal_reader_);
    writer.WriteAlignedBytes(tmp.GetData(), tmp.GetRawSize());
  } else {
    writer.WriteAlignedBytes(block_payload_.GetData(),
                             block_payload_.GetRawSize());
  }
}

// -----------------------------------------------------------------------------

bool Payload::operator==(const Payload& other) const {
  return payload_size_ == other.payload_size_ &&
         payload_position_ == other.payload_position_ &&
         payload_loaded_ == other.payload_loaded_ &&
         block_payload_ == other.block_payload_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
