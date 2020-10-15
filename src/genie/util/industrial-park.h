/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_INDUSTRIAL_PARK_H
#define GENIE_INDUSTRIAL_PARK_H

// ---------------------------------------------------------------------------------------------------------------------

#include <functional>
#include <map>
#include <typeindex>

#include "bitreader.h"
#include "factory.h"
#include "generic-factory.h"
#include "make-unique.h"
#include "runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 */
class IndustrialPark {
   private:
    std::map<std::type_index, std::unique_ptr<GenericFactory>> factories;  //!<

    /**
     *
     * @tparam T
     * @return
     */
    template <typename T>
    Factory<T>* findAndCreate();

    /**
     *
     * @tparam T
     * @return
     */
    template <typename T>
    Factory<T>* findAndFail() const;

   public:
    /**
     *
     * @tparam T
     * @param id
     * @param constructor
     */
    template <typename T>
    void registerConstructor(uint8_t id, const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor);

    /**
     *
     * @tparam T
     * @param constructor
     * @return
     */
    template <typename T>
    uint8_t registerConstructor(const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& constructor);

    /**
     *
     * @tparam T
     * @param id
     */
    template <typename T>
    void unregisterConstructor(uint8_t id);

    /**
     *
     * @tparam T
     * @param id
     * @param reader
     * @return
     */
    template <typename T>
    std::unique_ptr<T> construct(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "industrial-park.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_INDUSTRIAL_PARK_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------