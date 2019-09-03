
#ifndef GENIE_DATA_UNIT_H
#define GENIE_DATA_UNIT_H

#include <cstdint>

#include "bitwriter.h"

/**
 * ISO 23092-2 Section 3.1 table 3
 *
 * base trunc containing only data unit size. See parameter_set, raw_reference, access_unit for specialisations
 */
class Data_unit {
public:
    /**
     * ISO 23092-2 Section 3.1 table 4
     * ------------------------------------------------------------------------------------------------------------- */

    enum class Data_unit_type : uint8_t {
        raw_reference = 0, //!< Line 1
        parameter_set = 1, //!< Line 2
        access_unit = 2 //!< Line 3
    };

    /** ------------------------------------------------------------------------------------------------------------ */

    /**
     * ISO 23092-2 Section 3.3.2
     */
    enum class Dataset_type : uint8_t {
        non_aligned = 0, //!< Line 3
        aligned = 1, //!< Line 3
        reference = 2 //!< Line 3
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

private:
    Data_unit_type data_unit_type;  //!< : 8; Line 2

public:

    explicit Data_unit (const Data_unit_type& t);
    virtual void write(BitWriter* write);
};


#endif //GENIE_DATA_UNIT_H
