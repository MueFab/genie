/**
* Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_REGULAR_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_REGULAR_H_

// -----------------------------------------------------------------------------

#include "genie/core/parameter/descriptor_present/decoder.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

/**
 * @brief
 */
class DecoderRegular : public Decoder {
public:
 /**
  * @brief
  * @param encoding_mode_id
  */
 explicit DecoderRegular(uint8_t encoding_mode_id);

 /**
  * @brief
  */
 ~DecoderRegular() override = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_REGULAR_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------