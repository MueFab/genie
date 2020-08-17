/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENCODER_NONE_H
#define GENIE_ENCODER_NONE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/qv-encoder.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvwriteout {

/**
 *
 */
class NoneEncoder : public core::QVEncoder {
   public:
    /**
     *
     * @return
     */
    QVCoded process(const core::record::Chunk&) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvwriteout
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENCODER_NONE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------