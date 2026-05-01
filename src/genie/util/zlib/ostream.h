/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ZLIB_OSTREAM_H_
#define SRC_GENIE_UTIL_ZLIB_OSTREAM_H_

// -----------------------------------------------------------------------------

#include <fstream>
#include <memory>
#include <utility>

#include "genie/util/zlib/streambuffer.h"

// -----------------------------------------------------------------------------

namespace genie::util::zlib {

/**
 *
 */
class OutputStream final : public std::ostream {
  /**
   *
   */
  std::unique_ptr<StreamBuffer> buffer_;

 public:
  /**
   *
   * @param buffer
   */
  explicit OutputStream(std::unique_ptr<StreamBuffer> buffer);
};

// -----------------------------------------------------------------------------

}  // namespace genie::util::zlib

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ZLIB_OSTREAM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
