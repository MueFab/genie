/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_
#define SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <utility>
#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
Factory<T>* IndustrialPark::FindAndCreate() {
    auto type = std::type_index(typeid(T));
    auto it = factories.find(type);
    if (it == factories.end()) {
        factories.insert(std::make_pair(type, util::make_unique<Factory<T>>()));
        it = factories.find(type);
    }
    return reinterpret_cast<Factory<T>*>(it->second.get());
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
Factory<T>* IndustrialPark::FindAndFail() const {
    auto type = std::type_index(typeid(T));
    auto it = factories.find(type);
    UTILS_DIE_IF(it == factories.end(), "Unknown factory type");
    return reinterpret_cast<Factory<T>*>(it->second.get());
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void IndustrialPark::RegisterConstructor(
    uint8_t id, const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor) {
    FindAndCreate<T>()->RegisterType(id, constructor);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
uint8_t IndustrialPark::RegisterConstructor(
    const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor) {
    return FindAndCreate<T>()->RegisterType(constructor);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void IndustrialPark::UnregisterConstructor(uint8_t id) {
    FindAndFail<T>()->UnregisterType(id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
std::unique_ptr<T> IndustrialPark::Construct(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader) const {
    return FindAndFail<T>()->Create(id, desc, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
