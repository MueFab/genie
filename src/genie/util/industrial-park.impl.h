/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_
#define SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <utility>
#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
Factory<T>* IndustrialPark::findAndCreate() {
    auto type = std::type_index(typeid(T));
    auto it = factories.find(type);
    if (it == factories.end()) {
        factories.insert(std::make_pair(type, std::make_unique<Factory<T>>()));
        it = factories.find(type);
    }
    return reinterpret_cast<Factory<T>*>(it->second.get());
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
Factory<T>* IndustrialPark::findAndFail() const {
    auto type = std::type_index(typeid(T));
    auto it = factories.find(type);
    UTILS_DIE_IF(it == factories.end(), "Unknown factory type");
    return reinterpret_cast<Factory<T>*>(it->second.get());
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void IndustrialPark::registerConstructor(
    uint8_t id, const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor) {
    findAndCreate<T>()->registerType(id, constructor);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
std::unique_ptr<T> IndustrialPark::construct(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader) const {
    return findAndFail<T>()->create(id, desc, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
