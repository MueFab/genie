/**
* @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_
#define SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_
#include <zlib.h>

#include <optional>
#include <streambuf>
#include <string>

namespace genie::entropy::zlib {

class ZlibStreamBuffer : public std::streambuf {
 public:
    gzFile file;

    std::optional<char> buffer;

    explicit ZlibStreamBuffer(const std::string& file_path, bool mode);

    ~ZlibStreamBuffer() override;

 protected:
    std::streamsize showmanyc() override;

    std::streamsize xsgetn(char* s, std::streamsize n) override;

    int underflow() override;

    int uflow() override;

    int overflow(int c) override;

    std::streamsize xsputn(const char* s, std::streamsize n) override;
};

}  // namespace genie::entropy::zlib

#endif  // SRC_GENIE_ENTROPY_ZLIB_ZLIBSTREAMBUFFER_H_
