/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#include "genie/entropy/zlib/zlibistream.h"

#include <iostream>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::entropy::zlib {

// -----------------------------------------------------------------------------

ZlibInputStream::ZlibInputStream(std::unique_ptr<ZlibStreamBuffer> buffer)
    : std::istream(buffer.get()), buffer_(std::move(buffer)) {
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zlib

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
