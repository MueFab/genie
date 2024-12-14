/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#include "genie/entropy/paramcabac/state_vars.h"

#include <cmath>
#include <iostream>

#include "genie/entropy/paramcabac/binarization_parameters.h"
#include "genie/util/log.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Gabac";

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------
bool StateVars::operator==(const StateVars& bin) const {
  return num_alpha_subsym_ == bin.num_alpha_subsym_ &&
         num_subsyms_ == bin.num_subsyms_ &&
         num_ctx_subsym_ == bin.num_ctx_subsym_ &&
         c_length_bi_ == bin.c_length_bi_ &&
         coding_order_ctx_offset_[0] == bin.coding_order_ctx_offset_[0] &&
         coding_order_ctx_offset_[1] == bin.coding_order_ctx_offset_[1] &&
         coding_order_ctx_offset_[2] == bin.coding_order_ctx_offset_[2] &&
         coding_size_ctx_offset_ == bin.coding_size_ctx_offset_ &&
         num_ctx_luts_ == bin.num_ctx_luts_ &&
         num_ctx_total_ == bin.num_ctx_total_;
}

// -----------------------------------------------------------------------------
StateVars::StateVars()
    : num_alpha_subsym_(0),
      num_subsyms_(0),
      num_ctx_subsym_(0),
      c_length_bi_(0),
      coding_order_ctx_offset_{0, 0, 0},
      coding_size_ctx_offset_(0),
      num_ctx_luts_(0),
      num_ctx_total_(0) {}

// -----------------------------------------------------------------------------
uint64_t StateVars::GetNumAlphaSpecial(const core::GenSubIndex& subsequence_id,
                                       const core::AlphabetId alphabet_id) {
  uint64_t num_alpha_special = 0;
  if (subsequence_id == core::gen_sub::kMismatchType) {  // mmtype subseq 0
    num_alpha_special = 3;
  } else if (subsequence_id ==
                 core::gen_sub::kMismatchTypeSubstBase  // mmtype subseq 1
             || subsequence_id ==
                    core::gen_sub::kMismatchTypeInsert  // mmtype subseq 2
  ) {
    num_alpha_special = alphabet_id == core::AlphabetId::kAcgtn ? 5 : 16;
  }
  if (subsequence_id == core::gen_sub::kClipsType) {  // mmtype subseq 1
    num_alpha_special = 9;
  } else if (subsequence_id == core::gen_sub::kClipsSoftClip) {  // subseq 2
    num_alpha_special = (alphabet_id == core::AlphabetId::kAcgtn ? 5 : 16) + 1;
  } else if (subsequence_id == core::gen_sub::kUnalignedReads ||
             subsequence_id ==
                 core::gen_sub::kRefTransTransform) {  // ureads subseq 0
    num_alpha_special = alphabet_id == core::AlphabetId::kAcgtn ? 5 : 16;
  } else if (subsequence_id == core::gen_sub::kRtype) {  // rtype subseq 0
    num_alpha_special = 6;
  }

  return num_alpha_special;
}

// -----------------------------------------------------------------------------
uint8_t StateVars::GetNumLuts(
    const uint8_t coding_order, const bool share_subsym_lut_flag,
    const SupportValues::TransformIdSubsym trnsf_subsym_id) const {
  return coding_order > 0 && trnsf_subsym_id ==
                                 SupportValues::TransformIdSubsym::LUT_TRANSFORM
             ? (share_subsym_lut_flag ? 1 : static_cast<uint8_t>(num_subsyms_))
             : 0;
}

// -----------------------------------------------------------------------------
uint8_t StateVars::GetNumPrvs(const bool share_subsym_prv_flag) const {
  return share_subsym_prv_flag ? 1 : static_cast<uint8_t>(num_subsyms_);
}

