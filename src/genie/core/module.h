/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_MODULE_H_
#define SRC_GENIE_CORE_MODULE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

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
    void skipIn(const util::Section& id) override;

    /**
     * @brief
     */
    void flushIn(uint64_t& pos) override;

    /**
     * @brief
     */
    ~Module() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/module.imp.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_MODULE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
