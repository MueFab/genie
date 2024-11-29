/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// Equality coding is using a small hack to work as efficiently as possible in
// place. There are two implementations, one for wordsize 1 and one for other
// wordsizes. With wordsize 1, it is exploited, that there are as many equality
// flags as input symbol, so this version does only need to allocate memory for
// the raw values stream. To achieve this, the equality flags are written into
// the raw_values data block (containing the input values) and vice versa and
// swapped before returning. A user from outside will not notice this, but
// internally it is important to know that these two util::DataBlocks swap
// meaning. For wordsizes greater 1 this optimization is not possible, as the
// raw values do not fit into the same data block where input values are still
// inside (different word sizes)

#include "genie/entropy/gabac/equality_sub_seq_transform.h"

#include <cassert>
#include <vector>

#include "genie/util/block_stepper.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
// Optimized for wordsize 1. In place for equality flags
static void TransformEqualityCoding0(util::DataBlock* const values,
                                     util::DataBlock* const equality_flags) {
  uint64_t previous_symbol = 0;

  util::BlockStepper r = values->GetReader();
  // Treat value as equality_flags and vice versa
  while (r.IsValid()) {
    if (const uint64_t symbol = r.Get(); symbol == previous_symbol) {
      r.Set(1);
    } else {
      r.Set(0);
      if (symbol > previous_symbol) {
        equality_flags->PushBack(symbol - 1);
      } else {
        equality_flags->PushBack(symbol);
      }
      previous_symbol = symbol;
    }
    r.Inc();
  }

  // Swap back before returning
  equality_flags->Swap(values);
}

// -----------------------------------------------------------------------------
// Optimized for wordsize 1 > 0. In place for values
static void TransformEqualityCoding1(util::DataBlock* const values,
                                     util::DataBlock* const equality_flags) {
  uint64_t previous_symbol = 0;

  util::BlockStepper r = values->GetReader();
  util::BlockStepper w = values->GetReader();
  // Treat value as equality_flags and vice versa
  while (r.IsValid()) {
    if (const uint64_t symbol = r.Get(); symbol == previous_symbol) {
      equality_flags->PushBack(1);
    } else {
      equality_flags->PushBack(0);
      if (symbol > previous_symbol) {
        w.Set(symbol - 1);
      } else {
        w.Set(symbol);
      }
      w.Inc();
      previous_symbol = symbol;
    }
    r.Inc();
  }

  values->Resize(values->Size() - (w.end - w.curr) / w.word_size);
}

// -----------------------------------------------------------------------------
void TransformEqualityCoding(
    std::vector<util::DataBlock>* transformed_subseqs) {
  // Prepare internal and the output data structures
  const uint8_t wordsize = transformed_subseqs->front().GetWordSize();
  transformed_subseqs->resize(2);
  (*transformed_subseqs)[0].Swap(
      &(*transformed_subseqs)[1]);  // transformSubseq[0] = kFlags,
                                    // transformSubseq[1] = values
  util::DataBlock* const flags = &(*transformed_subseqs)[0];
  util::DataBlock* const raw_values = &(*transformed_subseqs)[1];

  flags->SetWordSize(1);
  raw_values->SetWordSize(wordsize);

  if (raw_values->GetWordSize() == 1) {
    TransformEqualityCoding0(raw_values,
                             flags);  // FIXME this might not be needed TBC
  } else {
    TransformEqualityCoding1(raw_values, flags);
  }
}

// -----------------------------------------------------------------------------
void InverseTransformEqualityCoding(
    std::vector<util::DataBlock>* transformed_subseqs) {
  assert(transformed_subseqs != nullptr);

  if (transformed_subseqs->size() != 2) {
    UTILS_DIE("invalid subseq count for equality inverse transform");
  }

  // Prepare internal and the output data structures
  const util::DataBlock* const flags = &(*transformed_subseqs)[0];
  const util::DataBlock* const raw_values = &(*transformed_subseqs)[1];
  util::DataBlock symbols(0, (raw_values->GetWordSize()));

  util::BlockStepper rflag = flags->GetReader();
  util::BlockStepper rval = raw_values->GetReader();

  // Re-compute the symbols from the equality kFlags and values
  uint64_t previous_symbol = 0;
  while (rflag.IsValid()) {
    if (rflag.Get() == 0) {
      const uint64_t val = rval.Get();
      rval.Inc();
      if (val >= previous_symbol) {
        previous_symbol = val + 1;
      } else {
        previous_symbol = val;
      }
    }

    symbols.PushBack(previous_symbol);
    rflag.Inc();
  }

  transformed_subseqs->resize(1);
  (*transformed_subseqs)[0].Clear();

  symbols.Swap(&(*transformed_subseqs)[0]);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
