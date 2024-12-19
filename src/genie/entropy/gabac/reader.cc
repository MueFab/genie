/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/reader.h"

#include <vector>

#include "genie/entropy/gabac/context_tables.h"

//
// #include binary-arithmetic-decoder.cc from here instead of compiling it
// separately, so that we may call inlined member functions of class
// BinaryArithmeticDecoder in this file.
//
#include "genie/entropy/gabac/binary_arithmetic_decoder.cc"  // NOLINT

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
Reader::Reader(util::DataBlock* bitstream, const bool bypass_flag,
               const uint64_t num_contexts)
    : m_bit_input_stream_(bitstream),
      m_dec_bin_cabac_(m_bit_input_stream_),
      m_bypass_flag_(bypass_flag),
      m_num_contexts_(num_contexts) {
  if (!bypass_flag && num_contexts > 0) {
    m_context_models_ = contexttables::BuildContextTable(m_num_contexts_);
  }
}

// -----------------------------------------------------------------------------
Reader::~Reader() = default;

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsBIbypass(const std::vector<unsigned int>& bin_params) {
  return m_dec_bin_cabac_.DecodeBinsEp(bin_params[0]);
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsBIcabac(const std::vector<unsigned int>& bin_params) {
  unsigned int bins = 0;
  const unsigned int cm = bin_params[3];
  const unsigned int c_length = bin_params[0];
  auto scan = m_context_models_.begin() + cm;
  for (size_t i = c_length; i > 0; i--) {
    bins = bins << 1u | m_dec_bin_cabac_.DecodeBin(&*scan++);
  }
  return bins;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsTUbypass(const std::vector<unsigned int>& bin_params) {
  unsigned int i = 0;
  const unsigned int c_max = bin_params[0];
  while (i < c_max) {
    if (m_dec_bin_cabac_.DecodeBinsEp(1) == 0) break;
    i++;
  }
  return i;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsTUcabac(const std::vector<unsigned int>& bin_params) {
  unsigned int i = 0;
  const unsigned int cm = bin_params[3];
  const unsigned int c_max = bin_params[0];
  auto scan = m_context_models_.begin() + cm;
  while (i < c_max) {
    if (m_dec_bin_cabac_.DecodeBin(&*scan) == 0) break;
    i++;
    ++scan;
  }
  return i;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsEGbypass(const std::vector<unsigned int>&) {
  unsigned int bins = 0;
  unsigned int i = 0;
  while (ReadAsBIbypass(std::vector<unsigned int>({1})) == 0) {
    i++;
  }
  if (i != 0) {
    bins = 1u << i | m_dec_bin_cabac_.DecodeBinsEp(i);
  } else {
    return 0;
  }
  return bins - 1;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsEGcabac(const std::vector<unsigned int>& bin_params) {
  const unsigned int cm = bin_params[3];
  auto scan = m_context_models_.begin() + cm;
  unsigned int i = 0;
  while (m_dec_bin_cabac_.DecodeBin(&*scan) == 0) {
    ++scan;
    i++;
  }
  unsigned int bins = 0;
  if (i != 0) {
    bins = 1u << i | m_dec_bin_cabac_.DecodeBinsEp(i);
  } else {
    return 0;
  }
  return bins - 1;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsTeGbypass(const std::vector<unsigned int>& bin_params) {
  uint64_t value = ReadAsTUbypass(bin_params);
  if (static_cast<unsigned int>(value) == bin_params[0]) {
    value += ReadAsEGbypass(std::vector<unsigned int>({0}));
  }
  return value;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsTeGcabac(const std::vector<unsigned int>& bin_params) {
  uint64_t value = ReadAsTUcabac(bin_params);
  if (static_cast<unsigned int>(value) == bin_params[0]) {
    value += ReadAsEGcabac(
        std::vector<unsigned int>({0, 0, 0, bin_params[3] + bin_params[0]}));
  }
  return value;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsSutUbypass(const std::vector<unsigned int>& bin_params) {
  const uint32_t output_sym_size = bin_params[0];
  const uint32_t split_unit_size = bin_params[1];

  uint64_t value = 0;

  for (uint32_t i = 0; i < output_sym_size; i += split_unit_size) {
    uint64_t val = 0;
    uint32_t c_max = i == 0 && output_sym_size % split_unit_size
                         ? (1u << output_sym_size % split_unit_size) - 1
                         : (1u << split_unit_size) - 1;
    val = ReadAsTUbypass(std::vector({c_max}));

    value = value << split_unit_size | val;
  }

  return value;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsSutUcabac(const std::vector<unsigned int>& bin_params) {
  const uint32_t output_sym_size = bin_params[0];
  const uint32_t split_unit_size = bin_params[1];

  uint32_t cm = bin_params[3];
  uint64_t value = 0;

  for (uint32_t i = 0; i < output_sym_size; i += split_unit_size) {
    uint64_t val = 0;
    uint32_t c_max = i == 0 && output_sym_size % split_unit_size
                         ? (1u << output_sym_size % split_unit_size) - 1
                         : (1u << split_unit_size) - 1;
    val = ReadAsTUcabac(std::vector<unsigned int>({c_max, 0, 0, cm}));
    cm += c_max;

    value = value << split_unit_size | val;
  }

  return value;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsDtUbypass(const std::vector<unsigned int>& bin_params) {
  const uint32_t c_max_dtu = bin_params[2];

  uint64_t value = ReadAsTUbypass(std::vector({c_max_dtu}));

  if (value >= c_max_dtu) {
    value += ReadAsSutUbypass(bin_params);
  }

  return value;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadAsDtUcabac(const std::vector<unsigned int>& bin_params) {
  const uint32_t c_max_dtu = bin_params[2];

  uint64_t value = ReadAsTUcabac(
      std::vector<unsigned int>({c_max_dtu, 0, 0, bin_params[3]}));

  if (value >= c_max_dtu) {
    value += ReadAsSutUcabac(std::vector<unsigned int>(
        {bin_params[0], bin_params[1], 0, bin_params[3] + c_max_dtu}));
  }

  return value;
}

// -----------------------------------------------------------------------------
uint64_t Reader::ReadLutSymbol(const uint8_t coding_subsym_size) {
  const std::vector<unsigned int> bin_params(
      {coding_subsym_size, 2, 0, 0});  // ctxIdx = 0
  return ReadAsSutUcabac(bin_params);
}

// -----------------------------------------------------------------------------
bool Reader::ReadSignFlag() {
  if (m_bypass_flag_)
    return static_cast<bool>(ReadAsBIbypass(std::vector<unsigned int>({1})));
  return static_cast<bool>(ReadAsBIcabac(std::vector<unsigned int>(
      {1, 0, 0, static_cast<unsigned int>(m_num_contexts_ - 1)})));
}

// -----------------------------------------------------------------------------
void Reader::Start() { m_dec_bin_cabac_.Start(); }

// -----------------------------------------------------------------------------
size_t Reader::Close() { return m_dec_bin_cabac_.Close(); }

// -----------------------------------------------------------------------------
void Reader::Reset() {
  m_dec_bin_cabac_.Reset();
  m_context_models_.clear();
  m_context_models_ = contexttables::BuildContextTable(m_num_contexts_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
