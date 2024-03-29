/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference/location/internal.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace reference {
namespace location {

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(uint8_t _reserved, uint8_t _internal_dataset_group_id, uint16_t _internal_dataset_id)
    : Location(_reserved, false),
      internal_dataset_group_id(_internal_dataset_group_id),
      internal_dataset_id(_internal_dataset_id) {}

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(genie::util::BitReader& reader) : Location(reader) {
    internal_dataset_group_id = reader.readBypassBE<uint8_t>();
    internal_dataset_id = reader.readBypassBE<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(genie::util::BitReader& reader, uint8_t _reserved) : Location(_reserved, false) {
    internal_dataset_group_id = reader.readBypassBE<uint8_t>();
    internal_dataset_id = reader.readBypassBE<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Internal::getDatasetGroupID() const { return internal_dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Internal::getDatasetID() const { return internal_dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

void Internal::write(genie::util::BitWriter& writer) {
    Location::write(writer);
    writer.writeBypassBE(internal_dataset_group_id);
    writer.writeBypassBE(internal_dataset_id);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> Internal::decapsulate() {
    auto ret = genie::util::make_unique<genie::core::meta::InternalRef>(internal_dataset_group_id, internal_dataset_id);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace location
}  // namespace reference
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
