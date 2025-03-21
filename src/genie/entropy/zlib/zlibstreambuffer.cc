#include "genie/entropy/zlib/zlibstreambuffer.h"
#include <zlib.h>
#include <cstring>
#include <stdexcept>

namespace genie::entropy::zlib {

ZlibStreamBuffer::ZlibStreamBuffer(const std::string& file_path, bool writeMode,
                                   std::size_t bufferSize, int compressionLevel)
    : writeMode_(writeMode), internalBufferSize_(bufferSize) {
  // Allocate internal buffer dynamically.
  buffer_ = new char[internalBufferSize_];

  // Build the mode string.
  std::string mode;
  if (writeMode_) {
    mode = "wb";
    // Append a compression level if specified.
    if (compressionLevel >= 1 && compressionLevel <= 9) {
      mode.push_back('0' + compressionLevel);
    }
  } else {
    mode = "rb";
  }

  file_ = gzopen(file_path.c_str(), mode.c_str());
  if (!file_) {
    delete[] buffer_;
    throw std::runtime_error("Failed to open file: " + file_path);
  }

  // Increase zlib's internal buffer size.
  if (gzbuffer(file_, static_cast<unsigned>(bufferSize)) != 0) {
    // gzbuffer returns 0 on success; you may log or handle errors here if desired.
  }

  if (writeMode_) {
    // Set up the output area (reserve one byte for overflow handling).
    setp(buffer_, buffer_ + internalBufferSize_ - 1);
  } else {
    // For reading, initially set the get area empty.
    setg(buffer_, buffer_ + internalBufferSize_, buffer_ + internalBufferSize_);
  }
}

ZlibStreamBuffer::~ZlibStreamBuffer() {
  sync();         // Flush any buffered output.
  gzclose(file_);
  delete[] buffer_;
}

int ZlibStreamBuffer::underflow() {
  if (writeMode_) {
    return traits_type::eof();
  }
  // If characters remain in the get area, return the current one.
  if (gptr() < egptr()) {
    return traits_type::to_int_type(*gptr());
  }
  // Fill the buffer with up to internalBufferSize_ bytes.
  int bytesRead = gzread(file_, buffer_, static_cast<unsigned>(internalBufferSize_));
  if (bytesRead <= 0) {
    return traits_type::eof();
  }
  // Reset get area pointers.
  setg(buffer_, buffer_, buffer_ + bytesRead);
  return traits_type::to_int_type(*gptr());
}

int ZlibStreamBuffer::uflow() {
  int ch = underflow();
  if (ch != traits_type::eof()) {
    gbump(1);
  }
  return ch;
}

std::streamsize ZlibStreamBuffer::xsgetn(char* s, std::streamsize n) {
  std::streamsize total = 0;
  while (n > 0) {
    int available = egptr() - gptr();
    if (available <= 0) {
      if (underflow() == traits_type::eof()) break;
      available = egptr() - gptr();
    }
    int toCopy = (available < n) ? available : n;
    std::memcpy(s, gptr(), toCopy);
    gbump(toCopy);
    s += toCopy;
    total += toCopy;
    n -= toCopy;
  }
  return total;
}

int ZlibStreamBuffer::overflow(int c) {
  if (!writeMode_) {
    return traits_type::eof();
  }
  // Flush the current contents of the buffer.
  int num = pptr() - pbase();
  if (num > 0) {
    int written = gzwrite(file_, pbase(), num);
    if (written != num) {
      return traits_type::eof();
    }
  }
  // Reset the put area.
  setp(buffer_, buffer_ + internalBufferSize_ - 1);
  // If c is not EOF, add it to the buffer.
  if (c != traits_type::eof()) {
    *pptr() = traits_type::to_char_type(c);
    pbump(1);
  }
  // Return a non-EOF value to indicate success.
  return traits_type::not_eof(c);
}


std::streamsize ZlibStreamBuffer::xsputn(const char* s, std::streamsize n) {
  // Optimized path for a single character.
  if (n == 1) {
    if (pptr() < epptr()) {
      *pptr() = s[0];
      pbump(1);
      return 1;
    } else {
      if (overflow(s[0]) == traits_type::eof()) {
        return 0;
      }
      return 1;
    }
  }

  std::streamsize total = 0;
  // If n is larger than the internal buffer, flush the current buffer and write the data directly.
  if (n >= static_cast<std::streamsize>(internalBufferSize_)) {
    // Flush any data already buffered.
    if (pptr() > pbase()) {
      if (overflow(traits_type::eof()) == traits_type::eof()) {
        return total;
      }
    }
    int written = gzwrite(file_, s, static_cast<unsigned>(n));
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
    int toCopy = (space < n) ? space : n;
    std::memcpy(pptr(), s, toCopy);
    pbump(toCopy);
    s += toCopy;
    total += toCopy;
    n -= toCopy;
  }
  return total;
}



int ZlibStreamBuffer::sync() {
  if (writeMode_) {
    if (overflow(traits_type::eof()) == traits_type::eof()) {
      return -1;
    }
  }
  return 0;
}

}  // namespace genie::entropy::zlib
