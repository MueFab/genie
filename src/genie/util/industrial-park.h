/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_INDUSTRIAL_PARK_H_
#define SRC_GENIE_UTIL_INDUSTRIAL_PARK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <functional>
#include <map>
#include <memory>
#include <typeindex>
#include "genie/util/bitreader.h"
#include "genie/util/factory.h"
#include "genie/util/generic-factory.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief
 */
class IndustrialPark {
 private:
    std::map<std::type_index, std::unique_ptr<GenericFactory>> factories;  //!< @brief

    /**
     * @brief
     * @tparam T
     * @return
     */
    template <typename T>
    Factory<T>* findAndCreate();

    /**
     * @brief
     * @tparam T
     * @return
     */
    template <typename T>
    Factory<T>* findAndFail() const;

 public:
    /**
     * @brief
     * @tparam T
     * @param id
     * @param constructor
     */
    template <typename T>
    void registerConstructor(
        uint8_t id, const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor);

    /**
     * @brief
     * @tparam T
     * @param constructor
     * @return
     */
    template <typename T>
    uint8_t registerConstructor(
        const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor);

    /**
     * @brief
     * @tparam T
     * @param id
     */
    template <typename T>
    void unregisterConstructor(uint8_t id);

    /**
     * @brief
     * @tparam T
     * @param id
     * @param desc
     * @param reader
     * @return
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
