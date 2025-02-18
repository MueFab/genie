/**
* @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_ZLIB_ZLIBISTREAM_H_
#define SRC_GENIE_ENTROPY_ZLIB_ZLIBISTREAM_H_
#include <istream>
#include <memory>

#include "genie/entropy/zlib/zlibstreambuffer.h"

#endif  // SRC_GENIE_ENTROPY_ZLIB_ZLIBISTREAM_H_

namespace genie::entropy::zlib {

class ZlibInputStream : public std::istream {
    std::unique_ptr<ZlibStreamBuffer> buffer;

 public:
    explicit ZlibInputStream(std::unique_ptr<ZlibStreamBuffer> buffer);
};

}  // namespace genie::entropy::zlib
