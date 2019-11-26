#include "crps_info.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
CrpsInfo::CrpsInfo(uint8_t _cr_pad_size, uint32_t _cr_buf_max_size)
    : cr_pad_size(_cr_pad_size), cr_buf_max_size(_cr_buf_max_size) {}

CrpsInfo::CrpsInfo() : CrpsInfo(0, 0) {}

// -----------------------------------------------------------------------------------------------------------------

void CrpsInfo::write(util::BitWriter *writer) const {
    writer->write(cr_pad_size, 8);
    writer->write(cr_buf_max_size, 24);
}
}  // namespace format
