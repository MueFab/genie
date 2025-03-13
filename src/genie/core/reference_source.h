/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_SOURCE_H_
#define SRC_GENIE_CORE_REFERENCE_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/core/meta/internal_ref.h"
#include "genie/core/reference.h"
#include "genie/core/reference_manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class ReferenceSource {
 protected:
    ReferenceManager* refMgr;  //!< @brief
 public:
    /**
     * @brief
     * @param mgr
     */
    explicit ReferenceSource(ReferenceManager* mgr);

    /**
     * @brief
     * @return
     */
    virtual genie::core::meta::Reference getMeta() const;

    /**
     * @brief
     */
    virtual ~ReferenceSource() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
