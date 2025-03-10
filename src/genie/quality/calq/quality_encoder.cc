/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/quality_encoder.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "genie/quality/calq/calq_coder.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

QualityEncoder::QualityEncoder(const EncodingOptions& options,
                               const std::map<int, util::Quantizer>& quant,
                               DecodingBlock* output)
    : nr_mapped_records_(0),
      min_pos_unencoded_(0),
      nr_quantizers_(options.quantization_max - options.quantization_min + 1),

      quality_value_offset_(options.quality_value_offset),
      pos_offset_(0),
      haplotyper_(
          options.filter_size, options.polyploidy, options.quality_value_offset,
          static_cast<size_t>(nr_quantizers_), options.hq_soft_clip_propagation,
          options.hq_soft_clip_streak, options.filter_cut_off,
          options.debug_pileup, options.squash, options.filter_type),
      genotyper_(static_cast<const int&>(options.polyploidy),
                 static_cast<const int&>(options.quality_value_offset),
                 nr_quantizers_, options.debug_pileup),

      out_(output),

      quantizers_(quant) {}

// -----------------------------------------------------------------------------

void QualityEncoder::QuantizeUntil(const uint64_t pos) {
  while (min_pos_unencoded_ < pos) {
    std::string pos_seqs;
    std::string pos_quality_values;

    std::tie(pos_seqs, pos_quality_values) =
        record_pileup_.GetPileup(min_pos_unencoded_);

    if (!pos_seqs.empty()) {
      auto l = static_cast<uint8_t>(
          genotyper_.ComputeQuantizerIndex(pos_seqs, pos_quality_values));
      out_->quantizer_indices.push_back(l);
    } else {
      out_->quantizer_indices.push_back(out_->quantizer_indices.back());
    }

    ++min_pos_unencoded_;
  }
}

// -----------------------------------------------------------------------------

void QualityEncoder::EncodeRecords(std::vector<EncodingRecord> records) const {
  for (auto& r : records) {
    for (size_t i = 0; i < r.positions.size(); ++i) {
      EncodeMappedQuality(r.quality_values[i], r.cigars[i], r.positions[i]);
    }
  }
}

// -----------------------------------------------------------------------------

void QualityEncoder::AddMappedRecordToBlock(EncodingRecord& record) {
  if (NrMappedRecords() == 0) {
    pos_offset_ = record.positions.front();
    min_pos_unencoded_ = record.positions.front();

    out_->code_books.clear();
    out_->step_indices.clear();
    for (int i = 0; i < nr_quantizers_; ++i) {
      const auto& map = quantizers_[i].InverseLut();
      out_->code_books.emplace_back();
      out_->step_indices.emplace_back();
      for (const auto& [fst, snd] : map) {
        (void) fst;
        out_->code_books.back().push_back(static_cast<uint8_t>(snd));
      }
    }

    // unaligned
    const auto& map = quantizers_[nr_quantizers_ - 1].InverseLut();
    out_->code_books.emplace_back();
    out_->step_indices.emplace_back();
    for (const auto& [fst, snd] : map) {
      (void) fst;
      out_->code_books.back().push_back(static_cast<uint8_t>(snd));
    }

    out_->quantizer_indices.clear();
  }

  record_pileup_.AddRecord(record);

  // calc quantizers
  QuantizeUntil(record.positions.front());

  // encode quality values
  EncodeRecords(record_pileup_.GetRecordsBefore(min_pos_unencoded_));

  ++nr_mapped_records_;
}

// -----------------------------------------------------------------------------

void QualityEncoder::FinishBlock() {
  // Compute all remaining quantizers
  QuantizeUntil(record_pileup_.GetMaxPos() + 1);

  // Encode remaining records
  EncodeRecords(record_pileup_.GetAllRecords());
}

// -----------------------------------------------------------------------------

size_t QualityEncoder::NrMappedRecords() const { return nr_mapped_records_; }

// -----------------------------------------------------------------------------

void QualityEncoder::EncodeMappedQuality(const std::string& quality_values,
                                         const std::string& cigar,
                                         const uint64_t pos) const {
  const size_t cigar_len = cigar.length();
  size_t op_len = 0;  // length of current CIGAR operation
  size_t quality_idx = 0;
  size_t quantizer_indices_idx = pos - pos_offset_;

  for (size_t cigar_idx = 0; cigar_idx < cigar_len; cigar_idx++) {
    if (isdigit(cigar[cigar_idx])) {
      op_len = op_len * 10 + static_cast<size_t>(cigar[cigar_idx]) -
               static_cast<size_t>('0');
      continue;
    }

    switch (cigar[cigar_idx]) {
      case 'A':
      case 'C':
      case 'G':
      case 'T':
      case 'N':
        op_len = 1;
        /* fall through */
      case '=':
        // Encode opLen quality values with computed quantizer indices
        for (size_t i = 0; i < op_len; i++) {
          const uint8_t q =
              static_cast<uint8_t>(quality_values[quality_idx++]) -
              quality_value_offset_;
          const uint8_t quantizer_index =
              out_->quantizer_indices[quantizer_indices_idx++];
          uint8_t quality_value_index = static_cast<uint8_t>(
              quantizers_.at(quantizer_index).ValueToIndex(q));
          out_->step_indices.at(quantizer_index).push_back(quality_value_index);
        }
        break;
      case '+':
      case ')':
        // Encode opLen quality values with max quantizer index
        for (size_t i = 0; i < op_len; i++) {
          auto q = static_cast<uint8_t>(quality_values[quality_idx++]) -
                   quality_value_offset_;
          uint8_t quality_value_index = static_cast<uint8_t>(
              quantizers_.at(nr_quantizers_ - 1).ValueToIndex(q));
          out_->step_indices.at(static_cast<size_t>(nr_quantizers_))
              .push_back(quality_value_index);
        }
        break;
      case '-':
        quantizer_indices_idx += op_len;
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
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
