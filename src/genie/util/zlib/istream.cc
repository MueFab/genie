/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#include "genie/util/zlib/istream.h"

#include <iostream>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util::zlib {

// -----------------------------------------------------------------------------

InputStream::InputStream(std::unique_ptr<StreamBuffer> buffer)
    : std::istream(buffer.get()), buffer_(std::move(buffer)) {
}

// -----------------------------------------------------------------------------

}  // namespace genie::util::zlib

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
