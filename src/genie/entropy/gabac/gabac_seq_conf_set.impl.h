/**
 * Copyright 2018-2024 The Genie Authors.
 * @file gabac_seq_conf_set.impl.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Implementation of template functions for the GabacSeqConfSet class.
 *
 * This file contains the implementation of template functions defined in the
 * `GabacSeqConfSet` class. The `FillDecoder` and `LoadDescriptorDecoderCfg`
 * functions are provided here to manage configurations for MPEG-G descriptor
 * decoding and setup the internal configuration of a decoder based on a
 * descriptor's properties.
 *
 * @details The `FillDecoder` function is used to initialize the decoder with
 * the appropriate subsequence configurations. It extracts the configuration for
 * each descriptor subsequence from the internal configuration set. The
 * `LoadDescriptorDecoderCfg` function extracts and verifies a
 * `DescriptorPresent` configuration from a `ParameterSet`. This ensures that
 * the appropriate MPEG-G parameter set configuration is used for CABAC
 * decoding.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_IMPL_H_
#define SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_IMPL_H_

// -----------------------------------------------------------------------------

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
template <typename T>
void GabacSeqConfSet::FillDecoder(const core::GenomicDescriptorProperties& desc,
                                  T& decoder_config) const {
  for (const auto& subdesc : desc.sub_seqs) {
    auto subseq_cfg = GetConfAsGabac(subdesc.id).GetSubSeqConfig();
    decoder_config.SetSubsequenceCfg(static_cast<uint8_t>(subdesc.id.second),
                                     std::move(subseq_cfg));
  }
}

// -----------------------------------------------------------------------------
template <typename T>
const T& GabacSeqConfSet::LoadDescriptorDecoderCfg(
    const ParameterSet& parameter_set, const core::GenDesc descriptor_id) {
  auto& cur_desc = parameter_set.GetEncodingSet().GetDescriptor(descriptor_id);
  UTILS_DIE_IF(cur_desc.IsClassSpecific(),
               "Class specific config not supported");
  constexpr auto present =
      core::parameter::desc_pres::DescriptorPresent::kPresent;
  auto& base_conf = cur_desc.Get();
  UTILS_DIE_IF(base_conf.GetPreset() != present, "Config not present");
  auto& decoder_conf =
      dynamic_cast<const core::parameter::desc_pres::DescriptorPresent&>(
          base_conf)
          .GetDecoder();
  UTILS_DIE_IF(decoder_conf.GetMode() != paramcabac::kModeCabac,
               "Config is not paramcabac");

  return dynamic_cast<const T&>(decoder_conf);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
