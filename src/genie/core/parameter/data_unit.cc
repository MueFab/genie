/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/data_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter {

// ---------------------------------------------------------------------------------------------------------------------

DataUnit::DataUnit(const DataUnitType &t) : data_unit_type(t) {}

// ---------------------------------------------------------------------------------------------------------------------

void DataUnit::write(util::BitWriter &writer) const { writer.writeBits(uint64_t(data_unit_type), 8); }

// ---------------------------------------------------------------------------------------------------------------------

DataUnit::DataUnitType DataUnit::getDataUnitType() const { return data_unit_type; }

// ---------------------------------------------------------------------------------------------------------------------

void DataUnit::print_debug(std::ostream &output, uint8_t, uint8_t) const {
    output << "* Data Unit: ";
    switch (data_unit_type) {
        case DataUnitType::RAW_REFERENCE:
            output << "raw reference";
            break;
        case DataUnitType::PARAMETER_SET:
            output << "parameter set";
            break;
        case DataUnitType::ACCESS_UNIT:
            output << "access unit";
            break;
    }
    output << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
