/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DECODER_REGULAR_H
#define GENIE_DECODER_REGULAR_H

// ---------------------------------------------------------------------------------------------------------------------

#include "decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

/**
 *
 */
class DecoderRegular : public Decoder {
   public:
    /**
     *
     * @param _encoding_mode_id
     */
    explicit DecoderRegular(uint8_t _encoding_mode_id);

    /**
     *
     */
    ~DecoderRegular() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace desc_pres
}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DECODER_REGULAR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------