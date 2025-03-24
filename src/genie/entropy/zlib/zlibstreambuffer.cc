/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#include "genie/entropy/zlib/zlibstreambuffer.h"

#include <zlib.h>

#include <cstring>
#include <stdexcept>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::entropy::zlib {

// -----------------------------------------------------------------------------

ZlibStreamBuffer::ZlibStreamBuffer(const std::string& file_path,
                                   const bool write_mode,
                                   const int compression_level)
    : write_mode_(write_mode), buffer_() {
  // Build the mode string.
  std::string mode;
  if (write_mode_) {
    mode = "wb";
    // Append a compression level if specified.
    if (compression_level >= 1 && compression_level <= 9) {
      mode.push_back('0' + compression_level);
    }
  } else {
    mode = "rb";
  }

  file_ = gzopen(file_path.c_str(), mode.c_str());
  if (!file_) {
    throw std::runtime_error("Failed to open file: " + file_path);
  }

  // Increase zlib's internal buffer size.
  if (gzbuffer(file_, buffer_size_) != 0) {
    // gzbuffer returns 0 on success; you may log or handle errors here if
    // desired.
  }

  if (write_mode_) {
    // Set up the output area (reserve one byte for overflow handling).
    setp(buffer_.data(), buffer_.data() + buffer_size_ - 1);
  } else {
    // For reading, initially set the get area empty.
    setg(buffer_.data(), buffer_.data() + buffer_size_,
         buffer_.data() + buffer_size_);
  }
}

// -----------------------------------------------------------------------------

ZlibStreamBuffer::~ZlibStreamBuffer() {
  sync();  // Flush any buffered output.
  gzclose(file_);
}

// -----------------------------------------------------------------------------

int ZlibStreamBuffer::underflow() {
  if (write_mode_) {
    return traits_type::eof();
  }
  // If characters remain in the get area, return the current one.
  if (gptr() < egptr()) {
    return traits_type::to_int_type(*gptr());
  }
  // Fill the buffer with up to internalBufferSize_ bytes.
  const int bytes_read = gzread(file_, buffer_.data(), buffer_size_);
  if (bytes_read <= 0) {
    return traits_type::eof();
  }
  // Reset get area pointers.
  setg(buffer_.data(), buffer_.data(), buffer_.data() + bytes_read);
  return traits_type::to_int_type(*gptr());
}

// -----------------------------------------------------------------------------

int ZlibStreamBuffer::uflow() {
  const int ch = underflow();
  if (ch != traits_type::eof()) {
    gbump(1);
  }
  return ch;
}

// -----------------------------------------------------------------------------

std::streamsize ZlibStreamBuffer::xsgetn(char* s, std::streamsize n) {
  std::streamsize total = 0;
  while (n > 0) {
    int available = egptr() - gptr();
    if (available <= 0) {
      if (underflow() == traits_type::eof()) break;
      available = egptr() - gptr();
    }
    const int to_copy = (available < n) ? available : n;
    std::memcpy(s, gptr(), to_copy);
    gbump(to_copy);
    s += to_copy;
    total += to_copy;
    n -= to_copy;
  }
  return total;
}

// -----------------------------------------------------------------------------

int ZlibStreamBuffer::overflow(int c) {
  if (!write_mode_) {
    return traits_type::eof();
  }
  // Flush the current contents of the buffer.
  if (const int num = pptr() - pbase(); num > 0) {
    if (const int written = gzwrite(file_, pbase(), num); written != num) {
      return traits_type::eof();
    }
  }
  // Reset the put area.
  setp(buffer_.data(), buffer_.data() + buffer_size_ - 1);
  // If c is not EOF, add it to the buffer.
  if (c != traits_type::eof()) {
    *pptr() = traits_type::to_char_type(c);
    pbump(1);
  }
  // Return a non-EOF value to indicate success.
  return traits_type::not_eof(c);
}

// -----------------------------------------------------------------------------

std::streamsize ZlibStreamBuffer::xsputn(const char* s, std::streamsize n) {
  // Optimized path for a single character.
  if (n == 1) {
    if (pptr() < epptr()) {
      *pptr() = s[0];
      pbump(1);
      return 1;
    }
    if (overflow(s[0]) == traits_type::eof()) {
      return 0;
    }
    return 1;
  }

  std::streamsize total = 0;
  // If n is larger than the internal buffer, flush the current buffer and write
  // the data directly.
  if (n >= static_cast<std::streamsize>(buffer_size_)) {
    // Flush any data already buffered.
    if (pptr() > pbase()) {
      if (overflow(traits_type::eof()) == traits_type::eof()) {
        return total;
      }
    }
    const int written = gzwrite(file_, s, static_cast<unsigned>(n));
    if (written <= 0) {
      return total;
    }
    total += written;
    return total;
  }

  // Otherwise, use the internal buffer as before.
  while (n > 0) {
    int space = epptr() - pptr();
    if (space == 0) {
      if (overflow(traits_type::eof()) == traits_type::eof()) {
        break;
      }
      space = epptr() - pptr();
    }
    const int to_copy = (space < n) ? space : n;
    std::memcpy(pptr(), s, to_copy);
    pbump(to_copy);
    s += to_copy;
    total += to_copy;
    n -= to_copy;
  }
  return total;
}

// -----------------------------------------------------------------------------

int ZlibStreamBuffer::sync() {
  if (write_mode_) {
    if (overflow(traits_type::eof()) == traits_type::eof()) {
      return -1;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zlib

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
