/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_DATA_UNIT_RECORD_H_
#define SRC_GENIE_CORE_RECORD_DATA_UNIT_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bitwriter.h"

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
 * @brief
 */
class DataUnit {
 public:
    enum class DataUnitType : uint8_t { RAW_REFERENCE = 0, PARAMETER_SET = 1, ACCESS_UNIT = 2 };

    enum class DatasetType : uint8_t { NON_ALIGNED = 0, ALIGNED = 1, REFERENCE = 2, COUNT = 3 };

    explicit DataUnit(const DataUnitType &t);

    virtual ~DataUnit() = default;

    virtual void write(util::BitWriter &write) const;

    DataUnitType getDataUnitType() const;

    virtual void print_debug(std::ostream &output, uint8_t, uint8_t) const;

 private:
    DataUnitType data_unit_type;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_DATA_UNIT_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
