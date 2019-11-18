#include "data_unit.h"
#include "util/bitwriter.h"
#include "util/bitreader.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
DataUnit::DataUnit(util::BitReader *bitReader)
{
    this->read(bitReader);
    fprintf(stdout, "%u\n", (uint8_t)data_unit_type);
}

// -----------------------------------------------------------------------------------------------------------------

DataUnit::DataUnit(const DataUnitType &t) : data_unit_type(t) {}

// -----------------------------------------------------------------------------------------------------------------

void DataUnit::write(util::BitWriter *writer) const
{
    //writer->write(uint8_t(data_unit_type),8); //TODO:?
}

// -----------------------------------------------------------------------------------------------------------------

void DataUnit::read(util::BitReader *bitReader)
{
    uint32_t value;
    bitReader->readNBitsDec(8, &value);
    data_unit_type = (DataUnit::DataUnitType) value;
    return;
}

}  // namespace format