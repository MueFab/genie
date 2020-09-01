#include "internal.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(uint8_t _dataset_group_ID, uint16_t _dataset_ID)
    : ReferenceLocation(ReferenceLocation::Flag::INTERNAL),
      dataset_group_ID(_dataset_group_ID),
      dataset_ID(_dataset_ID){}

// ---------------------------------------------------------------------------------------------------------------------

Internal::Internal(util::BitReader& reader)
    : ReferenceLocation(ReferenceLocation::Flag::INTERNAL),
      dataset_group_ID(reader.read<uint8_t>()),
      dataset_ID(reader.read<uint16_t>()){}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Internal::getLength() const {

    // internal_dataset_group_ID u(8)
    uint64_t bitlength = 8;

    // internal_dataset_ID u(16)
    bitlength += 16;

    return bitlength;
}

// ---------------------------------------------------------------------------------------------------------------------

void Internal::write(util::BitWriter& bit_writer) const {
    // internal_dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // internal_dataset_ID u(16)
    bit_writer.write(dataset_ID, 16);
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
