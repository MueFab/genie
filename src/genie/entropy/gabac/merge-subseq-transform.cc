/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/merge-subseq-transform.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include "genie/util/block-stepper.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

void transformMergeCoding(const paramcabac::Subsequence& subseqCfg,
                          std::vector<util::DataBlock>* const transformedSubseqs) {
    assert(transformedSubseqs != nullptr);
    const paramcabac::TransformedParameters& trnsfSubseqParams = subseqCfg.getTransformParameters();
    const uint16_t subseqCount = trnsfSubseqParams.getParam();
    const std::vector<uint8_t> subseqShiftSizes = trnsfSubseqParams.getMergeCodingShiftSizes();
    const std::vector<paramcabac::TransformedSubSeq>& trnsfCfgs = subseqCfg.getTransformSubseqCfgs();

    if (subseqCount <= 0) {
        UTILS_DIE("invalid subseq_count for merge coding");
    }

    // Prepare internal and output data structures
    util::DataBlock symbols(0, 1);
    symbols.swap(&(*transformedSubseqs)[0]);
    (*transformedSubseqs)[0].clear();
    transformedSubseqs->resize(subseqCount);

    const uint64_t symbolsCount = symbols.size();
    std::vector<uint64_t> trnsSubseqMasks(subseqCount, 0);
    for (uint64_t ts = 0; ts < subseqCount; ts++) {
        const uint8_t codingSubsymSize = trnsfCfgs[ts].getSupportValues().getCodingSubsymSize();
        trnsSubseqMasks[ts] = (1u << codingSubsymSize) - 1;
        (*transformedSubseqs)[ts].setWordSize(4);
        (*transformedSubseqs)[ts].resize(symbolsCount);
    }

    // split
    for (uint64_t i = 0; i < symbolsCount; i++) {
        uint64_t symbolValue = symbols.get(i);
        int64_t signedSymbolValue = paramcabac::StateVars::getSignedValue(symbolValue, symbols.getWordSize());

        bool isNegative = false;
        if (signedSymbolValue < 0) {
            isNegative = true;
            symbolValue = abs(signedSymbolValue);
        }

        for (uint64_t ts = 0; ts < subseqCount; ts++) {
            uint64_t trnsfSymbol = (symbolValue >> subseqShiftSizes[ts]) & trnsSubseqMasks[ts];
            if (isNegative && trnsfSymbol != 0) {
                int64_t trnsfSymbolSigned = -static_cast<int64_t>(trnsfSymbol);
                trnsfSymbol = static_cast<uint64_t>(trnsfSymbolSigned);
                isNegative = false;
            }

            (*transformedSubseqs)[ts].set(i, trnsfSymbol);
        }
    }

    symbols.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void inverseTransformMergeCoding(const paramcabac::Subsequence& subseqCfg,
                                 std::vector<util::DataBlock>* const transformedSubseqs) {
    const paramcabac::TransformedParameters& trnsfSubseqParams = subseqCfg.getTransformParameters();
    const uint16_t subseqCount = trnsfSubseqParams.getParam();
    const std::vector<uint8_t> subseqShiftSizes = trnsfSubseqParams.getMergeCodingShiftSizes();

    if (subseqCount <= 0 || subseqCount != (*transformedSubseqs).size()) {
        UTILS_DIE("invalid subseq_count for merge inverse transform");
    }

    // Prepare the output data structure
    const uint64_t symbolsCount = (*transformedSubseqs)[0].size();
    util::DataBlock symbols(symbolsCount, 4);

    for (uint64_t i = 0; i < symbolsCount; i++) {
        uint64_t symbolValue = 0;
        bool isNegative = false;
        for (uint64_t ts = 0; ts < subseqCount; ts++) {
            uint64_t decodedTrnsfSymbol = (*transformedSubseqs)[ts].get(i);
            if (static_cast<int64_t>(decodedTrnsfSymbol) < 0) {
                decodedTrnsfSymbol = -(static_cast<int64_t>(decodedTrnsfSymbol));
                isNegative = true;
            }

            symbolValue = (symbolValue << subseqShiftSizes[ts]) | decodedTrnsfSymbol;
        }

        if (isNegative) {
            int64_t symbolValueSigned = -static_cast<int64_t>(symbolValue);
            symbolValue = static_cast<uint64_t>(symbolValueSigned);
        }

        symbols.set(i, symbolValue);
    }

    (*transformedSubseqs).resize(1);
    (*transformedSubseqs)[0].clear();

    symbols.swap(&(*transformedSubseqs)[0]);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
