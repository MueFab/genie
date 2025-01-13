/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/run.h"

#include <cstdint>

#include "genie/entropy/gabac/decode_desc_sub_seq.h"
#include "genie/entropy/gabac/encode_desc_sub_seq.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

uint64_t Run(const IoConfiguration& conf, const EncodingConfiguration& en_conf,
             const bool decode) {
  if (decode) {
    return DecodeDescSubsequence(conf, en_conf);
  }
  return EncodeDescSubsequence(conf, en_conf);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
