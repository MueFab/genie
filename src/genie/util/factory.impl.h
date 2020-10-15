/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FACTORY_IMPL_H
#define GENIE_FACTORY_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------

#include "runtime-exception.h"
#include <genie/core/constants.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

using namespace genie::core;

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Factory<T>::registerType(uint8_t id, const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& factory) {
    factories.insert(std::make_pair(id, factory));
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
uint8_t Factory<T>::registerType(const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& factory) {
    uint8_t id = 0;
    auto it = factories.find(id);
    while (it != factories.end()) {
        id++;
        it = factories.find(id);
    }
    factories.insert(std::make_pair(id, factory));
    return id;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Factory<T>::unregisterType(uint8_t id) {
    factories.erase(id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
std::unique_ptr<T> Factory<T>::create(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader) {
    auto it = factories.find(id);
    UTILS_DIE_IF(it == factories.end(), "Unknown implementation in factory");
    return it->second(desc, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FACTORY_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------