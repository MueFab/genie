/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "diff-coding.h"

#include <algorithm>
#include <cassert>

#include "util/block-stepper.h"
#include "util/data-block.h"

namespace genie {
namespace gabac {

void transformDiffCoding(util::DataBlock *const transformedSymbols) {
    assert(transformedSymbols != nullptr);

    // Do the diff libs
    uint64_t previousSymbol = 0;
    util::BlockStepper r = transformedSymbols->getReader();
    while (r.isValid()) {
        uint64_t symbol = r.get();
        uint64_t diff = symbol - previousSymbol;
        r.set(diff);
        previousSymbol = symbol;
        r.inc();
    }
}

void inverseTransformDiffCoding(util::DataBlock *const symbols) {
    assert(symbols != nullptr);

    // Re-compute the symbols from the differences
    uint64_t previousSymbol = 0;
    util::BlockStepper r = symbols->getReader();
    while (r.isValid()) {
        uint64_t symbol = r.get();
        r.set(previousSymbol + symbol);
        previousSymbol = previousSymbol + symbol;
        r.inc();
    }
}
}  // namespace gabac
}  // namespace genie
