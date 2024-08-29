/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_TOKENTYPE_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_TOKENTYPE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/parameter/descriptor_present/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

/**
 * @brief
 */
class DecoderTokentype : public Decoder {
 public:
    /**
     * @brief
     * @param _encoding_mode_id
     */
    explicit DecoderTokentype(uint8_t _encoding_mode_id);

    /**
     * @brief
     */
    ~DecoderTokentype() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_TOKENTYPE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
