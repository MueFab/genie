/**
 * Copyright 2018-2024 The Genie Authors.
 * @file bit_reader.cc
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Implementation of the BitReader utility class for reading bits from a
 * stream.
 *
 * This file contains the implementation of the BitReader class, which provides
 * methods to read bits and bytes from an input stream. It supports both
 * non-aligned and byte-aligned reading operations.
 *
 * @details The BitReader class offers functionalities like reading a specific
 * number of bits, flushing bits, checking byte alignment, reading bytes,
 * reading and skipping aligned strings, and manipulating the input stream
 * position.
 */

#include "genie/util/bit_reader.h"

#include <string>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

constexpr uint8_t kBitsPerByte = 8u;
constexpr uint8_t kByteMask = 0xffu;

// -----------------------------------------------------------------------------

BitReader::BitReader(std::istream& istream)
    : input_stream_(istream),
      held_bits_(0),
      num_held_bits_(0),
      total_bits_read_(0) {}

// Non Byte-Aligned reading
// -----------------------------------------------------------------------------

inline uint64_t BitReader::LoadAlignedWord(const uint8_t num_bits) const {
  // NOTE: This unrolling is the result of profiling, do not change.
  uint64_t aligned_word = 0;
  const auto num_bits_minus_one = static_cast<uint8_t>(num_bits - 1);
  const auto num_bytes_to_load =
      static_cast<uint8_t>((num_bits_minus_one >> 3) + 1);

  if (num_bytes_to_load == 1) goto L1;
  if (num_bytes_to_load == 2) goto L2;
  if (num_bytes_to_load == 3) goto L3;
  if (num_bytes_to_load == 4) goto L4;
  if (num_bytes_to_load == 5) goto L5;
  if (num_bytes_to_load == 6) goto L6;
  if (num_bytes_to_load == 7) goto L7;
  if (num_bytes_to_load != 8) goto L0;
  aligned_word |= ReadAlignedByte() << kBitsPerByte * 7;
L7:
  aligned_word |= ReadAlignedByte() << kBitsPerByte * 6;
L6:
  aligned_word |= ReadAlignedByte() << kBitsPerByte * 5;
L5:
  aligned_word |= ReadAlignedByte() << kBitsPerByte * 4;
L4:
  aligned_word |= ReadAlignedByte() << kBitsPerByte * 3;
L3:
  aligned_word |= ReadAlignedByte() << kBitsPerByte * 2;
L2:
  aligned_word |= ReadAlignedByte() << kBitsPerByte;
L1:
  aligned_word |= ReadAlignedByte();
L0:
  return aligned_word;
}

// -----------------------------------------------------------------------------

uint64_t BitReader::ReadBits(uint8_t num_bits) {
  total_bits_read_ += num_bits;
  uint64_t result_bits;
  if (num_bits <= num_held_bits_) {
    result_bits = held_bits_ >> static_cast<uint8_t>(num_held_bits_ - num_bits);
    result_bits &= ~(static_cast<uint64_t>(kByteMask) << num_bits);
    num_held_bits_ -= num_bits;
    return result_bits;
  }

  num_bits -= num_held_bits_;
  result_bits = held_bits_ & ~(kByteMask << num_held_bits_);
  result_bits <<= num_bits;

  const uint64_t aligned_word = LoadAlignedWord(num_bits);
  const auto num_next_held_bits =
      static_cast<uint8_t>((kBitsPerByte * 8 - num_bits) % kBitsPerByte);
  result_bits |= aligned_word >> num_next_held_bits;

  num_held_bits_ = num_next_held_bits;
  held_bits_ = static_cast<uint8_t>(aligned_word & kByteMask);
  return result_bits;
}

// -----------------------------------------------------------------------------

uint64_t BitReader::GetTotalBitsRead() const { return total_bits_read_; }

// -----------------------------------------------------------------------------

uint64_t BitReader::FlushHeldBits() {
  total_bits_read_ += num_held_bits_;
  num_held_bits_ = 0;
  const auto ret = held_bits_;

  held_bits_ = 0;
  return ret;
}

// Byte-Aligned Reading
// -----------------------------------------------------------------------------

bool BitReader::IsByteAligned() const { return !num_held_bits_; }

// -----------------------------------------------------------------------------

uint64_t BitReader::ReadAlignedByte() const {
  char c = 0;
  input_stream_.read(&c, 1);
  return static_cast<uint8_t>(c);
}

// -----------------------------------------------------------------------------

std::string BitReader::ReadAlignedStringTerminated() {
  std::string result;
  char c;
  while ((c = ReadAlignedInt<char>()) != '\0') {
    result.push_back(c);
  }
  total_bits_read_ += result.length() * kBitsPerByte;
  return result;
}

// -----------------------------------------------------------------------------

void BitReader::ReadAlignedBytes(void* in, const size_t size) {
  total_bits_read_ += size * kBitsPerByte;
  input_stream_.read(static_cast<char*>(in),
                     static_cast<std::streamsize>(size));
}

// -----------------------------------------------------------------------------

void BitReader::SkipAlignedBytes(const size_t bytes) const {
  input_stream_.seekg(static_cast<std::istream::off_type>(bytes),
                      std::ios_base::cur);
}

// Input stream manipulation
// -----------------------------------------------------------------------------

uint64_t BitReader::GetStreamPosition() const { return input_stream_.tellg(); }

// -----------------------------------------------------------------------------

void BitReader::SetStreamPosition(const uint64_t pos) const {
  input_stream_.seekg(static_cast<std::ios::off_type>(pos), std::ios_base::beg);
}

// -----------------------------------------------------------------------------

void BitReader::ClearStreamState() const { input_stream_.clear(); }

// -----------------------------------------------------------------------------

bool BitReader::IsStreamGood() const {
  return static_cast<bool>(input_stream_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
