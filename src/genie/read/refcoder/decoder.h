/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_REFCODER_DECODER_H_
#define SRC_GENIE_READ_REFCODER_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/read-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace refcoder {

/**
 * @brief
 */
class Decoder : public core::ReadDecoder {
 public:
    /**
     * @brief
     */
    Decoder();

    /**
     * @brief
     */
    void flowIn(core::AccessUnit&&, const util::Section&) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_REFCODER_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
