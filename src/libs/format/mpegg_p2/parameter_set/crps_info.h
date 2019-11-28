#ifndef GENIE_CRPS_INFO_H
#define GENIE_CRPS_INFO_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {

/**
 * ISO 23092-2 Section 3.3.2.3 table 16 lines 3+4
 */
class CrpsInfo {
   private:
    uint8_t cr_pad_size : 8;        //!<< Line 3
    uint32_t cr_buf_max_size : 24;  //!<< Line 4
   public:
    CrpsInfo(uint8_t cr_pad_size, uint32_t cr_buf_max_size);
    CrpsInfo();
    virtual ~CrpsInfo() = default;

    virtual void write(util::BitWriter *writer) const;
};
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CRPS_INFO_H
