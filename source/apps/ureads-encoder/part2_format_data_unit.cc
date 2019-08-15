#include "part2_format_data_unit.h"

enum class Data_unit_type : uint8_t {
    raw_reference = 0,
    parameter_set = 1,
    access_unit = 2,
    skip = 3
};

Data_unit_type data_unit_type; // : 8

Data_unit::Data_unit(const Data_unit_type &t) : data_unit_type(t) {
}

void Data_unit::write(BitWriter *writer) {
    writer->write(uint8_t(data_unit_type), 8);
}