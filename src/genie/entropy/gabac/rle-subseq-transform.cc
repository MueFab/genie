/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/gabac/rle-subseq-transform.h"
#include <cassert>
#include <vector>
#include "genie/util/block_stepper.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

void transformRleCoding(const paramcabac::Subsequence &subseqCfg,
                        std::vector<util::DataBlock> *const transformedSubseqs) {
    assert(transformedSubseqs != nullptr);
    const auto guard = (uint8_t)subseqCfg.getTransformParameters().getParam();
    assert(guard > 0);
    auto wordsize = transformedSubseqs->front().GetWordSize();

    // Prepare internal and the output data structures
    util::DataBlock symbols(0, 1);
    symbols.Swap(&(*transformedSubseqs)[0]);
    (*transformedSubseqs)[0].Clear();
    transformedSubseqs->resize(2);

    util::DataBlock *const lengths = &((*transformedSubseqs)[0]);
    util::DataBlock *const rawValues = &((*transformedSubseqs)[1]);
    lengths->SetWordSize(1);
    rawValues->SetWordSize(wordsize);

    util::BlockStepper r = symbols.GetReader();
    uint64_t lastSym = 0;
    uint64_t runValue = 0;
    while (r.IsValid()) {
        uint64_t curSym = r.Get();
        r.Inc();
        if (lastSym != curSym && runValue > 0) {
            rawValues->PushBack(lastSym);

            while (runValue > guard) {
                lengths->PushBack(guard);
                runValue -= guard;
            }

            if (runValue > 0) {
                lengths->PushBack(runValue - 1);
            }

            runValue = 0;
        }

        lastSym = curSym;
        runValue++;
    }

    if (runValue > 0) {
        rawValues->PushBack(lastSym);

        while (runValue >= guard) {
            lengths->PushBack(guard);
            runValue -= guard;
        }

        if (runValue > 0) {
            lengths->PushBack(runValue - 1);  // non-guard run-value is coded as lengthValue-1
        }

        runValue = 0;
    }

    symbols.Clear();
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
    util::DataBlock symbols(0, (uint8_t)rawValues->GetWordSize());

    util::BlockStepper rVal = rawValues->GetReader();
    util::BlockStepper rLen = lengths->GetReader();

    // Re-compute the symbols from the lengths and raw values
    while (rVal.IsValid()) {
        uint64_t totalLengthValue = 0;
        uint64_t rawValue = rVal.Get();
        rVal.Inc();

        while (rLen.IsValid()) {
            uint64_t runValue = rLen.Get();
            rLen.Inc();

            if (runValue == guard) {
                totalLengthValue += runValue;
            } else {
                totalLengthValue += runValue + 1;  // non-guard run-value is coded as lengthValue-1
                break;
            }
        }

        while (totalLengthValue > 0) {
            totalLengthValue--;
            symbols.PushBack(rawValue);
        }
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
