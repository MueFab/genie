/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_NAME_ENCODER_NONE_H
#define GENIE_NAME_ENCODER_NONE_H

// ---------------------------------------------------------------------------------------------------------------------

#include "name-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class NameEncoderNone : public NameEncoder {
   public:
    /**
     *
     * @return
     */
    AccessUnit::Descriptor process(const record::Chunk&) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_NAME_ENCODER_NONE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------