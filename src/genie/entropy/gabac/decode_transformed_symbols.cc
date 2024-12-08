/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// -----------------------------------------------------------------------------

#include "genie/entropy/gabac/decode_transformed_symbols.h"

#include <algorithm>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
TransformedSymbolsDecoder::TransformedSymbolsDecoder(
    util::DataBlock* bitstream,
    const paramcabac::TransformedSubSeq& transformed_sub_seq_conf,
    const unsigned int num_symbols)
    : num_encoded_symbols_(num_symbols),
      num_decoded_symbols_(0),
      output_symbol_size_(
          transformed_sub_seq_conf.GetSupportValues().GetOutputSymbolSize()),
      coding_sub_sym_size_(
          transformed_sub_seq_conf.GetSupportValues().GetCodingSubsymSize()),
      coding_order_(
          transformed_sub_seq_conf.GetSupportValues().GetCodingOrder()),
      sub_symbol_mask_(paramcabac::StateVars::Get2PowN(coding_sub_sym_size_) -
                       1),
      num_sub_symbols_(static_cast<uint8_t>(
          transformed_sub_seq_conf.GetStateVars().GetNumSubsymbols())),
      num_luts_(transformed_sub_seq_conf.GetStateVars().GetNumLuts(
          coding_order_,
          transformed_sub_seq_conf.GetSupportValues().GetShareSubsymLutFlag(),
          transformed_sub_seq_conf.GetTransformIdSubsym())),
      num_previous_bits_(transformed_sub_seq_conf.GetStateVars().GetNumPrvs(
          transformed_sub_seq_conf.GetSupportValues().GetShareSubsymPrvFlag())),
      reader_(bitstream,
              transformed_sub_seq_conf.GetBinarization().GetBypassFlag(),
              static_cast<unsigned int>(
                  transformed_sub_seq_conf.GetStateVars().GetNumCtxTotal())),
      ctx_selector_(transformed_sub_seq_conf.GetStateVars()),
      inv_luts_sub_symbol_transform_(
          transformed_sub_seq_conf.GetSupportValues(),
          transformed_sub_seq_conf.GetStateVars(), num_luts_,
          num_previous_bits_, false),
      diff_enabled_(transformed_sub_seq_conf.GetTransformIdSubsym() ==
                    paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING),
      custom_c_max_tu_(false),
      default_c_max_(transformed_sub_seq_conf.GetBinarization()
                         .GetCabacBinarizationParameters()
                         .GetCMax()),
      bin_id_(transformed_sub_seq_conf.GetBinarization().GetBinarizationId()),
      bin_params_(std::vector<unsigned int>(
          4, 0)),  // first three elements are for binarization params, last one
                   // is for ctxIdx
      binarization_(GetBinarizationReader(
          output_symbol_size_,
          transformed_sub_seq_conf.GetBinarization().GetBypassFlag(), bin_id_,
          transformed_sub_seq_conf.GetBinarization()
              .GetCabacBinarizationParameters(),
          transformed_sub_seq_conf.GetStateVars(), bin_params_)) {
  if (bitstream == nullptr || bitstream->Empty())
    return;  // Simple return as bitstream can be empty
  if (num_encoded_symbols_ <= 0)
    return;  // Simple return as num_encoded_symbols_ can be zero

  reader_.Start();

  if (num_luts_ > 0) {
    inv_luts_sub_symbol_transform_.DecodeLuTs(reader_);
    if (bin_id_ == paramcabac::BinarizationParameters::BinarizationId::TU) {
      custom_c_max_tu_ = true;
    }
  }
}

// -----------------------------------------------------------------------------
TransformedSymbolsDecoder::TransformedSymbolsDecoder(
    const TransformedSymbolsDecoder& transformed_sub_seq_decoder) = default;

// -----------------------------------------------------------------------------
uint64_t TransformedSymbolsDecoder::DecodeNextSymbol(
    const uint64_t* dep_symbol) {
  if (SymbolsAvail() <= 0) UTILS_DIE("No more transformed symbols available.");

  switch (coding_order_) {
    case 0:
      return DecodeNextSymbolOrder0();
    case 1:
      return DecodeNextSymbolOrder1(dep_symbol);
    case 2:
      return DecodeNextSymbolOrder2();
    default:
      UTILS_DIE("Unknown coding order");
  }
}

// -----------------------------------------------------------------------------
uint64_t TransformedSymbolsDecoder::DecodeNextSymbolOrder0() {
  uint64_t symbol_value = 0;
  if (num_decoded_symbols_ < num_encoded_symbols_) {
    // Decode subs symbols and merge them to construct the symbol
    std::vector<Subsymbol> sub_symbols(num_sub_symbols_);
    for (uint8_t s = 0; s < num_sub_symbols_; s++) {
      sub_symbols[s].subsym_idx = s;
      bin_params_[3] = ctx_selector_.GetContextIdxOrder0(s);

      sub_symbols[s].subsym_value = (reader_.*binarization_)(bin_params_);

      if (diff_enabled_) {
        sub_symbols[s].subsym_value += sub_symbols[s].prv_values[0];
        sub_symbols[s].prv_values[0] = sub_symbols[s].subsym_value;
      }

      symbol_value =
          symbol_value << coding_sub_sym_size_ | sub_symbols[s].subsym_value;
    }

    DecodeSignFlag(reader_, bin_id_, symbol_value);

    num_decoded_symbols_++;
  }

  return symbol_value;
}

