/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/streams.h"

#include <algorithm>
#include <cstdio>

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
[[maybe_unused]] FileBuffer::FileBuffer(FILE* f) : fileptr_(f) {}

// -----------------------------------------------------------------------------
int FileBuffer::overflow(const int c) { return fputc(c, fileptr_); }

// -----------------------------------------------------------------------------
std::streamsize FileBuffer::xsputn(const char* s, const std::streamsize n) {
  return static_cast<std::streamsize>(
      fwrite(s, 1, static_cast<size_t>(n), fileptr_));
}

// -----------------------------------------------------------------------------
int FileBuffer::sync() { return fflush(fileptr_); }

// -----------------------------------------------------------------------------
std::streamsize FileBuffer::xsgetn(char* s, const std::streamsize n) {
  return static_cast<std::streamsize>(
      fread(s, 1, static_cast<size_t>(n), fileptr_));
}

// -----------------------------------------------------------------------------
int FileBuffer::underflow() { return fgetc(fileptr_); }

// -----------------------------------------------------------------------------
DataBlockBuffer::DataBlockBuffer(util::DataBlock* d, const size_t pos_i)
    : block_(0, 1), pos_(pos_i) {
  block_.Swap(d);
}

// -----------------------------------------------------------------------------
size_t DataBlockBuffer::Size() const { return block_.GetRawSize(); }

// -----------------------------------------------------------------------------
int DataBlockBuffer::overflow(const int c) {
  block_.PushBack(static_cast<uint64_t>(c));
  return c;
}

// -----------------------------------------------------------------------------

std::streamsize DataBlockBuffer::xsputn(const char* s,
                                        const std::streamsize n) {
  if (block_.ModByWordSize(n)) {
    UTILS_DIE("Invalid Data length");
  }
  const size_t old_size = block_.Size();
  block_.Resize(block_.Size() + block_.DivByWordSize(n));
  memcpy(
      static_cast<uint8_t*>(block_.GetData()) + block_.MulByWordSize(old_size),
      s, static_cast<size_t>(n));
  return n;
}

// -----------------------------------------------------------------------------

std::streamsize DataBlockBuffer::xsgetn(char* s, const std::streamsize n) {
  if (block_.ModByWordSize(n)) {
    UTILS_DIE("Invalid Data length");
  }
  const size_t bytes_read = std::min<size_t>(
      block_.GetRawSize() - block_.MulByWordSize(pos_), static_cast<size_t>(n));
  memcpy(s,
         static_cast<uint8_t*>(block_.GetData()) + block_.MulByWordSize(pos_),
         bytes_read);
  pos_ += block_.DivByWordSize(bytes_read);
  return static_cast<std::streamsize>(bytes_read);
}

// -----------------------------------------------------------------------------

int DataBlockBuffer::underflow() {
  if (pos_ == block_.Size()) {
    return EOF;
  }
  return static_cast<int>(block_.Get(pos_));
}

// -----------------------------------------------------------------------------

int DataBlockBuffer::uflow() {
  if (pos_ == block_.Size()) {
    return EOF;
  }
  return static_cast<int>(block_.Get(pos_++));
}

// -----------------------------------------------------------------------------

void DataBlockBuffer::FlushBlock(util::DataBlock* blk) { block_.Swap(blk); }

// -----------------------------------------------------------------------------

std::streambuf::pos_type DataBlockBuffer::seekpos(const pos_type sp,
                                                  std::ios_base::openmode) {
  return static_cast<std::streamoff>(pos_ = static_cast<size_t>(sp));
}

// -----------------------------------------------------------------------------

std::streambuf::pos_type DataBlockBuffer::seekoff(
    const off_type off, const std::ios_base::seekdir dir,
    const std::ios_base::openmode which) {
  (void)which;
  if (dir == std::ios_base::cur)
    pos_ = off < 0 && static_cast<size_t>(std::abs(off)) > pos_
               ? 0
               : std::min<size_t>(pos_ + off, block_.Size());
  else if (dir == std::ios_base::end)
    pos_ = off < 0 && static_cast<size_t>(std::abs(off)) > block_.Size()
               ? 0
               : std::min<size_t>(block_.Size() + off, block_.Size());
  else if (dir == std::ios_base::beg)
    pos_ = off < 0 && std::abs(off) > 0
               ? 0
               : std::min<size_t>(static_cast<size_t>(0) + off, block_.Size());
  return static_cast<std::streamoff>(pos_);
}

// -----------------------------------------------------------------------------

IBufferStream::IBufferStream(util::DataBlock* d, const size_t pos_i)
    : DataBlockBuffer(d, pos_i), std::istream(this) {}

// -----------------------------------------------------------------------------

OBufferStream::OBufferStream(util::DataBlock* d)
    : DataBlockBuffer(d, 0), std::ostream(this) {}

// -----------------------------------------------------------------------------

void OBufferStream::Flush(util::DataBlock* blk) { FlushBlock(blk); }

// -----------------------------------------------------------------------------

NullStream::NullStream() : std::ostream(&m_sb_) {}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
