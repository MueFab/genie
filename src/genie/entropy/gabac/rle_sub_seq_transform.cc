/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/rle_sub_seq_transform.h"

#include <cassert>
#include <vector>

#include "genie/util/block_stepper.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

void TransformRleCoding(const paramcabac::Subsequence& subseq_cfg,
                        std::vector<util::DataBlock>* transformed_subseqs) {
  assert(transformed_subseqs != nullptr);
  const auto guard =
      static_cast<uint8_t>(subseq_cfg.GetTransformParameters().GetParam());
  assert(guard > 0);
  const auto wordsize = transformed_subseqs->front().GetWordSize();

  // Prepare internal and the output data structures
  util::DataBlock symbols(0, 1);
  symbols.Swap(&(*transformed_subseqs)[0]);
  (*transformed_subseqs)[0].Clear();
  transformed_subseqs->resize(2);

  util::DataBlock* const lengths = &(*transformed_subseqs)[0];
  util::DataBlock* const raw_values = &(*transformed_subseqs)[1];
  lengths->SetWordSize(1);
  raw_values->SetWordSize(wordsize);

  util::BlockStepper r = symbols.GetReader();
  uint64_t last_sym = 0;
  uint64_t run_value = 0;
  while (r.IsValid()) {
    const uint64_t cur_sym = r.Get();
    r.Inc();
    if (last_sym != cur_sym && run_value > 0) {
      raw_values->PushBack(last_sym);

      while (run_value > guard) {
        lengths->PushBack(guard);
        run_value -= guard;
      }

      if (run_value > 0) {
        lengths->PushBack(run_value - 1);
      }

      run_value = 0;
    }

    last_sym = cur_sym;
    run_value++;
  }

  if (run_value > 0) {
    raw_values->PushBack(last_sym);

    while (run_value >= guard) {
      lengths->PushBack(guard);
      run_value -= guard;
    }

    if (run_value > 0) {
      lengths->PushBack(run_value -
                        1);  // non-kGuard run-value is coded as lengthValue-1
    }
  }

  symbols.Clear();
}

// -----------------------------------------------------------------------------

void InverseTransformRleCoding(
    const paramcabac::Subsequence& subseq_cfg,
    std::vector<util::DataBlock>* transformed_subseqs) {
  assert(transformed_subseqs != nullptr);

  if (transformed_subseqs->size() != 2) {
    UTILS_DIE("invalid subseq count for rle inverse transform");
  }

  const auto guard = subseq_cfg.GetTransformParameters().GetParam();
  assert(guard > 0);

  // Prepare internal and the output data structures
  const util::DataBlock* const lengths = &(*transformed_subseqs)[0];
  const util::DataBlock* const raw_values = &(*transformed_subseqs)[1];
  util::DataBlock symbols(0, (raw_values->GetWordSize()));

  util::BlockStepper r_val = raw_values->GetReader();
  util::BlockStepper r_len = lengths->GetReader();

  // Re-compute the symbols from the kLengths and raw values
  while (r_val.IsValid()) {
    uint64_t total_length_value = 0;
    const uint64_t raw_value = r_val.Get();
    r_val.Inc();

    while (r_len.IsValid()) {
      const uint64_t run_value = r_len.Get();
      r_len.Inc();

      if (run_value == guard) {
        total_length_value += run_value;
      } else {
        total_length_value +=
            run_value + 1;  // non-kGuard run-value is coded as lengthValue-1
        break;
      }
    }

    while (total_length_value > 0) {
      total_length_value--;
      symbols.PushBack(raw_value);
    }
  }

  transformed_subseqs->resize(1);
  (*transformed_subseqs)[0].Clear();

  symbols.Swap(&(*transformed_subseqs)[0]);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------