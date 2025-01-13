/**
 * Copyright 2018-2024 The Genie Authors.
 * @file call_template_functions.cc
 *
 * @brief Template function invocations for Spring's reordering and encoding
 * workflows.
 *
 * This file contains implementations for invoking specialized template
 * functions for the reordering and encoding steps in the Spring framework,
 * based on the maximum read length provided in the compression parameters. The
 * functions leverage template specialization to optimize performance for
 * specific bitset sizes.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/spring/call_template_functions.h"

#include <stdexcept>
#include <string>

#include "genie/read/spring/reorder.h"
#include "genie/read/spring/spring_encoding.h"
#include "genie/read/spring/util.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------

void CallReorder(const std::string& temp_dir, const CompressionParams& cp) {
  switch ((2 * cp.max_read_len - 1) / 64 * 64 + 64) {
    case 64:
      ReorderMain<64>(temp_dir, cp);
      break;
    case 128:
      ReorderMain<128>(temp_dir, cp);
      break;
    case 192:
      ReorderMain<192>(temp_dir, cp);
      break;
    case 256:
      ReorderMain<256>(temp_dir, cp);
      break;
    case 320:
      ReorderMain<320>(temp_dir, cp);
      break;
    case 384:
      ReorderMain<384>(temp_dir, cp);
      break;
    case 448:
      ReorderMain<448>(temp_dir, cp);
      break;
    case 512:
      ReorderMain<512>(temp_dir, cp);
      break;
    case 576:
      ReorderMain<576>(temp_dir, cp);
      break;
    case 640:
      ReorderMain<640>(temp_dir, cp);
      break;
    case 704:
      ReorderMain<704>(temp_dir, cp);
      break;
    case 768:
      ReorderMain<768>(temp_dir, cp);
      break;
    case 832:
      ReorderMain<832>(temp_dir, cp);
      break;
    case 896:
      ReorderMain<896>(temp_dir, cp);
      break;
    case 960:
      ReorderMain<960>(temp_dir, cp);
      break;
    case 1024:
      ReorderMain<1024>(temp_dir, cp);
      break;
    default:
      throw std::runtime_error("Wrong bitset Size.");
  }
}

// -----------------------------------------------------------------------------

void CallEncoder(const std::string& temp_dir, const CompressionParams& cp) {
  switch ((3 * cp.max_read_len - 1) / 64 * 64 + 64) {
    case 64:
      EncoderMain<64>(temp_dir, cp);
      break;
    case 128:
      EncoderMain<128>(temp_dir, cp);
      break;
    case 192:
      EncoderMain<192>(temp_dir, cp);
      break;
    case 256:
      EncoderMain<256>(temp_dir, cp);
      break;
    case 320:
      EncoderMain<320>(temp_dir, cp);
      break;
    case 384:
      EncoderMain<384>(temp_dir, cp);
      break;
    case 448:
      EncoderMain<448>(temp_dir, cp);
      break;
    case 512:
      EncoderMain<512>(temp_dir, cp);
      break;
    case 576:
      EncoderMain<576>(temp_dir, cp);
      break;
    case 640:
      EncoderMain<640>(temp_dir, cp);
      break;
    case 704:
      EncoderMain<704>(temp_dir, cp);
      break;
    case 768:
      EncoderMain<768>(temp_dir, cp);
      break;
    case 832:
      EncoderMain<832>(temp_dir, cp);
      break;
    case 896:
      EncoderMain<896>(temp_dir, cp);
      break;
    case 960:
      EncoderMain<960>(temp_dir, cp);
      break;
    case 1024:
      EncoderMain<1024>(temp_dir, cp);
      break;
    case 1088:
      EncoderMain<1088>(temp_dir, cp);
      break;
    case 1152:
      EncoderMain<1152>(temp_dir, cp);
      break;
    case 1216:
      EncoderMain<1216>(temp_dir, cp);
      break;
    case 1280:
      EncoderMain<1280>(temp_dir, cp);
      break;
    case 1344:
      EncoderMain<1344>(temp_dir, cp);
      break;
    case 1408:
      EncoderMain<1408>(temp_dir, cp);
      break;
    case 1472:
      EncoderMain<1472>(temp_dir, cp);
      break;
    case 1536:
      EncoderMain<1536>(temp_dir, cp);
      break;
    default:
      throw std::runtime_error("Wrong bitset Size.");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
