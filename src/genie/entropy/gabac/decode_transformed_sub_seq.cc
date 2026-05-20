/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/decode_transformed_sub_seq.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

#include "genie/entropy/gabac/context_selector.h"
#include "genie/entropy/gabac/luts_sub_symbol_transform.h"
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

void DecodeSignFlag(
    Reader& reader,
    const paramcabac::BinarizationParameters::BinarizationId bin_id,
    uint64_t& symbol_value) {
  if (symbol_value != 0) {
    switch (bin_id) {
      case paramcabac::BinarizationParameters::BinarizationId::SEG:
      case paramcabac::BinarizationParameters::BinarizationId::STEG:
      case paramcabac::BinarizationParameters::BinarizationId::SSUTU:
      case paramcabac::BinarizationParameters::BinarizationId::SDTU:
        if (reader.ReadSignFlag()) {
          const int64_t symbol_value_signed =
              -static_cast<int64_t>(symbol_value);
          symbol_value = static_cast<uint64_t>(symbol_value_signed);
        }
        break;
      default:
        break;
    }
  }
}

// -----------------------------------------------------------------------------

BinFunc GetBinarizationReader(
    const uint8_t output_symbol_size, const bool bypass_flag,
    const paramcabac::BinarizationParameters::BinarizationId bin_id,
    const paramcabac::BinarizationParameters& binarization_params,
    const paramcabac::StateVars& state_vars,
    std::vector<unsigned int>& bin_params) {
  BinFunc func = nullptr;
  if (bypass_flag) {
    switch (bin_id) {
      case paramcabac::BinarizationParameters::BinarizationId::BI:
        func = &Reader::ReadAsBIbypass;
        bin_params[0] = state_vars.GetCLengthBi();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::TU:
        func = &Reader::ReadAsTUbypass;
        bin_params[0] = binarization_params.GetCMax();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::EG:
      case paramcabac::BinarizationParameters::BinarizationId::SEG:
        func = &Reader::ReadAsEGbypass;
        break;
      case paramcabac::BinarizationParameters::BinarizationId::TEG:
      case paramcabac::BinarizationParameters::BinarizationId::STEG:
        func = &Reader::ReadAsTeGbypass;
        bin_params[0] = binarization_params.GetCMaxTeg();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::SUTU:
      case paramcabac::BinarizationParameters::BinarizationId::SSUTU:
        func = &Reader::ReadAsSutUbypass;
        bin_params[0] = output_symbol_size;
        bin_params[1] = binarization_params.GetSplitUnitSize();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::DTU:
      case paramcabac::BinarizationParameters::BinarizationId::SDTU:
        func = &Reader::ReadAsDtUbypass;
        bin_params[0] = output_symbol_size;
        bin_params[1] = binarization_params.GetSplitUnitSize();
        bin_params[2] = binarization_params.GetCMaxDtu();
        break;
      default:
        UTILS_DIE("Unknown Binarization");
    }
  } else {
    switch (bin_id) {
      case paramcabac::BinarizationParameters::BinarizationId::BI:
        func = &Reader::ReadAsBIcabac;
        bin_params[0] = state_vars.GetCLengthBi();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::TU:
        func = &Reader::ReadAsTUcabac;
        bin_params[0] = binarization_params.GetCMax();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::EG:
      case paramcabac::BinarizationParameters::BinarizationId::SEG:
        func = &Reader::ReadAsEGcabac;
        break;
      case paramcabac::BinarizationParameters::BinarizationId::TEG:
      case paramcabac::BinarizationParameters::BinarizationId::STEG:
        func = &Reader::ReadAsTeGcabac;
        bin_params[0] = binarization_params.GetCMaxTeg();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::SUTU:
      case paramcabac::BinarizationParameters::BinarizationId::SSUTU:
        func = &Reader::ReadAsSutUcabac;
        bin_params[0] = output_symbol_size;
        bin_params[1] = binarization_params.GetSplitUnitSize();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::DTU:
      case paramcabac::BinarizationParameters::BinarizationId::SDTU:
        func = &Reader::ReadAsDtUcabac;
        bin_params[0] = output_symbol_size;
        bin_params[1] = binarization_params.GetSplitUnitSize();
        bin_params[2] = binarization_params.GetCMaxDtu();
        break;
      default:
        UTILS_DIE("Unknown Binarization");
    }
  }

  return func;
}

// -----------------------------------------------------------------------------

