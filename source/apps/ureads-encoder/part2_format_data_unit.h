
#ifndef GENIE_PART2_FORMAT_DATA_UNIT_H
#define GENIE_PART2_FORMAT_DATA_UNIT_H

#include <cstdint>

#include "part2_format_bitwriter.h"

struct Data_unit {

    enum class Dataset_type : uint8_t {
        non_aligned = 0,
        aligned = 1,
        reference = 2
    };

    enum class AU_type : uint8_t {
        NONE = 0,
        P_TYPE_AU = 1,
        N_TYPE_AU = 2,
        M_TYPE_AU = 3,
        I_TYPE_AU = 4,
        HM_TYPE_AU = 5,
        U_TYPE_AU = 6
    };

    enum class Data_unit_type : uint8_t {
        raw_reference = 0,
        parameter_set = 1,
        access_unit = 2,
        skip = 3
    };

    Data_unit_type data_unit_type; // : 8

    explicit Data_unit (const Data_unit_type& t);

    virtual void write(BitWriter* write);
};


#endif //GENIE_PART2_FORMAT_DATA_UNIT_H
