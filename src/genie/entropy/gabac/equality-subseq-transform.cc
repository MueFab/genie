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

#include "equality-subseq-transform.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include <genie/util/block-stepper.h>
#include <genie/util/data-block.h>

namespace genie {
namespace entropy {
namespace gabac {

// Optimized for wordsize 1. In place for equality flags
static void transformEqualityCoding0(util::DataBlock *const values, util::DataBlock *const equalityFlags) {
    uint64_t previousSymbol = 0;
    *equalityFlags = util::DataBlock(0, values->getWordSize());

    util::BlockStepper r = values->getReader();
    // Treat value as equalityFlags and vice versa
    while (r.isValid()) {
        uint64_t symbol = r.get();
        if (symbol == previousSymbol) {
            r.set(1);
        } else {
            r.set(0);
            if (symbol > previousSymbol) {
                equalityFlags->push_back(symbol - 1);
            } else {
                equalityFlags->push_back(symbol);
            }
            previousSymbol = symbol;
        }
        r.inc();
    }

    // Swap back before returning
    equalityFlags->swap(values);
}

// Optimized for wordsize 1 > 0. In place for values
static void transformEqualityCoding1(util::DataBlock *const values, util::DataBlock *const equalityFlags) {
    uint64_t previousSymbol = 0;

    *equalityFlags = util::DataBlock(0, 1);
    util::BlockStepper r = values->getReader();
    util::BlockStepper w = values->getReader();
    // Treat value as equalityFlags and vice versa
    while (r.isValid()) {
        uint64_t symbol = r.get();
        if (symbol == previousSymbol) {
            equalityFlags->push_back(1);
        } else {
            equalityFlags->push_back(0);
            if (symbol > previousSymbol) {
                w.set(symbol - 1);
            } else {
                w.set(symbol);
            }
            w.inc();
            previousSymbol = symbol;
        }
        r.inc();
    }

    values->resize(values->size() - (w.end - w.curr) / w.wordSize);
}

void transformEqualityCoding(util::DataBlock *const values, util::DataBlock *const equalityFlags) {
    assert(equalityFlags != nullptr);
    assert(values != nullptr);

    if (values->getWordSize() == 1) {
        transformEqualityCoding0(values, equalityFlags);
    } else {
        transformEqualityCoding1(values, equalityFlags);
    }

    values->swap(equalityFlags); // transformSubseq[0] = flags, transformSubseq[1] = values,
}

void inverseTransformEqualityCoding(util::DataBlock *const values, util::DataBlock *const equalityFlags) {
    assert(values != nullptr);
    assert(equalityFlags != nullptr);
    util::DataBlock output(0, values->getWordSize());
    util::DataBlock *outputptr;

    // Wordsize 1 allows in place operation in equality flag buffer
    if (values->getWordSize() == 1) {
        outputptr = equalityFlags;
    } else {
        // Other wordsizes have to use a distinct buffer
        outputptr = &output;
        output.resize(equalityFlags->size());
    }

    // Re-compute the symbols from the equality flags and values
    uint64_t previousSymbol = 0;

    util::BlockStepper rflag = equalityFlags->getReader();
    util::BlockStepper rval = values->getReader();
    util::BlockStepper rwrite = outputptr->getReader();

    while (rflag.isValid()) {
        if (rflag.get() == 0) {
            uint64_t val = rval.get();
            rval.inc();
            if (val >= previousSymbol) {
                previousSymbol = val + 1;
            } else {
                previousSymbol = val;
            }
        }

        rwrite.set(previousSymbol);
        rwrite.inc();
        rflag.inc();
    }
    outputptr->resize(outputptr->size() - (rwrite.end - rwrite.curr) / rwrite.wordSize);

    // Swap memory to value buffer to meet conventions
    if (values->getWordSize() == 1) {
        values->swap(equalityFlags);
    } else {
        values->swap(&output);
    }

    // Clear equality buffer
    equalityFlags->clear();
    equalityFlags->shrink_to_fit();
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie