/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/binary_arithmetic_decoder.h"

#include <cassert>

#include "genie/entropy/gabac/bit_input_stream.h"
#include "genie/entropy/gabac/cabac_tables.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
BinaryArithmeticDecoder::BinaryArithmeticDecoder(
    const BitInputStream& bit_input_stream)
    : bit_input_stream_(bit_input_stream),
      num_bits_needed_(0),
      range_(0),
      value_(0) {}

// -----------------------------------------------------------------------------
inline unsigned int BinaryArithmeticDecoder::DecodeBin(
    ContextModel* context_model) {
  assert(context_model != nullptr);

  unsigned int decoded_byte;
  const unsigned int lps =
      cabactables::kLpsTable[context_model->GetState()][(range_ >> 6u) - 4];
  range_ -= lps;
  if (const unsigned int scaled_range = range_ << 7u; value_ < scaled_range) {
    decoded_byte = context_model->GetMps();
    context_model->UpdateMps();
    if (scaled_range >= (256u << 7u)) {
      return decoded_byte;
    }
    range_ = scaled_range >> 6u;
    value_ <<= 1;

    if (++num_bits_needed_ == 0) {
      num_bits_needed_ = -8;
      value_ += bit_input_stream_.ReadByte();
    }
  } else {
    const unsigned int num_bits = cabactables::kRenormTable[(lps >> 3u)];
    value_ = (value_ - scaled_range) << num_bits;
    range_ = (lps << num_bits);
    decoded_byte = 1 - static_cast<unsigned>(context_model->GetMps());
    context_model->UpdateLps();
    num_bits_needed_ += static_cast<int>(num_bits);
    if (num_bits_needed_ >= 0) {
      value_ += bit_input_stream_.ReadByte()
                << static_cast<unsigned int>(num_bits_needed_);
      num_bits_needed_ -= 8;
    }
  }

  return decoded_byte;
}

// -----------------------------------------------------------------------------
unsigned int BinaryArithmeticDecoder::DecodeBinsEp(unsigned int num_bins) {
  unsigned int bins = 0;
  unsigned int scaled_range;
  while (num_bins > 8) {
    value_ = (value_ << 8u) +
             (bit_input_stream_.ReadByte() << (8u + num_bits_needed_));
    scaled_range = range_ << 15u;
    for (int i = 0; i < 8; i++) {
      bins <<= 1;
      scaled_range >>= 1;
      if (value_ >= scaled_range) {
        bins++;
        value_ -= scaled_range;
      }
    }
    num_bins -= 8;
  }
  num_bits_needed_ += static_cast<int>(num_bins);
  value_ <<= num_bins;
  if (num_bits_needed_ >= 0) {
    value_ += bit_input_stream_.ReadByte()
              << static_cast<unsigned int>(num_bits_needed_);
    num_bits_needed_ -= 8;
  }
  scaled_range = range_ << (num_bins + 7);
  for (unsigned int i = 0; i < num_bins; i++) {
    bins <<= 1;
    scaled_range >>= 1;
    if (value_ >= scaled_range) {
      bins++;
      value_ -= scaled_range;
    }
  }

  return bins;
}

// -----------------------------------------------------------------------------
void BinaryArithmeticDecoder::DecodeBinTrm() {
  range_ -= 2;
  unsigned int scaled_range = range_ << 7u;
  if (value_ >= scaled_range) {
    // bin = 1;
  } else {
    // bin = 0;
    if (scaled_range < (256u << 7u)) {  // spec: ivlCurrRange < 256
      range_ = scaled_range >> 6u;      // spec: ivlCurrRange << 1
      value_ <<= 1;                     // spec: ivlOffset = ivlOffset << 1
      if (++num_bits_needed_ == 0) {
        num_bits_needed_ = -8;
        value_ += bit_input_stream_.ReadByte();
      }
    }
  }
}

// -----------------------------------------------------------------------------
void BinaryArithmeticDecoder::Reset() { DecodeBinTrm(); }

// -----------------------------------------------------------------------------
void BinaryArithmeticDecoder::Start() {
  assert(bit_input_stream_.GetNumBitsUntilByteAligned() == 0);

  num_bits_needed_ = -8;
  range_ = 510;
  value_ = (bit_input_stream_.ReadByte() << 8u);
  value_ |= bit_input_stream_.ReadByte();
}

// -----------------------------------------------------------------------------
size_t BinaryArithmeticDecoder::Close() {
  DecodeBinTrm();
  return bit_input_stream_.GetNumBytesRead();
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
