/**
 * Copyright 2018-2024 The Genie Authors.
 * @file industrial_park.impl.h
 * @brief Implementation of the IndustrialPark class template for managing
 * factories.
 *
 * This file contains the implementation of the IndustrialPark class template,
 * which provides mechanisms to manage and create factories for different types.
 * It includes methods for finding, creating, and registering constructors for
 * factories, as well as constructing objects using the registered constructors.
 *
 * @details The IndustrialPark class template provides methods to find and
 * create factories if they do not exist, register constructors for specific
 * types, and construct objects using the registered constructors. It ensures
 * that the correct factory is used for each type and handles unknown factory
 * types gracefully.
 *
 * @tparam T The type of the objects being managed by the factories.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_
#define SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <utility>

#include "genie/core/constants.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------
template <typename T>
Factory<T>* IndustrialPark::FindAndCreate() {
  auto type = std::type_index(typeid(T));
  auto it = factories_.find(type);
  if (it == factories_.end()) {
    factories_.insert(std::make_pair(type, std::make_unique<Factory<T>>()));
    it = factories_.find(type);
  }
  return reinterpret_cast<Factory<T>*>(it->second.get());
}

// -----------------------------------------------------------------------------
template <typename T>
Factory<T>* IndustrialPark::FindAndFail() const {
  const auto type = std::type_index(typeid(T));
  const auto it = factories_.find(type);
  UTILS_DIE_IF(it == factories_.end(), "Unknown factory type");
  return reinterpret_cast<Factory<T>*>(it->second.get());
}

// -----------------------------------------------------------------------------
template <typename T>
void IndustrialPark::RegisterConstructor(
    uint8_t id,
    const std::function<std::unique_ptr<T>(core::GenDesc desc, BitReader&)>&
        constructor) {
  FindAndCreate<T>()->RegisterType(id, constructor);
}

// -----------------------------------------------------------------------------
template <typename T>
std::unique_ptr<T> IndustrialPark::Construct(uint8_t id, core::GenDesc desc,
                                             BitReader& reader) const {
  return FindAndFail<T>()->Create(id, desc, reader);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_INDUSTRIAL_PARK_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
