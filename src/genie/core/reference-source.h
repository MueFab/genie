/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_REFERENCE_SOURCE_H
#define GENIE_REFERENCE_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ReferenceSource {
   public:
    /**
     *
     * @return
     */
    virtual std::vector<std::unique_ptr<core::Reference>> generateRefHandles() = 0;

    /**
     *
     */
    virtual ~ReferenceSource() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_REFERENCE_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------