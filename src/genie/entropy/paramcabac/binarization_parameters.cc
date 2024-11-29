/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/binarization_parameters.h"

#include <vector>

#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters()
    : BinarizationParameters(BinarizationId::BI, std::vector<uint8_t>()) {}

// -----------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters(const BinarizationId bin_id,
                                               util::BitReader& reader) {
  switch (bin_id) {
    case BinarizationId::TU:
      cmax_ = reader.Read<uint8_t>();
      break;
    case BinarizationId::TEG:
    case BinarizationId::STEG:
      cmax_teg_ = reader.Read<uint8_t>();
      break;
    case BinarizationId::DTU:
    case BinarizationId::SDTU:
      cmax_dtu_ = reader.Read<uint8_t>();  // Fall-through
    case BinarizationId::SUTU:
    case BinarizationId::SSUTU:
      split_unit_size_ = reader.Read<uint8_t>(4);
      break;
    default:
      break;
  }
}

// -----------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters(
    const BinarizationId& binarization_id, const std::vector<uint8_t>& params) {
  switch (binarization_id) {
    case BinarizationId::TU:
      cmax_ = params[0];
      break;
    case BinarizationId::TEG:
    case BinarizationId::STEG:
      cmax_teg_ = params[0];
      break;
    case BinarizationId::DTU:
    case BinarizationId::SDTU:
      cmax_dtu_ = params[1];  // Fall-through
    case BinarizationId::SUTU:
    case BinarizationId::SSUTU:
      split_unit_size_ = params[0];
      break;
    case BinarizationId::BI:
    case BinarizationId::EG:
    case BinarizationId::SEG:
      break;
    default:
      UTILS_THROW_RUNTIME_EXCEPTION("Binarization not supported");
  }
}

// -----------------------------------------------------------------------------

void BinarizationParameters::write(const BinarizationId bin_id,
                                   util::BitWriter& writer) const {
  switch (bin_id) {
    case BinarizationId::TU:
      writer.WriteBits(cmax_, 8);
      break;
    case BinarizationId::TEG:
    case BinarizationId::STEG:
      writer.WriteBits(cmax_teg_, 8);
      break;
    case BinarizationId::DTU:
    case BinarizationId::SDTU:
      writer.WriteBits(cmax_dtu_, 8);  // Fall-through
    case BinarizationId::SUTU:
    case BinarizationId::SSUTU:
      writer.WriteBits(split_unit_size_, 4);
      break;
    default:
      break;
  }
}

// -----------------------------------------------------------------------------

uint8_t BinarizationParameters::GetCMax() const { return cmax_; }

// -----------------------------------------------------------------------------

uint8_t BinarizationParameters::GetCMaxTeg() const { return cmax_teg_; }

// -----------------------------------------------------------------------------

uint8_t BinarizationParameters::GetCMaxDtu() const { return cmax_dtu_; }

// -----------------------------------------------------------------------------

uint8_t BinarizationParameters::GetSplitUnitSize() const {
  return split_unit_size_;
}

// -----------------------------------------------------------------------------

uint8_t BinarizationParameters::num_params_[static_cast<unsigned>(
                                                BinarizationId::SDTU) +
                                            1u] = {0, 1, 0, 0, 1,
                                                   1, 1, 1, 2, 2};

// -----------------------------------------------------------------------------

uint8_t BinarizationParameters::GetNumBinarizationParams(
    BinarizationId binarization_id) {
  return num_params_[static_cast<uint8_t>(binarization_id)];
}

// -----------------------------------------------------------------------------

bool BinarizationParameters::operator==(
    const BinarizationParameters& bin) const {
  return cmax_ == bin.cmax_ && cmax_teg_ == bin.cmax_teg_ &&
         cmax_dtu_ == bin.cmax_dtu_ && split_unit_size_ == bin.split_unit_size_;
}

// -----------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters(nlohmann::json j,
                                               const BinarizationId bin_id) {
  cmax_ = 0;
  cmax_teg_ = 0;
  cmax_dtu_ = 0;
  split_unit_size_ = 0;
  switch (bin_id) {
    case BinarizationId::TU:
      cmax_ = j["cmax"];
      break;
    case BinarizationId::TEG:
    case BinarizationId::STEG:
      cmax_ = j["cmax_teg"];
      break;
    case BinarizationId::DTU:
    case BinarizationId::SDTU:
      cmax_dtu_ = j["cmax_dtu"];  // Fall-through
    case BinarizationId::SUTU:
    case BinarizationId::SSUTU:
      cmax_dtu_ = j["split_unit_size"];
      break;
    default:
      break;
  }
}

// -----------------------------------------------------------------------------

nlohmann::json BinarizationParameters::ToJson(
    const BinarizationId bin_id) const {
  nlohmann::json ret;
  switch (bin_id) {
    case BinarizationId::TU:
      ret["cmax"] = cmax_;
      break;
    case BinarizationId::TEG:
    case BinarizationId::STEG:
      ret["cmax_teg"] = cmax_teg_;
      break;
    case BinarizationId::DTU:
    case BinarizationId::SDTU:
      ret["cmax_dtu"] = cmax_dtu_;  // Fall-through
    case BinarizationId::SUTU:
    case BinarizationId::SSUTU:
      ret["split_unit_size"] = cmax_dtu_;
      break;
    default:
      break;
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
