/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

//
// Created by Vanko on 19.11.2024.
//

#ifndef SRC_GENIE_ENTROPY_RANS_DECODER_H_
#define SRC_GENIE_ENTROPY_RANS_DECODER_H_
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>
#include "genie/entropy/rans/rans64.h"
#include "genie/entropy/rans/commons.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::rans {

// ---------------------------------------------------------------------------------------------------------------------

class RANSDecoder {
 public:
  RANSDecoder();
  void decode(std::istream& input, std::ostream& output, uint32_t num_interleavings);
 private:
  uint32_t PROB_BITS = 14;
  uint32_t PROB_SCALE = 1 << PROB_BITS;
};
}  // namespace genie::entropy::rans

#endif  // SRC_GENIE_ENTROPY_RANS_DECODER_H_
