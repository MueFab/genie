/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/gabac_seq_conf_set.h"

#include <memory>
#include <utility>

#include "genie/core/parameter/descriptor_present/descriptor_present.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

GabacSeqConfSet::GabacSeqConfSet() {
  // One configuration per subsequence
  for (const auto& desc : core::GetDescriptors()) {
    conf_.emplace_back();
    const core::GenomicDescriptorProperties& desc_prop = GetDescriptor(desc.id);
    for (const auto& subseq : desc_prop.sub_seqs) {
      conf_.back().emplace_back(GetEncoderConfigManual(subseq.id));
    }
  }
}

// -----------------------------------------------------------------------------

const EncodingConfiguration& GabacSeqConfSet::GetConfAsGabac(
    core::GenSubIndex sub) const {
  return conf_[static_cast<uint8_t>(sub.first)]
              [static_cast<uint8_t>(sub.second)];
}

// -----------------------------------------------------------------------------

EncodingConfiguration& GabacSeqConfSet::GetConfAsGabac(core::GenSubIndex sub) {
  if (GetDescriptor(sub.first).token_type) {
    return conf_[static_cast<uint8_t>(sub.first)][0];
  }
  return conf_[static_cast<uint8_t>(sub.first)]
              [static_cast<uint8_t>(sub.second)];
}

// -----------------------------------------------------------------------------

void GabacSeqConfSet::SetConfAsGabac(core::GenSubIndex sub,
                                     DescriptorSubsequenceCfg&& subseq_cfg) {
  conf_[static_cast<uint8_t>(sub.first)][static_cast<uint8_t>(sub.second)]
      .SetSubSeqConfig(std::move(subseq_cfg));
}

// -----------------------------------------------------------------------------

[[maybe_unused]] void GabacSeqConfSet::StoreParameters(
    ParameterSet& parameter_set) const {
  for (const auto& desc : core::GetDescriptors()) {
    auto descriptor_configuration =
        std::make_unique<core::parameter::desc_pres::DescriptorPresent>();

    if (desc.id == core::GenDesc::kReadName ||
        desc.id == core::GenDesc::kMultiSegmentAlignment) {
      auto decoder_config = std::make_unique<paramcabac::DecoderTokenType>();
      FillDecoder(desc, *decoder_config);
      descriptor_configuration->SetDecoder(std::move(decoder_config));
    } else {
      auto decoder_config =
          std::make_unique<paramcabac::DecoderRegular>(desc.id);
      FillDecoder(desc, *decoder_config);
      descriptor_configuration->SetDecoder(std::move(decoder_config));
    }

    auto descriptor_container = core::parameter::DescriptorSubSequenceCfg();
    descriptor_container.Set(std::move(descriptor_configuration));

    parameter_set.GetEncodingSet().SetDescriptor(
        desc.id, std::move(descriptor_container));
  }
}

// -----------------------------------------------------------------------------

void GabacSeqConfSet::StoreParameters(
    core::GenDesc desc,
    core::parameter::DescriptorSubSequenceCfg& parameter_set) const {
  auto descriptor_configuration =
      std::make_unique<core::parameter::desc_pres::DescriptorPresent>();

  if (desc == core::GenDesc::kReadName ||
      desc == core::GenDesc::kMultiSegmentAlignment) {
    auto decoder_config = std::make_unique<paramcabac::DecoderTokenType>();
    FillDecoder(GetDescriptor(desc), *decoder_config);
    descriptor_configuration->SetDecoder(std::move(decoder_config));
  } else {
    auto decoder_config = std::make_unique<paramcabac::DecoderRegular>(desc);
    FillDecoder(GetDescriptor(desc), *decoder_config);
    descriptor_configuration->SetDecoder(std::move(decoder_config));
  }

  parameter_set = core::parameter::DescriptorSubSequenceCfg();
  parameter_set.Set(std::move(descriptor_configuration));
}

// -----------------------------------------------------------------------------

[[maybe_unused]] void GabacSeqConfSet::LoadParameters(
    const ParameterSet& parameter_set) {
  for (const auto& desc : core::GetDescriptors()) {
    if (GetDescriptor(desc.id).token_type) {
      auto& desc_config =
          LoadDescriptorDecoderCfg<paramcabac::DecoderTokenType>(parameter_set,
                                                                 desc.id);
      for (const auto& subdesc : GetDescriptor(desc.id).sub_seqs) {
        auto subseq_cfg = desc_config.GetSubsequenceCfg(
            static_cast<uint8_t>(subdesc.id.second));

        SetConfAsGabac(subdesc.id, std::move(subseq_cfg));
      }
    } else {
      auto& desc_config = LoadDescriptorDecoderCfg<paramcabac::DecoderRegular>(
          parameter_set, desc.id);
      for (const auto& subdesc : GetDescriptor(desc.id).sub_seqs) {
        auto subseq_cfg = desc_config.GetSubsequenceCfg(
            static_cast<uint8_t>(subdesc.id.second));

        SetConfAsGabac(subdesc.id, std::move(subseq_cfg));
      }
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------