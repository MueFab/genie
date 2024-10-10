/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_FACTORY_IMPL_H_
#define SRC_GENIE_UTIL_FACTORY_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <utility>
#include "genie/core/constants.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Factory<T>::registerType(
    uint8_t id, const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& factory) {
    factories.insert(std::make_pair(id, factory));
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
std::unique_ptr<T> Factory<T>::create(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader) {
    auto it = factories.find(id);
    UTILS_DIE_IF(it == factories.end(), "Unknown implementation in factory");
    return it->second(desc, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_FACTORY_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
