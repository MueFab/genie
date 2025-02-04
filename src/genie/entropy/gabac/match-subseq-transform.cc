/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/gabac/match-subseq-transform.h"
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

void transformMatchCoding(const paramcabac::Subsequence &subseqCfg,
                          std::vector<util::DataBlock> *const transformedSubseqs) {
    assert(transformedSubseqs != nullptr);
    const uint16_t matchBufferSize = subseqCfg.getTransformParameters().getParam();
    uint8_t wordsize = transformedSubseqs->front().GetWordSize();
    // Prepare internal and the output data structures
    util::DataBlock symbols(0, 1);
    symbols.Swap(&(*transformedSubseqs)[0]);
    (*transformedSubseqs)[0].Clear();
    transformedSubseqs->resize(3);

    util::DataBlock *const pointers = &((*transformedSubseqs)[0]);
    util::DataBlock *const lengths = &((*transformedSubseqs)[1]);
    util::DataBlock *const rawValues = &((*transformedSubseqs)[2]);
    pointers->SetWordSize(core::range2bytes({0, matchBufferSize}));
    lengths->SetWordSize(core::range2bytes({0, matchBufferSize}));
    rawValues->SetWordSize(wordsize);

    const uint64_t symbolsSize = symbols.Size();

    if (matchBufferSize == 0) {
        lengths->Resize(symbolsSize);
        std::fill(lengths->begin(), lengths->end(), 0);
        return;
    }

    // Perform the match transformation
    for (uint64_t i = 0; i < symbolsSize; i++) {
        uint64_t pointer = 0;
        uint64_t length = 0;
        uint64_t windowStartIdx = i - matchBufferSize;
        uint64_t windowEndIdx = i;

        for (uint64_t w = windowStartIdx; w < windowEndIdx; w++) {
            uint64_t offset = i;
            while ((offset < symbolsSize) &&
                   (symbols.Get(offset) == (symbols.Get(w + offset - i)) && (offset - i) < (matchBufferSize - 1u))) {
                offset++;
            }
            offset -= i;
            if (offset >= length) {
                length = offset;
                pointer = w;
            }
        }
        if (length < 2) {
            lengths->PushBack(0);
            rawValues->PushBack(symbols.Get(i));
        } else {
            pointers->PushBack(i - pointer);
            lengths->PushBack(length);
            i += (length - 1);
        }
    }

    symbols.Clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void inverseTransformMatchCoding(std::vector<util::DataBlock> *const transformedSubseqs) {
    assert(transformedSubseqs != nullptr);

    if ((*transformedSubseqs).size() != 3) {
        UTILS_DIE("invalid subseq count for match inverse transform");
    }

    // Prepare internal and the output data structures
    util::DataBlock *const pointers = &((*transformedSubseqs)[0]);
    util::DataBlock *const lengths = &((*transformedSubseqs)[1]);
    util::DataBlock *const rawValues = &((*transformedSubseqs)[2]);

    util::DataBlock symbols(0, (uint8_t)rawValues->GetWordSize());
    assert(lengths->Size() == pointers->Size() + rawValues->Size());

    // Re-compute the symbols from the pointer, lengths and raw values
    size_t n = 0;
    util::BlockStepper t0 = pointers->GetReader();
    util::BlockStepper t1 = lengths->GetReader();
    util::BlockStepper t2 = rawValues->GetReader();
    while (t1.IsValid()) {
        uint64_t length = t1.Get();
        t1.Inc();
        if (length == 0) {
            symbols.PushBack(t2.Get());
            t2.Inc();
            n++;
        } else {
            uint64_t pointer = t0.Get();
            t0.Inc();
            for (uint64_t l = 0; l < length; l++) {
                symbols.PushBack(symbols.Get(n - pointer));
                n++;
            }
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
