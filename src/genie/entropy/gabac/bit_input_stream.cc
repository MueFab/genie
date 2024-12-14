/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/bit_input_stream.h"

#include <cassert>
#include <limits>
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

static unsigned char ReadIn(util::BlockStepper* reader) {
  if (!reader->IsValid()) {
    UTILS_DIE("Index out of bounds");
  }
  // TODO(Jan): We here rely on that get() returns exactly 1 byte. However, it
  // might happen that it returns multiple bytes. Fix that.
  const auto byte = static_cast<unsigned char>(reader->Get());
  reader->Inc();
  return byte;
}

// -----------------------------------------------------------------------------

BitInputStream::BitInputStream(util::DataBlock* const bitstream)
    : bitstream_(bitstream), held_bits_(0), num_held_bits_(0) {
  reader_ = bitstream_->GetReader();  // TODO(Jan): This line is redundant.
  Reset();
}

// -----------------------------------------------------------------------------

unsigned int BitInputStream::GetNumBitsUntilByteAligned() const {
  return num_held_bits_ & 0x7u;
}

// -----------------------------------------------------------------------------

size_t BitInputStream::GetNumBytesRead() const {
  return reader_.curr - static_cast<uint8_t*>(bitstream_->GetData());
}

// -----------------------------------------------------------------------------

unsigned char BitInputStream::ReadByte() {
  const unsigned int result = Read(8);
  assert(result <= std::numeric_limits<unsigned char>::max());
  return static_cast<unsigned char>(result);
}

// -----------------------------------------------------------------------------

void BitInputStream::Reset() {
  held_bits_ = 0;
  num_held_bits_ = 0;
  reader_ = bitstream_->GetReader();
}

// -----------------------------------------------------------------------------

unsigned int BitInputStream::Read(unsigned int num_bits) {
  // uint64_t BitInputStream::read(const unsigned int numBits) {
  // TODO(Jan): Make this 64-bit compatible. See also:
  //   mpegg-reference-sw/src/gm-common-v1/
  //   mpegg-stream.c:mpegg_input_bitstream_read_ubits()
  assert(num_bits <= 32);
  // assert(numBits <= 64);

  // Bits are extracted from the MSB of each byte
  unsigned int bits{};
  // uint64_t bits = 0;
  if (num_bits <= num_held_bits_) {
    // Get numBits most significant bits from heldBits as bits
    bits = held_bits_ >> (num_held_bits_ - num_bits);  // NOLINT
    bits &= ~(0xffu << num_bits);
    num_held_bits_ -= num_bits;
    return bits;
  }

  // More bits requested than currently held, flushHeldBits all heldBits to
  // bits
  num_bits -= num_held_bits_;
  bits = held_bits_ & ~(0xffu << num_held_bits_);
  // bits = static_cast<uint64_t>(heldBits & ~(0xffu << numHeldBits));
  bits <<= num_bits;  // make room for the bits to come

  // Read in more bytes to satisfy the request
  const unsigned int kNumBytesToLoad = ((num_bits - 1u) >> 3u) + 1;  // NOLINT
  unsigned int aligned_word = 0;
  // uint64_t aligned_word = 0;
  if (kNumBytesToLoad == 1) {
    goto L1;
  }
  if (kNumBytesToLoad == 2) {
    goto L2;
  }
  if (kNumBytesToLoad == 3) {
    goto L3;
  }
  if (kNumBytesToLoad != 4) {
    goto L0;
  }

  aligned_word |= ReadIn(&reader_) << 24u;
L3:
  aligned_word |= ReadIn(&reader_) << 16u;
L2:
  aligned_word |= ReadIn(&reader_) << 8u;
L1:
  aligned_word |= ReadIn(&reader_);
L0:

  // Resolve remainder bits
  const unsigned int num_next_held_bits = (32 - num_bits) % 8;

  // Copy required part of aligned_word into bits
  bits |= aligned_word >> num_next_held_bits;

  // Store held bits
  num_held_bits_ = num_next_held_bits;
  /* FIXME @Jan: Can you please check, why this assert is needed?
   * I checked in ref-soft file mpegg-stream.c (in
   * mpegg_input_bitstream_read_ubits(...)), and did not see any such assert.
   */
  held_bits_ = static_cast<unsigned char>(aligned_word & 0xffu);

  return bits;
}

// -----------------------------------------------------------------------------

[[maybe_unused]] void BitInputStream::SkipBytes(const unsigned int num_bytes) {
  for (unsigned int i = 0; i < num_bytes; i++) {
    ReadIn(&reader_);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