// -----------------------------------------------------------------------------
void StateVars::populate(
    const SupportValues::TransformIdSubsym transform_id_subsym,
    const SupportValues& support_values, const Binarization& cabac_binarization,
    const core::GenSubIndex& subsequence_id, const core::AlphabetId alphabet_id,
    const bool original) {
  const BinarizationParameters::BinarizationId binarization_id =
      cabac_binarization.GetBinarizationId();
  const BinarizationParameters& cabac_binaz_params =
      cabac_binarization.GetCabacBinarizationParameters();
  const Context& cabac_context_params =
      cabac_binarization.GetCabacContextParameters();

  const uint8_t coding_subsym_size = support_values.GetCodingSubsymSize();
  const uint8_t output_symbol_size = support_values.GetOutputSymbolSize();
  const uint8_t coding_order = support_values.GetCodingOrder();
  const bool share_sub_sym_ctx_flag =
      cabac_context_params.GetShareSubsymCtxFlag();
  const uint16_t num_contexts = cabac_context_params.GetNumContexts();
  num_alpha_subsym_ = 0;

  // numSubsyms
  if (coding_subsym_size > 0) {
    num_subsyms_ = output_symbol_size / coding_subsym_size;
  } else {
    static bool print = false;
    if (!print) {
      GENIE_LOG(util::Logger::Severity::WARNING,
                "coding_subsym_size = " + std::to_string(coding_subsym_size) +
                    " not supported");
      print = true;
    }
  }

  // numAlphaSubsym
  // Check for special cases for numAlphaSubsym
  if (original) {
    num_alpha_subsym_ = GetNumAlphaSpecial(subsequence_id, alphabet_id);
  }

  if (num_alpha_subsym_ == 0) {  // 0 == not special
    num_alpha_subsym_ = Get2PowN(coding_subsym_size);
  }

  // cLengthBI
  if (binarization_id == BinarizationParameters::BinarizationId::BI) {
    c_length_bi_ = coding_subsym_size;
  }

  if (!cabac_binarization.GetBypassFlag()) {
    // numCtxSubsym
    switch (binarization_id) {
      case BinarizationParameters::BinarizationId::BI:
        num_ctx_subsym_ = coding_subsym_size;

        break;
      case BinarizationParameters::BinarizationId::TU:
        num_ctx_subsym_ = cabac_binaz_params.GetCMax();  // cmax
        break;
      case BinarizationParameters::BinarizationId::EG:
        num_ctx_subsym_ = static_cast<uint32_t>(
            std::floor(std::log2(num_alpha_subsym_ + 1)) + 1);
        break;
      case BinarizationParameters::BinarizationId::SEG:
        num_ctx_subsym_ = static_cast<uint32_t>(
            std::floor(std::log2(num_alpha_subsym_ + 1)) + 2);
        break;
      case BinarizationParameters::BinarizationId::TEG:
        num_ctx_subsym_ = cabac_binaz_params.GetCMaxTeg()  // cmax_teg
                          +
                          static_cast<uint32_t>(
                              std::floor(std::log2(num_alpha_subsym_ + 1)) + 1);
        break;
      case BinarizationParameters::BinarizationId::STEG:
        num_ctx_subsym_ = cabac_binaz_params.GetCMaxTeg()  // cmax_teg
                          +
                          static_cast<uint32_t>(
                              std::floor(std::log2(num_alpha_subsym_ + 1)) + 2);
        break;
      case BinarizationParameters::BinarizationId::SUTU: {
        const uint8_t split_unit_size = cabac_binaz_params.GetSplitUnitSize();
        num_ctx_subsym_ = output_symbol_size / split_unit_size *
                              ((1 << split_unit_size) - 1) +
                          ((1 << output_symbol_size % split_unit_size) - 1);
      } break;
      case BinarizationParameters::BinarizationId::SSUTU: {
        const uint8_t split_unit_size = cabac_binaz_params.GetSplitUnitSize();
        num_ctx_subsym_ = output_symbol_size / split_unit_size *
                              ((1 << split_unit_size) - 1) +
                          ((1 << output_symbol_size % split_unit_size) - 1) + 1;
      } break;
      case BinarizationParameters::BinarizationId::DTU: {
        const uint8_t split_unit_size = cabac_binaz_params.GetSplitUnitSize();
        num_ctx_subsym_ = cabac_binaz_params.GetCMaxDtu() +
                          output_symbol_size / split_unit_size *
                              ((1 << split_unit_size) - 1) +
                          ((1 << output_symbol_size % split_unit_size) - 1);
      } break;
      case BinarizationParameters::BinarizationId::SDTU: {
        const uint8_t split_unit_size = cabac_binaz_params.GetSplitUnitSize();
        num_ctx_subsym_ = cabac_binaz_params.GetCMaxDtu() +
                          output_symbol_size / split_unit_size *
                              ((1 << split_unit_size) - 1) +
                          ((1 << output_symbol_size % split_unit_size) - 1) + 1;
      } break;
    }

    // codingOrderCtxOffset[]
    coding_order_ctx_offset_[0] = 0;
    if (coding_order >= 1) coding_order_ctx_offset_[1] = num_ctx_subsym_;
    if (coding_order == 2)
      coding_order_ctx_offset_[2] = num_ctx_subsym_ * num_alpha_subsym_;

    // codingSizeCtxOffset
    if (share_sub_sym_ctx_flag) {
      coding_size_ctx_offset_ = 0;
    } else if (coding_order == 0) {
      coding_size_ctx_offset_ = num_ctx_subsym_;
    } else {
      coding_size_ctx_offset_ =
          coding_order_ctx_offset_[coding_order] * num_alpha_subsym_;
    }

    // numCtxLuts
    num_ctx_luts_ = 0;
    if (transform_id_subsym ==
        SupportValues::TransformIdSubsym::LUT_TRANSFORM) {
      if ((binarization_id >= BinarizationParameters::BinarizationId::SUTU &&
           binarization_id <= BinarizationParameters::BinarizationId::SDTU) ||
          coding_order == 0 || 1u << coding_subsym_size > kMaxLutSize) {
        UTILS_THROW_RUNTIME_EXCEPTION(
            "LUT_TRANSFORM not supported with given configuration: "
            "coding_order = 0, binarization_ID = " +
            std::to_string(static_cast<uint8_t>(binarization_id)) +
            ", coding_subsym_size = " + std::to_string(coding_subsym_size));
      }
      num_ctx_luts_ = coding_subsym_size / 2 * ((1 << 2) - 1) +
                      ((1 << coding_subsym_size % 2) - 1);
    }

    // numCtxTotal
    if (num_contexts != 0) {
      num_ctx_total_ = num_contexts;
    } else {
      num_ctx_total_ = num_ctx_luts_;
      uint32_t context_count = 1;
      if (!share_sub_sym_ctx_flag) {
        context_count = num_subsyms_;
      }
      uint32_t context_size = num_ctx_subsym_;
      if (coding_order > 0) {
        context_size = static_cast<uint32_t>(
            coding_order_ctx_offset_[coding_order] * num_alpha_subsym_);
      }
      num_ctx_total_ += context_count * context_size;
    }
  }  // if (!cabac_binarization.getBypassFlag())
}

