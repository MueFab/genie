#include "data_unit.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
DataUnit::DataUnit(const DataUnitType &t) : data_unit_type(t) {}

// -----------------------------------------------------------------------------------------------------------------

void DataUnit::write(util::BitWriter *writer) const { writer->write(uint8_t(data_unit_type), 8); }
}  // namespace format