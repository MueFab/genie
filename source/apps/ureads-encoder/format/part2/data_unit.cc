#include "data_unit.h"

Data_unit::Data_unit(const Data_unit_type &t) : data_unit_type(t) {
}

void Data_unit::write(BitWriter *writer) {
    writer->write(uint8_t(data_unit_type), 8);
}