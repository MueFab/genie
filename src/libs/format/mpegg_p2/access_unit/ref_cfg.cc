#include "ref_cfg.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
void RefCfg::write(util::BitWriter *writer) {
    writer->write(ref_sequence_ID, 16);
    writer->write(ref_start_position, posSize);
    writer->write(ref_end_position, posSize);
}

// -----------------------------------------------------------------------------------------------------------------

RefCfg::RefCfg(uint16_t _ref_sequence_ID, uint64_t _ref_start_position, uint64_t _ref_end_position, uint8_t _posSize)
    : ref_sequence_ID(_ref_sequence_ID),
      ref_start_position(_ref_start_position),
      ref_end_position(_ref_end_position),
      posSize(_posSize) {}

// -----------------------------------------------------------------------------------------------------------------

RefCfg::RefCfg(uint8_t _posSize) : RefCfg(0, 0, 0, _posSize) {}
}  // namespace format
