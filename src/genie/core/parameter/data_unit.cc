/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/data_unit.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

// -----------------------------------------------------------------------------
DataUnit::DataUnit(const DataUnitType& t) : data_unit_type_(t) {}

// -----------------------------------------------------------------------------
void DataUnit::Write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint64_t>(data_unit_type_), 8);
}

// -----------------------------------------------------------------------------
DataUnit::DataUnitType DataUnit::GetDataUnitType() const {
  return data_unit_type_;
}

// -----------------------------------------------------------------------------
void DataUnit::PrintDebug(std::ostream& output, uint8_t, uint8_t) const {
  output << "* Data Unit: ";
  switch (data_unit_type_) {
    case DataUnitType::kRawReference:
      output << "raw reference";
      break;
    case DataUnitType::kParameterSet:
      output << "parameter set";
      break;
    case DataUnitType::kAccessUnit:
      output << "access unit";
      break;
  }
  output << std::endl;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