size_t DecodeTransformSubSeqOrder0(
    const paramcabac::TransformedSubSeq& transformed_sub_seq_conf,
    const unsigned int num_encoded_symbols, util::DataBlock* bitstream,
    const uint8_t word_size) {
  if (bitstream == nullptr) {
    UTILS_DIE("Bitstream is null");
  }

  if (num_encoded_symbols <= 0) return 0;

  const paramcabac::SupportValues& support_vals =
      transformed_sub_seq_conf.GetSupportValues();
  const paramcabac::Binarization& binarization =
      transformed_sub_seq_conf.GetBinarization();
  const paramcabac::BinarizationParameters& binarization_params =
      binarization.GetCabacBinarizationParameters();
  const paramcabac::StateVars& state_vars =
      transformed_sub_seq_conf.GetStateVars();
  const paramcabac::BinarizationParameters::BinarizationId bin_id =
      binarization.GetBinarizationId();

  const uint8_t output_symbol_size = support_vals.GetOutputSymbolSize();
  const uint8_t coding_sub_symbol_size = support_vals.GetCodingSubsymSize();
  const bool bypass_flag = binarization.GetBypassFlag();
  size_t payload_size_used = 0;

  Reader reader(bitstream, bypass_flag,
                static_cast<unsigned int>(state_vars.GetNumCtxTotal()));
  reader.Start();

  std::vector<unsigned int> bin_params(
      4,  // first three elements are for binarization params, last one is for
      // ctxIdx
      0);

  util::DataBlock decoded_symbols(num_encoded_symbols, word_size);
  util::BlockStepper r = decoded_symbols.GetReader();
  std::vector<Subsymbol> sub_symbols(state_vars.GetNumSubsymbols());

  const ContextSelector ctx_selector(state_vars);
  const bool diff_enabled =
      transformed_sub_seq_conf.GetTransformIdSubsym() ==
      paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING;

  const BinFunc func =
      GetBinarizationReader(output_symbol_size, bypass_flag, bin_id,
                            binarization_params, state_vars, bin_params);

  while (r.IsValid()) {
    // Decode sub symbols and merge them to construct symbols
    uint64_t symbol_value = 0;

    UTILS_DIE_IF(
        state_vars.GetNumSubsymbols() > std::numeric_limits<uint8_t>::max(),
        "Too many sub symbols");
    for (uint8_t s = 0; s < static_cast<uint8_t>(state_vars.GetNumSubsymbols());
         s++) {
      sub_symbols[s].subsym_idx = s;
      bin_params[3] = ctx_selector.GetContextIdxOrder0(s);

      sub_symbols[s].subsym_value = (reader.*func)(bin_params);

      if (diff_enabled) {
        sub_symbols[s].subsym_value += sub_symbols[s].prv_values[0];
        sub_symbols[s].prv_values[0] = sub_symbols[s].subsym_value;
      }

      symbol_value =
          symbol_value << coding_sub_symbol_size | sub_symbols[s].subsym_value;
    }

    DecodeSignFlag(reader, bin_id, symbol_value);

    r.Set(symbol_value);
    r.Inc();
  }

  payload_size_used = reader.Close();

  decoded_symbols.Swap(bitstream);

  return payload_size_used;
}

// -----------------------------------------------------------------------------

