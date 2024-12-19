/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/transformed_sub_seq.h"

#include <utility>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------

TransformedSubSeq::TransformedSubSeq()
    : TransformedSubSeq(SupportValues::TransformIdSubsym::NO_TRANSFORM,
                        SupportValues(), Binarization(),
                        core::gen_sub::kPositionFirst) {}

// -----------------------------------------------------------------------------

TransformedSubSeq::TransformedSubSeq(
    const SupportValues::TransformIdSubsym transform_id_sub_symbol,
    SupportValues&& support_values, Binarization&& cabac_binarization,
    core::GenSubIndex subsequence_id, const bool original,
    const core::AlphabetId alphabet_id)
    : transform_id_subsym_(transform_id_sub_symbol),
      support_values_(support_values),
      cabac_binarization_(cabac_binarization),
      subsequence_id_(std::move(subsequence_id)),
      alphabet_id_(alphabet_id) {
  state_vars_.populate(transform_id_subsym_, support_values_,
                       cabac_binarization_, subsequence_id_, alphabet_id_,
                       original);
}

// -----------------------------------------------------------------------------

TransformedSubSeq::TransformedSubSeq(util::BitReader& reader,
                                     core::GenSubIndex subsequence_id,
                                     const core::AlphabetId alphabet_id)
    : subsequence_id_(std::move(subsequence_id)), alphabet_id_(alphabet_id) {
  transform_id_subsym_ = reader.Read<SupportValues::TransformIdSubsym>(3);
  support_values_ = SupportValues(transform_id_subsym_, reader);
  cabac_binarization_ =
      Binarization(support_values_.GetOutputSymbolSize(),
                   support_values_.GetCodingSubsymSize(), reader);
  state_vars_.populate(transform_id_subsym_, support_values_,
                       cabac_binarization_, subsequence_id_, alphabet_id_);
}

// -----------------------------------------------------------------------------

SupportValues::TransformIdSubsym TransformedSubSeq::GetTransformIdSubsym()
    const {
  return transform_id_subsym_;
}

// -----------------------------------------------------------------------------

const SupportValues& TransformedSubSeq::GetSupportValues() const {
  return support_values_;
}

// -----------------------------------------------------------------------------

const Binarization& TransformedSubSeq::GetBinarization() const {
  return cabac_binarization_;
}

// -----------------------------------------------------------------------------

const StateVars& TransformedSubSeq::GetStateVars() const { return state_vars_; }

// -----------------------------------------------------------------------------

StateVars& TransformedSubSeq::GetStateVars() { return state_vars_; }

// -----------------------------------------------------------------------------

void TransformedSubSeq::write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint8_t>(transform_id_subsym_), 3);
  support_values_.write(transform_id_subsym_, writer);
  cabac_binarization_.write(writer);
}

// -----------------------------------------------------------------------------

void TransformedSubSeq::SetSubsequenceId(
    const core::GenSubIndex& subsequence_id) {
  subsequence_id_ = subsequence_id;
}

// -----------------------------------------------------------------------------

core::AlphabetId TransformedSubSeq::GetAlphabetId() const {
  return alphabet_id_;
}

// -----------------------------------------------------------------------------

void TransformedSubSeq::SetAlphabetId(const core::AlphabetId alphabet_id) {
  alphabet_id_ = alphabet_id;
}

// -----------------------------------------------------------------------------

bool TransformedSubSeq::operator==(const TransformedSubSeq& val) const {
  return transform_id_subsym_ == val.transform_id_subsym_ &&
         support_values_ == val.support_values_ &&
         cabac_binarization_ == val.cabac_binarization_ &&
         state_vars_ == val.state_vars_ &&
         subsequence_id_ == val.subsequence_id_ &&
         alphabet_id_ == val.alphabet_id_;
}

// -----------------------------------------------------------------------------

TransformedSubSeq::TransformedSubSeq(nlohmann::json j,
                                     const bool last_transformed) {
  transform_id_subsym_ = j["transform_id_subsym_"];
  support_values_ = SupportValues(j["support_values_"], transform_id_subsym_);
  cabac_binarization_ = Binarization(j["cabac_binarization_"]);
  state_vars_.populate(transform_id_subsym_, support_values_,
                       cabac_binarization_, subsequence_id_, alphabet_id_,
                       last_transformed);
}

// -----------------------------------------------------------------------------

nlohmann::json TransformedSubSeq::ToJson() const {
  nlohmann::json ret;
  ret["transform_id_subsym_"] = transform_id_subsym_;
  ret["support_values_"] = support_values_.ToJson(transform_id_subsym_);
  ret["cabac_binarization_"] = cabac_binarization_.ToJson();
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
