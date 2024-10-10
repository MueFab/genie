/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONTEXT_SELECTOR_H_
#define SRC_GENIE_ENTROPY_GABAC_CONTEXT_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cassert>
#include <vector>
#include "genie/entropy/gabac/context-tables.h"
#include "genie/entropy/gabac/subsymbol.h"
#include "genie/entropy/paramcabac/state_vars.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
class ContextSelector {
 public:
    /**
     * @brief
     * @param _stateVars
     */
    explicit ContextSelector(const paramcabac::StateVars& _stateVars);

    /**
     * @brief
     * @param src
     */
    ContextSelector(const ContextSelector& src);

    /**
     * @brief
     */
    ~ContextSelector() = default;

    /**
     * @brief
     * @param subsymIdx
     * @return
     */
    unsigned int getContextIdxOrder0(uint8_t subsymIdx);

    /**
     * @brief
     * @param subsymIdx
     * @param prvIdx
     * @param subsymbols
     * @param codingOrder
     * @return
     */
    unsigned int getContextIdxOrderGT0(uint8_t subsymIdx, uint8_t prvIdx, const std::vector<Subsymbol>& subsymbols,
                                       uint8_t codingOrder);

 private:
    const paramcabac::StateVars stateVars;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONTEXT_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
