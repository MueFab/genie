/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/signature_cfg.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------

bool SignatureCfg::operator==(const SignatureCfg& other) const {
  return u_cluster_signature_ == other.u_cluster_signature_ &&
         u_cluster_signature_length_ == other.u_cluster_signature_length_ &&
         u_signature_size_ == other.u_signature_size_;
}

// -----------------------------------------------------------------------------

SignatureCfg::SignatureCfg(util::BitReader& reader, uint8_t u_signature_size,
                           const uint8_t base_bits)
    : u_signature_size_(u_signature_size
                            ? std::optional(u_signature_size)
                            : std::optional<uint8_t>(std::nullopt)),
      base_bits_(base_bits) {
  const auto num_signatures = reader.Read<uint16_t>();
  for (uint16_t i = 0; i < num_signatures; ++i) {
    size_t len = 0;
    if (u_signature_size_ != std::nullopt) {
      len = *u_signature_size_;
    } else {
      len = reader.Read<uint8_t>(8);
      u_cluster_signature_length_.emplace_back(static_cast<uint8_t>(len));
    }
    u_cluster_signature_.emplace_back(
        reader.Read<uint64_t>(static_cast<uint8_t>(base_bits_ * len)));
  }
}

// -----------------------------------------------------------------------------

void SignatureCfg::AddSignature(uint64_t u_cluster_signature, uint8_t length) {
  if (u_cluster_signature_.empty()) {
    u_signature_size_ = length;
  } else {
    if (length != u_signature_size_) {
      u_signature_size_ = std::nullopt;
    }
  }
  u_cluster_signature_length_.emplace_back(length);
  u_cluster_signature_.emplace_back(u_cluster_signature);
}

// -----------------------------------------------------------------------------

void SignatureCfg::Write(util::BitWriter& writer) const {
  writer.WriteBits(u_cluster_signature_.size(), 16);
  for (size_t i = 0; i < u_cluster_signature_.size(); ++i) {
    if (u_signature_size_ != std::nullopt) {
      writer.WriteBits(u_cluster_signature_[i],
                       base_bits_ * *u_signature_size_);
    } else {
      writer.WriteBits(u_cluster_signature_[i],
                       base_bits_ * u_cluster_signature_length_[i]);
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
