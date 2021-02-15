/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/ref_cfg.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

uint16_t RefCfg::getSeqID() const { return ref_sequence_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t RefCfg::getStart() const { return ref_start_position; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t RefCfg::getEnd() const { return ref_end_position; }

// ---------------------------------------------------------------------------------------------------------------------

void RefCfg::write(util::BitWriter &writer) const {
    writer.write(ref_sequence_ID, 16);
    writer.write(ref_start_position, posSize);
    writer.write(ref_end_position, posSize);
}

// ---------------------------------------------------------------------------------------------------------------------

RefCfg::RefCfg(uint16_t _ref_sequence_ID, uint64_t _ref_start_position, uint64_t _ref_end_position, uint8_t _posSize)
    : ref_sequence_ID(_ref_sequence_ID),
      ref_start_position(_ref_start_position),
      ref_end_position(_ref_end_position),
      posSize(_posSize) {}

// ---------------------------------------------------------------------------------------------------------------------

RefCfg::RefCfg(uint8_t _posSize) : RefCfg(0, 0, 0, _posSize) {}

// ---------------------------------------------------------------------------------------------------------------------

RefCfg::RefCfg(uint8_t _posSize, util::BitReader &reader) : posSize(_posSize) {
    ref_sequence_ID = reader.read<uint16_t>();
    ref_start_position = reader.read<uint64_t>(posSize);
    ref_end_position = reader.read<uint64_t>(posSize);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
