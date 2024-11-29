/**
 * Copyright 2018-2024 The Genie Authors.
 * @file bit_writer.cc
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Implementation of the BitWriter utility class for writing bits to a
 * stream.
 *
 * This file contains the implementation of the BitWriter class, which provides
 * methods to write bits and bytes to an output stream. It supports both
 * non-aligned and byte-aligned writing operations.
 *
 * @details The BitWriter class offers functionalities like writing a specific
 * number of bits, flushing bits, checking byte alignment, writing bytes,
 * writing aligned streams, and manipulating the output stream position. The
 * class ensures efficient bit-level writing, while also handling any buffered
 * bits that cannot yet be written out as full bytes.
 */

#include "genie/util/bit_writer.h"

#include <istream>
#include <string>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

constexpr uint8_t kBitsPerByte = 8u;
constexpr uint8_t kByteMask = 0xffu;

// -----------------------------------------------------------------------------

BitWriter::BitWriter(std::ostream& str)
    : stream_(str), held_bits_(0), num_held_bits_(0), total_bits_written_(0) {}

// -----------------------------------------------------------------------------

BitWriter::~BitWriter() { FlushBits(); }

// -----------------------------------------------------------------------------

inline void BitWriter::WriteAlignedBytes(const uint64_t write_bits,
                                         const uint8_t num_bytes) {
  if (num_bytes == 1) goto L1;
  if (num_bytes == 2) goto L2;
  if (num_bytes == 3) goto L3;
  if (num_bytes == 4) goto L4;
  if (num_bytes == 5) goto L5;
  if (num_bytes == 6) goto L6;
  if (num_bytes == 7) goto L7;
  if (num_bytes != 8) return;
  WriteAlignedByte(
      static_cast<uint8_t>(write_bits >> kBitsPerByte * 7 & kByteMask));
L7:
  WriteAlignedByte(
      static_cast<uint8_t>(write_bits >> kBitsPerByte * 6 & kByteMask));
L6:
  WriteAlignedByte(
      static_cast<uint8_t>(write_bits >> kBitsPerByte * 5 & kByteMask));
L5:
  WriteAlignedByte(
      static_cast<uint8_t>(write_bits >> kBitsPerByte * 4 & kByteMask));
L4:
  WriteAlignedByte(
      static_cast<uint8_t>(write_bits >> kBitsPerByte * 3 & kByteMask));
L3:
  WriteAlignedByte(
      static_cast<uint8_t>(write_bits >> kBitsPerByte * 2 & kByteMask));
L2:
  WriteAlignedByte(
      static_cast<uint8_t>(write_bits >> kBitsPerByte & kByteMask));
L1:
  WriteAlignedByte(static_cast<uint8_t>(write_bits & kByteMask));
}

// -----------------------------------------------------------------------------

void BitWriter::WriteBits(const uint64_t value, const uint8_t bits) {
  const uint8_t num_total_bits = bits + num_held_bits_;
  const auto next_held_bit_count =
      static_cast<uint8_t>(num_total_bits % kBitsPerByte);
  const auto shift_size =
      static_cast<uint8_t>(kBitsPerByte - next_held_bit_count);
  const auto new_held_bits =
      static_cast<uint8_t>(value << shift_size & kByteMask);

  if (num_total_bits < kBitsPerByte) {
    held_bits_ |= new_held_bits;
    num_held_bits_ = next_held_bit_count;
    return;
  }

  const uint64_t top_word = static_cast<uint64_t>(bits - next_held_bit_count) &
                            static_cast<uint64_t>(~((1u << 3u) - 1u));
  const uint64_t write_bits =
      held_bits_ << top_word | value >> next_held_bit_count;

  const uint8_t num_bytes_to_write = num_total_bits / kBitsPerByte;
  WriteAlignedBytes(write_bits, num_bytes_to_write);

  held_bits_ = new_held_bits;
  num_held_bits_ = next_held_bit_count;
}

// -----------------------------------------------------------------------------

void BitWriter::FlushBits() {
  if (num_held_bits_ == 0) {
    return;
  }
  WriteBits(held_bits_, static_cast<uint8_t>(kBitsPerByte - num_held_bits_));
  held_bits_ = 0x00;
  num_held_bits_ = 0;
}

// -----------------------------------------------------------------------------

uint64_t BitWriter::GetTotalBitsWritten() const {
  return total_bits_written_ + num_held_bits_;
}

// -----------------------------------------------------------------------------

bool BitWriter::IsByteAligned() const { return num_held_bits_ == 0; }

// -----------------------------------------------------------------------------

inline void BitWriter::WriteAlignedByte(uint8_t byte) {
  stream_.write(reinterpret_cast<char*>(&byte), 1);
  total_bits_written_ += kBitsPerByte;
}

// -----------------------------------------------------------------------------

void BitWriter::WriteAlignedBytes(const void* in, const size_t size) {
  this->total_bits_written_ += size * kBitsPerByte;
  UTILS_DIE_IF(!IsByteAligned(), "Writer not aligned when it should be");
  stream_.write(static_cast<const char*>(in),
                static_cast<std::streamsize>(size));
}

// -----------------------------------------------------------------------------

constexpr size_t kWriteBufferSize = 100;
void BitWriter::WriteAlignedStream(std::istream& in) {
  UTILS_DIE_IF(!IsByteAligned(), "Writer not aligned when it should be");
  do {
    char byte[kWriteBufferSize];
    in.read(byte, kWriteBufferSize);
    stream_.write(byte, in.gcount());
    this->total_bits_written_ += in.gcount() * kBitsPerByte;
  } while (in.gcount() == kWriteBufferSize);
}

// -----------------------------------------------------------------------------

int64_t BitWriter::GetStreamPosition() const { return stream_.tellp(); }

// -----------------------------------------------------------------------------

void BitWriter::SetStreamPosition(const int64_t pos) const {
  stream_.seekp(pos, std::ios::beg);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
