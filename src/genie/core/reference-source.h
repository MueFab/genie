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
#include "reference-manager.h"
#include "reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ReferenceSource {
   protected:
    ReferenceManager* refMgr; //!<
   public:
    /**
     *
     * @param mgr
     */
    explicit ReferenceSource(ReferenceManager* mgr);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_REFERENCE_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------