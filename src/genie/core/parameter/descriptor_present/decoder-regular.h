/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_REGULAR_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_REGULAR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/parameter/descriptor_present/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

/**
 * @brief
 */
class DecoderRegular : public Decoder {
 public:
    /**
     * @brief
     * @param _encoding_mode_id
     */
    explicit DecoderRegular(uint8_t _encoding_mode_id);

    /**
     * @brief
     */
    ~DecoderRegular() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_REGULAR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
