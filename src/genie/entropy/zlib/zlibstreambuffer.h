/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_
#define SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_

// -----------------------------------------------------------------------------

#include <zlib.h>

#include <cstdint>
#include <optional>
#include <streambuf>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::entropy::zlib {

/**
 *
 */
class ZlibStreamBuffer final : public std::streambuf {
 public:
  /**
   *
   */
  gzFile file_;

  /**
   *
   */
  std::optional<char> buffer_;

  /**
   *
   */
  bool writeMode_;

  /**
   *
   * @param file_path
   * @param mode
   */
  explicit ZlibStreamBuffer(const std::string& file_path, bool mode);

  /**
   *
   */
  ~ZlibStreamBuffer() override;

 protected:
  /**
   *
   * @return
   */
  std::streamsize showmanyc() override;

  /**
   *
   * @param s
   * @param n
   * @return
   */
  std::streamsize xsgetn(char* s, std::streamsize n) override;

  /**
   *
   * @return
   */
  int underflow() override;

  /**
   *
   * @return
   */
  int uflow() override;

  /**
   *
   * @param c
   * @return
   */
  int overflow(int c) override;

  /**
   *
   * @param s
   * @param n
   * @return
   */
  std::streamsize xsputn(const char* s, std::streamsize n) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zlib

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
