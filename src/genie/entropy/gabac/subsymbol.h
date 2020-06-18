/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_SUBSYMBOL_H_
#define GABAC_SUBSYMBOL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "context-tables.h"

#include <genie/entropy/paramcabac/state_vars.h>

#include <cassert>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
struct Subsymbol {
    uint8_t subsymIdx = 0;           //!<
    uint64_t subsymValue = 0;        //!<
    uint64_t prvValues[2] = {0, 0};  //!<
    uint64_t lutNumMaxElems = 0;     //!<
    uint64_t lutEntryIdx = 0;        //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_SUBSYMBOL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------