/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/binary_arithmetic_encoder.h"

#include <cassert>

#include "genie/entropy/gabac/cabac_tables.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
BinaryArithmeticEncoder::BinaryArithmeticEncoder(
    const util::BitWriter& bit_output_stream)
    : bit_output_stream_(bit_output_stream),
      buffered_byte_(0),
      low_(0),
      num_bits_left_(0),
      num_buffered_bytes_(0),
      range_(0) {}

// -----------------------------------------------------------------------------
BinaryArithmeticEncoder::~BinaryArithmeticEncoder() = default;

// -----------------------------------------------------------------------------
inline void BinaryArithmeticEncoder::EncodeBin(unsigned int bin,
                                               ContextModel* context_model) {
  assert((bin == 0) || (bin == 1));
  assert(context_model != nullptr);

  unsigned int lps =
      cabactables::kLpsTable[context_model->GetState()][(range_ >> 6u) & 3u];
  range_ -= lps;
  if (bin != context_model->GetMps()) {
    unsigned int num_bits = cabactables::kRenormTable[(lps >> 3u)];
    low_ = (low_ + range_) << num_bits;
    range_ = lps << num_bits;
    context_model->UpdateLps();
    num_bits_left_ -= num_bits;
  } else {
    context_model->UpdateMps();
    if (range_ >= 256) {
      return;
    }
    low_ <<= 1;
    range_ <<= 1;
    num_bits_left_ -= 1;
  }
  if (num_bits_left_ < 12) {
    WriteOut();
  }
}

// -----------------------------------------------------------------------------
void BinaryArithmeticEncoder::EncodeBinEp(unsigned int bin) {
  assert((bin == 0) || (bin == 1));

  low_ <<= 1;
  if (bin > 0) {
    low_ += range_;
  }
  num_bits_left_ -= 1;
  if (num_bits_left_ < 12) {
    WriteOut();
  }
}

// -----------------------------------------------------------------------------
void BinaryArithmeticEncoder::EncodeBinsEp(unsigned int bins,
                                           unsigned int num_bins) {
  while (num_bins > 8) {
    num_bins -= 8;
    unsigned int pattern = bins >> num_bins;
    low_ <<= 8;
    low_ += (range_ * pattern);
    bins -= (pattern << num_bins);
    num_bits_left_ -= 8;
    if (num_bits_left_ < 12) {
      WriteOut();
    }
  }
  low_ <<= num_bins;
  low_ += (range_ * bins);
  num_bits_left_ -= num_bins;
  if (num_bits_left_ < 12) {
    WriteOut();
  }
}

// -----------------------------------------------------------------------------
void BinaryArithmeticEncoder::EncodeBinTrm(unsigned int bin) {
  // Encode the least-significant bit of bin as a terminating bin
  range_ -= 2;
  if (bin != 0) {
    low_ += range_;
    low_ <<= 7;
    range_ = (2u << 7u);
    num_bits_left_ -= 7;
  } else if (range_ >= 256) {
    return;
  } else {
    low_ <<= 1;
    range_ <<= 1;
    num_bits_left_ -= 1;
  }
  if (num_bits_left_ < 12) {
    WriteOut();
  }
}

// -----------------------------------------------------------------------------
void BinaryArithmeticEncoder::Flush() {
  EncodeBinTrm(1);
  Finish();
  bit_output_stream_.WriteBits(1, 1);
  bit_output_stream_.FlushBits();
  Start();
}

// -----------------------------------------------------------------------------
void BinaryArithmeticEncoder::Start() {
  buffered_byte_ = 0xff;
  low_ = 0;
  num_bits_left_ = 23;
  num_buffered_bytes_ = 0;
  range_ = 510;
}

// -----------------------------------------------------------------------------
void BinaryArithmeticEncoder::Finish() {
  if ((low_ >> (32u - num_bits_left_)) > 0) {
    bit_output_stream_.WriteBits((buffered_byte_ + 1), 8);
    while (num_buffered_bytes_ > 1) {
      bit_output_stream_.WriteBits(0x00, 8);
      num_buffered_bytes_ -= 1;
    }
    low_ -= (1u << (32u - num_bits_left_));
  } else {
    if (num_buffered_bytes_ > 0) {
      bit_output_stream_.WriteBits(buffered_byte_, 8);
    }
    while (num_buffered_bytes_ > 1) {
      bit_output_stream_.WriteBits(0xff, 8);
      num_buffered_bytes_ -= 1;
    }
  }
  bit_output_stream_.WriteBits(low_ >> 8u, uint8_t(24u - num_bits_left_));
}

// -----------------------------------------------------------------------------

void BinaryArithmeticEncoder::WriteOut() {
  unsigned int lead_byte = low_ >> (24u - num_bits_left_);
  num_bits_left_ += 8;
  low_ &= 0xffffffffu >> num_bits_left_;
  if (lead_byte == 0xff) {
    num_buffered_bytes_ += 1;
  } else {
    if (num_buffered_bytes_ > 0) {
      auto carry = static_cast<unsigned char>(lead_byte >> 8u);
      unsigned char byte = buffered_byte_ + carry;

      buffered_byte_ = static_cast<unsigned char>(lead_byte & 0xffu);
      bit_output_stream_.WriteBits(byte, 8);

      byte = static_cast<unsigned char>(0xff) + carry;
      while (num_buffered_bytes_ > 1) {
        bit_output_stream_.WriteBits(byte, 8);
        num_buffered_bytes_ -= 1;
      }
    } else {
      num_buffered_bytes_ = 1;
      buffered_byte_ = static_cast<unsigned char>(lead_byte & 0xffu);
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
