/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_ZLIB_ZLIBOSTREAM_H_
#define SRC_GENIE_ENTROPY_ZLIB_ZLIBOSTREAM_H_
#include <memory>

#include <fstream>
#include <utility>
#include "genie/entropy/zlib/zlibstreambuffer.h"

namespace genie::entropy::zlib {

class ZlibOutputStream : public std::ostream {
    std::unique_ptr<ZlibStreamBuffer> buffer;

 public:
    explicit ZlibOutputStream(std::unique_ptr<ZlibStreamBuffer> buffer);
};

}  // namespace genie::entropy::zlib

#endif  // SRC_GENIE_ENTROPY_ZLIB_ZLIBOSTREAM_H_
