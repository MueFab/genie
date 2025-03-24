/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#include "genie/entropy/zlib/zlibostream.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::entropy::zlib {

// -----------------------------------------------------------------------------

ZlibOutputStream::ZlibOutputStream(std::unique_ptr<ZlibStreamBuffer> buffer)
    : buffer_(std::move(buffer)) {
  std::ostream::init(buffer_.get());
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zlib

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
