/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_NONE_H_
#define SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/qv_encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvwriteout {

/**
 * @brief
 */
class NoneEncoder : public core::QVEncoder {
 public:
    /**
     * @brief
     * @return
     */
    QVCoded process(const core::record::Chunk&) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvwriteout
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
