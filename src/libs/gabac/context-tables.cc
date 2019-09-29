/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "context-tables.h"

namespace gabac {
namespace contexttables {

std::vector<ContextModel> buildContextTable() {
    std::vector<ContextModel> contextModels;

    for (const auto& contextSet : INIT_TRUNCATED_UNARY_CTX) {
        for (const auto j : contextSet) {
            ContextModel contextModel(j);
            contextModels.push_back(contextModel);
        }
    }

    for (const auto& contextSet : INIT_EXPONENTIAL_GOLOMB_CTX) {
        for (const auto j : contextSet) {
            ContextModel contextModel(j);
            contextModels.push_back(contextModel);
        }
    }

    for (const auto& contextSet : INIT_BINARY_CTX) {
        for (const auto j : contextSet) {
            ContextModel contextModel(j);
            contextModels.push_back(contextModel);
        }
    }

    return contextModels;
}

}  // namespace contexttables
}  // namespace gabac
