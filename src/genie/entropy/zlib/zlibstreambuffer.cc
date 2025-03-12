/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/muefab/genie for more details.
 */

#include "genie/entropy/zlib/zlibstreambuffer.h"

#include <zlib.h>

#include <iostream>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::entropy::zlib {

// -----------------------------------------------------------------------------

ZlibStreamBuffer::ZlibStreamBuffer(const std::string& file_path,
                                   const bool mode)
    : writeMode_(mode) {
  if (mode) {
    file_ = gzopen(file_path.c_str(), "wb");
  } else {
    file_ = gzopen(file_path.c_str(), "rb");
    char c;
    gzread(file_, &c, 1);
    this->buffer_ = c;
  }
  if (!file_) {
    throw std::runtime_error("Failed to open file: " + file_path);
  }
}

// -----------------------------------------------------------------------------

ZlibStreamBuffer::~ZlibStreamBuffer() { gzclose(file_); }

// -----------------------------------------------------------------------------

std::streamsize ZlibStreamBuffer::showmanyc() { return file_->have; }

// -----------------------------------------------------------------------------

std::streamsize ZlibStreamBuffer::xsgetn(char* s, const std::streamsize n) {
  if (this->buffer_.has_value()) {
    *s = this->buffer_.value();
    s++;
    int read_chars = 1;
    for (int i = 0; i < n; i++) {
      const int err = gzread(file_, s, 1);
      read_chars += err;
      if (gzeof(file_)) {
        break;
      }
      s++;
    }
    gzread(file_, &this->buffer_.value(), 1);
    return read_chars;
  }
  return 0;
}

// -----------------------------------------------------------------------------

std::streamsize ZlibStreamBuffer::xsputn(const char* s,
                                         const std::streamsize n) {
  if (this->buffer_.has_value()) {
    int write_chars = gzwrite(file_, &this->buffer_.value(), 1);
    for (int i = 0; i < n; i++) {
      write_chars += gzwrite(file_, s, 1);
      s++;
      if (gzeof(file_)) {
        return write_chars;
      }
    }
    this->buffer_.value() = *s;
    return write_chars;
  }

  int write_chars = 0;
  for (int i = 0; i < n; i++) {
    write_chars += gzwrite(file_, s, 1);
    s++;
    if (gzeof(file_)) {
      return write_chars;
    }
  }
  return write_chars;
}

// -----------------------------------------------------------------------------

int ZlibStreamBuffer::overflow(const int c) {
  if (c == EOF) {
    this->buffer_ = std::nullopt;
    return EOF;
  }
  this->buffer_.value() = c;
  return c;
}

// -----------------------------------------------------------------------------

int ZlibStreamBuffer::underflow() {
  if (this->buffer_.has_value()) {
    return this->buffer_.value();
  }
  return EOF;
}

// -----------------------------------------------------------------------------

int ZlibStreamBuffer::uflow() {
  if (this->buffer_.has_value()) {
    const char local_buffer = this->buffer_.value();
    gzread(file_, &this->buffer_, 1);
    if (gzeof(file_)) {
      this->buffer_ = std::nullopt;
    }
    return local_buffer;
  }
  return EOF;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zlib

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
