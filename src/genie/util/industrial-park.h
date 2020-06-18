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
#include "make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 */
class IndustrialPark {
   private:
    std::map<std::type_index, std::unique_ptr<GenericFactory>> factories; //!<

    /**
     *
     * @tparam T
     * @return
     */
    template <typename T>
    Factory<T>* findAndCreate() {
        auto type = std::type_index(typeid(T));
        auto it = factories.find(type);
        if (it == factories.end()) {
            factories.insert(std::make_pair(type, util::make_unique<Factory<T>>()));
            it = factories.find(type);
        }
        return reinterpret_cast<Factory<T>*>(it->second.get());
    }

    /**
     *
     * @tparam T
     * @return
     */
    template <typename T>
    Factory<T>* findAndFail() const {
        auto type = std::type_index(typeid(T));
        auto it = factories.find(type);
        UTILS_DIE_IF(it == factories.end(), "Unknown factory type");
        return reinterpret_cast<Factory<T>*>(it->second.get());
    }

   public:

    /**
     *
     * @tparam T
     * @param id
     * @param constructor
     */
    template <typename T>
    void registerConstructor(uint8_t id, const std::function<std::unique_ptr<T>(util::BitReader&)>& constructor) {
        findAndCreate<T>()->registerType(id, constructor);
    }

    /**
     *
     * @tparam T
     * @param constructor
     * @return
     */
    template <typename T>
    uint8_t registerConstructor(const std::function<std::unique_ptr<T>(util::BitReader&)>& constructor) {
        return findAndCreate<T>()->registerType(constructor);
    }

    /**
     *
     * @tparam T
     * @param id
     */
    template <typename T>
    void unregisterConstructor(uint8_t id) {
        findAndFail<T>()->unregisterType(id);
    }

    /**
     *
     * @tparam T
     * @param id
     * @param reader
     * @return
     */
    template <typename T>
    std::unique_ptr<T> construct(uint8_t id, util::BitReader& reader) const {
        return findAndFail<T>()->create(id, reader);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_INDUSTRIAL_PARK_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------