// -----------------------------------------------------------------------------
uint64_t TransformedSymbolsDecoder::DecodeNextSymbolOrder1(
    const uint64_t* dep_symbol) {
  uint64_t symbol_value = 0;
  if (num_decoded_symbols_ < num_encoded_symbols_) {
    // Decode sub symbols and merge them to construct the symbol
    uint64_t dep_sub_symbols_value = 0;

    uint32_t oss = output_symbol_size_;
    std::vector<Subsymbol> sub_symbols(num_sub_symbols_);
    for (uint8_t s = 0; s < num_sub_symbols_; s++) {
      const uint8_t lut_idx =
          num_luts_ > 1 ? s : 0;  // either private or shared LUT
      const uint8_t prv_idx =
          num_previous_bits_ > 1 ? s : 0;  // either private or shared PRV

      if (dep_symbol) {
        dep_sub_symbols_value =
            *dep_symbol >> (oss -= coding_sub_sym_size_) & sub_symbol_mask_;
        sub_symbols[prv_idx].prv_values[0] = dep_sub_symbols_value;
      }

      sub_symbols[s].subsym_idx = s;
      bin_params_[3] = ctx_selector_.GetContextIdxOrderGt0(
          s, prv_idx, sub_symbols, coding_order_);

      if (custom_c_max_tu_) {
        sub_symbols[s].lut_num_max_elems =
            inv_luts_sub_symbol_transform_.GetNumMaxElemsOrder1(
                sub_symbols, lut_idx, prv_idx);
        bin_params_[0] = static_cast<unsigned int>(
            std::min(default_c_max_,
                     sub_symbols[s].lut_num_max_elems));  // update cMax
      }
      sub_symbols[s].subsym_value = (reader_.*binarization_)(bin_params_);

      if (num_luts_ > 0) {
        sub_symbols[s].lut_entry_idx = sub_symbols[s].subsym_value;
        inv_luts_sub_symbol_transform_.InvTransformOrder1(sub_symbols, s,
                                                          lut_idx, prv_idx);
      }

      sub_symbols[prv_idx].prv_values[0] = sub_symbols[s].subsym_value;

      symbol_value =
          symbol_value << coding_sub_sym_size_ | sub_symbols[s].subsym_value;
    }

    DecodeSignFlag(reader_, bin_id_, symbol_value);

    num_decoded_symbols_++;
  }

  return symbol_value;
}

// -----------------------------------------------------------------------------
uint64_t TransformedSymbolsDecoder::DecodeNextSymbolOrder2() {
  uint64_t symbol_value = 0;
  if (num_decoded_symbols_ < num_encoded_symbols_) {
    // Decode sub symbols and merge them to construct the symbol
    std::vector<Subsymbol> sub_symbols(num_sub_symbols_);
    for (uint8_t s = 0; s < num_sub_symbols_; s++) {
      const uint8_t lut_idx =
          num_luts_ > 1 ? s : 0;  // either private or shared LUT
      const uint8_t prv_idx =
          num_previous_bits_ > 1 ? s : 0;  // either private or shared PRV

      sub_symbols[s].subsym_idx = s;
      bin_params_[3] = ctx_selector_.GetContextIdxOrderGt0(
          s, prv_idx, sub_symbols, coding_order_);

      if (custom_c_max_tu_) {
        sub_symbols[s].lut_num_max_elems =
            inv_luts_sub_symbol_transform_.GetNumMaxElemsOrder2(
                sub_symbols, lut_idx, prv_idx);
        bin_params_[0] = static_cast<unsigned int>(
            std::min(default_c_max_,
                     sub_symbols[s].lut_num_max_elems));  // update cMax
      }
      sub_symbols[s].subsym_value = (reader_.*binarization_)(bin_params_);

      if (num_luts_ > 0) {
        sub_symbols[s].lut_entry_idx = sub_symbols[s].subsym_value;
        inv_luts_sub_symbol_transform_.InvTransformOrder2(sub_symbols, s,
                                                          lut_idx, prv_idx);
      }

      sub_symbols[prv_idx].prv_values[1] = sub_symbols[prv_idx].prv_values[0];
      sub_symbols[prv_idx].prv_values[0] = sub_symbols[s].subsym_value;

      symbol_value =
          symbol_value << coding_sub_sym_size_ | sub_symbols[s].subsym_value;
    }

    DecodeSignFlag(reader_, bin_id_, symbol_value);

    num_decoded_symbols_++;
  }

  return symbol_value;
}

// -----------------------------------------------------------------------------
size_t TransformedSymbolsDecoder::SymbolsAvail() const {
  return num_encoded_symbols_ - num_decoded_symbols_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
