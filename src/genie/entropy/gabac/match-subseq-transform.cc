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
#include "genie/util/block-stepper.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

void transformMatchCoding(const paramcabac::Subsequence &subseqCfg,
                          std::vector<util::DataBlock> *const transformedSubseqs) {
    assert(transformedSubseqs != nullptr);
    const uint16_t matchBufferSize = subseqCfg.getTransformParameters().getParam();
    uint8_t wordsize = transformedSubseqs->front().getWordSize();
    // Prepare internal and the output data structures
    util::DataBlock symbols(0, 1);
    symbols.swap(&(*transformedSubseqs)[0]);
    (*transformedSubseqs)[0].clear();
    transformedSubseqs->resize(3);

    util::DataBlock *const pointers = &((*transformedSubseqs)[0]);
    util::DataBlock *const lengths = &((*transformedSubseqs)[1]);
    util::DataBlock *const rawValues = &((*transformedSubseqs)[2]);
    pointers->setWordSize(core::range2bytes({0, matchBufferSize}));
    lengths->setWordSize(core::range2bytes({0, matchBufferSize}));
    rawValues->setWordSize(wordsize);

    const uint64_t symbolsSize = symbols.size();

    if (matchBufferSize == 0) {
        lengths->resize(symbolsSize);
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
                   (symbols.get(offset) == (symbols.get(w + offset - i)) && (offset - i) < (matchBufferSize - 1u))) {
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
            rawValues->push_back(symbols.get(i));
        } else {
            pointers->push_back(i - pointer);
            lengths->push_back(length);
            i += (length - 1);
        }
    }

    symbols.clear();
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

    util::DataBlock symbols(0, (uint8_t)rawValues->getWordSize());
    assert(lengths->size() == pointers->size() + rawValues->size());

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

    (*transformedSubseqs).resize(1);
    (*transformedSubseqs)[0].clear();

    symbols.swap(&(*transformedSubseqs)[0]);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
