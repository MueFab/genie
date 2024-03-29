/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/mm_cfg.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

bool MmCfg::operator==(const MmCfg &other) const {
    return mm_threshold == other.mm_threshold && mm_count == other.mm_count;
}

// ---------------------------------------------------------------------------------------------------------------------

void MmCfg::write(util::BitWriter &writer) const {
    writer.write(mm_threshold, 16);
    writer.write(mm_count, 32);
}

// ---------------------------------------------------------------------------------------------------------------------

MmCfg::MmCfg(uint16_t _mm_threshold, uint32_t _mm_count) : mm_threshold(_mm_threshold), mm_count(_mm_count) {}

// ---------------------------------------------------------------------------------------------------------------------

MmCfg::MmCfg() : mm_threshold(0), mm_count(0) {}

// ---------------------------------------------------------------------------------------------------------------------

MmCfg::MmCfg(util::BitReader &reader) {
    mm_threshold = reader.read<uint16_t>();
    mm_count = reader.read<uint32_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
