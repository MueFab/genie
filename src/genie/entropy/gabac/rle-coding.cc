/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "rle-coding.h"

#include <cassert>

#include <genie/util/block-stepper.h>
namespace genie {
namespace entropy {
namespace gabac {

void transformRleCoding(const uint64_t guard, util::DataBlock *const rawValues, util::DataBlock *const lengths) {
    assert(guard > 0);
    assert(rawValues != nullptr);
    assert(lengths != nullptr);

    lengths->clear();

    if (rawValues->empty()) {
        return;
    }

    // input for rawValues is guaranteed to grow slower than reading process
    // -> in place possible

    util::BlockStepper r = rawValues->getReader();
    util::BlockStepper w = rawValues->getReader();

    uint64_t cur = r.get();
    r.inc();
    uint64_t lengthValue = 1;
    while (r.isValid()) {
        uint64_t tmp = r.get();
        r.inc();
        if (tmp == cur) {
            ++lengthValue;
        } else {
            w.set(cur);
            w.inc();
            cur = tmp;
            while (lengthValue > guard) {
                lengths->push_back(guard);
                lengthValue -= guard;
            }
            lengths->push_back(lengthValue - 1);
            lengthValue = 1;
        }
    }

    w.set(cur);
    w.inc();
    while (lengthValue > guard) {
        lengths->push_back(guard);
        lengthValue -= guard;
    }
    lengths->push_back(lengthValue - 1);

    rawValues->resize(rawValues->size() - (w.end - w.curr) / w.wordSize);
}

void inverseTransformRleCoding(const uint64_t guard, util::DataBlock *const rawValues, util::DataBlock *const lengths) {
    assert(rawValues != nullptr);
    assert(!rawValues->empty());
    assert(guard > 0);

    // input for rawValues is not guaranteed to grow slower than reading process
    // -> in place not possible

    util::DataBlock symbols(0, rawValues->getWordSize());

    util::BlockStepper rVal = rawValues->getReader();
    util::BlockStepper rLen = lengths->getReader();
    // Re-compute the symbol sequence
    while (rVal.isValid()) {
        uint64_t rawValue = rVal.get();
        uint64_t lengthValue = rLen.get();
        rLen.inc();
        uint64_t totalLengthValue = lengthValue;
        while (lengthValue == guard) {
            lengthValue = rLen.get();
            rLen.inc();
            totalLengthValue += lengthValue;
        }
        totalLengthValue++;
        while (totalLengthValue > 0) {
            totalLengthValue--;
            symbols.push_back(rawValue);
        }
        rVal.inc();
    }

    symbols.swap(rawValues);
    lengths->clear();
    lengths->shrink_to_fit();
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie