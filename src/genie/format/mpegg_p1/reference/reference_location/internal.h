/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_INTERNAL_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_INTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mpegg_p1/reference/reference_location/reference_location.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Internal : public ReferenceLocation {
 private:
    uint8_t dataset_group_ID;  //!< @brief
    uint16_t dataset_ID;       //!< @brief

 public:
    /**
     * @brief
     * @param _dataset_group_ID
     * @param _dataset_ID
     */
    Internal(uint8_t _dataset_group_ID, uint16_t _dataset_ID);

    /**
     * @brief
     * @param reader
     */
    explicit Internal(util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const override;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_INTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
