/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DATA_UNIT_H_
#define SRC_GENIE_CORE_PARAMETER_DATA_UNIT_H_

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
    /**
     * @brief
     */
    enum class DataUnitType : uint8_t { RAW_REFERENCE = 0, PARAMETER_SET = 1, ACCESS_UNIT = 2 };

    /**
     * @brief
     */
    enum class DatasetType : uint8_t { NON_ALIGNED = 0, ALIGNED = 1, REFERENCE = 2, COUNT = 3 };

 private:
    DataUnitType data_unit_type;  //!< @brief

 public:
    /**
     * @brief
     * @param t
     */
    explicit DataUnit(const DataUnitType &t);

    /**
     * @brief
     */
    virtual ~DataUnit() = default;

    /**
     * @brief
     * @param write
     */
    virtual void write(util::BitWriter &write) const;

    /**
     * @brief
     * @return
     */
    DataUnitType getDataUnitType() const;

    /**
     * @brief
     * @param output
     */
    virtual void print_debug(std::ostream &output, uint8_t, uint8_t) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DATA_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
