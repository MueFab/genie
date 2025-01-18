/**
 * Copyright 2018-2024 The Genie Authors.
 * @file transformed_parameters.cc
 * @brief Implementation of transformed parameters for CABAC in Genie.
 *
 * Defines the `TransformedParameters` class for managing transformation types,
 * parameters for match, RLE, and merge coding, and serialization support.
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/transformed_parameters.h"

#include <utility>
#include <vector>

#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------

TransformedParameters::TransformedParameters()
    : TransformedParameters(TransformIdSubseq::NO_TRANSFORM, 0) {}

// -----------------------------------------------------------------------------

TransformedParameters::TransformedParameters(util::BitReader& reader) {
  transform_id_subseq_ = reader.Read<TransformIdSubseq>(8);
  switch (transform_id_subseq_) {
    case TransformIdSubseq::NO_TRANSFORM:
    case TransformIdSubseq::EQUALITY_CODING:
      break;
    case TransformIdSubseq::MATCH_CODING:
      match_coding_buffer_size_ = reader.Read<uint16_t>();
      break;
    case TransformIdSubseq::RLE_CODING:
      rle_coding_guard_ = reader.Read<uint8_t>();
      break;
    case TransformIdSubseq::MERGE_CODING:
      merge_coding_subseq_count_ = reader.Read<uint8_t>(4);
      merge_coding_shift_size_.resize(*merge_coding_subseq_count_);
      for (int i = 0; i < *merge_coding_subseq_count_; i++) {
        merge_coding_shift_size_[i] = reader.Read<uint8_t>(5);
      }
      break;
    default:
      UTILS_THROW_RUNTIME_EXCEPTION("Invalid subseq transformation");
  }
}

// -----------------------------------------------------------------------------

TransformedParameters::TransformedParameters(
    const TransformIdSubseq& transform_id_subseq, uint16_t param)
    : transform_id_subseq_(transform_id_subseq), merge_coding_shift_size_(0) {
  switch (transform_id_subseq_) {
    case TransformIdSubseq::NO_TRANSFORM:
    case TransformIdSubseq::EQUALITY_CODING:
      break;
    case TransformIdSubseq::RLE_CODING:
      rle_coding_guard_ = static_cast<uint8_t>(param);
      break;
    case TransformIdSubseq::MATCH_CODING:
      match_coding_buffer_size_ = param;
      break;
    case TransformIdSubseq::MERGE_CODING:
      merge_coding_subseq_count_ = static_cast<uint8_t>(param);
      break;
    default:
      UTILS_THROW_RUNTIME_EXCEPTION("Invalid subseq transformation");
  }
}

// -----------------------------------------------------------------------------

size_t TransformedParameters::GetNumStreams() const {
  if (transform_id_subseq_ == TransformIdSubseq::MERGE_CODING) {
    return *merge_coding_subseq_count_;
  }
  static const std::vector<size_t> lut = {1, 2, 3, 2, 1};
  return lut[static_cast<uint8_t>(transform_id_subseq_)];
}

// -----------------------------------------------------------------------------

TransformedParameters::TransformIdSubseq
TransformedParameters::GetTransformIdSubseq() const {
  return transform_id_subseq_;
}

// -----------------------------------------------------------------------------

uint16_t TransformedParameters::GetParam() const {
  switch (transform_id_subseq_) {
    case TransformIdSubseq::NO_TRANSFORM:
    case TransformIdSubseq::EQUALITY_CODING:
      return 0;
    case TransformIdSubseq::MATCH_CODING:
      return *match_coding_buffer_size_;
    case TransformIdSubseq::RLE_CODING:
      return *rle_coding_guard_;
    case TransformIdSubseq::MERGE_CODING:
      return *merge_coding_subseq_count_;
  }
  return 0;
}

// -----------------------------------------------------------------------------

std::vector<uint8_t> TransformedParameters::GetMergeCodingShiftSizes() const {
  return merge_coding_shift_size_;
}

// -----------------------------------------------------------------------------

void TransformedParameters::SetMergeCodingShiftSizes(
    std::vector<uint8_t> merge_codingshift_sizes) {
  merge_coding_shift_size_ = std::move(merge_codingshift_sizes);
}

// -----------------------------------------------------------------------------

void TransformedParameters::write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint8_t>(transform_id_subseq_), 8);
  if (match_coding_buffer_size_) {
    writer.WriteBits(*match_coding_buffer_size_, 16);
  }
  if (rle_coding_guard_) {
    writer.WriteBits(*rle_coding_guard_, 8);
  }
  if (merge_coding_subseq_count_) {
    writer.WriteBits(*merge_coding_subseq_count_, 4);
  }
  for (auto& i : merge_coding_shift_size_) {
    writer.WriteBits(i, 5);
  }
}

// -----------------------------------------------------------------------------

bool TransformedParameters::operator==(const TransformedParameters& val) const {
  return transform_id_subseq_ == val.transform_id_subseq_ &&
         match_coding_buffer_size_ == val.match_coding_buffer_size_ &&
         rle_coding_guard_ == val.rle_coding_guard_ &&
         merge_coding_subseq_count_ == val.merge_coding_subseq_count_ &&
         merge_coding_shift_size_ == val.merge_coding_shift_size_;
}

// -----------------------------------------------------------------------------

nlohmann::json TransformedParameters::ToJson() const {
  nlohmann::json ret;
  ret["transform_ID_subseq"] = transform_id_subseq_;
  switch (transform_id_subseq_) {
    case TransformIdSubseq::NO_TRANSFORM:
    case TransformIdSubseq::EQUALITY_CODING:
      break;
    case TransformIdSubseq::MATCH_CODING:
      ret["match_coding_buffer_size"] = *match_coding_buffer_size_;
      break;
    case TransformIdSubseq::RLE_CODING:
      ret["rle_coding_guard"] = *rle_coding_guard_;
      break;
    case TransformIdSubseq::MERGE_CODING:
      ret["merge_coding_shift_size"] = merge_coding_shift_size_;
      break;
  }
  return ret;
}

// -----------------------------------------------------------------------------

TransformedParameters::TransformedParameters(const nlohmann::json& json) {
  transform_id_subseq_ = json["transform_ID_subseq"];
  switch (transform_id_subseq_) {
    case TransformIdSubseq::NO_TRANSFORM:
    case TransformIdSubseq::EQUALITY_CODING:
      break;
    case TransformIdSubseq::MATCH_CODING:
      match_coding_buffer_size_ = json["match_coding_buffer_size"];
      break;
    case TransformIdSubseq::RLE_CODING:
      rle_coding_guard_ = json["rle_coding_guard"];
      break;
    case TransformIdSubseq::MERGE_CODING:
      for (auto& elem : json["merge_coding_shift_size"])
        merge_coding_shift_size_.push_back(elem);
      break;
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
