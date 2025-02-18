/**
* @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/zlib/zlibistream.h"
#include <utility>

namespace genie::entropy::zlib {

     ZlibInputStream::ZlibInputStream(std::unique_ptr<ZlibStreamBuffer> buf): buffer(std::move(buf)) {
         std::istream::init(buffer.get());
     }

}  // namespace genie::entropy::zlib
