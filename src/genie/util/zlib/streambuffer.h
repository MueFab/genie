/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ZLIB_STREAMBUFFER_H_
#define SRC_GENIE_UTIL_ZLIB_STREAMBUFFER_H_

// -----------------------------------------------------------------------------

#include <zlib.h>

#include <array>
#include <streambuf>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::util::zlib {

/**
 * Stream buffer for zlib compression
 */
class StreamBuffer final : public std::streambuf {
 public:
  /**
   * Constructor.
   * @param file_path Path to the file.
   * @param write_mode True for write mode, false for read mode.
   * @param compression_level Compression level for writing (1-9, -1 for
   * default).
   */
  explicit StreamBuffer(const std::string& file_path, bool write_mode,
                            int compression_level = -1);

  /**
   *
   */
  ~StreamBuffer() override;

 protected:
  /**
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
   * @param s
   * @param n
   * @return
   */
  std::streamsize xsgetn(char* s, std::streamsize n) override;

  /**
   *
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

  /**
   *
   * @return
   */
  int sync() override;

 private:
  /**
   *
   */
  gzFile file_;

  /**
   *
   */
  bool write_mode_;

  /**
   *
   */
  static constexpr size_t buffer_size_ = 4096;

  /**
   *
   */
  std::array<char, buffer_size_> buffer_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util::zlib

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ZLIB_STREAMBUFFER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
