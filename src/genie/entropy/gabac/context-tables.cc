/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "context-tables.h"
#include<iostream>

namespace genie {
namespace entropy {
namespace gabac {
namespace contexttables {

std::vector<ContextModel> buildContextTable(unsigned long numContexts) {
    return std::vector<ContextModel>(numContexts);
}

}  // namespace contexttables
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
