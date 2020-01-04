#ifndef GENIE_DATA_UNIT_H
#define GENIE_DATA_UNIT_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>

#include "util/bitreader.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {

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

   private:
    DataUnitType data_unit_type;  //!< : 8; Line 2

   public:
    explicit DataUnit(const DataUnitType &t);

    virtual ~DataUnit() = default;

    virtual void write(util::BitWriter &write) const;

    void setDataUnitType(DataUnitType t) { data_unit_type = t; }

    DataUnitType getDataUnitType() { return data_unit_type; }

    std::vector<uint8_t> rawData;  // dataUnitSize and DataUnitType not included!
};

}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATA_UNIT_H
