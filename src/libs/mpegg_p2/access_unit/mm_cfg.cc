#include "mm_cfg.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
void MmCfg::write(util::BitWriter &writer) const {
    writer.write(mm_threshold, 16);
    writer.write(mm_count, 32);
}

// -----------------------------------------------------------------------------------------------------------------

MmCfg::MmCfg(uint16_t _mm_threshold, uint32_t _mm_count) : mm_threshold(_mm_threshold), mm_count(_mm_count) {}

// -----------------------------------------------------------------------------------------------------------------

MmCfg::MmCfg() : mm_threshold(0), mm_count(0) {}

MmCfg::MmCfg(util::BitReader &reader) {
    mm_threshold = reader.read(16);
    mm_count = reader.read(32);
}
}  // namespace mpegg_p2
}  // namespace genie