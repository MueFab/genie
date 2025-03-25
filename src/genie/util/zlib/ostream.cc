/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#include "genie/util/zlib/ostream.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util::zlib {

// -----------------------------------------------------------------------------

OutputStream::OutputStream(std::unique_ptr<StreamBuffer> buffer)
    : buffer_(std::move(buffer)) {
  std::ostream::init(buffer_.get());
}

// -----------------------------------------------------------------------------

}  // namespace genie::util::zlib

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
