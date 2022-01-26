/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/internal.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(uint8_t _dataset_group_ID, uint16_t _dataset_ID)
    : ReferenceLocation(ReferenceLocation::Flag::INTERNAL),
      dataset_group_ID(_dataset_group_ID),
      dataset_ID(_dataset_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(util::BitReader& reader)
    : ReferenceLocation(ReferenceLocation::Flag::INTERNAL),
      dataset_group_ID(reader.read<uint8_t>()),
      dataset_ID(reader.read<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Internal::getLength() const {
    // reserved u(7), external_ref_flag u(1), internal_dataset_group_ID u(8), internal_dataset_ID u(16)
    return ReferenceLocation::getLength() + 3;
}

// ---------------------------------------------------------------------------------------------------------------------

void Internal::write(util::BitWriter& bitwriter) const {
    // reserved u(7), external_ref_flag u(1)
    ReferenceLocation::write(bitwriter);

    // internal_dataset_group_ID u(8)
    bitwriter.write(dataset_group_ID, 8);

    // internal_dataset_ID u(16)
    bitwriter.write(dataset_ID, 16);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
