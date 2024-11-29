/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_GLOBAL_CFG_H_
#define SRC_GENIE_CORE_GLOBAL_CFG_H_

// -----------------------------------------------------------------------------

#include "genie/util/industrial_park.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief Configuration regarding modules, factories, ...
 */
class GlobalCfg {
  /**
   * @brief
   */
  GlobalCfg() = default;

  /**
   * @brief
   * @param cfg
   */
  GlobalCfg(GlobalCfg& cfg) = default;

  util::IndustrialPark fpark_;  //! @brief Our factories

 public:
  /**
   * @brief Access the set of all active factories
   * @return Factories
   */
  util::IndustrialPark& GetIndustrialPark();

  /**
   * @brief Access the global configuration
   * @return Global genie configuration
   */
  static GlobalCfg& GetSingleton();
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_GLOBAL_CFG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
