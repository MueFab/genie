/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_CONTEXT_SELECTOR_H_
#define GABAC_CONTEXT_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/entropy/paramcabac/state_vars.h>
#include "context-tables.h"
#include "subsymbol.h"

#include <cassert>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
class ContextSelector {
   public:
    /**
     *
     * @param _stateVars
     */
    ContextSelector(const paramcabac::StateVars& _stateVars);

    /**
     *
     */
    ~ContextSelector() = default;

    /**
     *
     * @param subsymIdx
     * @return
     */
    unsigned int getContextIdxOrder0(const uint8_t subsymIdx);

    /**
     *
     * @param subsymIdx
     * @param prvIdx
     * @param subsymbols
     * @param codingOrder
     * @return
     */
    unsigned int getContextIdxOrderGT0(const uint8_t subsymIdx, const uint8_t prvIdx,
                                       const std::vector<Subsymbol>& subsymbols, const uint8_t codingOrder);

   private:
    const paramcabac::StateVars& stateVars;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_CONTEXT_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------