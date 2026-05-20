/**
 * Copyright 2018-2024 The Genie Authors.
 * @file binarization.cc
 *
 * @brief Implementation of CABAC binarization for Genie.
 *
 * Provides the `Binarization` class for managing CABAC binarization parameters,
 * context handling, serialization, and JSON conversion.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/binarization.h"

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------

Binarization::Binarization()
    : Binarization(BinarizationParameters::BinarizationId::BI, false,
                   BinarizationParameters(), Context()) {}

// -----------------------------------------------------------------------------

Binarization::Binarization(
    const BinarizationParameters::BinarizationId binarization_id,
    const bool bypass_flag,
    BinarizationParameters&& cabac_binarization_parameters,
    Context&& cabac_context_parameters)
    : binarization_id_(binarization_id),
      bypass_flag_(bypass_flag),
      cabac_binarization_parameters_(cabac_binarization_parameters) {
  if (!bypass_flag_) {
    cabac_context_parameters_ = cabac_context_parameters;
  }
}

// -----------------------------------------------------------------------------

Binarization::Binarization(const uint8_t output_symbol_size,
                           const uint8_t coding_subsym_size,
                           util::BitReader& reader) {
  binarization_id_ = reader.Read<BinarizationParameters::BinarizationId>(5);
  bypass_flag_ = reader.Read<bool>(1);
  cabac_binarization_parameters_ =
      BinarizationParameters(binarization_id_, reader);
  if (!bypass_flag_) {
    cabac_context_parameters_ =
        Context(output_symbol_size, coding_subsym_size, reader);
  }
}

// -----------------------------------------------------------------------------

BinarizationParameters::BinarizationId Binarization::GetBinarizationId() const {
  return binarization_id_;
}

// -----------------------------------------------------------------------------

bool Binarization::GetBypassFlag() const { return bypass_flag_; }

// -----------------------------------------------------------------------------

const BinarizationParameters& Binarization::GetCabacBinarizationParameters()
    const {
  return cabac_binarization_parameters_;
}

// -----------------------------------------------------------------------------

const Context& Binarization::GetCabacContextParameters() const {
  return cabac_context_parameters_;
}

// -----------------------------------------------------------------------------

void Binarization::SetContextParameters(Context&& cabac_context_parameters) {
  bypass_flag_ = false;
  cabac_context_parameters_ = cabac_context_parameters;
}

// -----------------------------------------------------------------------------

void Binarization::write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint8_t>(binarization_id_), 5);
  writer.WriteBits(bypass_flag_, 1);
  cabac_binarization_parameters_.write(binarization_id_, writer);
  if (!bypass_flag_) {
    cabac_context_parameters_.write(writer);
  }
}

// -----------------------------------------------------------------------------

uint8_t Binarization::GetNumBinarizationParams() const {
  return BinarizationParameters::GetNumBinarizationParams(binarization_id_);
}

// -----------------------------------------------------------------------------

bool Binarization::operator==(const Binarization& bin) const {
  return binarization_id_ == bin.binarization_id_ &&
         bypass_flag_ == bin.bypass_flag_ &&
         cabac_binarization_parameters_ == bin.cabac_binarization_parameters_ &&
         cabac_context_parameters_ == bin.cabac_context_parameters_;
}

// -----------------------------------------------------------------------------

Binarization::Binarization(nlohmann::json j) {
  binarization_id_ =
      static_cast<BinarizationParameters::BinarizationId>(j["binarization_ID"]);
  bypass_flag_ = static_cast<bool>(static_cast<uint8_t>(j["bypass_flag"]));
  cabac_binarization_parameters_ = BinarizationParameters(
      j["cabac_binarization_parameters"], binarization_id_);
  if (!bypass_flag_) {
    cabac_context_parameters_ = Context(j["cabac_context_parameters"]);
  }
}

// -----------------------------------------------------------------------------

nlohmann::json Binarization::ToJson() const {
  nlohmann::json ret;
  ret["binarization_ID"] = binarization_id_;
  ret["bypass_flag"] = static_cast<bool>(bypass_flag_);
  if (!cabac_binarization_parameters_.ToJson(binarization_id_).is_null()) {
    ret["cabac_binarization_parameters"] =
        cabac_binarization_parameters_.ToJson(binarization_id_);
  }
  if (!bypass_flag_) {
    ret["cabac_context_parameters"] = cabac_context_parameters_.ToJson();
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------