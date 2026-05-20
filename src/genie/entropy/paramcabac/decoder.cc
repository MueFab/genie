/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 * @brief Implementation of CABAC decoders for Genie.
 *
 * Defines `DecoderRegular` and `DecoderTokenType` for managing descriptor
 * subsequence configurations, RLE guards, cloning, serialization, and equality
 * comparisons.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/decoder.h"

#include <memory>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------

DecoderRegular::DecoderRegular()
    : core::parameter::desc_pres::DecoderRegular(kModeCabac) {}

// -----------------------------------------------------------------------------

DecoderRegular::DecoderRegular(core::GenDesc desc)
    : core::parameter::desc_pres::DecoderRegular(kModeCabac) {
  for (size_t i = 0;
       i < core::GetDescriptors()[static_cast<uint8_t>(desc)].sub_seqs.size();
       ++i) {
    descriptor_subsequence_cfgs_.emplace_back(static_cast<uint16_t>(i), false);
  }
}

// -----------------------------------------------------------------------------

DecoderRegular::DecoderRegular(core::GenDesc desc, util::BitReader& reader)
    : core::parameter::desc_pres::DecoderRegular(kModeCabac) {
  const uint8_t num_descriptor_subsequence_cfgs = reader.Read<uint8_t>() + 1;
  for (size_t i = 0; i < num_descriptor_subsequence_cfgs; ++i) {
    descriptor_subsequence_cfgs_.emplace_back(false, desc, reader);
  }
}

// -----------------------------------------------------------------------------

void DecoderRegular::SetSubsequenceCfg(const uint8_t index, Subsequence&& cfg) {
  descriptor_subsequence_cfgs_[index] = cfg;
}

// -----------------------------------------------------------------------------

const Subsequence& DecoderRegular::GetSubsequenceCfg(
    const uint8_t index) const {
  return descriptor_subsequence_cfgs_[index];
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::Decoder> DecoderRegular::Clone()
    const {
  return std::make_unique<DecoderRegular>(*this);
}

// -----------------------------------------------------------------------------

Subsequence& DecoderRegular::GetSubsequenceCfg(const uint8_t index) {
  return descriptor_subsequence_cfgs_[index];
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::DecoderRegular>
DecoderRegular::create(core::GenDesc desc, util::BitReader& reader) {
  return std::make_unique<DecoderRegular>(desc, reader);
}

// -----------------------------------------------------------------------------

void DecoderRegular::Write(util::BitWriter& writer) const {
  Decoder::Write(writer);
  writer.WriteBits(descriptor_subsequence_cfgs_.size() - 1, 8);
  for (auto& i : descriptor_subsequence_cfgs_) {
    i.write(writer);
  }
}

// -----------------------------------------------------------------------------

bool DecoderRegular::Equals(const Decoder* dec) const {
  return Decoder::Equals(dec) &&
         dynamic_cast<const DecoderRegular*>(dec)
                 ->descriptor_subsequence_cfgs_ == descriptor_subsequence_cfgs_;
}

// -----------------------------------------------------------------------------

DecoderTokenType::DecoderTokenType()
    : core::parameter::desc_pres::DecoderTokenType(kModeCabac),
      rle_guard_tokentype_(0) {
  for (uint16_t i = 0; i < 2; ++i) {
    descriptor_subsequence_cfgs_.emplace_back(i, true);
  }
}

// -----------------------------------------------------------------------------

DecoderTokenType::DecoderTokenType(core::GenDesc desc, util::BitReader& reader)
    : core::parameter::desc_pres::DecoderTokenType(kModeCabac) {
  constexpr uint8_t num_descriptor_subsequence_cfgs = 2;
  rle_guard_tokentype_ = reader.Read<uint8_t>();
  for (size_t i = 0; i < num_descriptor_subsequence_cfgs; ++i) {
    descriptor_subsequence_cfgs_.emplace_back(true, desc, reader);
  }
}

// -----------------------------------------------------------------------------

void DecoderTokenType::SetSubsequenceCfg(const uint8_t index,
                                         Subsequence&& cfg) {
  descriptor_subsequence_cfgs_[index] = cfg;
}

// -----------------------------------------------------------------------------

const Subsequence& DecoderTokenType::GetSubsequenceCfg(
    const uint8_t index) const {
  return descriptor_subsequence_cfgs_[index];
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::Decoder> DecoderTokenType::Clone()
    const {
  return std::make_unique<DecoderTokenType>(*this);
}

// -----------------------------------------------------------------------------

Subsequence& DecoderTokenType::GetSubsequenceCfg(const uint8_t index) {
  return descriptor_subsequence_cfgs_[index];
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::DecoderTokenType>
DecoderTokenType::create(core::GenDesc desc, util::BitReader& reader) {
  return std::make_unique<DecoderTokenType>(desc, reader);
}

// -----------------------------------------------------------------------------

void DecoderTokenType::Write(util::BitWriter& writer) const {
  Decoder::Write(writer);
  writer.WriteBits(rle_guard_tokentype_, 8);
  for (auto& i : descriptor_subsequence_cfgs_) {
    i.write(writer);
  }
}

// -----------------------------------------------------------------------------

uint8_t DecoderTokenType::GetRleGuardTokentype() const {
  return rle_guard_tokentype_;
}

// -----------------------------------------------------------------------------

bool DecoderTokenType::Equals(const Decoder* dec) const {
  return Decoder::Equals(dec) &&
         dynamic_cast<const DecoderTokenType*>(dec)->rle_guard_tokentype_ ==
             rle_guard_tokentype_ &&
         dynamic_cast<const DecoderTokenType*>(dec)->rle_guard_tokentype_ ==
             rle_guard_tokentype_ &&
         dynamic_cast<const DecoderTokenType*>(dec)
                 ->descriptor_subsequence_cfgs_ == descriptor_subsequence_cfgs_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------