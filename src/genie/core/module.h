/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MODULE_H
#define GENIE_MODULE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/source.h>

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
    void flushIn() override;

    /**
     *
     */
    ~Module() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "module.imp.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MODULE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------