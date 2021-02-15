/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_FACTORY_H_
#define SRC_GENIE_UTIL_FACTORY_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <memory>
#include "genie/core/constants.h"
#include "genie/util/generic-factory.h"

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
    std::map<uint8_t, std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>> factories;  //!<

 public:
    /**
     *
     * @param id
     * @param factory
     */
    void registerType(uint8_t id,
                      const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& factory);

    /**
     *
     * @param factory
     * @return
     */
    uint8_t registerType(const std::function<std::unique_ptr<T>(genie::core::GenDesc desc, util::BitReader&)>& factory);

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
    std::unique_ptr<T> create(uint8_t id, genie::core::GenDesc desc, util::BitReader& reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/factory.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_FACTORY_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