size_t DecodeTransformSubSeqOrder1(
    const paramcabac::TransformedSubSeq& trans_sub_seq_conf,
    const unsigned int num_encoded_symbols, util::DataBlock* bitstream,
    util::DataBlock* const dep_symbols, uint8_t word_size) {
  if (bitstream == nullptr) {
    UTILS_DIE("Bitstream is null");
  }

  if (num_encoded_symbols <= 0) return 0;

  const paramcabac::SupportValues& support_vals =
      trans_sub_seq_conf.GetSupportValues();
  const paramcabac::Binarization& binarization =
      trans_sub_seq_conf.GetBinarization();
  const paramcabac::BinarizationParameters& binarization_params =
      binarization.GetCabacBinarizationParameters();
  const paramcabac::StateVars& state_vars =
      trans_sub_seq_conf.GetStateVars();
  const paramcabac::BinarizationParameters::BinarizationId bin_id =
      binarization.GetBinarizationId();

  const uint8_t output_symbol_size = support_vals.GetOutputSymbolSize();
  const uint8_t coding_sub_symbol_size = support_vals.GetCodingSubsymSize();
  const uint8_t coding_order = support_vals.GetCodingOrder();
  const uint64_t sub_symbol_mask =
      paramcabac::StateVars::Get2PowN(coding_sub_symbol_size) - 1;
  const bool bypass_flag = binarization.GetBypassFlag();
  assert(bypass_flag == false);

  const uint8_t num_luts =
      state_vars.GetNumLuts(coding_order, support_vals.GetShareSubsymLutFlag(),
                            trans_sub_seq_conf.GetTransformIdSubsym());
  const uint8_t num_previous =
      state_vars.GetNumPrvs(support_vals.GetShareSubsymPrvFlag());
  size_t payload_size_used = 0;

  Reader reader(bitstream, bypass_flag,
                static_cast<unsigned int>(state_vars.GetNumCtxTotal()));
  reader.Start();

  std::vector<unsigned int> bin_params(
      4,  // first three elements are for binarization params, last one is for
      // ctxIdx
      0);

  util::DataBlock decoded_symbols(num_encoded_symbols, word_size);
  util::BlockStepper r = decoded_symbols.GetReader();
  std::vector<Subsymbol> sub_symbols(state_vars.GetNumSubsymbols());

  LuTsSubSymbolTransform inv_luts_sub_symbol_transform(
      support_vals, state_vars, num_luts, num_previous, false);
  bool custom_c_max_tu = false;
  if (num_luts > 0) {
    inv_luts_sub_symbol_transform.DecodeLuTs(reader);
    if (bin_id == paramcabac::BinarizationParameters::BinarizationId::TU) {
      custom_c_max_tu = true;
    }
  }

  util::BlockStepper r_dep;
  if (dep_symbols) {
    r_dep = dep_symbols->GetReader();
  }

  ContextSelector ctx_selector(state_vars);

  BinFunc func =
      GetBinarizationReader(output_symbol_size, bypass_flag, bin_id,
                            binarization_params, state_vars, bin_params);

  while (r.IsValid()) {
    // Decode sub symbols and merge them to construct symbols
    uint64_t symbol_value = 0;

    uint64_t dep_symbol_value = 0, dep_sub_sym_value = 0;
    if (r_dep.IsValid()) {
      dep_symbol_value = r_dep.Get();
      r_dep.Inc();
    }

    uint32_t oss = output_symbol_size;
    UTILS_DIE_IF(
        state_vars.GetNumSubsymbols() > std::numeric_limits<uint8_t>::max(),
        "Too many sub symbols");
    for (uint8_t s = 0; s < static_cast<uint8_t>(state_vars.GetNumSubsymbols());
         s++) {
      const uint8_t lut_idx =
          num_luts > 1 ? s : 0;  // either private or shared LUT
      const uint8_t prv_idx =
          num_previous > 1 ? s : 0;  // either private or shared PRV

      if (dep_symbols) {
        dep_sub_sym_value =
            dep_symbol_value >> (oss -= coding_sub_symbol_size) &
            sub_symbol_mask;
        sub_symbols[prv_idx].prv_values[0] = dep_sub_sym_value;
      }

      sub_symbols[s].subsym_idx = s;
      bin_params[3] = ctx_selector.GetContextIdxOrderGt0(
          s, prv_idx, sub_symbols, coding_order);

      if (custom_c_max_tu) {
        sub_symbols[s].lut_num_max_elems =
            inv_luts_sub_symbol_transform.GetNumMaxElemsOrder1(
                sub_symbols, lut_idx, prv_idx);
        bin_params[0] = static_cast<unsigned int>(
            std::min(static_cast<uint64_t>(binarization_params.GetCMax()),
                     sub_symbols[s].lut_num_max_elems));  // update cMax
      }
      sub_symbols[s].subsym_value = (reader.*func)(bin_params);

      if (num_luts > 0) {
        sub_symbols[s].lut_entry_idx = sub_symbols[s].subsym_value;
        inv_luts_sub_symbol_transform.InvTransformOrder1(sub_symbols, s,
                                                         lut_idx, prv_idx);
      }

      sub_symbols[prv_idx].prv_values[0] = sub_symbols[s].subsym_value;

      symbol_value =
          symbol_value << coding_sub_symbol_size | sub_symbols[s].subsym_value;
    }

    DecodeSignFlag(reader, bin_id, symbol_value);

    r.Set(symbol_value);
    r.Inc();
  }

  payload_size_used = reader.Close();

  decoded_symbols.Swap(bitstream);

  return payload_size_used;
}

// -----------------------------------------------------------------------------

