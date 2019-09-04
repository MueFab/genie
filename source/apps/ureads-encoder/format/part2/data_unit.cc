#include "data_unit.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    DataUnit::DataUnit(const DataUnitType &t) : data_unit_type(t) {
    }

    // -----------------------------------------------------------------------------------------------------------------

    void DataUnit::write(BitWriter *writer) {
        writer->write(uint8_t(data_unit_type), 8);
    }
}