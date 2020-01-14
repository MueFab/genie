/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "match-coding.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "util/block-stepper.h"
#include "util/data-block.h"

namespace genie {
namespace gabac {

void transformMatchCoding(const uint32_t windowSize, util::DataBlock *const symbols, util::DataBlock *const pointers,
                          util::DataBlock *const lengths) {
    assert(pointers != nullptr);
    assert(lengths != nullptr);
    assert(symbols != nullptr);
    util::DataBlock rawValues(0, symbols->getWordSize());

    // Prepare the output vectors
    pointers->clear();
    lengths->clear();

    if (windowSize == 0) {
        lengths->resize(symbols->size());
        std::fill(lengths->begin(), lengths->end(), 0);
        return;
    }

    // Do the match libs
    const uint64_t symbolsSize = symbols->size();
    for (uint64_t i = 0; i < symbolsSize; i++) {
        uint64_t pointer = 0;
        uint64_t length = 0;
        uint64_t windowStartIdx = i - windowSize;
        uint64_t windowEndIdx = i;

        for (uint64_t w = windowStartIdx; w < windowEndIdx; w++) {
            uint64_t offset = i;
            while ((offset < symbolsSize) && (symbols->get(offset) == (symbols->get(w + offset - i)))) {
                offset++;
            }
            offset -= i;
            if (offset >= length) {
                length = offset;
                pointer = w;
            }
        }
        if (length < 2) {
            lengths->push_back(0);
            rawValues.push_back(symbols->get(i));
        } else {
            pointers->push_back(i - pointer);
            lengths->push_back(length);
            i += (length - 1);
        }
    }

    rawValues.swap(symbols);
}

void inverseTransformMatchCoding(util::DataBlock *const rawValues, util::DataBlock *const pointers,
                                 util::DataBlock *const lengths) {
    util::DataBlock symbols(0, rawValues->getWordSize());
    assert(lengths->size() == pointers->size() + rawValues->size());

    // In-place probably not possible

    // Re-compute the symbols from the pointer, lengths and raw values
    size_t n = 0;
    util::BlockStepper t0 = pointers->getReader();
    util::BlockStepper t1 = lengths->getReader();
    util::BlockStepper t2 = rawValues->getReader();
    while (t1.isValid()) {
        uint64_t length = t1.get();
        t1.inc();
        if (length == 0) {
            symbols.push_back(t2.get());
            t2.inc();
            n++;
        } else {
            uint64_t pointer = t0.get();
            t0.inc();
            for (uint64_t l = 0; l < length; l++) {
                symbols.push_back(symbols.get(n - pointer));
                n++;
            }
        }
    }

    symbols.swap(rawValues);
    pointers->clear();
    pointers->shrink_to_fit();
    lengths->clear();
    lengths->shrink_to_fit();
}
}  // namespace gabac
}  // namespace genie
