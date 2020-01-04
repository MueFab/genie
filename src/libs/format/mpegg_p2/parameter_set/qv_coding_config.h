#ifndef GENIE_QV_CODING_CONFIG_H
#define GENIE_QV_CODING_CONFIG_H

// -----------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/make_unique.h>
#include <cstdint>
#include <memory>
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace mpegg_p2 {

/**
 * ISO 23092-2 Section 3.3.2 table 7 lines 32 to 40
 */
        class QvCodingConfig {
        public:
            /**
             * Inserted for convenience, not named in ISO 23092-2
             */
            enum class QvCodingMode : uint8_t {
                ONE = 1
            };

            virtual void write(util::BitWriter &writer) const = 0;

            virtual ~QvCodingConfig() = default;

            QvCodingConfig(QvCodingMode _qv_coding_mode, bool _qv_reverse_flag);

            static std::unique_ptr<QvCodingConfig> factory(util::BitReader &reader);

            virtual std::unique_ptr<QvCodingConfig> clone() const = 0;

        protected:
            QvCodingMode qv_coding_mode;  //!< : 4; Line 32
            bool qv_reverse_flag : 1;     //!< Line 40
        };
    }
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_CODING_CONFIG_H
