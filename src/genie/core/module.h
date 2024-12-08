/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_MODULE_H_
#define SRC_GENIE_CORE_MODULE_H_

// -----------------------------------------------------------------------------

#include "genie/util/source.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 * @tparam Tin
 * @tparam Tout
 */
template <typename Tin, typename Tout>
class Module : public util::Source<Tout>, public util::Drain<Tin> {
 public:
  /**
   * @brief
   * @param id
   */
  void SkipIn(const util::Section& id) override;

  /**
   * @brief
   */
  void FlushIn(uint64_t& pos) override;

  /**
   * @brief
   */
  ~Module() override = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#include "genie/core/module.imp.h"

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_MODULE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
