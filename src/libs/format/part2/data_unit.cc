#include "data_unit.h"
#include "access_unit.h"
#include "parameter_set.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
DataUnit::DataUnit(const DataUnitType &t) : data_unit_type(t) {}

// -----------------------------------------------------------------------------------------------------------------

DataUnit::DataUnit(const DataUnitType &t, const uint32_t &s) : data_unit_type(t), data_unit_size(s) {}

// -----------------------------------------------------------------------------------------------------------------
void DataUnit::write(util::BitWriter *writer) const { writer->write(uint64_t(data_unit_type), 8); }

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<format::DataUnit> DataUnit::createFromBitReader(util::BitReader *bitReader) {
    uint32_t buffer;
    bitReader->readNBitsDec(8, &buffer);

    switch ((DataUnitType)buffer) {
        case DataUnitType::RAW_REFERENCE: {
            fprintf(stderr, "DataUnitFactory RAW_REFERENZE not supported yet!\n");
            bitReader->setFailBit();
            break;
        }
        case DataUnitType::PARAMETER_SET: {
            fprintf(stdout, "DataUnitFactory PARAMETER_SET detected!\n");
            return std::unique_ptr<DataUnit>(new ParameterSet(bitReader));
            break;
        }
        case DataUnitType::ACCESS_UNIT: {
            fprintf(stdout, "DataUnitFactory ACCESS_UNIT detected!\n");
            return std::unique_ptr<DataUnit>(new AccessUnit(bitReader));
            break;
        }
        default: {
            fprintf(stderr, "DataUnitFactory invalid DataUnitType!\n");
            bitReader->setFailBit();
        }
    }

    return nullptr;
}
}  // namespace format