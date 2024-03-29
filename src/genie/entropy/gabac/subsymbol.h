/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_SUBSYMBOL_H_
#define SRC_GENIE_ENTROPY_GABAC_SUBSYMBOL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cassert>
#include "genie/entropy/gabac/context-tables.h"
#include "genie/entropy/paramcabac/state_vars.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief
 */
struct Subsymbol {
    uint8_t subsymIdx = 0;           //!< @brief
    uint64_t subsymValue = 0;        //!< @brief
    uint64_t prvValues[2] = {0, 0};  //!< @brief
    uint64_t lutNumMaxElems = 0;     //!< @brief
    uint64_t lutEntryIdx = 0;        //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_SUBSYMBOL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
