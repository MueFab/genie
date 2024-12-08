/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/writer.h"

#include <cmath>
#include <vector>

#include "genie/entropy/gabac/context_tables.h"
//
// #include binary-arithmetic-decoder.cc from here instead of compiling it
// separately, so that we may call inlined member functions of class
// BinaryArithmeticDecoder in this file.
//
#include "genie/entropy/gabac/binary_arithmetic_encoder.cc"  // NOLINT

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
Writer::Writer(OBufferStream* const bitstream, const bool bypass_flag,
               const uint64_t num_contexts)
    : bit_output_stream_(*bitstream),
      binary_arithmetic_encoder_(bit_output_stream_),
      bypass_flag_(bypass_flag),
      num_contexts_(num_contexts) {
  if (!bypass_flag && num_contexts > 0) {
    context_models_ = contexttables::BuildContextTable(num_contexts_);
  }
}

// -----------------------------------------------------------------------------
Writer::~Writer() = default;

// -----------------------------------------------------------------------------
void Writer::Start() { binary_arithmetic_encoder_.Start(); }

// -----------------------------------------------------------------------------
void Writer::Close() { binary_arithmetic_encoder_.Flush(); }

// -----------------------------------------------------------------------------
void Writer::Reset() {
  binary_arithmetic_encoder_.Flush();
  context_models_.clear();
  context_models_ = contexttables::BuildContextTable(num_contexts_);
}

// -----------------------------------------------------------------------------
void Writer::WriteAsBiBypass(const uint64_t input,
                             const std::vector<unsigned int>& bin_params) {
  binary_arithmetic_encoder_.EncodeBinsEp(static_cast<unsigned int>(input),
                                          bin_params[0]);
}

// -----------------------------------------------------------------------------
void Writer::WriteAsBiCabac(const uint64_t input,
                            const std::vector<unsigned int>& bin_params) {
  const unsigned int c_length = bin_params[0];
  const unsigned int cm = bin_params[3];
  auto scan = context_models_.begin() + cm;
  for (int i = static_cast<int>(c_length) - 1; i >= 0;
       i--) {  // i must be signed
    const unsigned int bin =
        static_cast<unsigned int>(input >> static_cast<uint8_t>(i)) & 0x1u;
    binary_arithmetic_encoder_.EncodeBin(bin, &*scan++);
  }
}

// -----------------------------------------------------------------------------
void Writer::WriteAsTuBypass(const uint64_t input,
                             const std::vector<unsigned int>& bin_params) {
  const unsigned int c_max = bin_params[0];
  for (uint64_t i = 0; i < input; i++) {
    binary_arithmetic_encoder_.EncodeBinEp(1);
  }
  if (c_max > input) {
    binary_arithmetic_encoder_.EncodeBinEp(0);
  }
}

// -----------------------------------------------------------------------------
void Writer::WriteAsTuCabac(const uint64_t input,
                            const std::vector<unsigned int>& bin_params) {
  const unsigned int c_max = bin_params[0];
  const unsigned int cm = bin_params[3];
  auto scan = context_models_.begin() + cm;
  for (uint64_t i = 0; i < input; i++) {
    binary_arithmetic_encoder_.EncodeBin(1, &*scan++);
  }
  if (c_max > input) {
    binary_arithmetic_encoder_.EncodeBin(0, &*scan);
  }
}

// -----------------------------------------------------------------------------
void Writer::WriteAsEgBypass(const uint64_t input,
                             const std::vector<unsigned int>&) {
  const auto value_plus1 = static_cast<unsigned int>(input + 1);
  auto num_lead_zeros = static_cast<unsigned int>(floor(log2(value_plus1)));

  /* prefix */
  WriteAsBiBypass(1, std::vector({num_lead_zeros + 1}));
  if (num_lead_zeros) {
    /* suffix */
    WriteAsBiBypass(value_plus1 & ((1u << num_lead_zeros) - 1),  // NOLINT
                    std::vector({num_lead_zeros}));              // NOLINT
  }
}

// -----------------------------------------------------------------------------
void Writer::WriteAsEgCabac(const uint64_t input,
                            const std::vector<unsigned int>& bin_params) {
  const auto value_plus1 = static_cast<unsigned int>(input + 1);
  auto num_lead_zeros = static_cast<unsigned int>(floor(log2(value_plus1)));

  /* prefix */
  WriteAsBiCabac(
      1, std::vector<unsigned int>({num_lead_zeros + 1, 0, 0, bin_params[3]}));
  if (num_lead_zeros) {
    /* suffix */
    WriteAsBiBypass(value_plus1 & ((1u << num_lead_zeros) - 1),  // NOLINT
                    std::vector({num_lead_zeros}));              // NOLINT
  }
}

