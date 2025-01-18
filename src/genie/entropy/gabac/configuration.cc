/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/configuration.h"

#include <utility>
#include <vector>

#include "genie/entropy/gabac/config_manual.h"
#include "genie/entropy/gabac/streams.h"
#include "genie/entropy/paramcabac/transformed_sub_seq.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

EncodingConfiguration::EncodingConfiguration() = default;

// -----------------------------------------------------------------------------

EncodingConfiguration::EncodingConfiguration(const core::GenSubIndex& sub) {
  sub_seq_cfg_ = GetEncoderConfigManual(sub);
}

// -----------------------------------------------------------------------------

EncodingConfiguration::EncodingConfiguration(
    paramcabac::Subsequence&& sub_seq) {
  sub_seq_cfg_ = sub_seq;
}

// -----------------------------------------------------------------------------

EncodingConfiguration::~EncodingConfiguration() = default;

// -----------------------------------------------------------------------------

bool EncodingConfiguration::operator==(
    const EncodingConfiguration& conf) const {
  return conf.sub_seq_cfg_ == this->sub_seq_cfg_;
}

// -----------------------------------------------------------------------------

bool EncodingConfiguration::operator!=(
    const EncodingConfiguration& conf) const {
  return !(conf == *this);
}

// -----------------------------------------------------------------------------

[[maybe_unused]] uint8_t EncodingConfiguration::GetSubSeqWordSize() const {
  const std::vector<paramcabac::TransformedSubSeq>& transform_configs =
      sub_seq_cfg_.GetTransformSubSeqConfigs();

  switch (sub_seq_cfg_.GetTransformParameters().GetTransformIdSubseq()) {
    case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
      return paramcabac::StateVars::GetMinimalSizeInBytes(
          transform_configs[0].GetSupportValues().GetOutputSymbolSize());
    case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
      return paramcabac::StateVars::GetMinimalSizeInBytes(
          transform_configs[1].GetSupportValues().GetOutputSymbolSize());
    case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
      return paramcabac::StateVars::GetMinimalSizeInBytes(
          transform_configs[2].GetSupportValues().GetOutputSymbolSize());
    case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
      return paramcabac::StateVars::GetMinimalSizeInBytes(
          transform_configs[1].GetSupportValues().GetOutputSymbolSize());
    case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING: {
      const std::vector<uint8_t> sub_seq_shift_sizes =
          sub_seq_cfg_.GetTransformParameters().GetMergeCodingShiftSizes();
      return paramcabac::StateVars::GetMinimalSizeInBytes(
          sub_seq_shift_sizes[0] +
          transform_configs[0].GetSupportValues().GetOutputSymbolSize());
    }
    default:
      UTILS_DIE("Invalid sub sequence transformation");
  }
}

// -----------------------------------------------------------------------------

const paramcabac::Subsequence& EncodingConfiguration::GetSubSeqConfig() const {
  return sub_seq_cfg_;
}

// -----------------------------------------------------------------------------

void EncodingConfiguration::SetSubSeqConfig(
    paramcabac::Subsequence&& sub_seq_cfg) {
  sub_seq_cfg_ = sub_seq_cfg;
}

// -----------------------------------------------------------------------------

void IoConfiguration::Validate() const {
  if (!input_stream) {
    UTILS_DIE("Invalid input stream");
  }
  if (!output_stream) {
    UTILS_DIE("Invalid output stream");
  }
  if (!log_stream) {
    UTILS_DIE("Invalid logging output stream");
  }
  if (static_cast<unsigned>(this->level) >
      static_cast<unsigned>(LogLevel::LOG_FATAL)) {
    UTILS_DIE("Invalid logging level");
  }
}

// -----------------------------------------------------------------------------

[[maybe_unused]] std::ostream& IoConfiguration::Log(const LogLevel& l) const {
  static NullStream null_str;
  if (static_cast<int>(l) >= static_cast<int>(level)) {
    return *log_stream;
  }
  return null_str;
}

// -----------------------------------------------------------------------------

EncodingConfiguration::EncodingConfiguration(nlohmann::json j)
    : sub_seq_cfg_(std::move(j)) {}

// -----------------------------------------------------------------------------

nlohmann::json EncodingConfiguration::ToJson() const {
  return sub_seq_cfg_.ToJson();
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
