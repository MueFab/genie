#ifndef GENIE_CRPS_INFO_H
#define GENIE_CRPS_INFO_H

// -----------------------------------------------------------------------------------------------------------------

#include <util/make_unique.h>
#include <cstdint>
#include <memory>
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace mpegg_p2 {

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

            uint32_t getBufMaxSize() const {
                return cr_buf_max_size;
            }

            virtual void write(util::BitWriter &writer) const;

            std::unique_ptr<CrpsInfo> clone() const {
                auto ret = util::make_unique<CrpsInfo>();
                *ret = *this;
                return ret;
            }
        };
    }
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CRPS_INFO_H
