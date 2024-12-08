/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/match_sub_seq_transform.h"

#include <cassert>
#include <vector>

#include "genie/util/block_stepper.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
void TransformMatchCoding(
    const paramcabac::Subsequence& sub_sequence_cfg,
    std::vector<util::DataBlock>* transformed_sub_sequences) {
  assert(transformed_sub_sequences != nullptr);
  const uint16_t match_buffer_size =
      sub_sequence_cfg.GetTransformParameters().GetParam();
  const uint8_t word_size = transformed_sub_sequences->front().GetWordSize();
  // Prepare internal and the output data structures
  util::DataBlock symbols(0, 1);
  symbols.Swap(&(*transformed_sub_sequences)[0]);
  (*transformed_sub_sequences)[0].Clear();
  transformed_sub_sequences->resize(3);

  util::DataBlock* const pointers = &(*transformed_sub_sequences)[0];
  util::DataBlock* const lengths = &(*transformed_sub_sequences)[1];
  util::DataBlock* const raw_values = &(*transformed_sub_sequences)[2];
  pointers->SetWordSize(core::Range2Bytes({0, match_buffer_size}));
  lengths->SetWordSize(core::Range2Bytes({0, match_buffer_size}));
  raw_values->SetWordSize(word_size);

  const uint64_t symbols_size = symbols.Size();

  if (match_buffer_size == 0) {
    lengths->Resize(symbols_size);
    std::fill(lengths->begin(), lengths->end(), 0);
    return;
  }

  // Perform the match transformation
  for (uint64_t i = 0; i < symbols_size; i++) {
    uint64_t pointer = 0;
    uint64_t length = 0;
    const uint64_t window_start_idx = i - match_buffer_size;
    const uint64_t window_end_idx = i;

    for (uint64_t w = window_start_idx; w < window_end_idx; w++) {
      uint64_t offset = i;
      while (offset < symbols_size &&
             (symbols.Get(offset) == symbols.Get(w + offset - i) &&
              offset - i < match_buffer_size - 1u)) {
        offset++;
      }
      offset -= i;
      if (offset >= length) {
        length = offset;
        pointer = w;
      }
    }
    if (length < 2) {
      lengths->PushBack(0);
      raw_values->PushBack(symbols.Get(i));
    } else {
      pointers->PushBack(i - pointer);
      lengths->PushBack(length);
      i += length - 1;
    }
  }

  symbols.Clear();
}

// -----------------------------------------------------------------------------
void InverseTransformMatchCoding(
    std::vector<util::DataBlock>* transformed_sub_sequences) {
  assert(transformed_sub_sequences != nullptr);

  if (transformed_sub_sequences->size() != 3) {
    UTILS_DIE("invalid sub sequence count for match inverse transform");
  }

  // Prepare internal and the output data structures
  const util::DataBlock* const pointers = &(*transformed_sub_sequences)[0];
  const util::DataBlock* const lengths = &(*transformed_sub_sequences)[1];
  const util::DataBlock* const raw_values = &(*transformed_sub_sequences)[2];

  util::DataBlock symbols(0, (raw_values->GetWordSize()));
  assert(lengths->Size() == pointers->Size() + raw_values->Size());

  // Re-compute the symbols from the pointer, lengths and raw values
  size_t n = 0;
  util::BlockStepper t0 = pointers->GetReader();
  util::BlockStepper t1 = lengths->GetReader();
  util::BlockStepper t2 = raw_values->GetReader();
  while (t1.IsValid()) {
    const uint64_t length = t1.Get();
    t1.Inc();
    if (length == 0) {
      symbols.PushBack(t2.Get());
      t2.Inc();
      n++;
    } else {
      const uint64_t pointer = t0.Get();
      t0.Inc();
      for (uint64_t l = 0; l < length; l++) {
        symbols.PushBack(symbols.Get(n - pointer));
        n++;
      }
    }
  }

  transformed_sub_sequences->resize(1);
  (*transformed_sub_sequences)[0].Clear();

  symbols.Swap(&(*transformed_sub_sequences)[0]);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
