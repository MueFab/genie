/**
 * @file factory.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Factory class template for creating objects using a generic factory pattern.
 *
 * This file contains the declaration of the Factory class template, which provides a mechanism for
 * creating instances of various types based on an identifier. It uses a registry-based approach to
 * associate unique IDs with factory functions, allowing for flexible object instantiation.
 *
 * @details The Factory class is built on top of the `GenericFactory` base class and utilizes
 * `std::function` to store factory methods for different types. It supports registering new types
 * and creating instances based on an identifier and additional parameters like descriptors and
 * bit readers. This design is useful for modular systems where object creation needs to be
 * dynamically controlled.
 */

#ifndef SRC_GENIE_UTIL_FACTORY_H_
#define SRC_GENIE_UTIL_FACTORY_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <memory>
#include "genie/core/constants.h"
#include "genie/util/generic-factory.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief A template-based factory class for creating objects of a specified type.
 *
 * The `Factory` class template provides a generic factory pattern for creating objects of type `T`.
 * It allows for registering custom factory functions mapped to unique identifiers, enabling the
 * creation of objects dynamically based on their ID. The class supports flexible instantiation
 * using descriptors and bit readers, making it suitable for complex data parsing and processing tasks.
 *
 * @tparam T The type of objects this factory will create.
 */
template <typename T>
class Factory : public GenericFactory {
 private:
    /**
     * @brief A map storing factory functions for different types.
     *
     * The `factories` map associates a unique identifier (`uint8_t`) with a factory function.
     * Each factory function is responsible for creating a `std::unique_ptr` to an object of type `T`
     * using the given parameters.
     */
    std::map<uint8_t, std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>>
        factories;  //!< @brief Stores the registered factory functions for each type ID.

 public:
    /**
     * @brief Registers a new type with its corresponding factory function.
     *
     * This function allows for adding a new type to the factory by associating a unique identifier
     * with a specific factory function. The registered factory function should be capable of
     * creating an object of type `T` based on the provided `GenDesc` descriptor and `BitReader`.
     *
     * @param id The unique identifier for the type to be registered.
     * @param factory The factory function for creating instances of the type.
     */
    void registerType(uint8_t id,
                      const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& factory);

    /**
     * @brief Creates an object of a registered type based on the given ID.
     *
     * This function attempts to create an object of type `T` using the factory function
     * associated with the provided ID. It uses the `GenDesc` descriptor and `BitReader`
     * parameters to initialize the object as required.
     *
     * @param id The unique identifier of the type to create.
     * @param desc The descriptor used for object creation.
     * @param reader The bit reader providing additional data for object construction.
     * @return A `std::unique_ptr` to the created object of type `T`. If the ID is not registered, it returns `nullptr`.
     */
    std::unique_ptr<T> create(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/factory.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_FACTORY_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
