#include "context-selector.h"

namespace genie {
namespace entropy {
namespace gabac {

ContextSelector::ContextSelector(const paramcabac::StateVars& _stateVars) : stateVars(_stateVars){}

unsigned int ContextSelector::getContextIdxOrder0(const uint8_t subsymIdx) { return subsymIdx * stateVars.getCodingSizeCtxOffset(); }

unsigned int ContextSelector::getContextIdxOrderGT0(const uint8_t subsymIdx, const uint8_t prvIdx,
                                   const std::vector<Subsymbol>& subsymbols, const uint8_t codingOrder) {
    unsigned int ctxIdx = 0;
    ctxIdx += stateVars.getNumCtxLUTs();
    ctxIdx += subsymIdx * stateVars.getCodingSizeCtxOffset();
    for (unsigned int i = 1; i <= codingOrder; i++) {
        ctxIdx += subsymbols[prvIdx].prvValues[i - 1] * stateVars.getCodingOrderCtxOffset(i);
    }

    return ctxIdx;
}

}
}
}