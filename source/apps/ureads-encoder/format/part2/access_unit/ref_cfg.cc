#include "ref_cfg.h"
#include "ureads-encoder/format/part2/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    void RefCfg::write(BitWriter *writer) {
        writer->write(ref_sequence_ID, 16);
        writer->write(ref_start_position, posSize);
        writer->write(ref_end_position, posSize);
    }

    // -----------------------------------------------------------------------------------------------------------------

    RefCfg::RefCfg(uint16_t _ref_sequence_ID, uint64_t _ref_start_position, uint64_t _ref_end_position, uint8_t _posSize)
            : ref_sequence_ID(_ref_sequence_ID), ref_start_position(_ref_start_position),
              ref_end_position(_ref_end_position),
              posSize(_posSize){

    }
}