/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/context-selector.h"
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

ContextSelector::ContextSelector(const paramcabac::StateVars& _stateVars) : stateVars(_stateVars) {}

// ---------------------------------------------------------------------------------------------------------------------

ContextSelector::ContextSelector(const ContextSelector& src) = default;

// ---------------------------------------------------------------------------------------------------------------------

unsigned int ContextSelector::getContextIdxOrder0(const uint8_t subsymIdx) {
    return static_cast<unsigned int>(subsymIdx * stateVars.getCodingSizeCtxOffset());
}

// ---------------------------------------------------------------------------------------------------------------------

unsigned int ContextSelector::getContextIdxOrderGT0(const uint8_t subsymIdx, const uint8_t prvIdx,
                                                    const std::vector<Subsymbol>& subsymbols,
                                                    const uint8_t codingOrder) {
    unsigned int ctxIdx = 0;
    ctxIdx += stateVars.getNumCtxLUTs();
    ctxIdx += static_cast<unsigned int>(subsymIdx * stateVars.getCodingSizeCtxOffset());
    for (uint8_t i = 1; i <= codingOrder; i++) {
        ctxIdx += static_cast<unsigned int>(subsymbols[prvIdx].prvValues[i - 1] * stateVars.getCodingOrderCtxOffset(i));
    }

    return ctxIdx;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
