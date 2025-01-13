/**
 * Copyright 2018-2024 The Genie Authors.
 * @file support_values.cc
 * @brief Implementation of CABAC support values for Genie.
 *
 * Provides the `SupportValues` class to manage parameters like output symbol
 * size, coding subsymbol size, coding order, and flags for shared subsymbol
 * LUTs and PRVs. Includes methods for serialization and JSON conversion.
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/support_values.h"

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------

SupportValues::SupportValues() : SupportValues(8, 8, 0) {}

// -----------------------------------------------------------------------------

SupportValues::SupportValues(const uint8_t output_symbol_size,
                             const uint8_t coding_subsym_size,
                             const uint8_t coding_order,
                             const bool share_subsym_lut_flag,
                             const bool share_subsym_prv_flag)
    : output_symbol_size_(output_symbol_size),
      coding_subsym_size_(coding_subsym_size),
      coding_order_(coding_order),
      share_subsym_lut_flag_(share_subsym_lut_flag),
      share_subsym_prv_flag_(share_subsym_prv_flag) {}

// -----------------------------------------------------------------------------

SupportValues::SupportValues(const TransformIdSubsym transform_id_subsym,
                             util::BitReader& reader)
    : share_subsym_lut_flag_(false), share_subsym_prv_flag_(false) {
  output_symbol_size_ = reader.Read<uint8_t>(6);
  coding_subsym_size_ = reader.Read<uint8_t>(6);
  coding_order_ = reader.Read<uint8_t>(2);
  if (coding_subsym_size_ < output_symbol_size_ && coding_order_ > 0) {
    if (transform_id_subsym == TransformIdSubsym::LUT_TRANSFORM) {
      share_subsym_lut_flag_ = reader.Read<bool>(1);
    }
    share_subsym_prv_flag_ = reader.Read<bool>(1);
  }
}

// -----------------------------------------------------------------------------

void SupportValues::write(const TransformIdSubsym transform_id_subsym,
                          util::BitWriter& writer) const {
  writer.WriteBits(output_symbol_size_, 6);
  writer.WriteBits(coding_subsym_size_, 6);
  writer.WriteBits(coding_order_, 2);
  if (coding_subsym_size_ < output_symbol_size_ && coding_order_ > 0) {
    if (transform_id_subsym == TransformIdSubsym::LUT_TRANSFORM) {
      writer.WriteBits(share_subsym_lut_flag_, 1);
    }
    writer.WriteBits(share_subsym_prv_flag_, 1);
  }
}

// -----------------------------------------------------------------------------

uint8_t SupportValues::GetOutputSymbolSize() const {
  return output_symbol_size_;
}

// -----------------------------------------------------------------------------

uint8_t SupportValues::GetCodingSubsymSize() const {
  return coding_subsym_size_;
}

// -----------------------------------------------------------------------------

uint8_t SupportValues::GetCodingOrder() const { return coding_order_; }

// -----------------------------------------------------------------------------

bool SupportValues::GetShareSubsymLutFlag() const {
  return share_subsym_lut_flag_;
}

// -----------------------------------------------------------------------------

bool SupportValues::GetShareSubsymPrvFlag() const {
  return share_subsym_prv_flag_;
}

// -----------------------------------------------------------------------------

bool SupportValues::operator==(const SupportValues& val) const {
  return output_symbol_size_ == val.output_symbol_size_ &&
         coding_subsym_size_ == val.coding_subsym_size_ &&
         coding_order_ == val.coding_order_ &&
         share_subsym_lut_flag_ == val.share_subsym_lut_flag_ &&
         share_subsym_prv_flag_ == val.share_subsym_prv_flag_;
}

// -----------------------------------------------------------------------------

SupportValues::SupportValues(nlohmann::json j,
                             const TransformIdSubsym transform_id_subsym) {
  share_subsym_lut_flag_ = false;
  share_subsym_prv_flag_ = false;
  output_symbol_size_ = j["output_symbol_size"];
  coding_subsym_size_ = j["coding_subsym_size"];
  coding_order_ = j["coding_order"];
  if (coding_subsym_size_ < output_symbol_size_ && coding_order_ > 0) {
    if (transform_id_subsym == TransformIdSubsym::LUT_TRANSFORM) {
      share_subsym_lut_flag_ = j["share_subsym_lut_flag"];
    }
    share_subsym_prv_flag_ = j["share_subsym_prv_flag"];
  }
}

// -----------------------------------------------------------------------------

nlohmann::json SupportValues::ToJson(
    const TransformIdSubsym transform_id_subsym) const {
  nlohmann::json ret;
  ret["output_symbol_size"] = output_symbol_size_;
  ret["coding_subsym_size"] = coding_subsym_size_;
  ret["coding_order"] = coding_order_;
  if (coding_subsym_size_ < output_symbol_size_ && coding_order_ > 0) {
    if (transform_id_subsym == TransformIdSubsym::LUT_TRANSFORM) {
      ret["share_subsym_lut_flag"] = share_subsym_lut_flag_;
    }
    ret["share_subsym_prv_flag"] = share_subsym_prv_flag_;
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
