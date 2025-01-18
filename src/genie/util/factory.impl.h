/**
 * Copyright 2018-2024 The Genie Authors.
 * @file factory.impl.h
 * @brief Implementation of the Factory class template for managing object
 * creation.
 *
 * This file contains the implementation of the Factory class template, which
 * provides mechanisms to register and create objects of different types. It
 * includes methods for registering constructors for specific types and creating
 * objects using the registered constructors.
 *
 * @details The Factory class template allows for the registration of
 * constructors for different types identified by an ID. It provides methods to
 * create objects using these constructors, ensuring that the correct
 * constructor is used based on the provided ID. If an unknown ID is
 * encountered, an exception is thrown.
 *
 * @tparam T The type of the objects being managed by the factory.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_FACTORY_IMPL_H_
#define SRC_GENIE_UTIL_FACTORY_IMPL_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <utility>

#include "genie/core/constants.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

template <typename T>
void Factory<T>::RegisterType(
    uint8_t id,
    const std::function<std::unique_ptr<T>(core::GenDesc desc, BitReader&)>&
        factory) {
  factories_.insert(std::make_pair(id, factory));
}

// -----------------------------------------------------------------------------

template <typename T>
std::unique_ptr<T> Factory<T>::Create(uint8_t id, core::GenDesc desc,
                                      BitReader& reader) {
  auto it = factories_.find(id);
  UTILS_DIE_IF(it == factories_.end(), "Unknown implementation in factory");
  return it->second(desc, reader);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_FACTORY_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
