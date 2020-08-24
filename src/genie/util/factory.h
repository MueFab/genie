/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FACTORY_H
#define GENIE_FACTORY_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "generic-factory.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 * @tparam T
 */
template <typename T>
class Factory : public GenericFactory {
   private:
    std::map<uint8_t, std::function<std::unique_ptr<T>(util::BitReader&)>> factories;  //!<

   public:
    /**
     *
     * @param id
     * @param factory
     */
    void registerType(uint8_t id, const std::function<std::unique_ptr<T>(util::BitReader&)>& factory);

    /**
     *
     * @param factory
     * @return
     */
    uint8_t registerType(const std::function<std::unique_ptr<T>(util::BitReader&)>& factory);

    /**
     *
     * @param id
     */
    void unregisterType(uint8_t id);

    /**
     *
     * @param id
     * @param reader
     * @return
     */
    std::unique_ptr<T> create(uint8_t id, util::BitReader& reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "factory.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FACTORY_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------