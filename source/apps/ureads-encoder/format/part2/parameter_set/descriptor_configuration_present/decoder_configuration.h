#ifndef GENIE_DECODER_CONFIGURATION_H
#define GENIE_DECODER_CONFIGURATION_H

// -----------------------------------------------------------------------------------------------------------------

#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h"

#include <memory>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    class BitWriter;
    namespace desc_conf_pres {

        /**
        * ISO 23092-2 Section 3.3.2.1 table 8 lines 4 to 8
        */
        class DecoderConfiguration {
        public:
            enum class EncodingModeId : uint8_t {
                CABAC = 0 //!< See Text in section
            };

            virtual void write(BitWriter *writer) const;

            virtual std::unique_ptr<DecoderConfiguration> clone() const = 0;

            DecoderConfiguration(EncodingModeId _encoding_mode_id);

            virtual ~DecoderConfiguration() = default;

        protected:
            EncodingModeId encoding_mode_ID; //!< : 8; Line 4
        };
    }
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_DECODER_CONFIGURATION_H
