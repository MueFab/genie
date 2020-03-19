/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_CONTEXT_SELECTOR_H_
#define GABAC_CONTEXT_SELECTOR_H_

#include "context-tables.h"

#include <genie/entropy/paramcabac/state_vars.h>

#include <cassert>

namespace genie {
namespace entropy {
namespace gabac {

struct Subsymbol {
    uint8_t subsymIdx;
    uint32_t prvValues[2];
};

class ContextSelector {
   public:
    ContextSelector() = default;

    ~ContextSelector() = default;

    unsigned int getContextIdx(const paramcabac::StateVars &stateVars,
                               bool bypassFlag,
                               uint8_t codingOrder,
                               const Subsymbol &subsym) {
        unsigned int ctxIdx = 0;
        if (!bypassFlag) {
            if (codingOrder > 0) {
                ctxIdx += stateVars.getNumCtxLUTs();
                ctxIdx += subsym.subsymIdx * stateVars.getCodingSizeCtxOffset();
                for (unsigned int i = 1; i <= codingOrder; i++) {
                    ctxIdx += subsym.prvValues[i - 1] * stateVars.getCodingOrderCtxOffset(i);
                }
            } else {
                ctxIdx += subsym.subsymIdx * stateVars.getCodingSizeCtxOffset();
            }
        }

        return ctxIdx;
    }
};

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_CONTEXT_SELECTOR_H_
