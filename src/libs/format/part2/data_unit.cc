#include "data_unit.h"
#include "parameter_set.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
DataUnit::DataUnit(const DataUnitType &t) : data_unit_type(t) {}

// -----------------------------------------------------------------------------------------------------------------

DataUnit::DataUnit(const DataUnitType &t, const uint32_t &s)
:data_unit_type(t), data_unit_size(s)
{}

// -----------------------------------------------------------------------------------------------------------------
void DataUnit::write(util::BitWriter *writer) const
{
    writer->write(uint64_t(data_unit_type), 8);
}

// -----------------------------------------------------------------------------------------------------------------

DataUnit* DataUnit::createFromBitReader(util::BitReader *bitReader)
{
    uint32_t buffer;
    bitReader->readNBitsDec(8, &buffer);
    DataUnit * dataUnit = new DataUnit((DataUnitType)buffer);

    switch (dataUnit->getDataUnitType()) {
        case DataUnitType::RAW_REFERENCE : {
            bitReader->readNBitsDec(64, &buffer);
            dataUnit->setDataUnitSize(buffer);
            fprintf(stderr, "DataUnitFactory RAW_REFERENZE not supported yet!\n");
            break;
        }
        case DataUnitType::PARAMETER_SET : {
            fprintf(stdout, "DataUnitFactory PARAMETER_SET detected!\n");
            bitReader->skipNBits(10);
            bitReader->readNBitsDec(22, &buffer);
            dataUnit->setDataUnitSize(buffer);

            ParameterSet *parameterSet = new ParameterSet(dataUnit);
            return parameterSet;
            break;
        }
        case DataUnitType::ACCESS_UNIT : {
            fprintf(stdout, "DataUnitFactory ACCESS_UNIT detected!\n");
            bitReader->skipNBits(3);
            bitReader->readNBitsDec(29, &buffer);
            dataUnit->setDataUnitSize(buffer);
            break;
        }
        default:{
            fprintf(stderr, "DataUnitFactory invalid DataUnitType!\n");
        }
    }

    delete dataUnit;
    return nullptr; //TODO: return dataUnit?
}
}  // namespace format