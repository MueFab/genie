/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/subsequence.h"

#include <utility>
#include <vector>

#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------
Subsequence::Subsequence()
    : Subsequence(TransformedParameters(), 0, false,
                  std::vector({TransformedSubSeq()})) {}

// -----------------------------------------------------------------------------

Subsequence::Subsequence(const uint16_t descriptor_subsequence_id,
                         const bool token_type_flag)
    : Subsequence(TransformedParameters(), descriptor_subsequence_id,
                  token_type_flag, std::vector({TransformedSubSeq()})) {}

// -----------------------------------------------------------------------------

Subsequence::Subsequence(
    TransformedParameters&& transform_sub_seq_parameters,
    uint16_t descriptor_subsequence_id, const bool token_type_flag,
    std::vector<TransformedSubSeq>&& transform_sub_seq_configs)
    : transform_sub_seq_parameters_(transform_sub_seq_parameters) {
  if (!token_type_flag) {
    descriptor_subsequence_id_ = descriptor_subsequence_id;
  }

  if (transform_sub_seq_configs.size() ==
      transform_sub_seq_parameters_.GetNumStreams()) {
    transform_sub_seq_configs_ = std::move(transform_sub_seq_configs);
  }
}

// -----------------------------------------------------------------------------

Subsequence::Subsequence(const bool token_type, core::GenDesc desc,
                         util::BitReader& reader) {
  core::GenSubIndex sub_seq;
  if (!token_type) {
    descriptor_subsequence_id_ = reader.Read<uint16_t>(10);
    sub_seq = std::pair(desc, *descriptor_subsequence_id_);
  } else {
    sub_seq = std::pair(desc,
                        static_cast<uint16_t>(0));  // FIXME use zero always?
  }

  transform_sub_seq_parameters_ = TransformedParameters(reader);
  uint8_t num_subseq = 0;
  switch (transform_sub_seq_parameters_.GetTransformIdSubseq()) {
    case TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
      num_subseq = 1;
      break;
    case TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
    case TransformedParameters::TransformIdSubseq::RLE_CODING:
      num_subseq = 2;
      break;
    case TransformedParameters::TransformIdSubseq::MATCH_CODING:
      num_subseq = 3;
      break;
    case TransformedParameters::TransformIdSubseq::MERGE_CODING:
      UTILS_DIE("MERGE core unsupported");
  }
  for (size_t i = 0; i < num_subseq; ++i) {
    transform_sub_seq_configs_.emplace_back(reader, sub_seq);
  }
}

// -----------------------------------------------------------------------------

void Subsequence::SetTransformSubSeqCfg(
    const size_t index, TransformedSubSeq&& transform_sub_seq_cfg) {
  transform_sub_seq_configs_[index] = transform_sub_seq_cfg;
}

// -----------------------------------------------------------------------------

void Subsequence::write(util::BitWriter& writer) const {
  if (descriptor_subsequence_id_) {
    writer.WriteBits(*descriptor_subsequence_id_, 10);
  }
  transform_sub_seq_parameters_.write(writer);
  for (auto& i : transform_sub_seq_configs_) {
    i.write(writer);
  }
}

// -----------------------------------------------------------------------------

uint16_t Subsequence::GetDescriptorSubsequenceId() const {
  UTILS_DIE_IF(!descriptor_subsequence_id_,
               "descriptor_subsequence_ID not present");
  return descriptor_subsequence_id_.value();
}

// -----------------------------------------------------------------------------

const TransformedParameters& Subsequence::GetTransformParameters() const {
  return transform_sub_seq_parameters_;
}

// -----------------------------------------------------------------------------

const TransformedSubSeq& Subsequence::GetTransformSubSeqCfg(
    const uint8_t index) const {
  return transform_sub_seq_configs_[index];
}

// -----------------------------------------------------------------------------

size_t Subsequence::GetNumTransformSubSeqConfigs() const {
  return transform_sub_seq_configs_.size();
}

// -----------------------------------------------------------------------------

const std::vector<TransformedSubSeq>& Subsequence::GetTransformSubSeqConfigs()
    const {
  return transform_sub_seq_configs_;
}

// -----------------------------------------------------------------------------

bool Subsequence::GetTokenTypeFlag() const {
  return descriptor_subsequence_id_ == std::nullopt;
}

// -----------------------------------------------------------------------------

bool Subsequence::operator==(const Subsequence& seq) const {
  return descriptor_subsequence_id_ == seq.descriptor_subsequence_id_ &&
         transform_sub_seq_parameters_ == seq.transform_sub_seq_parameters_ &&
         transform_sub_seq_configs_ == seq.transform_sub_seq_configs_;
}

// -----------------------------------------------------------------------------

Subsequence::Subsequence(nlohmann::json j) {
  if (j.contains("descriptor_subsequence_ID")) {
    descriptor_subsequence_id_ = j["descriptor_subsequence_ID"];
  }
  transform_sub_seq_parameters_ =
      TransformedParameters(j["transform_subseq_parameters"]);
  auto remaining_seqs = j["transform_subseq_cfgs"].size();
  for (const auto& it : j["transform_subseq_cfgs"]) {
    remaining_seqs--;
    transform_sub_seq_configs_.emplace_back(it, !remaining_seqs);
  }
}

// -----------------------------------------------------------------------------

nlohmann::json Subsequence::ToJson() const {
  nlohmann::json ret;
  if (descriptor_subsequence_id_ != std::nullopt) {
    ret["descriptor_subsequence_ID"] = *descriptor_subsequence_id_;
  }
  ret["transform_subseq_parameters"] = transform_sub_seq_parameters_.ToJson();
  std::vector<nlohmann::json> tmp;
  tmp.reserve(transform_sub_seq_configs_.size());
  for (const auto& a : transform_sub_seq_configs_) {
    tmp.emplace_back(a.ToJson());
  }
  ret["transform_subseq_cfgs"] = tmp;
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
