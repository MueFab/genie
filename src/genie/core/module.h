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

namespace genie {
namespace core {

/**
 *
 * @tparam Tin
 * @tparam Tout
 */
template <typename Tin, typename Tout>
class Module : public util::Source<Tout>, public util::Drain<Tin> {
 public:
    /**
     *
     * @param id
     */
    void skipIn(const util::Section& id) override;

    /**
     *
     */
    void flushIn(uint64_t& pos) override;

    /**
     *
     */
    ~Module() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/module.imp.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_MODULE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
