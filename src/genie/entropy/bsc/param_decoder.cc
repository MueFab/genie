/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/bsc/param_decoder.h"

#include <memory>

// -----------------------------------------------------------------------------

namespace genie::entropy::bsc {

// -----------------------------------------------------------------------------

DecoderRegular::DecoderRegular()
    : core::parameter::desc_pres::DecoderRegular(mode_bsc_) {}

// -----------------------------------------------------------------------------

DecoderRegular::DecoderRegular(core::GenDesc desc)
    : core::parameter::desc_pres::DecoderRegular(mode_bsc_) {
  for (size_t i = 0;
       i < core::GetDescriptors()[static_cast<uint8_t>(desc)].sub_seqs.size();
       ++i) {
    auto bits_p2 = core::Range2Bytes(GetDescriptor(desc).sub_seqs[i].range);
    descriptor_subsequence_cfgs_.emplace_back(bits_p2);
  }
}

// -----------------------------------------------------------------------------

DecoderRegular::DecoderRegular(core::GenDesc, util::BitReader& reader)
    : core::parameter::desc_pres::DecoderRegular(mode_bsc_) {
  const uint8_t num_descriptor_subsequence_configs = reader.Read<uint8_t>() + 1;
  for (size_t i = 0; i < num_descriptor_subsequence_configs; ++i) {
    descriptor_subsequence_cfgs_.emplace_back(reader.Read<uint8_t>(6));
  }
}

// -----------------------------------------------------------------------------

void DecoderRegular::SetSubsequenceCfg(const uint8_t index, Subsequence&& cfg) {
  descriptor_subsequence_cfgs_[index] = cfg;
}

// -----------------------------------------------------------------------------

[[maybe_unused]] const Subsequence& DecoderRegular::GetSubsequenceCfg(
    const uint8_t index) const {
  return descriptor_subsequence_cfgs_[index];
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::Decoder> DecoderRegular::Clone()
    const {
  return std::make_unique<DecoderRegular>(*this);
}

// -----------------------------------------------------------------------------

[[maybe_unused]] Subsequence& DecoderRegular::GetSubsequenceCfg(
    const uint8_t index) {
  return descriptor_subsequence_cfgs_[index];
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::DecoderRegular>
DecoderRegular::Create(core::GenDesc desc, util::BitReader& reader) {
  return std::make_unique<DecoderRegular>(desc, reader);
}

// -----------------------------------------------------------------------------

void DecoderRegular::Write(util::BitWriter& writer) const {
  Decoder::Write(writer);
  writer.WriteBits(descriptor_subsequence_cfgs_.size() - 1, 8);
  for (auto& i : descriptor_subsequence_cfgs_) {
    i.Write(writer);
  }
}

// -----------------------------------------------------------------------------

bool DecoderRegular::Equals(const Decoder* dec) const {
  return Decoder::Equals(dec) &&
         dynamic_cast<const DecoderRegular*>(dec)
                 ->descriptor_subsequence_cfgs_ == descriptor_subsequence_cfgs_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