size_t DecodeTransformSubSeqOrder2(
    const paramcabac::TransformedSubSeq& transform_sub_seq_conf,
    const unsigned int num_encoded_symbols, util::DataBlock* bitstream,
    uint8_t word_size) {
  if (bitstream == nullptr) {
    UTILS_DIE("Bitstream is null");
  }

  if (num_encoded_symbols <= 0) return 0;

  const paramcabac::SupportValues& support_vals =
      transform_sub_seq_conf.GetSupportValues();
  const paramcabac::Binarization& binarization =
      transform_sub_seq_conf.GetBinarization();
  const paramcabac::BinarizationParameters& binarization_params =
      binarization.GetCabacBinarizationParameters();
  const paramcabac::StateVars& state_vars =
      transform_sub_seq_conf.GetStateVars();
  const paramcabac::BinarizationParameters::BinarizationId bin_id =
      binarization.GetBinarizationId();

  const uint8_t output_symbol_size = support_vals.GetOutputSymbolSize();
  const uint8_t coding_sub_symbol_size = support_vals.GetCodingSubsymSize();
  const uint8_t coding_order = support_vals.GetCodingOrder();
  const bool bypass_flag = binarization.GetBypassFlag();
  assert(bypass_flag == false);

  const uint8_t num_luts =
      state_vars.GetNumLuts(coding_order, support_vals.GetShareSubsymLutFlag(),
                            transform_sub_seq_conf.GetTransformIdSubsym());
  const uint8_t num_previous =
      state_vars.GetNumPrvs(support_vals.GetShareSubsymPrvFlag());
  size_t payload_size_used = 0;

  Reader reader(bitstream, bypass_flag,
                static_cast<unsigned int>(state_vars.GetNumCtxTotal()));
  reader.Start();

  std::vector<unsigned int> bin_params(
      4,  // first three elements are for binarization params, last one is for
      // ctxIdx
      0);

  util::DataBlock decoded_symbols(num_encoded_symbols, word_size);
  util::BlockStepper r = decoded_symbols.GetReader();
  std::vector<Subsymbol> sub_symbols(state_vars.GetNumSubsymbols());

  LuTsSubSymbolTransform inv_luts_sub_symbol_transform(
      support_vals, state_vars, num_luts, num_previous, false);
  bool custom_c_max_tu = false;
  if (num_luts > 0) {
    inv_luts_sub_symbol_transform.DecodeLuTs(reader);
    if (bin_id == paramcabac::BinarizationParameters::BinarizationId::TU) {
      custom_c_max_tu = true;
    }
  }

  ContextSelector ctx_selector(state_vars);

  BinFunc func =
      GetBinarizationReader(output_symbol_size, bypass_flag, bin_id,
                            binarization_params, state_vars, bin_params);

  while (r.IsValid()) {
    // Decode sub symbols and merge them to construct symbols
    uint64_t symbol_value = 0;

    UTILS_DIE_IF(
        state_vars.GetNumSubsymbols() > std::numeric_limits<uint8_t>::max(),
        "Too many sub symbols");
    for (uint8_t s = 0; s < static_cast<uint8_t>(state_vars.GetNumSubsymbols());
         s++) {
      const uint8_t lut_idx =
          num_luts > 1 ? s : 0;  // either private or shared LUT
      const uint8_t prv_idx =
          num_previous > 1 ? s : 0;  // either private or shared PRV

      sub_symbols[s].subsym_idx = s;
      bin_params[3] = ctx_selector.GetContextIdxOrderGt0(
          s, prv_idx, sub_symbols, coding_order);

      if (custom_c_max_tu) {
        sub_symbols[s].lut_num_max_elems =
            inv_luts_sub_symbol_transform.GetNumMaxElemsOrder2(
                sub_symbols, lut_idx, prv_idx);
        bin_params[0] = static_cast<unsigned int>(
            std::min(static_cast<uint64_t>(binarization_params.GetCMax()),
                     sub_symbols[s].lut_num_max_elems));  // update cMax
      }
      sub_symbols[s].subsym_value = (reader.*func)(bin_params);

      if (num_luts > 0) {
        sub_symbols[s].lut_entry_idx = sub_symbols[s].subsym_value;
        inv_luts_sub_symbol_transform.InvTransformOrder2(sub_symbols, s,
                                                         lut_idx, prv_idx);
      }

      sub_symbols[prv_idx].prv_values[1] = sub_symbols[prv_idx].prv_values[0];
      sub_symbols[prv_idx].prv_values[0] = sub_symbols[s].subsym_value;

      symbol_value =
          symbol_value << coding_sub_symbol_size | sub_symbols[s].subsym_value;
    }

    DecodeSignFlag(reader, bin_id, symbol_value);

    r.Set(symbol_value);
    r.Inc();
  }

  payload_size_used = reader.Close();

  decoded_symbols.Swap(bitstream);

  return payload_size_used;
}

// -----------------------------------------------------------------------------

size_t DecodeTransformSubSeq(
    const paramcabac::TransformedSubSeq& transform_sub_seq_conf,
    const unsigned int num_encoded_symbols, util::DataBlock* bitstream,
    const uint8_t word_size, util::DataBlock* dep_symbols) {
  switch (transform_sub_seq_conf.GetSupportValues().GetCodingOrder()) {
    case 0:
      return DecodeTransformSubSeqOrder0(
          transform_sub_seq_conf, num_encoded_symbols, bitstream, word_size);
    case 1:
      return DecodeTransformSubSeqOrder1(transform_sub_seq_conf,
                                         num_encoded_symbols, bitstream,
                                         dep_symbols, word_size);
    case 2:
      return DecodeTransformSubSeqOrder2(
          transform_sub_seq_conf, num_encoded_symbols, bitstream, word_size);
    default:
      UTILS_DIE("Unknown coding order");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------