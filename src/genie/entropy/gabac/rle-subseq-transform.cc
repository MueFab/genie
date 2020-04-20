/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <cassert>

#include <genie/util/block-stepper.h>
#include "rle-subseq-transform.h"
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

    util::DataBlock trnsfRawValues(0, rawValues->getWordSize());
    util::BlockStepper r = rawValues->getReader();

    uint64_t lastSym = 0;
    uint64_t runValue = 0;
    while (r.isValid()) {
        uint64_t curSym = r.get();
        r.inc();
        if (lastSym != curSym && runValue > 0) {
            trnsfRawValues.push_back(lastSym);

            while (runValue >= guard) {
                lengths->push_back(guard);
                runValue -= guard;
            }

            if (runValue > 0) {
                lengths->push_back(runValue - 1);
            }

            runValue = 0;
        }

        lastSym = curSym;
        runValue++;
    }

    if (runValue > 0) {
        trnsfRawValues.push_back(lastSym);

        while (runValue >= guard) {
            lengths->push_back(guard);
            runValue -= guard;
        }

        if (runValue > 0) {
            lengths->push_back(runValue - 1); // non-guard run-value is coded as lengthValue-1
        }

        runValue = 0;
    }

    rawValues->clear();
    rawValues->swap(&trnsfRawValues);

    rawValues->swap(lengths); // transformSubseq[0] = lengths, transformSubseq[1] = values
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
        uint64_t totalLengthValue = 0;
        uint64_t rawValue = rVal.get();
        rVal.inc();

        while (rLen.isValid()) {
            uint64_t runValue = rLen.get();
            rLen.inc();

            if (runValue == guard) {
                totalLengthValue += runValue;
            } else {
                totalLengthValue += runValue + 1; // non-guard run-value is coded as lengthValue-1
                break;
            }
        }

        while (totalLengthValue > 0) {
            totalLengthValue--;
            symbols.push_back(rawValue);
        }
    }

    rawValues->clear();
    lengths->clear();
    symbols.swap(lengths);
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
