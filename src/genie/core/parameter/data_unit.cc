/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "data_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

// ---------------------------------------------------------------------------------------------------------------------

DataUnit::DataUnit(const DataUnitType &t) : data_unit_type(t) {}

// ---------------------------------------------------------------------------------------------------------------------

void DataUnit::write(util::BitWriter &writer, genie::core::stats::PerfStats *) const {
    writer.write(uint64_t(data_unit_type), 8);
}

// ---------------------------------------------------------------------------------------------------------------------

DataUnit::DataUnitType DataUnit::getDataUnitType() const { return data_unit_type; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
