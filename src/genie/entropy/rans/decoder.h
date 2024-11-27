//
// Created by Vanko on 19.11.2024.
//

#ifndef GENIE_ENTROPY_RANS_DECODER_H
#define GENIE_ENTROPY_RANS_DECODER_H
#include <cstdint>
#include <cstddef>
#include "rans64.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "commons.h"
#include <istream>
#include <ostream>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::rans {

// ---------------------------------------------------------------------------------------------------------------------

class RANSDecoder {
public:
  RANSDecoder();
  void decode(std::istream &input, std::ostream &output);
private:
  uint32_t PROB_BITS = 14;
  uint32_t PROB_SCALE = 1 << PROB_BITS;
};
}

#endif // DECODER_H
