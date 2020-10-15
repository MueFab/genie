/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_INDUSTRIAL_PARK_IMPL_H
#define GENIE_INDUSTRIAL_PARK_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------
#include <genie/core/constants.h>

namespace genie {
namespace util {

using namespace genie::core;

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
Factory<T>* IndustrialPark::findAndCreate() {
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
Factory<T>* IndustrialPark::findAndFail() const {
    auto type = std::type_index(typeid(T));
    auto it = factories.find(type);
    UTILS_DIE_IF(it == factories.end(), "Unknown factory type");
    return reinterpret_cast<Factory<T>*>(it->second.get());
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void IndustrialPark::registerConstructor(uint8_t id,
                                         const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor) {
    findAndCreate<T>()->registerType(id, constructor);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
uint8_t IndustrialPark::registerConstructor(const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor) {
    return findAndCreate<T>()->registerType(constructor);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void IndustrialPark::unregisterConstructor(uint8_t id) {
    findAndFail<T>()->unregisterType(id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
std::unique_ptr<T> IndustrialPark::construct(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader) const {
    return findAndFail<T>()->create(id, desc, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_INDUSTRIAL_PARK_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------