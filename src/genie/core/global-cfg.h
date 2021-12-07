/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_GLOBAL_CFG_H_
#define SRC_GENIE_CORE_GLOBAL_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/industrial-park.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Configuration regarding modules, factories, ...
 */
class GlobalCfg {
 private:
    /**
     * @brief
     */
    GlobalCfg() = default;

    /**
     * @brief
     * @param cfg
     */
    GlobalCfg(GlobalCfg& cfg) = default;

    util::IndustrialPark fpark;  //! @brief Our factories

 public:
    /**
     * @brief Access the set of all active factories
     * @return Factories
     */
    util::IndustrialPark& getIndustrialPark();

    /**
     * @brief Access the global configuration
     * @return Global genie configuration
     */
    static GlobalCfg& getSingleton();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_GLOBAL_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
