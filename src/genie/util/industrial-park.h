/**
 * @file industrial-park.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the IndustrialPark class for managing multiple factory instances.
 *
 * This file contains the declaration of the `IndustrialPark` class, which serves as a central
 * registry for managing multiple factory instances of different types. It provides methods for
 * registering factory constructors and creating objects using these factories based on unique IDs.
 *
 * @details The `IndustrialPark` class allows for dynamic registration and lookup of factory objects
 * using type indices, enabling a modular and extensible factory management system. The class supports
 * type-safe registration and creation of objects using a unified interface, making it suitable for
 * complex object creation scenarios.
 */

#ifndef SRC_GENIE_UTIL_INDUSTRIAL_PARK_H_
#define SRC_GENIE_UTIL_INDUSTRIAL_PARK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <functional>
#include <map>
#include <memory>
#include <typeindex>
#include "genie/util/bit-reader.h"
#include "genie/util/factory.h"
#include "genie/util/generic-factory.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Central registry for managing factory instances and creating objects.
 *
 * The `IndustrialPark` class acts as a container for managing multiple factory instances,
 * each associated with a different type. It provides an interface for registering new
 * factory constructors, finding factories based on type, and creating objects using
 * factory methods. The class supports dynamic type-safe registration and lookup, enabling
 * extensible and flexible object creation.
 */
class IndustrialPark {
 private:
    /**
     * @brief Stores the registered factory instances.
     *
     * The `factories` map uses `std::type_index` as the key to uniquely identify each factory
     * type. The value is a `std::unique_ptr` to a `GenericFactory` object, enabling polymorphic
     * storage of different factory types.
     */
    std::map<std::type_index, std::unique_ptr<GenericFactory>> factories;  //!< @brief Map of factory instances.

    /**
     * @brief Finds or creates a factory of the specified type.
     *
     * This template function checks if a factory of type `T` is already registered.
     * If the factory is not found, it creates a new `Factory<T>` instance, stores it in the
     * `factories` map, and returns a pointer to it.
     *
     * @tparam T The type of the factory to find or create.
     * @return A pointer to the factory of type `T`.
     */
    template <typename T>
    Factory<T>* findAndCreate();

    /**
     * @brief Finds a factory of the specified type.
     *
     * This template function searches for a factory of type `T` in the `factories` map.
     * If the factory is not found, it throws a runtime exception indicating that the factory
     * is missing.
     *
     * @tparam T The type of the factory to find.
     * @return A pointer to the factory of type `T`.
     * @throws RuntimeException if the factory is not found.
     */
    template <typename T>
    Factory<T>* findAndFail() const;

 public:
    /**
     * @brief Registers a new factory constructor for the specified type.
     *
     * This function allows for registering a factory constructor for creating objects
     * of type `T` using a unique identifier (`id`). The constructor function should
     * accept a `GenDesc` descriptor and a `BitReader` reference as parameters, returning
     * a `std::unique_ptr<T>`.
     *
     * @tparam T The type of objects that the factory will create.
     * @param id The unique identifier for the type to be registered.
     * @param constructor The factory constructor function.
     */
    template <typename T>
    void registerConstructor(
        uint8_t id, const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor);

    /**
     * @brief Creates an object using the registered factory for the given type and ID.
     *
     * This function uses the registered factory for type `T` to create an object using
     * the provided unique identifier (`id`), `GenDesc` descriptor, and `BitReader` reference.
     * If the factory for the given type is not found, it throws a runtime exception.
     *
     * @tparam T The type of the object to create.
     * @param id The unique identifier for the type to create.
     * @param desc The descriptor used for object creation.
     * @param reader The bit reader providing additional data for object construction.
     * @return A `std::unique_ptr` to the created object of type `T`.
     * @throws RuntimeException if the factory for the specified type is not found.
     */
    template <typename T>
    std::unique_ptr<T> construct(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/industrial-park.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_INDUSTRIAL_PARK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
