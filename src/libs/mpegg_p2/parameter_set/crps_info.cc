#include "crps_info.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
CrpsInfo::CrpsInfo(uint8_t _cr_pad_size, uint32_t _cr_buf_max_size)
    : cr_pad_size(_cr_pad_size), cr_buf_max_size(_cr_buf_max_size) {}

CrpsInfo::CrpsInfo() : CrpsInfo(0, 0) {}

// -----------------------------------------------------------------------------------------------------------------

uint32_t CrpsInfo::getBufMaxSize() const { return cr_buf_max_size; }

void CrpsInfo::write(util::BitWriter &writer) const {
    writer.write(cr_pad_size, 8);
    writer.write(cr_buf_max_size, 24);
}

std::unique_ptr<CrpsInfo> CrpsInfo::clone() const {
    auto ret = util::make_unique<CrpsInfo>();
    *ret = *this;
    return ret;
}

}  // namespace mpegg_p2
}  // namespace genie
