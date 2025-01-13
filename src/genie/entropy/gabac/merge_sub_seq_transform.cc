/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/merge_sub_seq_transform.h"

#include <cassert>
#include <vector>

#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

void TransformMergeCoding(const paramcabac::Subsequence& sub_seq_cfg,
                          std::vector<util::DataBlock>* transformed_sub_seqs) {
  assert(transformed_sub_seqs != nullptr);
  const paramcabac::TransformedParameters& transformed_sub_seq_params =
      sub_seq_cfg.GetTransformParameters();
  const uint16_t sub_seq_count = transformed_sub_seq_params.GetParam();
  const std::vector<uint8_t> sub_seq_shift_sizes =
      transformed_sub_seq_params.GetMergeCodingShiftSizes();
  const std::vector<paramcabac::TransformedSubSeq>& transform_configs =
      sub_seq_cfg.GetTransformSubSeqConfigs();

  if (sub_seq_count <= 0) {
    UTILS_DIE("invalid sub seq count for merge coding");
  }

  // Prepare internal and output data structures
  util::DataBlock symbols(0, 1);
  symbols.Swap(&(*transformed_sub_seqs)[0]);
  (*transformed_sub_seqs)[0].Clear();
  transformed_sub_seqs->resize(sub_seq_count);

  const uint64_t symbols_count = symbols.Size();
  std::vector<uint64_t> transformed_sub_seq_masks(sub_seq_count, 0);
  for (uint64_t ts = 0; ts < sub_seq_count; ts++) {
    const uint8_t coding_sub_symbol_size =
        transform_configs[ts].GetSupportValues().GetCodingSubsymSize();
    transformed_sub_seq_masks[ts] = (1u << coding_sub_symbol_size) - 1;
    (*transformed_sub_seqs)[ts].SetWordSize(4);
    (*transformed_sub_seqs)[ts].Resize(symbols_count);
  }

  // split
  for (uint64_t i = 0; i < symbols_count; i++) {
    uint64_t symbol_value = symbols.Get(i);
    const int64_t signed_symbol_value = paramcabac::StateVars::GetSignedValue(
        symbol_value, symbols.GetWordSize());

    bool is_negative = false;
    if (signed_symbol_value < 0) {
      is_negative = true;
      symbol_value = abs(static_cast<int>(signed_symbol_value));
    }

    for (uint64_t ts = 0; ts < sub_seq_count; ts++) {
      uint64_t transformed_symbol = symbol_value >> sub_seq_shift_sizes[ts] &
                                    transformed_sub_seq_masks[ts];
      if (is_negative && transformed_symbol != 0) {
        const int64_t transformed_symbol_signed =
            -static_cast<int64_t>(transformed_symbol);
        transformed_symbol = static_cast<uint64_t>(transformed_symbol_signed);
        is_negative = false;
      }

      (*transformed_sub_seqs)[ts].Set(i, transformed_symbol);
    }
  }

  symbols.Clear();
}

// -----------------------------------------------------------------------------

void InverseTransformMergeCoding(
    const paramcabac::Subsequence& sub_seq_cfg,
    std::vector<util::DataBlock>* transformed_sub_seqs) {
  const paramcabac::TransformedParameters& transformed_sub_seq_params =
      sub_seq_cfg.GetTransformParameters();
  const uint16_t sub_seq_count = transformed_sub_seq_params.GetParam();
  const std::vector<uint8_t> sub_seq_shift_sizes =
      transformed_sub_seq_params.GetMergeCodingShiftSizes();

  if (sub_seq_count <= 0 || sub_seq_count != transformed_sub_seqs->size()) {
    UTILS_DIE("invalid sub seq count for merge inverse transform");
  }

  // Prepare the output data structure
  const uint64_t symbols_count = (*transformed_sub_seqs)[0].Size();
  util::DataBlock symbols(symbols_count, 4);

  for (uint64_t i = 0; i < symbols_count; i++) {
    uint64_t symbol_value = 0;
    bool is_negative = false;
    for (uint64_t ts = 0; ts < sub_seq_count; ts++) {
      uint64_t decoded_transformed_symbol = (*transformed_sub_seqs)[ts].Get(i);
      if (static_cast<int64_t>(decoded_transformed_symbol) < 0) {
        decoded_transformed_symbol =
            -static_cast<int64_t>(decoded_transformed_symbol);
        is_negative = true;
      }

      symbol_value =
          symbol_value << sub_seq_shift_sizes[ts] | decoded_transformed_symbol;
    }

    if (is_negative) {
      const int64_t symbol_value_signed = -static_cast<int64_t>(symbol_value);
      symbol_value = static_cast<uint64_t>(symbol_value_signed);
    }

    symbols.Set(i, symbol_value);
  }

  transformed_sub_seqs->resize(1);
  (*transformed_sub_seqs)[0].Clear();

  symbols.Swap(&(*transformed_sub_seqs)[0]);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
