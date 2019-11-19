#ifndef GENIE_DATA_UNIT_H
#define GENIE_DATA_UNIT_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>

#include "util/bitwriter.h"
#include "util/bitreader.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {

/**
 * ISO 23092-2 Section 3.1 table 3
 *
 * base trunc containing only data unit size. See parameter_set, raw_reference, access_unit for specialisations
 */
class DataUnit {
   public:
    /**
     * ISO 23092-2 Section 3.1 table 4
     * ------------------------------------------------------------------------------------------------------------- */

    enum class DataUnitType : uint8_t {
        RAW_REFERENCE = 0,  //!< Line 1
        PARAMETER_SET = 1,  //!< Line 2
        ACCESS_UNIT = 2     //!< Line 3
    };

    /** ------------------------------------------------------------------------------------------------------------ */

    /**
     * ISO 23092-2 Section 3.3.2
     */
    enum class DatasetType : uint8_t {
        NON_ALIGNED = 0,  //!< Line 3
        ALIGNED = 1,      //!< Line 3
        REFERENCE = 2     //!< Line 3
    };

    enum class AuType : uint8_t {
        NONE = 0,
        P_TYPE_AU = 1,
        N_TYPE_AU = 2,
        M_TYPE_AU = 3,
        I_TYPE_AU = 4,
        HM_TYPE_AU = 5,
        U_TYPE_AU = 6
    };

   private:
    DataUnitType data_unit_type;  //!< : 8; Line 2
    uint32_t data_unit_size;

   public:
    explicit DataUnit(const DataUnitType &t);

    explicit DataUnit(const DataUnitType &t, const uint32_t &s);

    virtual ~DataUnit()= default;

    virtual void write(util::BitWriter *write) const;

    static DataUnit* createFromBitReader(util::BitReader *bitReader);

    void setDataUnitType(DataUnitType t){ data_unit_type = t; }
    DataUnitType getDataUnitType(){return data_unit_type;}

    void setDataUnitSize(uint32_t size){ data_unit_size = size; }
    uint32_t getDataUnitSize(){ return data_unit_size; }
};

}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATA_UNIT_H
