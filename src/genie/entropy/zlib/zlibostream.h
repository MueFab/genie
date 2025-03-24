/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_ZLIB_ZLIBOSTREAM_H_
#define SRC_GENIE_ENTROPY_ZLIB_ZLIBOSTREAM_H_

// -----------------------------------------------------------------------------

#include <fstream>
#include <memory>
#include <utility>

#include "genie/entropy/zlib/zlibstreambuffer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::zlib {

/**
 *
 */
class ZlibOutputStream final : public std::ostream {
  /**
   *
   */
  std::unique_ptr<ZlibStreamBuffer> buffer_;

 public:
  /**
   *
   * @param buffer
   */
  explicit ZlibOutputStream(std::unique_ptr<ZlibStreamBuffer> buffer);
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zlib

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_ZLIB_ZLIBOSTREAM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
