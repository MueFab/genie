/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_CONTEXT_TABLES_H_
#define GABAC_CONTEXT_TABLES_H_

#include <vector>

#include "context-model.h"

namespace genie {
namespace entropy {
namespace gabac {
namespace contexttables {

std::vector<ContextModel> buildContextTable(unsigned long numContexts);

}  // namespace contexttables
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_CONTEXT_TABLES_H_
