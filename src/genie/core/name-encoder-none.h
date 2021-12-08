/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_NAME_ENCODER_NONE_H_
#define SRC_GENIE_CORE_NAME_ENCODER_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include <vector>
#include "genie/core/name-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class NameEncoderNone : public NameEncoder {
 public:
    /**
     * @brief
     * @return
     */
    std::tuple<AccessUnit::Descriptor, core::stats::PerfStats> process(const record::Chunk&) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_NAME_ENCODER_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
