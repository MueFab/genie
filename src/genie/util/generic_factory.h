/**
 * Copyright 2018-2024 The Genie Authors.
 * @file generic_factory.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the GenericFactory base class for factory pattern
 * implementation.
 *
 * This file contains the declaration of the `GenericFactory` class, which
 * serves as an abstract base class for implementing the factory pattern. It
 * provides a virtual destructor to ensure proper cleanup of derived classes.
 * The `GenericFactory` class is intended to be inherited by other factory
 * classes to support polymorphic behavior.
 *
 * @details The `GenericFactory` base class offers a common interface for all
 * factories within the GENIE framework. This allows for a unified management of
 * different factory types, enabling dynamic creation and registration of
 * objects.
 */

#ifndef SRC_GENIE_UTIL_GENERIC_FACTORY_H_
#define SRC_GENIE_UTIL_GENERIC_FACTORY_H_

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Abstract base class for implementing the factory pattern.
 *
 * The `GenericFactory` class is a simple base class that provides a common
 * interface for all factory classes. It includes a virtual destructor, ensuring
 * that derived classes can be safely destructed through a pointer to the
 * `GenericFactory` interface. This class does not define any specific factory
 * methods and serves only as a base for implementing concrete factory classes.
 */
class GenericFactory {
 public:
  /**
   * @brief Virtual destructor for safe polymorphic deletion.
   *
   * This destructor ensures that derived factory classes can be properly
   * destructed when handled through a pointer to `GenericFactory`. It enables
   * safe cleanup of resources in factory patterns where base pointers are
   * used.
   */
  virtual ~GenericFactory() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_GENERIC_FACTORY_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
