/**
 * Copyright 2018-2024 The Genie Authors.
 * @file quality_decoder.cc
 *
 * @brief Implements the QualityDecoder class for decoding quality values of
 * genomic data.
 *
 * This file is part of the Genie project, designed for efficient genomic data
 * compression and reconstruction. The `QualityDecoder` class decodes quality
 * information from compressed genomic blocks, reconstructing the original
 * quality values using advanced quantization and decoding techniques.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/quality_decoder.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "genie/quality/calq/calq_coder.h"
#include "genie/quality/calq/quality_encoder.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

QualityDecoder::QualityDecoder(const DecodingBlock& input,
                               const uint64_t position_offset,
                               const uint8_t quality_offset,
                               EncodingBlock* output)
    : pos_offset_(position_offset),
      quality_value_offset_(quality_offset),
      qvi_idx_(input.step_indices.size(), 0),
      quantizers_(0),
      out_(output),
      in_(input) {
  out_->quality_values.clear();
  for (const auto& q : input.code_books) {
    std::map<size_t, int> steps;
    for (unsigned int i = 0; i < q.size(); ++i) {
      steps[i] = q[i];
    }
    quantizers_.emplace_back(steps);
  }
}

// -----------------------------------------------------------------------------

QualityDecoder::~QualityDecoder() = default;

// -----------------------------------------------------------------------------

void QualityDecoder::DecodeMappedRecordFromBlock(
    const DecodingRead& sam_record) {
  std::string quality;

  const size_t cigar_len = sam_record.cigar.length();
  size_t op_len = 0;
  size_t quality_value_pos = sam_record.pos_min - pos_offset_;

  for (size_t cigar_idx = 0; cigar_idx < cigar_len; cigar_idx++) {
    if (isdigit(sam_record.cigar[cigar_idx])) {
      op_len = op_len * 10 + static_cast<size_t>(sam_record.cigar[cigar_idx]) -
               static_cast<size_t>('0');
      continue;
    }

    switch (sam_record.cigar[cigar_idx]) {
      case 'A':
      case 'C':
      case 'G':
      case 'T':
      case 'N':
        op_len = 1;
        /* fall through */
      case '=':
        // Decode opLen quality value indices with computed
        // quantizer indices
        for (size_t i = 0; i < op_len; i++) {
          uint8_t quantizer_index = 0;
          if (!in_.quantizer_indices.empty()) {
            quantizer_index = in_.quantizer_indices[quality_value_pos++];
          }

          const uint8_t quality_value_index =
              in_.step_indices.at(quantizer_index)[qvi_idx_[quantizer_index]++];

          const uint8_t q = static_cast<uint8_t>(
              quantizers_.at(quantizer_index)
                  .IndexToReconstructionValue(quality_value_index));

          quality += static_cast<char>(q + quality_value_offset_);
        }
        break;
      case '+':
      case ')':
        // Decode opLen quality values with max quantizer index
        for (size_t i = 0; i < op_len; i++) {
          int quality_value_index = in_.step_indices.at(
              quantizers_.size() - 1)[qvi_idx_[quantizers_.size() - 1]++];
          const int q = quantizers_.at(quantizers_.size() - 1)
                            .IndexToReconstructionValue(quality_value_index);
          quality += static_cast<char>(q + quality_value_offset_);
        }
        break;
      case '-':
        quality_value_pos += op_len;
        break;  // do nothing as these bases are not present
      case ']':
      case '(':
      case '[':
        break;  // ignore first clip char
      default:
        UTILS_DIE("Bad CIGAR string");
    }
    op_len = 0;
  }
  out_->quality_values[0].push_back(std::move(quality));
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
