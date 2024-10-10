/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/rle-subseq-transform.h"
#include <cassert>
#include <vector>
#include "genie/util/block-stepper.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

void transformRleCoding(const paramcabac::Subsequence &subseqCfg,
                        std::vector<util::DataBlock> *const transformedSubseqs) {
    assert(transformedSubseqs != nullptr);
    const auto guard = (uint8_t)subseqCfg.getTransformParameters().getParam();
    assert(guard > 0);
    auto wordsize = transformedSubseqs->front().getWordSize();

    // Prepare internal and the output data structures
    util::DataBlock symbols(0, 1);
    symbols.swap(&(*transformedSubseqs)[0]);
    (*transformedSubseqs)[0].clear();
    transformedSubseqs->resize(2);

    util::DataBlock *const lengths = &((*transformedSubseqs)[0]);
    util::DataBlock *const rawValues = &((*transformedSubseqs)[1]);
    lengths->setWordSize(1);
    rawValues->setWordSize(wordsize);

    util::BlockStepper r = symbols.getReader();
    uint64_t lastSym = 0;
    uint64_t runValue = 0;
    while (r.isValid()) {
        uint64_t curSym = r.get();
        r.inc();
        if (lastSym != curSym && runValue > 0) {
            rawValues->push_back(lastSym);

            while (runValue > guard) {
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
        rawValues->push_back(lastSym);

        while (runValue >= guard) {
            lengths->push_back(guard);
            runValue -= guard;
        }

        if (runValue > 0) {
            lengths->push_back(runValue - 1);  // non-guard run-value is coded as lengthValue-1
        }

        runValue = 0;
    }

    symbols.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void inverseTransformRleCoding(const paramcabac::Subsequence &subseqCfg,
                               std::vector<util::DataBlock> *const transformedSubseqs) {
    assert(transformedSubseqs != nullptr);

    if ((*transformedSubseqs).size() != 2) {
        UTILS_DIE("invalid subseq count for rle inverse transform");
    }

    const auto guard = (uint16_t)subseqCfg.getTransformParameters().getParam();
    assert(guard > 0);

    // Prepare internal and the output data structures
    util::DataBlock *const lengths = &((*transformedSubseqs)[0]);
    util::DataBlock *const rawValues = &((*transformedSubseqs)[1]);
    util::DataBlock symbols(0, (uint8_t)rawValues->getWordSize());

    util::BlockStepper rVal = rawValues->getReader();
    util::BlockStepper rLen = lengths->getReader();

    // Re-compute the symbols from the lengths and raw values
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
                totalLengthValue += runValue + 1;  // non-guard run-value is coded as lengthValue-1
                break;
            }
        }

        while (totalLengthValue > 0) {
            totalLengthValue--;
            symbols.push_back(rawValue);
        }
    }

    (*transformedSubseqs).resize(1);
    (*transformedSubseqs)[0].clear();

    symbols.swap(&(*transformedSubseqs)[0]);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
