/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_REF_DECODER_H
#define GENIE_REF_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <read-decoder.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace refcoder {

/**
 *
 */
class Decoder : public core::ReadDecoder {
   public:
    /**
     *
     */
    Decoder();

    /**
     *
     */
    void flowIn(core::AccessUnit&&, const util::Section&) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------