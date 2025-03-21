#ifndef SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_
#define SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_

#include <zlib.h>
#include <streambuf>
#include <string>
#include <stdexcept>
#include <cstring>
#include <cstdint>

namespace genie::entropy::zlib {

class ZlibStreamBuffer final : public std::streambuf {
public:
  /**
   * Constructor.
   * @param file_path Path to the file.
   * @param writeMode True for write mode, false for read mode.
   * @param bufferSize Size of the internal buffer (default 4096 bytes).
   * @param compressionLevel Compression level for writing (1-9, -1 for default).
   */
  explicit ZlibStreamBuffer(const std::string& file_path, bool writeMode,
                            std::size_t bufferSize = 4096, int compressionLevel = -1);

  ~ZlibStreamBuffer() override;

protected:
  // Reading operations
  int underflow() override;
  int uflow() override;
  std::streamsize xsgetn(char* s, std::streamsize n) override;

  // Writing operations
  int overflow(int c) override;
  std::streamsize xsputn(const char* s, std::streamsize n) override;
  int sync() override;

private:
  gzFile file_;
  bool writeMode_;
  std::size_t internalBufferSize_;
  char* buffer_; // Dynamically allocated buffer of size 'internalBufferSize_'
};

}  // namespace genie::entropy::zlib

#endif  // SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_
