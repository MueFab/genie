/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATA_UNIT_H
#define GENIE_DATA_UNIT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <memory>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

namespace stats {
class PerfStats;
}

// ---------------------------------------------------------------------------------------------------------------------

namespace parameter {

/**
 *
 */
class DataUnit {
   public:
    /**
     *
     */
    enum class DataUnitType : uint8_t { RAW_REFERENCE = 0, PARAMETER_SET = 1, ACCESS_UNIT = 2 };

    /**
     *
     */
    enum class DatasetType : uint8_t { NON_ALIGNED = 0, ALIGNED = 1, REFERENCE = 2 };

   private:
    DataUnitType data_unit_type;  //!<

   public:
    /**
     *
     * @param t
     */
    explicit DataUnit(const DataUnitType &t);

    /**
     *
     */
    virtual ~DataUnit() = default;

    /**
     *
     * @param write
     * @param stats
     */
    virtual void write(util::BitWriter &write) const;

    /**
     *
     * @return
     */
    DataUnitType getDataUnitType() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATA_UNIT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