// -----------------------------------------------------------------------------
void Writer::WriteAsTegBypass(const uint64_t input,
                              const std::vector<unsigned int>& bin_params) {
  if (const unsigned int c_trunc_exp_gol_param = bin_params[0];
      input < c_trunc_exp_gol_param) {
    WriteAsTuBypass(input, bin_params);
  } else {
    WriteAsTuBypass(c_trunc_exp_gol_param, bin_params);
    WriteAsEgBypass(input - c_trunc_exp_gol_param,
                    std::vector<unsigned int>({0}));
  }
}

// -----------------------------------------------------------------------------

void Writer::WriteAsTegCabac(const uint64_t input,
                             const std::vector<unsigned int>& bin_params) {
  const unsigned int c_trunc_exp_gol_param = bin_params[0];

  WriteAsTuCabac(input, std::vector<unsigned int>(
                            {c_trunc_exp_gol_param, 0, 0, bin_params[3]}));
  if (input >= c_trunc_exp_gol_param) {
    WriteAsEgCabac(input - c_trunc_exp_gol_param,
                   std::vector<unsigned int>(
                       {0, 0, 0, bin_params[3] + c_trunc_exp_gol_param}));
  }
}

// -----------------------------------------------------------------------------

void Writer::WriteAsSutuBypass(const uint64_t input,
                               const std::vector<unsigned int>& bin_params) {
  const unsigned int output_sym_size = bin_params[0];
  const unsigned int split_unit_size = bin_params[1];

  unsigned int i, j;
  for (i = 0, j = output_sym_size; i < output_sym_size; i += split_unit_size) {
    const unsigned int unit_size = i == 0 && output_sym_size % split_unit_size
                                       ? output_sym_size % split_unit_size
                                       : split_unit_size;
    unsigned int c_max = (1u << unit_size) - 1;
    const unsigned int val = input >> (j -= unit_size) & c_max;
    WriteAsTuBypass(val, std::vector({c_max}));
  }
}

// -----------------------------------------------------------------------------

void Writer::WriteAsSutuCabac(const uint64_t input,
                              const std::vector<unsigned int>& bin_params) {
  const unsigned int output_sym_size = bin_params[0];
  const unsigned int split_unit_size = bin_params[1];

  unsigned int cm = bin_params[3];
  unsigned int i, j;
  for (i = 0, j = output_sym_size; i < output_sym_size; i += split_unit_size) {
    const unsigned int unit_size = i == 0 && output_sym_size % split_unit_size
                                       ? output_sym_size % split_unit_size
                                       : split_unit_size;
    unsigned int c_max = (1u << unit_size) - 1;
    const unsigned int val = input >> (j -= unit_size) & c_max;
    WriteAsTuCabac(val, std::vector<unsigned int>({c_max, 0, 0, cm}));
    cm += c_max;
  }
}

// -----------------------------------------------------------------------------

void Writer::WriteAsDtuBypass(uint64_t input,
                              const std::vector<unsigned int>& bin_params) {
  const unsigned int c_max_dtu = bin_params[2];

  WriteAsTuBypass(input < c_max_dtu ? input : c_max_dtu,
                  std::vector({c_max_dtu}));

  if (input >= c_max_dtu) {
    input -= c_max_dtu;

    WriteAsSutuBypass(input, bin_params);
  }
}

// -----------------------------------------------------------------------------

void Writer::WriteAsDtuCabac(uint64_t input,
                             const std::vector<unsigned int>& bin_params) {
  const unsigned int c_max_dtu = bin_params[2];

  WriteAsTuCabac(input < c_max_dtu ? input : c_max_dtu,
                 std::vector<unsigned int>({c_max_dtu, 0, 0, bin_params[3]}));

  if (input >= c_max_dtu) {
    input -= c_max_dtu;

    WriteAsSutuCabac(input,
                     std::vector<unsigned int>({bin_params[0], bin_params[1], 0,
                                                bin_params[3] + c_max_dtu}));
  }
}

// -----------------------------------------------------------------------------

void Writer::WriteLutSymbol(const uint64_t input,
                            const uint8_t coding_sub_sym_size) {
  const std::vector<unsigned int> bin_params(
      {coding_sub_sym_size, 2, 0, 0});  // ctxIdx = 0
  WriteAsSutuCabac(input, bin_params);
}

// -----------------------------------------------------------------------------

[[maybe_unused]] [[maybe_unused]] void Writer::WriteSignFlag(
    const int64_t input) {
  if (bypass_flag_)
    WriteAsBiBypass(input < 0, std::vector<unsigned int>({1}));
  else
    WriteAsBiCabac(
        input < 0,
        std::vector<unsigned int>(
            {1, 0, 0, static_cast<unsigned int>(num_contexts_ - 1)}));
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
