#include "data_unit.h"
#include "access_unit.h"
#include "parameter_set.h"

// -----------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_p2 {
DataUnit::DataUnit(const DataUnitType &t) : data_unit_type(t) {}

// -----------------------------------------------------------------------------------------------------------------
void DataUnit::write(util::BitWriter &writer) const { writer.write(uint64_t(data_unit_type), 8); }

DataUnit::DataUnitType DataUnit::getDataUnitType() const { return data_unit_type; }

// -----------------------------------------------------------------------------------------------------------------
}  // namespace mpegg_p2
}  // namespace genie