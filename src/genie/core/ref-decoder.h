/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_REF_DECODER_H_
#define SRC_GENIE_CORE_REF_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

#include "genie/core/access-unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class RefDecoder {
 public:
    /**
     * @brief
     */
    virtual ~RefDecoder() = default;

    /**
     * @brief
     * @param t
     * @return
     */
    virtual std::string decode(core::AccessUnit&& t) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REF_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
