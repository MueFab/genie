/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// Equality coding is using a small hack to work as efficiently as possible in place. There are two implementations, one
// for wordsize 1 and one for other wordsizes. With wordsize 1, it is exploited, that there are as many equality flags
// as input symbol, so this version does only need to allocate memory for the raw values stream. To achieve this, the
// equality flags are written into the raw_values data block (containing the input values) and vice versa and swapped
// before returning. A user from outside will note notice this, but internally it is important to know that these two
// util::DataBlocks swap meaning. For wordsizes greater 1 this optimization is not possible, as the raw values do not
// fit into the same data block where input values are still inside (different word sizes)

#include "genie/entropy/gabac/equality-subseq-transform.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include "genie/util/block_stepper.h"
#include "genie/util/data_block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

// Optimized for wordsize 1. In place for equality flags
static void transformEqualityCoding0(util::DataBlock *const values, util::DataBlock *const equalityFlags) {
    uint64_t previousSymbol = 0;

    util::BlockStepper r = values->GetReader();
    // Treat value as equalityFlags and vice versa
    while (r.IsValid()) {
        uint64_t symbol = r.Get();
        if (symbol == previousSymbol) {
            r.Set(1);
        } else {
            r.Set(0);
            if (symbol > previousSymbol) {
                equalityFlags->PushBack(symbol - 1);
            } else {
                equalityFlags->PushBack(symbol);
            }
            previousSymbol = symbol;
        }
        r.Inc();
    }

    // Swap back before returning
    equalityFlags->Swap(values);
}

// ---------------------------------------------------------------------------------------------------------------------

// Optimized for wordsize 1 > 0. In place for values
static void transformEqualityCoding1(util::DataBlock *const values, util::DataBlock *const equalityFlags) {
    uint64_t previousSymbol = 0;

    util::BlockStepper r = values->GetReader();
    util::BlockStepper w = values->GetReader();
    // Treat value as equalityFlags and vice versa
    while (r.IsValid()) {
        uint64_t symbol = r.Get();
        if (symbol == previousSymbol) {
            equalityFlags->PushBack(1);
        } else {
            equalityFlags->PushBack(0);
            if (symbol > previousSymbol) {
                w.Set(symbol - 1);
            } else {
                w.Set(symbol);
            }
            w.Inc();
            previousSymbol = symbol;
        }
        r.Inc();
    }

    values->Resize(values->Size() - (w.end - w.curr) / w.word_size);
}

// ---------------------------------------------------------------------------------------------------------------------

void transformEqualityCoding(std::vector<util::DataBlock> *const transformedSubseqs) {
    // Prepare internal and the output data structures
    uint8_t wordsize = transformedSubseqs->front().GetWordSize();
    transformedSubseqs->resize(2);
    (*transformedSubseqs)[0].Swap(
        &(*transformedSubseqs)[1]);  // transformSubseq[0] = flags, transformSubseq[1] = values
    util::DataBlock *const flags = &((*transformedSubseqs)[0]);
    util::DataBlock *const rawValues = &((*transformedSubseqs)[1]);

    flags->SetWordSize(1);
    rawValues->SetWordSize(wordsize);

    if (rawValues->GetWordSize() == 1) {
        transformEqualityCoding0(rawValues, flags);  // FIXME this might not be needed TBC
    } else {
        transformEqualityCoding1(rawValues, flags);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverseTransformEqualityCoding(std::vector<util::DataBlock> *const transformedSubseqs) {
    assert(transformedSubseqs != nullptr);

    if ((*transformedSubseqs).size() != 2) {
        UTILS_DIE("invalid subseq count for equality inverse transform");
    }

    // Prepare internal and the output data structures
    util::DataBlock *const flags = &((*transformedSubseqs)[0]);
    util::DataBlock *const rawValues = &((*transformedSubseqs)[1]);
    util::DataBlock symbols(0, (uint8_t)rawValues->GetWordSize());

    util::BlockStepper rflag = flags->GetReader();
    util::BlockStepper rval = rawValues->GetReader();

    // Re-compute the symbols from the equality flags and values
    uint64_t previousSymbol = 0;
    while (rflag.IsValid()) {
        if (rflag.Get() == 0) {
            uint64_t val = rval.Get();
            rval.Inc();
            if (val >= previousSymbol) {
                previousSymbol = val + 1;
            } else {
                previousSymbol = val;
            }
        }

        symbols.PushBack(previousSymbol);
        rflag.Inc();
    }

    (*transformedSubseqs).resize(1);
    (*transformedSubseqs)[0].Clear();

    symbols.Swap(&(*transformedSubseqs)[0]);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
