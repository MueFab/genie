/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONTEXT_TABLES_H_
#define SRC_GENIE_ENTROPY_GABAC_CONTEXT_TABLES_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/entropy/gabac/context-model.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {
namespace contexttables {

/**
 * @brief
 * @param numContexts
 * @return
 */
std::vector<ContextModel> buildContextTable(uint64_t numContexts);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contexttables
}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONTEXT_TABLES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
