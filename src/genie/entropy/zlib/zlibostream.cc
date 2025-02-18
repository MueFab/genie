/**
* @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/zlib/zlibostream.h"

namespace genie::entropy::zlib {

ZlibOutputStream::ZlibOutputStream(std::unique_ptr<ZlibStreamBuffer> buf) : buffer(std::move(buf)) {
    std::ostream::init(buffer.get());
}

}  // namespace genie::entropy::zlib
