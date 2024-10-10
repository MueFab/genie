/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/location/internal.h"
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location {

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(uint8_t _reserved, uint8_t _internal_dataset_group_id, uint16_t _internal_dataset_id)
    : Location(_reserved, false),
      internal_dataset_group_id(_internal_dataset_group_id),
      internal_dataset_id(_internal_dataset_id) {}

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(genie::util::BitReader& reader) : Location(reader) {
    internal_dataset_group_id = reader.readAlignedInt<uint8_t>();
    internal_dataset_id = reader.readAlignedInt<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(genie::util::BitReader& reader, uint8_t _reserved) : Location(_reserved, false) {
    internal_dataset_group_id = reader.readAlignedInt<uint8_t>();
    internal_dataset_id = reader.readAlignedInt<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Internal::getDatasetGroupID() const { return internal_dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Internal::getDatasetID() const { return internal_dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

void Internal::write(genie::util::BitWriter& writer) {
    Location::write(writer);
    writer.writeAlignedInt(internal_dataset_group_id);
    writer.writeAlignedInt(internal_dataset_id);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> Internal::decapsulate() {
    auto ret = std::make_unique<genie::core::meta::InternalRef>(internal_dataset_group_id, internal_dataset_id);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
