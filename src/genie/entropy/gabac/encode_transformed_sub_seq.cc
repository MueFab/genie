/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/encode_transformed_sub_seq.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

#include "genie/entropy/gabac/context_selector.h"
#include "genie/entropy/gabac/luts_sub_symbol_transform.h"
#include "genie/entropy/gabac/writer.h"
#include "genie/util/block_stepper.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

using BinFunc = void (Writer::*)(uint64_t, const std::vector<unsigned int>&);

// -----------------------------------------------------------------------------
static BinFunc GetBinarizationWriter(
    const uint8_t output_symbol_size, const bool bypass_flag,
    const paramcabac::BinarizationParameters::BinarizationId bin_id,
    const paramcabac::BinarizationParameters& binarization_params,
    const paramcabac::StateVars& state_vars,
    std::vector<unsigned int>& bin_params) {
  BinFunc func = nullptr;
  if (bypass_flag) {
    switch (bin_id) {
      case paramcabac::BinarizationParameters::BinarizationId::BI:
        func = &Writer::WriteAsBiBypass;
        bin_params[0] = state_vars.GetCLengthBi();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::TU:
        func = &Writer::WriteAsTuBypass;
        bin_params[0] = binarization_params.GetCMax();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::EG:
      case paramcabac::BinarizationParameters::BinarizationId::SEG:
        func = &Writer::WriteAsEgBypass;
        break;
      case paramcabac::BinarizationParameters::BinarizationId::TEG:
      case paramcabac::BinarizationParameters::BinarizationId::STEG:
        func = &Writer::WriteAsTegBypass;
        bin_params[0] = binarization_params.GetCMaxTeg();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::SUTU:
      case paramcabac::BinarizationParameters::BinarizationId::SSUTU:
        func = &Writer::WriteAsSutuBypass;
        bin_params[0] = output_symbol_size;
        bin_params[1] = binarization_params.GetSplitUnitSize();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::DTU:
      case paramcabac::BinarizationParameters::BinarizationId::SDTU:
        func = &Writer::WriteAsDtuBypass;
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
        func = &Writer::WriteAsBiCabac;
        bin_params[0] = state_vars.GetCLengthBi();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::TU:
        func = &Writer::WriteAsTuCabac;
        bin_params[0] = binarization_params.GetCMax();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::EG:
      case paramcabac::BinarizationParameters::BinarizationId::SEG:
        func = &Writer::WriteAsEgCabac;
        break;
      case paramcabac::BinarizationParameters::BinarizationId::TEG:
      case paramcabac::BinarizationParameters::BinarizationId::STEG:
        func = &Writer::WriteAsTegCabac;
        bin_params[0] = binarization_params.GetCMaxTeg();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::SUTU:
      case paramcabac::BinarizationParameters::BinarizationId::SSUTU:
        func = &Writer::WriteAsSutuCabac;
        bin_params[0] = output_symbol_size;
        bin_params[1] = binarization_params.GetSplitUnitSize();
        break;
      case paramcabac::BinarizationParameters::BinarizationId::DTU:
      case paramcabac::BinarizationParameters::BinarizationId::SDTU:
        func = &Writer::WriteAsDtuCabac;
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
size_t EncodeTransformSubSeqOrder0(
    const paramcabac::TransformedSubSeq& transform_sub_seq_conf,
    util::DataBlock* symbols, size_t max_size) {
  assert(symbols != nullptr);

  if (size_t num_symbols = symbols->Size(); num_symbols <= 0) {
    return 0;
  }

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
  const uint8_t coding_sub_sym_size = support_vals.GetCodingSubsymSize();
  const uint64_t sub_sym_mask =
      paramcabac::StateVars::Get2PowN(coding_sub_sym_size) - 1;
  const bool bypass_flag = binarization.GetBypassFlag();

  util::DataBlock block(0, 1);
  OBufferStream bitstream(&block);
  Writer writer(&bitstream, bypass_flag,
                static_cast<unsigned int>(state_vars.GetNumCtxTotal()));
  writer.Start();

  std::vector<unsigned int> bin_params(
      4,  // first three elements are for binarization params, last one is for
      // ctxIdx
      0);

  util::BlockStepper r = symbols->GetReader();
  std::vector<Subsymbol> sub_symbols(state_vars.GetNumSubsymbols());

  ContextSelector ctx_selector(state_vars);
  const bool diff_enabled =
      transform_sub_seq_conf.GetTransformIdSubsym() ==
      paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING;

  BinFunc func =
      GetBinarizationWriter(output_symbol_size, bypass_flag, bin_id,
                            binarization_params, state_vars, bin_params);

  while (r.IsValid()) {
    if (max_size <= bitstream.Size()) {
      break;
    }

    // Split symbol into sub symbols and then encode sub symbols
    const uint64_t orig_symbol = r.Get();
    const uint64_t symbol_value = orig_symbol;  // abs(signedSymbolValue);
    uint64_t sub_sym_val_to_code = 0;

    uint32_t oss = output_symbol_size;

    UTILS_DIE_IF(
        state_vars.GetNumSubsymbols() > std::numeric_limits<uint8_t>::max(),
        "Too many sub symbols");
    for (uint8_t s = 0; s < static_cast<uint8_t>(state_vars.GetNumSubsymbols());
         s++) {
      sub_sym_val_to_code = sub_symbols[s].subsym_value =
          symbol_value >> (oss -= coding_sub_sym_size) & sub_sym_mask;
      sub_symbols[s].subsym_idx = s;

      if (diff_enabled) {
        assert(sub_symbols[s].prv_values[0] <= sub_sym_val_to_code);
        sub_sym_val_to_code -= sub_symbols[s].prv_values[0];
        sub_symbols[s].prv_values[0] = sub_symbols[s].subsym_value;
      }

      bin_params[3] = ctx_selector.GetContextIdxOrder0(s);

      (writer.*func)(sub_sym_val_to_code, bin_params);
    }

    r.Inc();
  }

  writer.Close();

  bitstream.Flush(symbols);

  return symbols->Size();  // Size of bitstream
}

// -----------------------------------------------------------------------------
size_t EncodeTransformSubSeqOrder1(
    const paramcabac::TransformedSubSeq& transform_sub_seq_conf,
    util::DataBlock* symbols, util::DataBlock* const dep_symbols,
    size_t max_size) {
  assert(symbols != nullptr);

  if (size_t num_symbols = symbols->Size(); num_symbols <= 0) {
    return 0;
  }

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
  const uint64_t sub_symbol_mask =
      paramcabac::StateVars::Get2PowN(coding_sub_symbol_size) - 1;
  const bool bypass_flag = binarization.GetBypassFlag();
  assert(bypass_flag == false);

  const uint8_t num_luts =
      state_vars.GetNumLuts(coding_order, support_vals.GetShareSubsymLutFlag(),
                            transform_sub_seq_conf.GetTransformIdSubsym());
  const uint8_t num_previous =
      state_vars.GetNumPrvs(support_vals.GetShareSubsymPrvFlag());

  util::DataBlock block(0, 1);
  OBufferStream bitstream(&block);
  Writer writer(&bitstream, bypass_flag,
                static_cast<unsigned int>(state_vars.GetNumCtxTotal()));
  writer.Start();

  std::vector<unsigned int> bin_params(
      4,  // first three elements are for binarization params, last one is for
      // ctxIdx
      0);

  util::BlockStepper r = symbols->GetReader();
  std::vector<Subsymbol> sub_symbols(state_vars.GetNumSubsymbols());

  LuTsSubSymbolTransform luts_sub_symbol_transform(
      support_vals, state_vars, num_luts, num_previous, true);
  if (num_luts > 0) {
    luts_sub_symbol_transform.EncodeLuTs(writer, symbols, dep_symbols);
  }

  util::BlockStepper r_dep;
  if (dep_symbols) {
    r_dep = dep_symbols->GetReader();
  }

  ContextSelector ctx_selector(state_vars);

  BinFunc func =
      GetBinarizationWriter(output_symbol_size, bypass_flag, bin_id,
                            binarization_params, state_vars, bin_params);

  while (r.IsValid()) {
    if (max_size <= bitstream.Size()) {
      break;
    }

    // Split symbol into sub symbols and then encode sub symbols
    const uint64_t orig_symbol = r.Get();
    const uint64_t symbol_value = orig_symbol;  // abs(signedSymbolValue);
    UTILS_DIE_IF(orig_symbol != symbol_value, "Loss of information");

    uint64_t sub_symbol_val_to_code = 0;

    uint64_t dep_symbol_value = 0, dep_sub_symbol_value = 0;
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
        dep_sub_symbol_value =
            (dep_symbol_value >> (oss - coding_sub_symbol_size)) &  // NOLINT
            sub_symbol_mask;                                        // NOLINT
        sub_symbols[prv_idx].prv_values[0] = dep_sub_symbol_value;
      }

      sub_symbol_val_to_code = sub_symbols[s].subsym_value =
          symbol_value >> (oss -= coding_sub_symbol_size) & sub_symbol_mask;
      sub_symbols[s].subsym_idx = s;

      bin_params[3] = ctx_selector.GetContextIdxOrderGt0(
          s, prv_idx, sub_symbols, coding_order);

      if (num_luts > 0) {
        sub_symbols[s].lut_entry_idx = 0;
        luts_sub_symbol_transform.TransformOrder1(sub_symbols, s, lut_idx,
                                                  prv_idx);
        sub_symbol_val_to_code = sub_symbols[s].lut_entry_idx;
        if (bin_id == paramcabac::BinarizationParameters::BinarizationId::TU) {
          bin_params[0] = static_cast<unsigned int>(
              std::min(static_cast<uint64_t>(binarization_params.GetCMax()),
                       sub_symbols[s].lut_num_max_elems));  // update cMax
        }
      }

      (writer.*func)(sub_symbol_val_to_code, bin_params);

      sub_symbols[prv_idx].prv_values[0] = sub_symbols[s].subsym_value;
    }

    r.Inc();
  }

  writer.Close();

  bitstream.Flush(symbols);

  return symbols->Size();  // Size of bitstream
}

// -----------------------------------------------------------------------------
size_t EncodeTransformSubSeqOrder2(
    const paramcabac::TransformedSubSeq& transform_sub_seq_conf,
    util::DataBlock* symbols, size_t max_size) {
  assert(symbols != nullptr);

  if (size_t num_symbols = symbols->Size(); num_symbols <= 0) {
    return 0;
  }

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
  const uint64_t sub_symbol_mask =
      paramcabac::StateVars::Get2PowN(coding_sub_symbol_size) - 1;
  const bool bypass_flag = binarization.GetBypassFlag();
  assert(bypass_flag == false);

  const uint8_t num_luts =
      state_vars.GetNumLuts(coding_order, support_vals.GetShareSubsymLutFlag(),
                            transform_sub_seq_conf.GetTransformIdSubsym());
  const uint8_t num_previous_flags =
      state_vars.GetNumPrvs(support_vals.GetShareSubsymPrvFlag());

  util::DataBlock block(0, 1);
  OBufferStream bitstream(&block);
  Writer writer(&bitstream, bypass_flag,
                static_cast<unsigned int>(state_vars.GetNumCtxTotal()));
  writer.Start();

  std::vector<unsigned int> bin_params(
      4,  // first three elements are for binarization params, last one is for
      // ctxIdx
      0);

  util::BlockStepper r = symbols->GetReader();
  std::vector<Subsymbol> sub_symbols(state_vars.GetNumSubsymbols());

  LuTsSubSymbolTransform luts_sub_symbol_transform(
      support_vals, state_vars, num_luts, num_previous_flags, true);
  if (num_luts > 0) {
    luts_sub_symbol_transform.EncodeLuTs(writer, symbols, nullptr);
  }

  ContextSelector ctx_selector(state_vars);

  BinFunc func =
      GetBinarizationWriter(output_symbol_size, bypass_flag, bin_id,
                            binarization_params, state_vars, bin_params);

  while (r.IsValid()) {
    if (max_size <= bitstream.Size()) {
      break;
    }

    // Split symbol into sub symbols and then encode sub symbols
    const uint64_t orig_symbol = r.Get();

    const uint64_t symbol_value = orig_symbol;  // abs(signedSymbolValue);
    UTILS_DIE_IF(orig_symbol != symbol_value, "Loss of information");
    uint64_t sub_symbol_val_to_code = 0;

    uint32_t oss = output_symbol_size;
    UTILS_DIE_IF(
        state_vars.GetNumSubsymbols() > std::numeric_limits<uint8_t>::max(),
        "Too many sub symbols");
    for (uint8_t s = 0; s < static_cast<uint8_t>(state_vars.GetNumSubsymbols());
         s++) {
      const uint8_t lut_idx =
          num_luts > 1 ? s : 0;  // either private or shared LUT
      const uint8_t prv_idx =
          num_previous_flags > 1 ? s : 0;  // either private or shared PRV

      sub_symbol_val_to_code = sub_symbols[s].subsym_value =
          symbol_value >> (oss -= coding_sub_symbol_size) & sub_symbol_mask;
      sub_symbols[s].subsym_idx = s;

      bin_params[3] = ctx_selector.GetContextIdxOrderGt0(
          s, prv_idx, sub_symbols, coding_order);

      if (num_luts > 0) {
        sub_symbols[s].lut_entry_idx = 0;
        luts_sub_symbol_transform.TransformOrder2(sub_symbols, s, lut_idx,
                                                  prv_idx);
        sub_symbol_val_to_code = sub_symbols[s].lut_entry_idx;
        if (bin_id == paramcabac::BinarizationParameters::BinarizationId::TU) {
          bin_params[0] = static_cast<unsigned int>(
              std::min(static_cast<uint64_t>(binarization_params.GetCMax()),
                       sub_symbols[s].lut_num_max_elems));  // update cMax
        }
      }

      (writer.*func)(sub_symbol_val_to_code, bin_params);

      sub_symbols[prv_idx].prv_values[1] = sub_symbols[prv_idx].prv_values[0];
      sub_symbols[prv_idx].prv_values[0] = sub_symbols[s].subsym_value;
    }

    r.Inc();
  }

  writer.Close();

  bitstream.Flush(symbols);

  return symbols->Size();  // Size of bitstream
}

// -----------------------------------------------------------------------------
size_t EncodeTransformSubSeq(
    const paramcabac::TransformedSubSeq& transform_sub_seq_conf,
    util::DataBlock* symbols, util::DataBlock* dep_symbols,
    const size_t max_size) {
  switch (transform_sub_seq_conf.GetSupportValues().GetCodingOrder()) {
    case 0:
      return EncodeTransformSubSeqOrder0(transform_sub_seq_conf, symbols,
                                         max_size);
    case 1:
      return EncodeTransformSubSeqOrder1(transform_sub_seq_conf, symbols,
                                         dep_symbols, max_size);
    case 2:
      return EncodeTransformSubSeqOrder2(transform_sub_seq_conf, symbols,
                                         max_size);
    default:
      UTILS_DIE("Unknown coding order");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
