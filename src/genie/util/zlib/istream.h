/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ZLIB_ISTREAM_H_
#define SRC_GENIE_UTIL_ZLIB_ISTREAM_H_

// -----------------------------------------------------------------------------

#include <istream>
#include <memory>

#include "genie/util/zlib/streambuffer.h"

// -----------------------------------------------------------------------------

namespace genie::util::zlib {

/**
 *
 */
class InputStream final : public std::istream {
  /**
   *
   */
  std::unique_ptr<StreamBuffer> buffer_;

 public:
  /**
   *
   * @param buffer
   */
  explicit InputStream(std::unique_ptr<StreamBuffer> buffer);
};

// -----------------------------------------------------------------------------

}  // namespace genie::util::zlib

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ZLIB_ISTREAM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