// -----------------------------------------------------------------------------
uint32_t StateVars::GetNumSubsymbols() const { return num_subsyms_; }

// -----------------------------------------------------------------------------
uint64_t StateVars::GetNumAlphaSubsymbol() const { return num_alpha_subsym_; }

// -----------------------------------------------------------------------------
uint32_t StateVars::GetNumCtxPerSubsymbol() const { return num_ctx_subsym_; }

// -----------------------------------------------------------------------------

uint32_t StateVars::GetCLengthBi() const { return c_length_bi_; }

// -----------------------------------------------------------------------------

uint64_t StateVars::GetCodingOrderCtxOffset(const uint8_t index) const {
  return coding_order_ctx_offset_[index];
}

// -----------------------------------------------------------------------------

uint64_t StateVars::GetCodingSizeCtxOffset() const {
  return coding_size_ctx_offset_;
}

// -----------------------------------------------------------------------------

uint32_t StateVars::GetNumCtxLuts() const { return num_ctx_luts_; }

// -----------------------------------------------------------------------------

uint64_t StateVars::GetNumCtxTotal() const { return num_ctx_total_; }

// -----------------------------------------------------------------------------

uint8_t StateVars::GetMinimalSizeInBytes(const uint8_t size_in_bits) {
  return size_in_bits / 8 + (size_in_bits % 8 ? 1 : 0);
}

// -----------------------------------------------------------------------------

uint8_t StateVars::GetLgWordSize(const uint8_t size_in_bits) {
  return GetMinimalSizeInBytes(size_in_bits) >> 3;
}

// -----------------------------------------------------------------------------

int64_t StateVars::GetSignedValue(const uint64_t input,
                                  const uint8_t size_in_bytes) {
  int64_t signed_value = 0;

  switch (size_in_bytes) {
    case 1:
      signed_value =
          static_cast<int64_t>(static_cast<int8_t>(input));  // NOLINT
      break;
    case 2:
      signed_value = static_cast<int64_t>(static_cast<int16_t>(input));
      break;
    case 4:
      signed_value = static_cast<int64_t>(static_cast<int32_t>(input));
      break;
    case 8:
      signed_value = static_cast<int64_t>(input);
      break;
    default:
      UTILS_THROW_RUNTIME_EXCEPTION("Invalid Size");
  }

  return signed_value;
}

// -----------------------------------------------------------------------------

uint64_t StateVars::Get2PowN(const uint8_t n) {
  assert(n <= 32);
  constexpr uint64_t one = 1u;
  return one << n;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
